#ifndef _PARTITIONHASHINDEX_H_
#define _PARTITIONHASHINDEX_H_

#include <vector>
#include <set>
#include <iostream>
#include <execution>
#include <tbb/tbb.h>
#include <omp.h>
#include <algorithm> // For sort, iota
#include <limits>    // For numeric_limits
#include "PartitionHasher.h"
#include "../../util/util.h"
#define EMH_EXT
#include "../../../emhash/hash_table7.hpp"

using namespace std;

typedef emhash7::HashMap<unsigned int, pair<unsigned int, unsigned int>> hash_pair; 

class PartitionIndex {
private:
    bool **quickRef2D; // 2D quick reference array
    bool **negRef2D;   // 2D negative reference array

public:
    double mem_cost[MAXTHREADNUM] = {0};
    double find_cost[MAXTHREADNUM] = {0};
    double alloc_cost[MAXTHREADNUM] = {0};

    // Index-related containers
    vector<hash_pair> part_index;
    vector<hash_pair> odkey_index;
    vector<vector<unsigned int>> parts_rids;
    vector<vector<unsigned int>> ods_rids;

    // Search-related vectors that needed when allocate by greedy heap method
    vector<pair<unsigned int,unsigned int>> invPtrArr[MAXTHREADNUM];
    vector<pair<unsigned int,unsigned int>> intPtrArr[MAXTHREADNUM];
    vector<vector<pair<unsigned int,unsigned int>>> onePtrArr[MAXTHREADNUM];
    set < pair<int, unsigned int>> valuesArr[MAXTHREADNUM]; // <value, loc>
    vector < unsigned int > scoresArr[MAXTHREADNUM];
    unsigned int max_len;       // the maximum length in the current group

    double det;
    double coe;
    

    PartitionIndex(double delta, double _coe): det(delta), coe(_coe){}

    ~PartitionIndex(){
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
        part_index.resize(partNum);
        odkey_index.resize(partNum);
        // Allocate memory for those in the searching
        for (auto tid = 0; tid < MAXTHREADNUM; tid++) {
            invPtrArr[tid].resize(partNum);
            intPtrArr[tid].resize(partNum);
            onePtrArr[tid].resize(partNum);
            // valuesArr[tid].resize(partNum);
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

        // Build Index for partitions
#pragma omp parallel for
        for (auto pid = 0; pid < partNum; pid++) {
            unsigned int len = parts_keys.size();
            unsigned int one_deletion_amount = 0;

            // First Index Partitions
            auto &cur_rids = parts_rids[pid];
            cur_rids.resize(len);
            auto & cur_pindex = part_index[pid];
            cur_pindex.reserve(cur_rids.size());
            
            // Insert each partition hash value into the emhash
            for (unsigned int j = 0; j < len; j++) {
                cur_pindex[parts_keys[j][pid]].first++;
                one_deletion_amount += odkeys_st[j][pid + 1] - odkeys_st[j][pid];
            }

            // Iterate the count and then records the start pos in the cur_rids
            unsigned int cnt = 0;
            for (auto &it : cur_pindex) {
                unsigned int pre_cnt = cnt;
                cnt += it.second.first;
                it.second.first = pre_cnt;
            }

            // Write them into the cur_rids
            for (unsigned int j = 0; j < len; j++) {
                auto & v = cur_pindex[parts_keys[j][pid]];
                auto & st = v.first;
                auto & v_len = v.second;
                cur_rids[ st + v_len ] = j;
                v_len++;
            }

            // Index for one-deletion keys
            if (one_deletion_amount == 0) // it means that in current range group, current partition, all of them are empty, we don't have elements partitioned in this partition
                continue;

            auto &cur_ods_rids = ods_rids[pid];
            cur_ods_rids.resize(one_deletion_amount);
            auto & cur_odindex = odkey_index[pid];
            cur_odindex.reserve(one_deletion_amount);

            // Insert each onedeletion hash value into the emhash
            for (unsigned int rid = 0; rid < len; rid++) {
                unsigned int const od_loc_st = odkeys_st[rid][pid];
                unsigned int const od_loc_ed = odkeys_st[rid][pid + 1];
                for (unsigned int od_loc = od_loc_st; od_loc < od_loc_ed; od_loc++) {
                    cur_odindex[onedelete_keys[rid][od_loc]].first++;
                }
            }
            
            // Iterate the count and then records the start pos in the cur_rids
            cnt = 0;
            for (auto &it : cur_odindex) {
                unsigned int pre_cnt = cnt;
                cnt += it.second.first;
                it.second.first = pre_cnt;
            }

            // Write them into the cur_ods_rids
            for (unsigned int rid = 0; rid < len; rid++) {
                unsigned int const od_loc_st = odkeys_st[rid][pid];
                unsigned int const od_loc_ed = odkeys_st[rid][pid + 1];
                for (unsigned int od_loc = od_loc_st; od_loc < od_loc_ed; od_loc++) {
                    auto & v = cur_odindex[onedelete_keys[rid][od_loc]];
                    auto & st = v.first;
                    auto & v_len = v.second;
                    cur_ods_rids[ st + v_len ] = rid;
                    v_len++;
                }
            }
        }

        return RepTime(time_st);
    }

    double greedy_search(PartitionHasher &part_hasher, const int &tid, const unsigned int rid, const vector<unsigned int> &p_keys, const vector<unsigned int> &od_keys, const vector<unsigned int> &odk_st, vector<unsigned int> &candidates_saver) {
        auto mem_st = LogTime();
        double lowerbound_cost = 0;

        auto & odkeys_st = part_hasher.odkeys_st;
        auto & onedelete_keys = part_hasher.onedelete_keys;

        auto const indexPartNum = p_keys.size();
        unsigned int const len = od_keys.size();

        // unsigned int min_subrecLen = max((len / indexPartNum) + 3,(long unsigned int) 0 );

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
            // assert(odk_st[pid + 1] >= odk_st[pid]);             // todo: delete it
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
            auto &cur_hvs = part_index[pid];
            auto* pvalue = cur_hvs.try_get(pkey); //return nullptr if key is not exist
            // auto invit = cur_hvs[pkey] lower_bound(cur_hvs.begin(), cur_hvs.end(), pkey);

            if (pvalue != nullptr) {
                // auto dis = invit - cur_hvs.begin();
                // invPtr[pid] = dis;
                // v1 = -parts_index_cnt[pid][dis];
                invPtr[pid] = *pvalue;
                v1 = - (pvalue->second);
            } else {
                invPtr[pid].first = UINT_MAX;
            }
            // values[pid] = make_pair(v1, pid);
            values.insert(make_pair(v1, pid));
            scores[pid] = 0;
        }
        find_cost[tid] += RepTime(find_st);

        auto alloc_st = LogTime();
        // Prepare heap for greedy selection
        unsigned int heap_cnt = indexPartNum;
        // make_heap(values.begin(), values.begin() + heap_cnt);

        // Initialize some variables for the greedy selection
        int cost = 0;
        unsigned int rLen = min(max_len, len);
        unsigned int maxH = floor(coe * (len + rLen) + EPS);
        
        // We need use greedy selection in maxH + 1 times
        for (unsigned int i = 0; i < maxH + 1; ++i) {
            // auto sel = values.front();
            // pop_heap(values.begin(), values.begin() + heap_cnt);

            auto sel = *values.rbegin();
             // Delete the maximum value
            values.erase(std::prev(values.end()));

            auto pid = sel.second;
            auto const pid_pos = (int)odk_st[pid];
            ++scores[pid];
            cost -= sel.first;
            auto const &pkey = p_keys[pid];

            if (scores[sel.second] == 1) {
                if (invPtr[pid].first != UINT_MAX) {
                    // Iterate the candidates that shares the same partition
                    auto ofs_st = invPtr[pid].first;
                    auto ofs_ed = invPtr[pid].first + invPtr[pid].second;
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

                // if the current 1-delete neighbor not exist
                if(odk_st[pid + 1] == odk_st[pid]){
                    // heap_cnt--;
                    continue;
                }

                // the values set is too big we do not need so much
                while(values.size() > (maxH - i)){
                    values.erase(values.begin());
                }

                auto minValue_pid = (values.begin())->second;
                auto last_cand_len = odk_st[minValue_pid + 1] - odk_st[minValue_pid];
                if((maxH - i) <= values.size() && odk_st[pid + 1] - odk_st[pid] - 2 <= last_cand_len){
                    // heap_cnt--;
                    continue;
                }
                    

                int v2 = 0;

                // search if the 1-deletion
                const auto &cur_od_hvs = odkey_index[pid];
                // auto oneit = lower_bound(cur_od_hvs.begin(), cur_od_hvs.end(), pkey);
                auto * oneit = cur_od_hvs.try_get(pkey);
                // auto oneit = oneIndex.find(PACK(lenPart, hashValues[pid]));
                if (oneit != nullptr) {
                    // auto dis = oneit - cur_od_hvs.begin();
                    // intPtr[pid] = oneit - cur_od_hvs.begin();
                    // v2 -= od_index_cnt[pid][dis];
                    intPtr[pid] = * oneit;
                    v2 -= oneit->second;
                } else {
                    intPtr[pid].first = UINT_MAX;
                }

                auto const &id_st = odk_st[pid];
                auto const &id_ed = odk_st[pid + 1];
                for (auto id = id_st; id < id_ed; id++) {
                    auto const &odk = od_keys[id];
                    const auto &cur_hvs = part_index[pid];
                    auto invit = cur_hvs.try_get(odk);
                    // auto invit = lower_bound(cur_hvs.begin(), cur_hvs.end(), odk);
                    if (invit != nullptr) {
                        onePtr[pid].emplace_back(invit->first, invit->second);
                        v2 -= invit->second;
                    } else {
                        onePtr[pid].emplace_back(UINT_MAX, 0);
                    }
                }

                // values[heap_cnt - 1].first = v2;
                // push_heap(values.begin(), values.begin() + heap_cnt);
                values.insert(make_pair(v2, pid));
            } else {
                auto ov_st = LogTime();
                // add candidates
                if (intPtr[pid].first != UINT_MAX) {
                    auto ofs_st = intPtr[pid].first;
                    auto ofs_ed = intPtr[pid].first + intPtr[pid].second;
                    for (auto ofs = ofs_st; ofs < ofs_ed; ofs++) {
                        auto const tmp_rid = ods_rids[pid][ofs];
                        if (tmp_rid >= rid) break;
                        int tmp_pos = odkeys_st[tmp_rid][pid];
                        // Attention here is the ods_arr
                        // auto const &hrp = ods_arr[ofs];
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

                for (int id = 0; id < (int)onePtr[pid].size(); ++id) {
                    if (onePtr[pid][id].first != UINT_MAX) {
                        auto ofs_st = onePtr[pid][id].first;
                        auto ofs_ed = onePtr[pid][id].first + onePtr[pid][id].second;

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
                // overlap_cost += RepTime(ov_st);
                // maintain heap
                // --heap_cnt;
            }
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

        return lowerbound_cost;
    }
};

#endif
