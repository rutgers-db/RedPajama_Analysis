#ifndef _PARTITIONSMALLINDEX_H_
#define _PARTITIONSMALLINDEX_H_

#include <vector>
#include <iostream>
#include <execution>
#include <tbb/tbb.h>
#include <omp.h>
#include <algorithm> // For sort, iota
#include <limits>    // For numeric_limits
#include "PartitionHasher.h"
#include "../../util/util.h"

using namespace std;

extern vector<unsigned int> tmp_rid_arr[MAXTHREADNUM];
extern vector<unsigned int> tmp_odlocs_arr[MAXTHREADNUM];
extern unsigned int available_threads_amount;

class PartitionSmallIndex {
private:
    bool **quickRef2D; // 2D quick reference array
    bool **negRef2D;   // 2D negative reference array

public:
    double mem_cost[MAXTHREADNUM] = {0};
    double find_cost[MAXTHREADNUM] = {0};
    double alloc_cost[MAXTHREADNUM] = {0};

    // Index-related containers
    vector<vector<unsigned int>> parts_rids;
    vector<vector<unsigned int>> ods_rids;
    vector<vector<unsigned int>> parts_index_hv;
    vector<vector<unsigned int>> od_index_hv;
    vector<vector<unsigned int>> parts_index_offset;
    vector<vector<unsigned int>> parts_index_cnt;
    vector<vector<unsigned int>> od_index_offset;
    vector<vector<unsigned int>> od_index_cnt;

    // Search-related vectors that needed when allocate by greedy heap method
    vector<unsigned int> invPtrArr[MAXTHREADNUM];
    vector<unsigned int> intPtrArr[MAXTHREADNUM];
    vector<vector<unsigned int>> onePtrArr[MAXTHREADNUM];
    vector < pair<int, unsigned int>> valuesArr[MAXTHREADNUM]; // <value, loc>
    vector < unsigned int > scoresArr[MAXTHREADNUM];
    unsigned int max_len;       // the maximum length in the current group

    double det;
    double coe;
    
    static void allocateTmpVecSpace(){
        for(auto tid = 0; tid <MAXTHREADNUM; tid++){
            tmp_rid_arr[tid].reserve(1e6);
            tmp_odlocs_arr[tid].reserve(1e6);
        }
    }

    PartitionSmallIndex(double delta, double _coe): det(delta), coe(_coe){}

    ~PartitionSmallIndex(){
        for (int i = 0; i < MAXTHREADNUM; ++i) {
            delete[] quickRef2D[i];
            delete[] negRef2D[i];
        }
        delete[] quickRef2D;
        delete[] negRef2D;
    }
    
public:
    // Method to create index based on PartitionHasher
    double createIndex(PartitionHasher &part_hasher, unsigned int partNum) {
        auto time_st = LogTime();

        // Allocate Memory for index
        parts_rids.resize(partNum);
        ods_rids.resize(partNum);
        parts_index_hv.resize(partNum);
        od_index_hv.resize(partNum);
        parts_index_offset.resize(partNum);
        parts_index_cnt.resize(partNum);
        od_index_offset.resize(partNum);
        od_index_cnt.resize(partNum);

        // Allocate memory for those in the searching
        for (auto tid = 0; tid < MAXTHREADNUM; tid++) {
            invPtrArr[tid].resize(partNum);
            intPtrArr[tid].resize(partNum);
            onePtrArr[tid].resize(partNum);
            valuesArr[tid].resize(partNum);
            scoresArr[tid].resize(partNum);
        }

        // Initialize quick reference arrays
        unsigned int n = part_hasher.parts_keys.size();
        quickRef2D = new bool *[MAXTHREADNUM];
        negRef2D = new bool *[MAXTHREADNUM];
        for (unsigned int i = 0; i < MAXTHREADNUM; ++i) {
            quickRef2D[i] = new bool[n];
            memset(quickRef2D[i], 0, sizeof(bool) * n);
            negRef2D[i] = new bool[n];
            memset(negRef2D[i], 0, sizeof(bool) * n);
        }

        // Obtain required data members from PartitionHasher
        const auto &parts_keys = part_hasher.parts_keys;
        const auto &onedelete_keys = part_hasher.onedelete_keys;
        const auto &odkeys_st = part_hasher.odkeys_st;

        max_len = onedelete_keys[n-1].size();

        // calculate each tbb sort use how many threads
        bool use_tbb = false;
        if(available_threads_amount>partNum){
            use_tbb = true;
            
            // int tbb_threads =int( ceil(double(available_threads_amount) / partNum));
            cout<<"using tbb "  << endl;
        }
        // Build Index for partitions
#pragma omp parallel for
        for (auto pid = 0; pid < partNum; pid++) {
            unsigned int len = parts_keys.size();
            unsigned int one_deletion_amount = 0;

            // First Index Partitions
            auto &cur_rids = parts_rids[pid];
            cur_rids.reserve(len);

            for (unsigned int rid = 0; rid < len; rid++) {
                cur_rids.emplace_back(rid);
                one_deletion_amount += odkeys_st[rid][pid + 1] - odkeys_st[rid][pid];
            }

            
            // Sort the partitions based on hashing value and record id
            if (use_tbb)
                sort(execution::par_unseq, cur_rids.begin(), cur_rids.end(), [&pid, &parts_keys](const unsigned int &rid_1, const unsigned int &rid_2) {
                    if (parts_keys[rid_1][pid] == parts_keys[rid_2][pid])
                        return rid_1 < rid_2;
                    return parts_keys[rid_1][pid] < parts_keys[rid_2][pid];
                });
            else
                sort(cur_rids.begin(), cur_rids.end(), [&pid, &parts_keys](const unsigned int &rid_1, const unsigned int &rid_2) {
                    if (parts_keys[rid_1][pid] == parts_keys[rid_2][pid])
                        return rid_1 < rid_2;
                    return parts_keys[rid_1][pid] < parts_keys[rid_2][pid];
                });

            // Build index pointers for current partitions
            unsigned int prev_hv = parts_keys[cur_rids[0]][pid];
            unsigned int tmp_cnt = 1;
            unsigned int ofs = 0;
            for (unsigned int j = 1; j < cur_rids.size(); j++) {
                auto &cur_hv = parts_keys[cur_rids[j]][pid];
                if (cur_hv != prev_hv) {
                    parts_index_hv[pid].emplace_back(prev_hv);
                    parts_index_offset[pid].emplace_back(ofs);
                    parts_index_cnt[pid].emplace_back(tmp_cnt);
                    prev_hv = cur_hv;
                    ofs = j;
                    tmp_cnt = 0;
                }
                if (tmp_cnt < numeric_limits<unsigned int>::max()) // the maximum of TokenLen
                    tmp_cnt++;
            }
        }
        return RepTime(time_st);
    }

    void greedy_search(PartitionHasher &part_hasher, const int &tid, const unsigned int rid, const vector<unsigned int> &p_keys, const vector<unsigned int> &od_keys, const vector<unsigned int> &odk_st, vector<unsigned int> &candidates_saver) {
        auto mem_st = LogTime();

        auto & odkeys_st = part_hasher.odkeys_st;
        auto & onedelete_keys = part_hasher.onedelete_keys;

        auto const indexPartNum = p_keys.size();
        unsigned int const len = od_keys.size();

        vector<unsigned int> candidates;
        // Prepare thread-local storage for various data structures
        auto &invPtr = invPtrArr[tid];
        auto &intPtr = intPtrArr[tid];
        auto &onePtr = onePtrArr[tid];
        auto &values = valuesArr[tid];
        auto &scores = scoresArr[tid];
        auto negRef = negRef2D[tid];
        auto quickRef = quickRef2D[tid];

        // Initialize onePtr and reserve space for each part
        for (unsigned int pid = 0; pid < indexPartNum; ++pid) {
            onePtr[pid].clear();
            assert(odk_st[pid + 1] >= odk_st[pid]);             // todo: delete it
            onePtr[pid].reserve(odk_st[pid + 1] - odk_st[pid]); // seems: useless whynot reserve before
        }

        mem_cost[tid] += RepTime(mem_st);

        // Iterate each part find each part if there is identical part already exits in inverted list
        // Based on the above result, initialize the value and scores
        auto find_st = LogTime();
        for (unsigned int pid = 0; pid < indexPartNum; pid++) {
            // A hash
            int v1 = 0;
            const auto &pkey = p_keys[pid];
            const auto &cur_hvs = parts_index_hv[pid];
            auto invit = lower_bound(cur_hvs.begin(), cur_hvs.end(), pkey);
            if (invit != cur_hvs.end() && *invit == pkey) {
                auto dis = invit - cur_hvs.begin();
                invPtr[pid] = dis;
                v1 = -parts_index_cnt[pid][dis];
            } else {
                invPtr[pid] = UINT_MAX;
            }
            values[pid] = make_pair(v1, pid);
            scores[pid] = 0;
        }
        find_cost[tid] += RepTime(find_st);

        auto alloc_st = LogTime();
        // Prepare heap for greedy selection
        unsigned int heap_cnt = indexPartNum;
        make_heap(values.begin(), values.begin() + heap_cnt);

        // Initialize some variables for the greedy selection
        int cost = 0;
        unsigned int rLen = min(max_len, len);
        unsigned int maxH = floor(coe * (len + rLen) + EPS);

        maxH = min(maxH, (unsigned int)values.size() - 1);
        
        // We need use greedy selection in maxH + 1 times
        for (unsigned int i = 0; i < maxH + 1; ++i) {
            auto sel = values.front();
            pop_heap(values.begin(), values.begin() + heap_cnt);
            auto pid = sel.second;
            auto const pid_pos = (int)odk_st[pid];
            ++scores[pid];
            cost -= sel.first;
            auto const &pkey = p_keys[pid];

            if (scores[sel.second] == 1) {
                if (invPtr[pid] != UINT_MAX) {
                    // Iterate the candidates that shares the same partition
                    auto ofs_st = parts_index_offset[pid][invPtr[pid]];
                    auto ofs_ed = ofs_st + parts_index_cnt[pid][invPtr[pid]];
                    for (auto ofs = ofs_st; ofs < ofs_ed; ofs++) {
                        // auto const &hrp = parts_arr[ofs];
                        auto const tmp_rid = parts_rids[pid][ofs];
                        if (tmp_rid >= rid) break;
                        int tmp_pos = odkeys_st[tmp_rid][pid];
                        unsigned int rLen = onedelete_keys[tmp_rid].size();
                        int Ha = floor((len - det * rLen) / (1 + det) + EPS);
                        int Hb = floor((rLen - det * len) / (1 + det) + EPS);
                        unsigned int H = Ha + Hb; // maximum allowable difference
                        // If the current iteration index i is greater than this maximum allowable difference H
                        // current entry in the inverted list is skipped,
                        if (i > H) continue;
                        // position filter
                        if (negRef[tmp_rid] == false && quickRef[tmp_rid] == false)
                            candidates.push_back(tmp_rid);

                        // We need to let them be int in case of negative minus result
                        if (pid_pos - tmp_pos > Ha || tmp_pos - pid_pos > Hb)
                            negRef[tmp_rid] = true;
                        else
                            quickRef[tmp_rid] = true;
                    }
                }
                // maintain heap
                // Here is to find the next pair that insert to the heap
                // Basically it is to consider the situation of the 1-deletion of the current part
                int v2 = 0;

                // search if the 1-deletion
                intPtr[pid] = UINT_MAX;

                auto const &id_st = odk_st[pid];
                auto const &id_ed = odk_st[pid + 1];
                for (auto id = id_st; id < id_ed; id++) {
                    auto const &odk = od_keys[id];
                    const auto &cur_hvs = parts_index_hv[pid];
                    auto invit = lower_bound(cur_hvs.begin(), cur_hvs.end(), odk);
                    if (invit != cur_hvs.end() && *invit == odk) {
                        auto dis = invit - cur_hvs.begin();
                        onePtr[pid].push_back(dis);
                        v2 -= parts_index_cnt[pid][dis];
                    } else {
                        onePtr[pid].push_back(UINT_MAX);
                    }
                }

                values[heap_cnt - 1].first = v2;
                push_heap(values.begin(), values.begin() + heap_cnt);
            }else {
                auto ov_st = LogTime();
                // add candidates

                for (int id = 0; id < (int)onePtr[pid].size(); ++id) {
                    if (onePtr[pid][id] != UINT_MAX) {
                        auto ofs_st = parts_index_offset[pid][onePtr[pid][id]];
                        auto ofs_ed = ofs_st + parts_index_cnt[pid][onePtr[pid][id]];

                        for (auto ofs = ofs_st; ofs < ofs_ed; ofs++) {
                            // auto const &hrp = parts_arr[ofs];
                            auto const tmp_rid = parts_rids[pid][ofs];
                            if (tmp_rid >= rid) break;
                            int tmp_pos = odkeys_st[tmp_rid][pid];

                            unsigned int rLen = onedelete_keys[tmp_rid].size();
                            int Ha = floor((len - det * rLen) / (1 + det) + EPS);
                            int Hb = floor((rLen - det * len) / (1 + det) + EPS);
                            unsigned int H = Ha + Hb; // maximum allowable difference
                            // If the current iteration index i is greater than this maximum allowable difference H
                            // current entry in the inverted list is skipped,
                            if (i > H) continue;
                            // position filter
                            if (negRef[tmp_rid] == false && quickRef[tmp_rid] == false)
                                candidates.push_back(tmp_rid);

                            // We need to let them be int in case of negative minus result
                            // auto const hrp_pos = (int)hrp.pos;
                            if (pid_pos - tmp_pos > Ha || tmp_pos - pid_pos > Hb)
                                negRef[tmp_rid] = true;
                            else
                                quickRef[tmp_rid] = true;
                        }
                    }
                } 
            }
            // maintain heap
            --heap_cnt;
        }
        alloc_cost[tid] += RepTime(alloc_st);

        // Clear candidates and update global results
        for (unsigned int idx = 0; idx < candidates.size(); idx++) {
            if (negRef[candidates[idx]] == false && quickRef[candidates[idx]] == true) {
                candidates_saver.emplace_back(candidates[idx]);
                // candidateNum++;
            }
            quickRef[candidates[idx]] = false;
            negRef[candidates[idx]] = false;
        }
        candidates.clear();
    }
};

#endif
