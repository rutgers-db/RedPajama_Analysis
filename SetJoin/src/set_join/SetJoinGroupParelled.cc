#include "SetJoinGroupParelled.h"
#include <omp.h>
#include <limits.h>
#include "../util/util.h"
#include "../util/io.h"
#include "group_paralled/PartitionHasher.h"
#include "group_paralled/PartitionIndex.h"
#include "group_paralled/RangeLoader.h"

vector<vector<unsigned int>> subquery[MAXTHREADNUM];

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
}

void SetJoinGroupParelled::setjoin(double threshold) {
    auto search_st = LogTime();
    initializeParameters(threshold);
    processDataset();
    setjoin_cost = RepTime(search_st);
}

// void SetJoinGroupParelled::setjoin(double threshold){
//     auto time_st = LogTime();

//     det = threshold;
//     ALPHA = 1.0 / threshold + 0.01;
//     coe = (1 - det) / (1 + det);
    

//     // create range loader
//     RangeLoader rangeloader(dataset_path, ALPHA);
//     vector<vector<unsigned int>> * prev_range_dataset = nullptr;
//     vector<vector<unsigned int>> * cur_range_dataset = new vector<vector<unsigned int>>();
//     PartitionHasher * prev_hasher = nullptr;
//     PartitionHasher * cur_hasher;
//     PartitionIndex * prev_index = nullptr;
//     PartitionIndex * cur_index;

//     unsigned int beforePrev_rid;
//     unsigned int prev_rid = 0;
//     while(rangeloader.loadNextRange(*cur_range_dataset)){
//         rangeloader.showCurrentRangeInfo();
//         const auto & cur_dataset = *cur_range_dataset;
//         // search in the previous datsaet and index
//         if(prev_range_dataset != nullptr){
//             printf("Compare with previous dataset\n");
//             auto & prev_dataset = *prev_range_dataset;
//             unsigned int partNum = floor(2 * coe * prev_dataset[0].size() + EPS) + 1;
//              // find candidates
// #pragma omp parallel for
//             for(unsigned int rid = 0; rid < cur_dataset.size(); rid++){
//                 vector<unsigned int> candidates;
//                 int tid = omp_get_thread_num();

//                 vector<unsigned int> p_keys;
//                 vector<unsigned int> od_keys;
//                 vector<unsigned int> odk_st;
//                 prev_hasher->hash(cur_dataset[rid],partNum, tid, p_keys, od_keys, odk_st);

//                 // make sure current rid is larger than all those in the previous range
//                 unsigned int fake_rid = rid + prev_dataset.size();
//                 prev_index->greedy_search(*prev_hasher, tid, fake_rid , p_keys, od_keys, odk_st, candidates);
                
//                 // verify the pairs
//                 for (unsigned int idx = 0; idx < candidates.size(); idx++) {
//                     auto & x = cur_dataset[rid];
//                     auto & y = prev_dataset[candidates[idx]];
//                     if (overlap(x, y) == true) {
//                         result_pairs[tid].emplace_back(prev_rid + rid, beforePrev_rid + candidates[idx]);
//                     }
//                 }
//             }
//         }

//         // due to the reference that use
//         delete prev_index; // remove the range that not used
//         delete prev_hasher; // remove the range that not used
//         delete prev_range_dataset; // remove the range that not used

//         printf("Index Current Group\n");
//         cur_hasher = new PartitionHasher(*cur_range_dataset);
       
//         unsigned int partNum = floor(2 * coe * cur_dataset[0].size() + EPS) + 1; 
//         cur_hasher->hash(cur_dataset, partNum);
//         cur_index = new PartitionIndex(det, coe);
//         cur_index->createIndex(*cur_hasher, partNum);
        
//         printf("Compare with Current dataset\n");
//         // find candidates
// #pragma omp parallel for
//         for(unsigned int rid = 0; rid < cur_dataset.size(); rid++){
//             vector<unsigned int> candidates;
//             int tid = omp_get_thread_num();

//             auto & p_keys = cur_hasher->parts_keys[rid];
//             auto & od_keys =cur_hasher->onedelete_keys[rid];
//             auto & odk_st = cur_hasher->odkeys_st[rid];
//             cur_index->greedy_search(*cur_hasher, tid, rid, p_keys, od_keys, odk_st, candidates);
            
//             // verify the pairs
//             for (unsigned int idx = 0; idx < candidates.size(); idx++) {
//                 auto & x = cur_dataset[rid];
//                 auto & y = cur_dataset[candidates[idx]];
//                 if (overlap(x, y) == true) {
//                     result_pairs[tid].emplace_back(prev_rid + rid, prev_rid + candidates[idx]);
//                 }
//             }
//         }

//         beforePrev_rid = prev_rid;
//         prev_rid += cur_dataset.size();

//         prev_range_dataset = cur_range_dataset;
//         prev_hasher = cur_hasher;
//         prev_index = cur_index;
        
//         cur_range_dataset = new vector<vector<unsigned int>>();

//     }

//     printf("Total Cost time is %f  in these %u documents that divided into %d ranges\n", RepTime(time_st), prev_rid, rangeloader.range_num);
// }

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

        
        // if(rangeLoader.range_num < 33){
        //     currentDataset->clear();
        //     continue;
        // }
            
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

        vector<unsigned int> p_keys;
        vector<unsigned int> od_keys;
        vector<unsigned int> odk_st;

        hashInFind_cost[tid] += previousHasher.hash(currentDataset[docId],partitionNum, tid, p_keys, od_keys, odk_st);
        
        auto alloc_st = LogTime();
        // make sure current docId is larger than all those in the previous range
        unsigned int fake_docId = docId + previousDataset.size();
        previousIndex.greedy_search(previousHasher, tid, fake_docId , p_keys, od_keys, odk_st, candidates);
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
    alloc_cost[threadId] += RepTime(verif_st);
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
    sum = total_hash_cost + total_alloc_cost + total_verif_cost;

    double search_cost = setjoin_cost - index_cost;
    
    total_hash_cost = total_hash_cost / sum * search_cost;
    total_alloc_cost = total_alloc_cost / sum * search_cost;
    total_verif_cost = total_verif_cost / sum * search_cost;

    printf("|index_cost| total_hash_cost| alloc_cost| verif_cost|\n");
    printf("|%f|%f|%f|%f|\n", index_cost, total_hash_cost, total_alloc_cost, total_verif_cost);
}
