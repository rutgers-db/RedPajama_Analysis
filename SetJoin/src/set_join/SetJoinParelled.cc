#include "SetJoinParelled.h"
#include <omp.h>
#include <limits.h>
// #include <tbb/parallel_sort.h>
#include "../util/util.h"

// the precalculated hashvalue key for the partitions and ondeletions
vector<vector<unsigned int>> parts_keys;
vector<vector<unsigned int>> onedelete_keys;
vector<vector<TokenLen>> odkeys_st; // Stores position of one deletion information

// index for partitions and one deletions
// There are two dimensions:
// 1st: (Pointer Level) Indicates the range
// 2nd: (External Vector Level) Indicates the pid (partition ID)
// Inner vector stores the vector of rid (record ID)
vector<vector<unsigned int>> *parts_rids; // <rid>
vector<vector<unsigned int>> *ods_rids;   // <rid>

// The index pointers
vector<vector<unsigned int>> *parts_index_hv;
vector<vector<unsigned int>> *od_index_hv;
vector<vector<unsigned int>> *parts_index_offset;
vector<vector<unsigned int>> *od_index_offset;
// Mention: if the cnt is greater than the limit of TokenLen, we will just treat it as the maximum value of TokenLen
vector<vector<TokenLen>> *parts_index_cnt;
vector<vector<TokenLen>> *od_index_cnt;

// Function to check if two sets overlap based on a certain condition
bool SetJoinParelled::overlap(unsigned int x, unsigned int y, int posx, int posy, int current_overlap) {
#if VERSION == 2
    // if we are doing bottomk
    return bottomKJaccard(dataset[x], dataset[y], det);
#endif

    // Calculate required overlap based on a formula
    int require_overlap = ceil(det / (1 + det) * (int)(dataset[x].size() + dataset[y].size()) - EPS);

    // Loop through both sets to find overlap
    while (posx < (int)dataset[x].size() && posy < (int)dataset[y].size()) {
        // Check if remaining elements are sufficient for required overlap
        if ((int)dataset[x].size() - posx + current_overlap < require_overlap || (int)dataset[y].size() - posy + current_overlap < require_overlap) return false;

        if (dataset[x][posx] == dataset[y][posy]) {
            current_overlap++;
            posx++;
            posy++;
        } else if (dataset[x][posx] < dataset[y][posy]) {
            posx++;
        } else {
            posy++;
        }
    }
    return current_overlap >= require_overlap;
}

// Function to create the index of Partitions and one-deletion neighbors
void SetJoinParelled::index(double threshold) {
    // Initilaize the Parameters
    det = threshold;
    ALPHA = 1.0 / threshold + 0.01;
    coe = (1 - det) / (1 + det);
    n = dataset.size();
    maxSize = dataset.back().size();
    maxIndexPartNum = floor(2 * coe * maxSize + EPS) + 1; // the possible maximum part amount of the maximum record

    showPara(); // show parameters

    // Know how many tokens there
    unsigned int tokenNum = 0;
    for (int i = 0; i < dataset.size(); i++) {
        if (tokenNum < dataset[i].back()) tokenNum = dataset[i].back();
    }

    printf("The tokenNum is %u \n", tokenNum);

    // Initialize prime exponential array for hashing
    prime_exp = new int[n + 1];
    prime_exp[0] = 1;
    for (unsigned int i = 1; i <= n; ++i)
        prime_exp[i] = prime_exp[i - 1] * PRIME;

    // Initialize quick reference arrays
    quickRef2D = new bool *[MAXTHREADNUM];
    negRef2D = new bool *[MAXTHREADNUM];
    for (unsigned int i = 0; i < MAXTHREADNUM; ++i) {
        quickRef2D[i] = new bool[n];
        memset(quickRef2D[i], 0, sizeof(bool) * n);
        negRef2D[i] = new bool[n];
        memset(negRef2D[i], 0, sizeof(bool) * n);
    }

    // Get the ranges(The group based on the record's size)
    // We group those size similar ones together
    range_id.resize(n);
    unsigned int low = 0, high = 0;
    for (unsigned int rid = 0; rid < n; rid++) {
        unsigned int len = dataset[rid].size();
        if (len > high) {
            low = len;
            high = low * ALPHA;
            range.push_back(make_pair(low, high));
            range_st.push_back(rid);
        }
        range_id[rid] = range.size() - 1;
    }
    range_st.push_back(n);

    // Initialize subquery and hash value vectors
    vector<vector<TokenLen>> subquery[MAXTHREADNUM]; // first thread id second tokens
    for (unsigned int i = 0; i < MAXTHREADNUM; i++) {
        subquery[i].resize(maxIndexPartNum);
    }

    // To store the keys of parts and the one deletion neighbors
    parts_keys.resize(n);
    onedelete_keys.resize(n);
    odkeys_st.resize(n);

    // Calculate hash values for partitions and one-deletion neighbors
    auto calHash_st = LogTime();
// Parallel loop to calculate hash values
#pragma omp parallel for
    for (unsigned int rid = 0; rid < n; rid++) {
        unsigned int len = dataset[rid].size();

        const unsigned int cur_low = range[range_id[rid]].first;
        const unsigned int partNum = floor(2 * coe * cur_low + EPS) + 1;

        unsigned int tid = omp_get_thread_num();
        subquery[tid].clear();
        parts_keys[rid].resize(partNum);
        subquery[tid].resize(partNum);

        // Attention: If the parition is empty, then its hashvalue is 0
        for (auto &token : dataset[rid]) {
            unsigned int pid = token % partNum;
            auto &subrec = subquery[tid][pid];
            subrec.push_back(token);

            // get the keys of partitions (or get the hash values)
            parts_keys[rid][pid] = PRIME * parts_keys[rid][pid] + token + 1;
        }

        // for getting the keys
        onedelete_keys[rid].resize(len);
        odkeys_st[rid].resize(partNum + 1);
        TokenLen tmp_cnt = 0;
        for (unsigned int pid = 0; pid < partNum; ++pid) {
            int64_t lenPart = cur_low + pid * (maxSize + 1);
            unsigned int mhv = 0, hv = parts_keys[rid][pid];
            auto &subrec = subquery[tid][pid];

            // we store the keys into one array for each record.
            // so we need to know the boundary of keys for onedeletions of each partition
            odkeys_st[rid][pid] = tmp_cnt;

            // Maybe the subrec.size() is 0 cause some partitions are empty
            for (unsigned int idx = 0; idx < subrec.size(); idx++) {
                int chv = hv + mhv * prime_exp[subrec.size() - 1 - idx];
                mhv = mhv * PRIME + subrec[idx] + 1;
                chv -= mhv * prime_exp[subrec.size() - 1 - idx];

                // get the keys of one-deletion neighbors from the current partition
                // auto odkey = PACK(lenPart, chv);
                onedelete_keys[rid][tmp_cnt++] = chv;
            }
            // pos += subrec.size();
        }
        odkeys_st[rid][partNum] = tmp_cnt;
    }

    index_cost = RepTime(calHash_st);
    cout << "Calculating Hash Values of Partitions and OneDeleteion Neighbors Time Cost: " << index_cost << endl;
    cout << "Now Let's building the inverted Index of these Paritions" << endl;

    auto sort_st = LogTime();
    printf("There are %lu range groups in the setJoin\n", range.size());

    // Initialize index arrays
    // Actually you can wrap them into a self-defined class or struct
    parts_rids = new vector<vector<unsigned int>>[range.size()];
    ods_rids = new vector<vector<unsigned int>>[range.size()];
    parts_index_hv = new vector<vector<unsigned int>>[range.size()];
    od_index_hv = new vector<vector<unsigned int>>[range.size()];
    parts_index_offset = new vector<vector<unsigned int>>[range.size()];
    od_index_offset = new vector<vector<unsigned int>>[range.size()];
    parts_index_cnt = new vector<vector<TokenLen>>[range.size()];
    od_index_cnt = new vector<vector<TokenLen>>[range.size()];

    // Building Inverted Index for the hashed partitions and one-deletion neghbors
#pragma omp parallel for
    for (unsigned int i = 0; i < range.size(); i++) {
        auto const &rid_st = range_st[i];
        auto const &rid_ed = range_st[i + 1];
        const TokenLen partNum = parts_keys[rid_st].size();

        // Allocate Memory for index
        parts_rids[i].resize(partNum);
        ods_rids[i].resize(partNum);
        parts_index_hv[i].resize(partNum);
        od_index_hv[i].resize(partNum);
        parts_index_offset[i].resize(partNum);
        parts_index_cnt[i].resize(partNum);
        od_index_offset[i].resize(partNum);
        od_index_cnt[i].resize(partNum);

        // Build Index
        for (TokenLen pid = 0; pid < partNum; pid++) {
            unsigned int parts_amount = rid_ed - rid_st;
            unsigned int one_deletion_amount = 0;
            // First Index Partitions

            auto &cur_rids = parts_rids[i][pid];
            cur_rids.reserve(parts_amount);

            for (unsigned int rid = rid_st; rid < rid_ed; rid++) {
                cur_rids.emplace_back(rid);
                one_deletion_amount += odkeys_st[rid][pid + 1] - odkeys_st[rid][pid];
            }

            // sort the parts_rid[i][pid]
            sort(cur_rids.begin(), cur_rids.end(), [&pid](const unsigned int &rid_1, const unsigned int &rid_2) {
                if (parts_keys[rid_1][pid] == parts_keys[rid_2][pid])
                    return rid_1 < rid_2;
                return parts_keys[rid_1][pid] < parts_keys[rid_2][pid];
            });

            // Build the index pointers for the cur_rids
            unsigned int prev_hv = parts_keys[cur_rids[0]][pid];
            TokenLen tmp_cnt = 1;
            unsigned int ofs = 0;
            for (unsigned int j = 1; j < cur_rids.size(); j++) {
                auto &cur_hv = parts_keys[cur_rids[j]][pid];
                if (cur_hv != prev_hv) {
                    parts_index_hv[i][pid].emplace_back(prev_hv);
                    parts_index_offset[i][pid].emplace_back(ofs);
                    parts_index_cnt[i][pid].emplace_back(tmp_cnt);
                    prev_hv = cur_hv;
                    ofs = j;
                    tmp_cnt = 0;
                }
                if (tmp_cnt < numeric_limits<TokenLen>::max()) // the maximum of TokenLen
                    tmp_cnt++;
            }
            // We need to emplace_back more time out of the loop
            // because the last hash value do not have next hash value to trigger "cur_hv != prev_hv"
            parts_index_hv[i][pid].emplace_back(prev_hv);
            parts_index_offset[i][pid].emplace_back(ofs);
            parts_index_cnt[i][pid].emplace_back(tmp_cnt);

            // Now build the index for od key
            if (one_deletion_amount == 0) // it means that in current range group, current partition, all of them are empty, we don't have elements partitioned in this partition
                continue;

            auto &cur_ods_rids = ods_rids[i][pid];
            cur_ods_rids.resize(one_deletion_amount);

            iota(cur_ods_rids.begin(), cur_ods_rids.end(), 0); // cur_rids temporarily filled with 0 to one_deletion_amount-1

            // temporary vector for the sorting the array
            vector<unsigned int> tmp_rid(one_deletion_amount);
            tmp_rid.reserve(one_deletion_amount);
            vector<TokenLen> tmp_od_locs;
            tmp_od_locs.reserve(one_deletion_amount);
            for (int rid = rid_st; rid < rid_ed; rid++) {
                auto const &od_loc_st = odkeys_st[rid][pid];
                auto const &od_loc_ed = odkeys_st[rid][pid + 1];
                for (auto od_loc = od_loc_st; od_loc < od_loc_ed; od_loc++) {
                    tmp_rid.emplace_back(rid);
                    tmp_od_locs.emplace_back(od_loc);
                }
            }

            // sort the parts_rid[i][pid]
            sort(cur_ods_rids.begin(), cur_ods_rids.end(), [&tmp_rid, &tmp_od_locs, &pid](const unsigned int &id1, const unsigned int &id2) {
                auto const &rid_1 = tmp_rid[id1];
                auto const &rid_2 = tmp_rid[id2];
                auto const &od_loc_1 = tmp_od_locs[id1];
                auto const &od_loc_2 = tmp_od_locs[id2];
                if (onedelete_keys[rid_1][od_loc_1] == onedelete_keys[rid_2][od_loc_2])
                    return rid_1 < rid_2;
                return onedelete_keys[rid_1][od_loc_1] < onedelete_keys[rid_2][od_loc_2];
            });

            // Build the index pointers for the cur_rids
            auto const &rid = tmp_rid[cur_ods_rids[0]];
            auto const &od_loc = tmp_od_locs[cur_ods_rids[0]];
            prev_hv = onedelete_keys[rid][od_loc];
            tmp_cnt = 1;
            ofs = 0;
            for (unsigned int j = 1; j < cur_ods_rids.size(); j++) {
                auto const &_rid = tmp_rid[cur_ods_rids[j]];
                auto const &_od_loc = tmp_od_locs[cur_ods_rids[j]];
                auto const &cur_hv = onedelete_keys[_rid][_od_loc];
                if (cur_hv != prev_hv) {
                    od_index_hv[i][pid].emplace_back(prev_hv);
                    od_index_offset[i][pid].emplace_back(ofs);
                    od_index_cnt[i][pid].emplace_back(tmp_cnt);
                    prev_hv = cur_hv;
                    ofs = j;
                    tmp_cnt = 0;
                }
                if (tmp_cnt < numeric_limits<TokenLen>::max()) // the maximum of TokenLen
                    tmp_cnt++;
            }
            od_index_hv[i][pid].emplace_back(prev_hv);
            od_index_offset[i][pid].emplace_back(ofs);
            od_index_cnt[i][pid].emplace_back(tmp_cnt);

            // convert the id to rid
            for (unsigned int j = 0; j < cur_ods_rids.size(); j++) {
                cur_ods_rids[j] = tmp_rid[cur_ods_rids[j]];
            }
        }
    }
    index_cost += RepTime(sort_st);
    cout << "Sorting them And Partition Time Cost: " << RepTime(sort_st) << endl;

    // cout << "Indexing Partitions and OneDeleteion Neighbors Time Cost: " << RepTime(index_st) << endl;

    print_memory();
}

void SetJoinParelled::GreedyFindCandidateAndSimPairs(const int &tid, const int indexLenGrp, const unsigned int rid, const vector<unsigned int> &p_keys, const vector<unsigned int> &od_keys, const vector<TokenLen> &odk_st) {
    auto mem_st = LogTime();

    auto const indexPartNum = p_keys.size();
    assert(indexPartNum == odk_st.size() - 1);
    unsigned int const len = dataset[rid].size();
    auto const indexLen = range[indexLenGrp].first;

    // Prepare thread-local storage for various data structures
    vector<unsigned int> candidates;
    auto &invPtr = invPtrArr[tid];
    auto &intPtr = intPtrArr[tid];
    auto &onePtr = onePtrArr[tid];
    auto &values = valuesArr[tid];
    auto &scores = scoresArr[tid];
    invPtr.resize(indexPartNum);
    intPtr.resize(indexPartNum);
    onePtr.resize(indexPartNum);
    values.resize(indexPartNum);
    scores.resize(indexPartNum);
    auto negRef = negRef2D[tid];
    auto quickRef = quickRef2D[tid];

    // Initialize onePtr and reserve space for each part
    for (unsigned int pid = 0; pid < indexPartNum; ++pid) {
        onePtr[pid].clear();
        assert(odk_st[pid + 1] >= odk_st[pid]);
        onePtr[pid].reserve(odk_st[pid + 1] - odk_st[pid]);
    }

    mem_cost[tid] += RepTime(mem_st);

    // Iterate each part find each part if there is identical part already exits in inverted list
    // Based on the above result, initialize the value and scores
    auto find_st = LogTime();
    for (TokenLen pid = 0; pid < indexPartNum; pid++) {
        // A hash
        // int64_t lenPart = indexLen + pid * (maxSize + 1);
        int v1 = 0;
        const auto &pkey = p_keys[pid];
        const auto &cur_hvs = parts_index_hv[indexLenGrp][pid];
        auto invit = lower_bound(cur_hvs.begin(), cur_hvs.end(), pkey);
        // auto invit = lower_bound(parts_index[indexLenGrp].begin(), parts_index[indexLenGrp].end(), HashOfsCnt(pkey, 0, 0));
        if (invit != cur_hvs.end() && *invit == pkey) {
            auto dis = invit - cur_hvs.begin();
            invPtr[pid] = dis;
            v1 = -parts_index_cnt[indexLenGrp][pid][dis];
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
    unsigned int rLen = min(range[indexLenGrp].second, len);
    unsigned int Ha = floor((len - det * rLen) / (1 + det) + EPS);
    unsigned int Hb = floor((rLen - det * len) / (1 + det) + EPS);
    unsigned int maxH = Ha + Hb;
    maxH = floor(coe * (len + rLen) + EPS);

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
                auto ofs_st = parts_index_offset[indexLenGrp][pid][invPtr[pid]];
                auto ofs_ed = ofs_st + parts_index_cnt[indexLenGrp][pid][invPtr[pid]];
                for (auto ofs = ofs_st; ofs < ofs_ed; ofs++) {
                    // auto const &hrp = parts_arr[indexLenGrp][ofs];
                    auto const tmp_rid = parts_rids[indexLenGrp][pid][ofs];
                    if (tmp_rid >= rid) break;
                    int tmp_pos = odkeys_st[tmp_rid][pid];
                    unsigned int rLen = dataset[tmp_rid].size();
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
            const auto &cur_od_hvs = od_index_hv[indexLenGrp][pid];
            auto oneit = lower_bound(cur_od_hvs.begin(), cur_od_hvs.end(), pkey);
            // auto oneit = oneIndex.find(PACK(lenPart, hashValues[pid]));
            if (oneit != cur_od_hvs.end() && *oneit == pkey) {
                auto dis = oneit - cur_od_hvs.begin();
                intPtr[pid] = oneit - cur_od_hvs.begin();
                v2 -= od_index_cnt[indexLenGrp][pid][dis];
            } else {
                intPtr[pid] = UINT_MAX;
            }

            auto const &id_st = odk_st[pid];
            auto const &id_ed = odk_st[pid + 1];
            for (auto id = id_st; id < id_ed; id++) {
                auto const &odk = od_keys[id];
                const auto &cur_hvs = parts_index_hv[indexLenGrp][pid];
                auto invit = lower_bound(cur_hvs.begin(), cur_hvs.end(), odk);
                if (invit != cur_hvs.end() && *invit == odk) {
                    auto dis = invit - cur_hvs.begin();
                    onePtr[pid].push_back(dis);
                    v2 -= parts_index_cnt[indexLenGrp][pid][dis];
                } else {
                    onePtr[pid].push_back(UINT_MAX);
                }
            }

            values[heap_cnt - 1].first = v2;
            push_heap(values.begin(), values.begin() + heap_cnt);
        } else {
            auto ov_st = LogTime();
            // add candidates
            if (intPtr[pid] != UINT_MAX) {
                auto ofs_st = od_index_offset[indexLenGrp][pid][intPtr[pid]];
                auto ofs_ed = ofs_st + od_index_cnt[indexLenGrp][pid][intPtr[pid]];
                // auto ofs_st = intPtr[pid].ofs;
                // auto ofs_ed = intPtr[pid].ofs + intPtr[pid].cnt;
                for (auto ofs = ofs_st; ofs < ofs_ed; ofs++) {
                    auto const tmp_rid = ods_rids[indexLenGrp][pid][ofs];
                    if (tmp_rid >= rid) break;
                    int tmp_pos = odkeys_st[tmp_rid][pid];
                    // Attention here is the ods_arr
                    // auto const &hrp = ods_arr[indexLenGrp][ofs];
                    unsigned int rLen = dataset[tmp_rid].size();
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
                if (onePtr[pid][id] != UINT_MAX) {
                    auto ofs_st = parts_index_offset[indexLenGrp][pid][onePtr[pid][id]];
                    auto ofs_ed = ofs_st + parts_index_cnt[indexLenGrp][pid][onePtr[pid][id]];
                    // auto ofs_st = onePtr[pid][id].ofs;
                    // auto ofs_ed = onePtr[pid][id].ofs + onePtr[pid][id].cnt;
                    for (auto ofs = ofs_st; ofs < ofs_ed; ofs++) {
                        // auto const &hrp = parts_arr[indexLenGrp][ofs];
                        auto const tmp_rid = parts_rids[indexLenGrp][pid][ofs];
                        if (tmp_rid >= rid) break;
                        int tmp_pos = odkeys_st[tmp_rid][pid];

                        unsigned int rLen = dataset[tmp_rid].size();
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
            --heap_cnt;
        }
    }
    alloc_cost[tid] += RepTime(alloc_st);

    auto verif_st = LogTime();
    // Clear candidates and update global results
    for (unsigned int idx = 0; idx < candidates.size(); idx++) {
        if (negRef[candidates[idx]] == false && quickRef[candidates[idx]] == true) {
            if (overlap(candidates[idx], rid) == true) {
                // resultNum++;
                result_pairs[tid].emplace_back(rid, candidates[idx]);
            }
            // candidateNum++;
        }
        quickRef[candidates[idx]] = false;
        negRef[candidates[idx]] = false;
    }
    candidates.clear();
    verif_cost[tid] += RepTime(verif_st);
}

void SetJoinParelled::findSimPairs() {
    auto find_st = LogTime();
    cout << "Start finding similar pairs " << endl;

    // Initialize thread-local storage for sub-queries, partition keys, one-deletion keys, and one-deletion key starts
    vector<vector<TokenLen>> subquery[MAXTHREADNUM];
    vector<unsigned int> p_keys[MAXTHREADNUM];
    vector<unsigned int> od_keys[MAXTHREADNUM];
    vector<TokenLen> odk_st[MAXTHREADNUM];
    // Resize the thread-local storage vectors
    for (int i = 0; i < MAXTHREADNUM; i++) {
        subquery[i].resize(maxIndexPartNum);
        p_keys[i].resize(maxIndexPartNum);
        od_keys[i].resize(maxSize);
        odk_st[i].resize(maxIndexPartNum);
    }

    // Allocation and calculate candidates
#pragma omp parallel for
    for (int rid = 0; rid < n; rid++) {
        int len = dataset[rid].size();
        if (len == 0)
            continue;

        // Get the thread ID for OpenMP
        const int tid = omp_get_thread_num();
        // We only need to access the indexLenGrp that current record belongs to and the previous group
        // But the previous group we have not created the partition key,etc for the current record
        // In this case we need generate them now
        for (int indexLenGrp = max(0, range_id[rid] - 1); indexLenGrp <= range_id[rid]; ++indexLenGrp) {
            // If the current length group is not the one the record belongs to, we need to recalculate partition keys, etc.
            if (indexLenGrp != range_id[rid]) {
                auto hash_st = LogTime();

                const int &indexLen = range[indexLenGrp].first;
                const int indexPartNum = floor(2 * coe * indexLen + EPS) + 1;
                p_keys[tid].resize(indexPartNum);
                od_keys[tid].resize(len);
                odk_st[tid].resize(indexPartNum + 1);

                // clear subquery oneHashValues  hashValues
                for (int pid = 0; pid < indexPartNum; ++pid) {
                    subquery[tid][pid].clear();
                    p_keys[tid][pid] = 0;
                }

                // allocate the tokens into subquery and hash each part
                // That is the way how the query is splitted into indexPartNum
                for (auto &token : dataset[rid]) {
                    unsigned int pid = token % indexPartNum;
                    subquery[tid][pid].push_back(token);
                    p_keys[tid][pid] = PRIME * p_keys[tid][pid] + token + 1;
                }

                TokenLen pos = 0;
                TokenLen tmp_cnt = 0;
                for (unsigned int pid = 0; pid < indexPartNum; ++pid) {
                    int64_t lenPart = indexLen + pid * (maxSize + 1);
                    unsigned int mhv = 0, hv = p_keys[tid][pid];
                    auto &subrec = subquery[tid][pid];

                    // we store the keys into one array for each record.
                    // so we need to know the boundary of keys for onedeletions of each partition
                    odk_st[tid][pid] = tmp_cnt;
                    for (unsigned int idx = 0; idx < subrec.size(); idx++) {
                        unsigned int chv = hv + mhv * prime_exp[subrec.size() - 1 - idx];
                        mhv = mhv * PRIME + subrec[idx] + 1;
                        chv -= mhv * prime_exp[subrec.size() - 1 - idx];

                        // get the keys of one-deletion neighbors from the current partition
                        // auto odkey = PACK(lenPart, chv);
                        od_keys[tid][tmp_cnt++] = chv;
                    }
                    pos += subrec.size();
                }
                odk_st[tid][indexPartNum] = tmp_cnt;

                hashInFind_cost[tid] += RepTime(hash_st);
                // Now all the information we have got
                GreedyFindCandidateAndSimPairs(tid, indexLenGrp, rid, p_keys[tid], od_keys[tid], odk_st[tid]);
            } else {
                // We have calculated them if current indexLenGroup is the group this record belongs to the
                const auto &existing_p_keys = parts_keys[rid];
                const auto &existing_od_keys = onedelete_keys[rid];
                const auto &existing_odk_st = odkeys_st[rid];
                GreedyFindCandidateAndSimPairs(tid, indexLenGrp, rid, existing_p_keys, existing_od_keys, existing_odk_st);
            }
        }
    }

    fprintf(stderr, "%lu %lu %lu \n", resultNum, candidateNum, listlens);
    search_cost = RepTime(find_st);
    cout << "Finding Similar Pairs Time Cost: " << search_cost << endl;

    // release memory
    delete[] prime_exp;
    delete[] parts_rids;
    delete[] ods_rids;
    delete[] parts_index_hv;
    delete[] od_index_hv;
    delete[] parts_index_offset;
    delete[] od_index_offset;
    delete[] parts_index_cnt;
    delete[] od_index_cnt;

    for (int i = 0; i < MAXTHREADNUM; ++i) {
        delete[] quickRef2D[i];
        delete[] negRef2D[i];
    }
    delete[] quickRef2D;
    delete[] negRef2D;
}