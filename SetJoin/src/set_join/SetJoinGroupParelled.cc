#include "SetJoinGroupParelled.h"
#include <omp.h>
#include <limits.h>
#include "../util/util.h"
#include "../util/io.h"
#include "group_paralled/PartitionHasher.h"
#include "group_paralled/PartitionIndex.h"
#include "group_paralled/RangeLoader.h"

vector<unsigned int> tmp_p_keys[MAXTHREADNUM];
vector<unsigned int> tmp_od_keys[MAXTHREADNUM];
vector<unsigned int> tmp_odk_st[MAXTHREADNUM];

vector<int> prime_exp;
vector<unsigned int> tmp_rid_arr[MAXTHREADNUM];
vector<unsigned int> tmp_odlocs_arr[MAXTHREADNUM];

bool SetJoinGroupParelled::overlap(const vector<unsigned int>& x, const vector<unsigned int>& y) {
    int requiredOverlap = ceil(det / (1 + det) * (int)(x.size() + y.size()) - EPS);
    int xIdx = 0, yIdx = 0, currentOverlap = 0;

    // Calculate overlapping elements between two vectors.
    while (xIdx < (int)x.size() && yIdx < (int)y.size()) {
        if ((int)x.size() - xIdx + currentOverlap < requiredOverlap || 
            (int)y.size() - yIdx + currentOverlap < requiredOverlap) return false;

        if (x[xIdx] == y[yIdx]) {
            currentOverlap++;
            xIdx++;
            yIdx++;
        } else if (x[xIdx] < y[yIdx]) {
            xIdx++;
        } else {
            yIdx++;
        }
    }
    return currentOverlap >= requiredOverlap;
}

void SetJoinGroupParelled::initializeParameters(double threshold) {
    det = threshold;
    ALPHA = 1.0 / threshold + 0.01;
    coe = (1 - det) / (1 + det);
    PartitionHasher::init_primeArr();

    // reserve space for tmp vector
    for(auto i = 0;i<MAXTHREADNUM;i++){
        tmp_p_keys[i].reserve(3e4);
        tmp_od_keys[i].reserve(3e4);
        tmp_odk_st[i].reserve(3e4);
    }
}

void SetJoinGroupParelled::setjoin(double threshold) {
    auto search_st = LogTime();
    initializeParameters(threshold);
    processDataset();
    setjoin_cost = RepTime(search_st);
}

void SetJoinGroupParelled::processDataset() {
    auto startTime = LogTime();

    RangeLoader rangeLoader(dataset_path, ALPHA);
    vector<vector<unsigned int>> *previousDataset = nullptr;
    vector<vector<unsigned int>> *currentDataset = new vector<vector<unsigned int>>();
    PartitionHasher *previousHasher = nullptr;
    PartitionHasher *currentHasher;
    PartitionIndex *previousIndex = nullptr;
    PartitionIndex *currentIndex;
    unsigned int beforePreviousDocId, previousDocId = 0;
    
    while (rangeLoader.loadNextRange(*currentDataset)) {
        rangeLoader.showCurrentRangeInfo();

        // Compare with the previous dataset.
        if (previousDataset) {
            processPreviousDataset(*previousDataset, *currentDataset, *previousHasher, *previousIndex, beforePreviousDocId, previousDocId);
        }
        
        // Cleanup old data.
        delete previousIndex;
        delete previousHasher;
        delete previousDataset;
        
        // Compare within the current dataset.
        processCurrentDataset(*currentDataset, currentHasher, currentIndex, previousDocId);
        
        // Update references for the next iteration.
        beforePreviousDocId = previousDocId;
        previousDocId += currentDataset->size();
        previousDataset = currentDataset;
        previousHasher = currentHasher;
        previousIndex = currentIndex;
        currentDataset = new vector<vector<unsigned int>>();
    }

    IO_cost = rangeLoader.load_cost;
    
    printf("Total Cost time is %f  in these %u documents that divided into %d ranges\n", 
           RepTime(startTime), rangeLoader.docs_amount, rangeLoader.range_num);
}

void SetJoinGroupParelled::processCurrentDataset(
    const vector<vector<unsigned int>>& currentDataset, 
    PartitionHasher*& currentHasher, PartitionIndex*& currentIndex,const unsigned int& previousDocId) {
    printf("Index Current Group\n");
    
    auto index_st = LogTime();

    currentHasher = new PartitionHasher(currentDataset);
    unsigned int partitionNum = floor(2 * coe * currentDataset[0].size() + EPS) + 1;
    
    currentHasher->hash(currentDataset, partitionNum);
    // cout<<currentHasher->onedelete_keys.size() <<"sdfsfdf" << endl;
    currentIndex = new PartitionIndex(det, coe);
    currentIndex->createIndex(*currentHasher, partitionNum);
    
    index_cost += RepTime(index_st);

    printf("Finding Simpairs in the current index\n");
    
#pragma omp parallel for
    for (unsigned int docId = 0; docId < currentDataset.size(); docId++) {
        vector<unsigned int> candidates;
        int tid = omp_get_thread_num();

        auto alloc_st = LogTime();
        auto & p_keys = currentHasher->parts_keys[docId];
        auto & od_keys =currentHasher->onedelete_keys[docId];
        auto & odk_st = currentHasher->odkeys_st[docId];

        // Obtain candidates
        currentIndex->greedy_search(*currentHasher, tid, docId, p_keys, od_keys, odk_st, candidates);
        alloc_cost[tid] += RepTime(alloc_st);
        // perform verification.
        verifyPairs(currentDataset, currentDataset, docId, candidates, previousDocId, previousDocId);
    }
}

void SetJoinGroupParelled::processPreviousDataset(
    const vector<vector<unsigned int>>& previousDataset, 
    const vector<vector<unsigned int>>& currentDataset, 
    PartitionHasher& previousHasher, PartitionIndex& previousIndex,
    const unsigned int & beforePreviousDocId,const unsigned int& previousDocId) {
    printf("Finding Simpairs in the previous index\n");
    
    unsigned int partitionNum = floor(2 * coe * previousDataset[0].size() + EPS) + 1;
    
#pragma omp parallel for
    for (unsigned int docId = 0; docId < currentDataset.size(); docId++) {
        vector<unsigned int> candidates;
        int tid = omp_get_thread_num();

        tmp_p_keys[tid].clear();
        tmp_od_keys[tid].clear();
        tmp_odk_st[tid].clear();

        hashInFind_cost[tid] += previousHasher.hash(currentDataset[docId],partitionNum, tid, tmp_p_keys[tid], tmp_od_keys[tid], tmp_odk_st[tid]);
        
        auto alloc_st = LogTime();
        // make sure current docId is larger than all those in the previous range
        unsigned int fake_docId = docId + previousDataset.size();
        previousIndex.greedy_search(previousHasher, tid, fake_docId , tmp_p_keys[tid], tmp_od_keys[tid], tmp_odk_st[tid], candidates);
        alloc_cost[tid] += RepTime(alloc_st);

        // Obtain candidates and perform verification.
        verifyPairs(previousDataset, currentDataset, docId, candidates , beforePreviousDocId, previousDocId);
    }
}

void SetJoinGroupParelled::verifyPairs(
    const vector<vector<unsigned int>>& datasetA,
    const vector<vector<unsigned int>>& datasetB,
    unsigned int docId, const vector<unsigned int> &candidates,
    unsigned int datasetAStartId, unsigned int datasetBStartId) {
    int threadId = omp_get_thread_num();
    auto verif_st = LogTime();
    // Verify the candidate pairs.
    for (unsigned int candidate : candidates) {
        if (overlap(datasetB[docId], datasetA[candidate])) {
            // we need to add the startId to get the true id of them
            result_pairs[threadId].emplace_back(datasetBStartId + docId, datasetAStartId + candidate);
        }
    }
    verif_cost[threadId] += RepTime(verif_st);
}

// Function to get the total number of result pairs found by all threads
unsigned long long SetJoinGroupParelled::getResultPairsAmount() {
    unsigned long long pairs_amount = 0;
    for (int i = 0; i < MAXTHREADNUM; i++) {
        pairs_amount += result_pairs[i].size();
    }
    return pairs_amount;
}

void SetJoinGroupParelled::reportTimeCost() {
    double total_hash_cost = 0;
    double total_alloc_cost = 0;
    double total_verif_cost = 0;
    double sum;
    for (int i = 0; i < MAXTHREADNUM; i++) {
        total_hash_cost += hashInFind_cost[i];
        total_alloc_cost += alloc_cost[i];
        total_verif_cost += verif_cost[i];
    }

    // cout<<"Before shrinking," << total_hash_cost<<endl;
    sum = total_hash_cost + total_alloc_cost + total_verif_cost;

    double search_cost = setjoin_cost - index_cost;

    total_hash_cost = total_hash_cost / sum * search_cost;
    total_alloc_cost = total_alloc_cost / sum * search_cost;
    total_verif_cost = total_verif_cost / sum * search_cost;

    printf("|IO_cost|index_cost| total_hash_cost| alloc_cost| verif_cost|\n");
    printf("|%f|%f|%f|%f|%f|\n", IO_cost, index_cost, total_hash_cost, total_alloc_cost, total_verif_cost);
}
