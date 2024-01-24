#include "SetJoinGroupParelled.h"
#include <omp.h>
#include <limits.h>
#include "../util/util.h"
#include "../util/io.h"
#include "group_paralled/PartitionHasher.h"
// #include "group_paralled/PartitionIndex.h"
#include "group_paralled/PartitionMapIndex.h"
#include "group_paralled/PartitionSmallIndex.h"
#include "group_paralled/RangeLoader.h"
#include <google/dense_hash_map>
#include "../../emhash/hash_table7.hpp"

vector<unsigned int> tmp_p_keys[MAXTHREADNUM];
vector<unsigned int> tmp_od_keys[MAXTHREADNUM];
vector<unsigned int> tmp_odk_st[MAXTHREADNUM];

vector<int> prime_exp;

unsigned int available_threads_amount;

typedef google::dense_hash_map<unsigned int, vector<unsigned int>> hashmap_t;
typedef emhash7::HashMap<unsigned int, unsigned int> hash_t;

bool SetJoinGroupParelled::overlap(const vector<unsigned int> &x, const vector<unsigned int> &y) {
    int requiredOverlap = ceil(det / (1 + det) * (int)(x.size() + y.size()) - EPS);
    int xIdx = 0, yIdx = 0, currentOverlap = 0;

    // Calculate overlapping elements between two vectors.
    while (xIdx < (int)x.size() && yIdx < (int)y.size()) {
        if ((int)x.size() - xIdx + currentOverlap < requiredOverlap || (int)y.size() - yIdx + currentOverlap < requiredOverlap) return false;

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
    available_threads_amount = getHowManyThreads();
    det = threshold;
    ALPHA = 1.0 / threshold + 0.01;
    coe = (1 - det) / (1 + det);

    PartitionHasher::init_primeArr();
    // PartitionIndex::allocateTmpVecSpace();
    // PartitionSmallIndex::allocateTmpVecSpace();
    // reserve space for tmp vector
    for (auto i = 0; i < MAXTHREADNUM; i++) {
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
    PartitionHasher *currentHasher = nullptr;
    PartitionIndex *previousIndex = nullptr;
    PartitionIndex *currentIndex = nullptr;
    // PartitionSmallIndex *previousIndex = nullptr;
    // PartitionSmallIndex *currentIndex;
    unsigned int beforePreviousDocId = 0, previousDocId = 0;
    unsigned int prev_max_len = 0;
    unsigned int prev_dataset_size = 0;
    while (rangeLoader.loadNextRange(*currentDataset)) {
        rangeLoader.showCurrentRangeInfo();

        // Compare with the previous dataset.
        if (previousHasher) {
            processPreviousDataset(*previousDataset, prev_max_len, prev_dataset_size, *currentDataset, *previousHasher, *previousIndex, beforePreviousDocId, previousDocId);
        }

        // Cleanup old data
        if (previousIndex != nullptr) {
            delete previousIndex;
            previousIndex = nullptr;
        }
        if (previousHasher != nullptr) {
            delete previousHasher;
            previousHasher = nullptr;
        }
        if (previousDataset != nullptr) {
            delete previousDataset;
            previousDataset = nullptr;
        }

        if (currentIndex != nullptr) {
            delete currentIndex;
            currentIndex = nullptr;
        }
        if (currentHasher != nullptr) {
            delete currentHasher;
            currentHasher = nullptr;
        }

        auto cur_dataset_size = currentDataset->size();
        prev_max_len = (*currentDataset)[cur_dataset_size - 1].size();
        // Compare within the current dataset.

        processCurrentDataset(currentDataset, currentHasher, currentIndex, previousDocId);

        // Update references for the next iteration.
        beforePreviousDocId = previousDocId;
        previousDocId += cur_dataset_size;
        previousDataset = currentDataset; // be careful here maybe nullptr
        previousHasher = currentHasher;   // be careful here maybe nullptr
        previousIndex = currentIndex;
        currentIndex = nullptr;
        currentHasher = nullptr;
        prev_dataset_size = cur_dataset_size;
        currentDataset = new vector<vector<unsigned int>>();
    }

    IO_cost = rangeLoader.load_cost;
    docsAmount = rangeLoader.docs_amount;

    printf("Total Cost time is %f  in these %u documents that divided into %d ranges\n",
           RepTime(startTime), rangeLoader.docs_amount, rangeLoader.range_num);
}

void SetJoinGroupParelled::processCurrentDataset(
    vector<vector<unsigned int>> *&currentDataset,
    PartitionHasher *&currentHasher,
    PartitionIndex *&currentIndex,
    const unsigned int &previousDocId) {
    // printf("Index Current Group\n");

    auto index_st = LogTime();

    currentHasher = new PartitionHasher(*currentDataset);
    unsigned int partitionNum = floor(2 * coe * (*currentDataset)[0].size() + EPS) + 1;

    currentHasher->hash(*currentDataset, partitionNum);

    cout<<currentHasher->onedelete_keys.size() <<"sdfsfdf" << endl;
    currentIndex = new PartitionIndex(det, coe);
    currentIndex->createIndex(*currentHasher, partitionNum);

    index_cost += RepTime(index_st);

    // printf("Finding Simpairs in the current index\n");

    auto len = currentHasher->parts_keys.size();
#pragma omp parallel for
    for (unsigned int docId = 0; docId < len; docId++) {
        vector<unsigned int> candidates;
        int tid = omp_get_thread_num();

        auto alloc_st = LogTime();
        auto & p_keys = currentHasher->parts_keys[docId];
        auto & od_keys =currentHasher->onedelete_keys[docId];
        auto & odk_st = currentHasher->odkeys_st[docId];

        // Obtain candidates
        lw_cost[tid] += currentIndex->greedy_search(*currentHasher, tid, docId, p_keys, od_keys, odk_st, candidates);
        alloc_cost[tid] += RepTime(alloc_st);
        // perform verification.
        verifyPairs(*currentDataset, *currentDataset, docId, candidates, previousDocId, previousDocId);

        //todo add ifdef to change directory
        // need filling
    }

    // todo add ifdef
    currentHasher->release_keys();
    // todo need remove this
}

void SetJoinGroupParelled::processPreviousDataset(
    const vector<vector<unsigned int>> &previousDataset,
    const unsigned int prev_max_len,
    const unsigned int prev_dataset_len,
    const vector<vector<unsigned int>> &currentDataset,
    PartitionHasher &previousHasher,
    PartitionIndex &previousIndex,
    const unsigned int &beforePreviousDocId,
    const unsigned int &previousDocId) {
    // printf("Finding Simpairs in the previous index\n");

    // judge if the maximum doc of previous group is too short, then we not need to compare anymore
    unsigned int cur_min_len = currentDataset[0].size();
    if ((double)cur_min_len * det > (double)prev_max_len) {
        printf("Adjacent group differ too much\n");
        return;
    }

    // unsigned int partitionNum = floor(2 * coe * previousDataset[0].size() + EPS) + 1;
    unsigned int partitionNum = previousIndex.partition_num;

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
        unsigned int fake_docId = docId + prev_dataset_len;
        lw_cost[tid] += previousIndex.greedy_search(previousHasher, tid, fake_docId , tmp_p_keys[tid], tmp_od_keys[tid], tmp_odk_st[tid], candidates);
        alloc_cost[tid] += RepTime(alloc_st);

        // Obtain candidates and perform verification.
        verifyPairs(previousDataset, currentDataset, docId, candidates , beforePreviousDocId, previousDocId);

        //todo add ifdef to change directory
        // need filling
    }
}

void SetJoinGroupParelled::verifyPairs(
    const vector<vector<unsigned int>> &datasetA,
    const vector<vector<unsigned int>> &datasetB,
    unsigned int docId,
    const vector<unsigned int> &candidates,
    unsigned int datasetAStartId,
    unsigned int datasetBStartId) {
    
    int threadId = omp_get_thread_num();
    auto verif_st = LogTime();
    assert(docId<datasetB.size() && docId>=0);
    // Verify the candidate pairs.
    for (unsigned int candidate : candidates) {
        assert(candidate<datasetA.size() && candidate>=0);
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
    simPairsAmount = pairs_amount;
    return pairs_amount;
}

void SetJoinGroupParelled::reportTimeCost() {
    double total_hash_cost = 0;
    double total_alloc_cost = 0;
    double total_verif_cost = 0;
    double total_lw_cost = 0;
    double sum;
    for (int i = 0; i < MAXTHREADNUM; i++) {
        total_hash_cost += hashInFind_cost[i];
        total_alloc_cost += alloc_cost[i];
        total_verif_cost += verif_cost[i];
        total_lw_cost += lw_cost[i];
    }

    // cout<<"Before shrinking," << total_hash_cost<<endl;
    sum = total_hash_cost + total_alloc_cost + total_verif_cost;

    double search_cost = setjoin_cost - index_cost;

    total_hash_cost = total_hash_cost / sum * search_cost;
    total_alloc_cost = total_alloc_cost / sum * search_cost;
    total_verif_cost = total_verif_cost / sum * search_cost;
    total_lw_cost = total_lw_cost / sum * search_cost;

    printf("| Docs Amount| Found Pairs |IO_cost|index_cost| total_hash_cost| alloc_cost| verif_cost| total_cost |\n");
    printf("|%u |%llu |%f|%f|%f|%f|%f|%f|\n", docsAmount, simPairsAmount, IO_cost, index_cost, total_hash_cost, total_alloc_cost, total_verif_cost, setjoin_cost);
}

void SetJoinGroupParelled::testUnordered_map(double threshold) {
    initializeParameters(threshold);

    RangeLoader rangeLoader(dataset_path, ALPHA);
    vector<vector<unsigned int>> *previousDataset = nullptr;
    vector<vector<unsigned int>> *currentDataset = new vector<vector<unsigned int>>();
    PartitionHasher *previousHasher = nullptr;
    PartitionHasher *currentHasher;
    PartitionIndex *previousIndex = nullptr;
    PartitionIndex *currentIndex;
    // PartitionSmallIndex *previousIndex = nullptr;
    // PartitionSmallIndex *currentIndex;
    unsigned int beforePreviousDocId, previousDocId = 0;

    auto timer_st = LogTime();
    while (rangeLoader.loadNextRange(*currentDataset)) {
        rangeLoader.showCurrentRangeInfo();

        const vector<vector<unsigned int>> &curDataset = *currentDataset;
        // test unordered+map now,
        currentHasher = new PartitionHasher(curDataset);
        unsigned int partitionNum = floor(2 * coe * curDataset[0].size() + EPS) + 1;

        delete currentDataset;
        currentDataset = nullptr;
        // cout << "Hashing cost: " << currentHasher->hash(curDataset, partitionNum)<<endl;

        // currentIndex = new PartitionIndex(det, coe);
        // currentIndex->createIndex(*currentHasher, partitionNum);

        // Allocate memory for unordered_map
        vector<hash_t> part_index(partitionNum);
        vector<hash_t> odkey_index(partitionNum);

        const auto &odkeys_st = currentHasher->odkeys_st;
        const auto &onedelete_keys = currentHasher->onedelete_keys;
        const auto &parts_keys = currentHasher->parts_keys;
        unsigned int len = parts_keys.size();
        vector<vector<unsigned int>> part_rids(partitionNum);
        vector<vector<unsigned int>> odk_rids(partitionNum);

#pragma omp parallel for
        for (auto pid = 0; pid < partitionNum; pid++) {
            size_t one_deletion_amount = 0;
            auto &tmp_vec = part_rids[pid];
            tmp_vec.resize(len);
            part_index[pid].reserve(len);
            auto &cur_pindex = part_index[pid];
            for (unsigned int rid = 0; rid < len; rid++) {
                cur_pindex[parts_keys[rid][pid]]++;
                one_deletion_amount += odkeys_st[rid][pid + 1] - odkeys_st[rid][pid];
            }

            unsigned int cnt = 0;
            unsigned int pre_cnt = -1;

            for (auto &it : part_index[pid]) {
                pre_cnt = cnt;
                cnt += it.second;
                it.second = pre_cnt;
            }

            for (unsigned int rid = 0; rid < len; rid++) {
                tmp_vec[cur_pindex[parts_keys[rid][pid]]++] = rid;
            }

            // Now for one deletion key
            auto &tmp_vec_odk = odk_rids[pid];
            tmp_vec_odk.resize(one_deletion_amount);
            odkey_index[pid].reserve(one_deletion_amount);
            auto &cur_odkindex = odkey_index[pid];
            // cur_odkindex.max_load_factor(2.0);
            // cout<<"there are"<< one_deletion_amount <<endl;
            for (unsigned int rid = 0; rid < len; rid++) {
                unsigned int const od_loc_st = odkeys_st[rid][pid];
                unsigned int const od_loc_ed = odkeys_st[rid][pid + 1];
                for (unsigned int od_loc = od_loc_st; od_loc < od_loc_ed; od_loc++) {
                    auto &odk = onedelete_keys[rid][od_loc];
                    cur_odkindex[odk]++;
                }
            }

            cnt = 0;
            for (auto &it : odkey_index[pid]) {
                pre_cnt = cnt;
                cnt += it.second;
                it.second = pre_cnt;
            }

            for (unsigned int rid = 0; rid < len; rid++) {
                unsigned int const od_loc_st = odkeys_st[rid][pid];
                unsigned int const od_loc_ed = odkeys_st[rid][pid + 1];
                for (unsigned int od_loc = od_loc_st; od_loc < od_loc_ed; od_loc++) {
                    auto &odk = onedelete_keys[rid][od_loc];
                    tmp_vec_odk[cur_odkindex[odk]++] = rid;
                }
            }
        }

        // Cleanup old data.
        delete previousIndex;
        delete previousHasher;
        delete previousDataset;
        // Update references for the next iteration.
        beforePreviousDocId = previousDocId;
        // previousDocId += currentDataset->size();
        previousDataset = currentDataset;
        previousHasher = currentHasher;
        previousIndex = currentIndex;
        currentDataset = new vector<vector<unsigned int>>();
    }
    cout << "Indexing cost :" << RepTime(timer_st) << endl;
}