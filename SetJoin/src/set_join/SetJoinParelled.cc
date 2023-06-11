
#include "SetJoinParelled.h"
#include <omp.h>
#include <tbb/parallel_sort.h>
#include "../util/util.h"

// vector<vector<pair<int, int>>> indexVecs;
int *prime_exp;
bool **quickRef2D;
bool **negRef2D;

vector<pair<int, int>> range;
vector<unsigned int> range_st;
vector<int> range_id;

// the precalculated <key,rid,pos> for the partitions and ondeletions
vector<vector<HashRidPos>> parts_keys;
vector<vector<HashRidPos>> onedelete_keys;
vector<vector<unsigned short>> odkeys_st; // Because the max length of the record is almost 60000

// index for partitions and one deletions
vector<HashOfsCnt> *parts_index;
vector<HashOfsCnt> *od_index;
vector<HashRidPos> *parts_arr;
vector<HashRidPos> *ods_arr;

// vectors needed when allocate by greedy heap method
vector<HashOfsCnt> invPtrArr[MAXTHREADNUM];
vector<HashOfsCnt> intPtrArr[MAXTHREADNUM];
vector<vector<HashOfsCnt>> onePtrArr[MAXTHREADNUM];
vector<pair<int, unsigned short>> valuesArr[MAXTHREADNUM]; // <value, loc>
vector<unsigned short> scoresArr[MAXTHREADNUM];
bool SetJoinParelled::overlap(int x, int y, int posx, int posy, int current_overlap) {
    int require_overlap = ceil(det / (1 + det) * (int)(dataset[x].size() + dataset[y].size()) - EPS);
    while (posx < (int)dataset[x].size() && posy < (int)dataset[y].size()) {
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

// The index part is to create the index of Partitions and those one deletion neighbors
void SetJoinParelled::index(double threshold) {
    det = threshold;
    coe = (1 - det) / (1 + det);
    n = dataset.size();
    maxSize = dataset.back().size();
    const unsigned int maxIndexPartNum = floor(2 * coe * maxSize + EPS) + 1; // the possible maximum part amount of the maximum record

    // check if the dataset is qualified
    unsigned long long tokens_amount = 0;
    int tokenNum = 0;
    for (int i = 0; i < dataset.size(); i++) {
        tokens_amount += dataset[i].size();
        if (tokenNum < dataset[i].back()) tokenNum = dataset[i].back();
    }

    tokenNum += 1;
    printf("The tokenNum is %d, it cannot exceed 65,535 cause it cannot exceed the range of unsigned short\n", tokenNum);
    assert(tokenNum < 65535);

    cout << ALPHA << endl;
    // Initialize prime exponential array to hash the
    prime_exp = new int[n + 1];
    prime_exp[0] = 1;
    for (int i = 1; i <= n; ++i)
        prime_exp[i] = prime_exp[i - 1] * PRIME;

    quickRef2D = new bool*[MAXTHREADNUM];
    negRef2D = new bool*[MAXTHREADNUM];
    for(int i = 0; i < MAXTHREADNUM; ++i){
        quickRef2D[i] = new bool[n];
        memset(quickRef2D[i], 0, sizeof(bool) * n);
        negRef2D[i] = new bool[n];
        memset(negRef2D[i], 0, sizeof(bool) * n);
    }
    
    // Get the ranges(The group based on the record's size)
    // We group those size similar ones together

    range_id.resize(n);
    int low = 0, high = 0;
    for (int rid = 0; rid < n; rid++) {
        int len = dataset[rid].size();
        if (len > high) {
            low = len;
            high = low * ALPHA;
            range.push_back(make_pair(low, high));
            range_st.push_back(rid);
        }
        range_id[rid] = range.size() - 1;
    }
    range_st.push_back(n);

    vector<int> hashValues[MAXTHREADNUM];
    vector<vector<int>> subquery[MAXTHREADNUM]; // first part id second tokens

    for (int i = 0; i < MAXTHREADNUM; i++) {
        subquery[i].resize(maxIndexPartNum);
        hashValues[i].resize(maxIndexPartNum);
    }

    // To store the keys of parts and the one deletion neighbors
    // vector<vector<unsigned short>> positions(n, vector<unsigned short>());
    parts_keys.resize(n);
    onedelete_keys.resize(n);
    odkeys_st.resize(n); // Because the max length of the record is almost 60000

    // get hash values part
    auto calHash_st = LogTime();

#pragma omp parallel for
    for (int rid = 0; rid < n; rid++) {
        int len = dataset[rid].size();

        const int cur_low = range[range_id[rid]].first;
        const int partNum = floor(2 * coe * cur_low + EPS) + 1;

        int tid = omp_get_thread_num();
        hashValues[tid].clear();
        subquery[tid].clear();
        hashValues[tid].resize(partNum);
        subquery[tid].resize(partNum);
        for (auto &token : dataset[rid]) {
            int pid = token % partNum;
            auto &subrec = subquery[tid][pid];
            subrec.push_back(token);
            int hv = hashValues[tid][pid] = PRIME * hashValues[tid][pid] + token + 1;
        }

        // for getting the positions
        unsigned short pos = 0;

        // for getting the keys
        parts_keys[rid].resize(partNum);
        onedelete_keys[rid].resize(len);
        odkeys_st[rid].resize(partNum + 1);
        unsigned short j = 0;
        unsigned short tmp_cnt = 0;
        for (int pid = 0; pid < partNum; ++pid) {
            int64_t lenPart = cur_low + pid * (maxSize + 1);
            int mhv = 0, hv = hashValues[tid][pid];
            auto &subrec = subquery[tid][pid];
            
            // get the keys of partitions
            auto part_key = PACK(lenPart, hv);

            parts_keys[rid][pid] = HashRidPos(part_key, rid, pos);

            // we store the keys into one array for each record.
            // so we need to know the boundary of keys for onedeletions of each partition
            odkeys_st[rid][pid] = tmp_cnt;
            for (int idx = 0; idx < subrec.size(); idx++) {
                int chv = hv + mhv * prime_exp[subrec.size() - 1 - idx];
                mhv = mhv * PRIME + subrec[idx] + 1;
                chv -= mhv * prime_exp[subrec.size() - 1 - idx];

                // get the keys of one-deletion neighbors from the current partition
                auto odkey = PACK(lenPart, chv);
                onedelete_keys[rid][tmp_cnt++] = HashRidPos(odkey, rid, pos);
            }
            pos += subrec.size();
        }
        odkeys_st[rid][partNum] = tmp_cnt;
    }

    cout << "Calculating Hash Values of Partitions and OneDeleteion Neighbors Time Cost: " << RepTime(calHash_st) << endl;
    cout << "Now Let's building the inverted Index of these Paritions" << endl;

    // the universe U: 1st dimension: length + part, 2nd dimension, hashvalue
    // google_unordered_map<int64_t, vector<pair<unsigned int, unsigned short>> > * invIndex = new google_unordered_map<int64_t, vector<pair<unsigned int, unsigned short>> >[range.size()];
    // google_unordered_map<int64_t, unsigned int > * invIndex = new google_unordered_map<int64_t, unsigned int >[range.size()];
    // google_unordered_map<int64_t, vector<pair<unsigned int, unsigned short>> > * oneIndex = new google_unordered_map<int64_t, vector<pair<unsigned int, unsigned short>> >[range.size()];

    auto sort_st = LogTime();

    cout << range.size() << endl;

    // Reserve the size of merged_vector
    parts_arr = new vector<HashRidPos>[range.size()];
    ods_arr = new vector<HashRidPos>[range.size()];

#pragma omp parallel for
    for (int i = 0; i < range.size(); i++) {
        unsigned long long parts_amount = 0;
        unsigned long long one_deletion_amount = 0;
        auto const &rid_st = range_st[i];
        auto const &rid_ed = range_st[i + 1];
        for (int rid = rid_st; rid < rid_ed; rid++) {
            parts_amount += parts_keys[rid].size();
            one_deletion_amount += onedelete_keys[rid].size();
        }
        parts_arr[i].reserve(parts_amount);
        ods_arr[i].reserve(one_deletion_amount);
        for (int rid = rid_st; rid < rid_ed; rid++) {
            auto const &part_key = parts_keys[rid];
            parts_arr[i].insert(parts_arr[i].end(), part_key.begin(), part_key.end());
            auto const &onedelete_key = onedelete_keys[rid];
            ods_arr[i].insert(ods_arr[i].end(), onedelete_key.begin(), onedelete_key.end());
        }
        sort(parts_arr[i].begin(), parts_arr[i].end());
        sort(ods_arr[i].begin(), ods_arr[i].end());
    }
    cout << "Moving the data and sorting them Time Cost: " << RepTime(sort_st) << endl;

    auto index_st = LogTime();
    // Build the index
    parts_index = new vector<HashOfsCnt>[range.size()];
    od_index = new vector<HashOfsCnt>[range.size()];

#pragma omp parallel for
    for (int i = 0; i < range.size(); i++) {
        if (parts_arr[i].size() > 0) {
            int64_t tmp_hv = parts_arr[i][0].hashvalue;
            int64_t ofs = 0;
            unsigned int tmp_cnt = 1;
            for (unsigned long long j = 1; j < parts_arr[i].size(); j++) {
                auto &cur_hv = parts_arr[i][j].hashvalue;
                if (cur_hv != tmp_hv) {
                    parts_index[i].emplace_back(tmp_hv, ofs, tmp_cnt);
                    tmp_hv = cur_hv;
                    ofs = j;
                    tmp_cnt = 0;
                }
                tmp_cnt++;
            }
            // the last index should be added manually
            parts_index[i].emplace_back(tmp_hv, ofs, tmp_cnt);
        }

        if (ods_arr[i].size() > 0) {
            int64_t tmp_hv = ods_arr[i][0].hashvalue;
            int64_t ofs = 0;
            unsigned int tmp_cnt = 1;
            for (unsigned long long j = 1; j < ods_arr[i].size(); j++) {
                auto &cur_hv = ods_arr[i][j].hashvalue;
                if (cur_hv != tmp_hv) {
                    od_index[i].emplace_back(tmp_hv, ofs, tmp_cnt);
                    tmp_hv = cur_hv;
                    ofs = j;
                    tmp_cnt = 0;
                }
                tmp_cnt++;
            }
            // the last index should be added manually
            od_index[i].emplace_back(tmp_hv, ofs, tmp_cnt);
        }
    }
    // printf("pair_amount: %llu space_capacity: %llu map_sizes: %llu ",pair_amount,space_capacity, map_sizes);
    cout << "Indexing Partitions and OneDeleteion Neighbors Time Cost: " << RepTime(index_st) << endl;

    print_memory();
}

void SetJoinParelled::GreedyFindCandidateAndSimPairs(const int & tid, const int indexLenGrp, const unsigned int rid, const vector<HashRidPos> &p_keys, const vector<HashRidPos> &od_keys, const vector<unsigned short> &odk_st) {
    auto const indexPartNum = p_keys.size();
    assert(indexPartNum == odk_st.size() - 1);
    int const len = dataset[rid].size();
    auto const indexLen = range[indexLenGrp].first;

    vector<unsigned int> candidates;
    auto & invPtr = invPtrArr[tid];
    auto & intPtr = intPtrArr[tid];
    auto & onePtr = onePtrArr[tid];
    auto & values = valuesArr[tid];
    auto & scores = scoresArr[tid];
    invPtr.resize(indexPartNum);
    intPtr.resize(indexPartNum);
    onePtr.resize(indexPartNum);
    values.resize(indexPartNum);
    scores.resize(indexPartNum);
    auto negRef = negRef2D[tid];
    auto quickRef = quickRef2D[tid];
    // Initialize onePtr and reserve each space
    for (unsigned int pid = 0; pid < indexPartNum; ++pid) {
        onePtr[pid].clear();
        // cout<<rid << " "<< odk_st[pid+1]<<" " << odk_st[pid]<<endl;
        // if(odk_st[pid+1] < odk_st[pid]){
        //     cout<<"Something wrong"<<endl;
        //     cout<<indexLenGrp<<" " << rid<<endl;
        //     cout<<odk_st[pid+1]<<" " << odk_st[pid]<<endl;
        // }
        assert(odk_st[pid+1] >= odk_st[pid]);
        
        onePtr[pid].reserve(odk_st[pid+1] - odk_st[pid]);
    }
    
    // Iterate each part find each part if there is identical part already exits in inverted list
    // Based on the above result, initialize the value and scores
    for (unsigned short pid = 0; pid < indexPartNum; pid++) {
        // A hash
        // int64_t lenPart = indexLen + pid * (maxSize + 1);
        int v1 = 0;
        const auto &pkey = p_keys[pid].hashvalue;
        auto invit = lower_bound(parts_index[indexLenGrp].begin(), parts_index[indexLenGrp].end(), HashOfsCnt(pkey, 0, 0));
        // auto invit = invIndex.find(PACK(lenPart, hashValues[pid]));
        if (invit != parts_index[indexLenGrp].end() && invit->hashvalue == pkey) {
            invPtr[pid] = *invit;
            v1 = -invit->cnt;
        } else {
            invPtr[pid].hashvalue = 0;
        }
        values[pid] = make_pair(v1, pid);
        scores[pid] = 0;
    }

    unsigned int heap_cnt = indexPartNum;
    make_heap(values.begin(), values.begin() + heap_cnt);

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
        auto const pid_pos = (int)p_keys[pid].pos;
        ++scores[pid];
        cost -= sel.first;
        auto pkey = p_keys[pid].hashvalue;
        
        // int64_t lenPart = indexLen + pid * (maxSize + 1);

        if (scores[sel.second] == 1) {
            if (invPtr[pid].hashvalue != 0) {
                // auto &vec = indexLists[invPtr[pid]].getVector();

                // Iterate the candidates that shares the same partition
                auto ofs_st = invPtr[pid].ofs;
                auto ofs_ed = invPtr[pid].ofs + invPtr[pid].cnt;
                for (auto ofs = ofs_st; ofs < ofs_ed; ofs++) {
                    auto const &hrp = parts_arr[indexLenGrp][ofs];
                    if(hrp.rid >= rid) break;
                    unsigned int rLen = dataset[hrp.rid].size();
                    int Ha = floor((len - det * rLen) / (1 + det) + EPS);
                    int Hb = floor((rLen - det * len) / (1 + det) + EPS);
                    unsigned int H = Ha + Hb; // maximum allowable difference
                    // If the current iteration index i is greater than this maximum allowable difference H
                    // current entry in the inverted list is skipped,
                    if (i > H) continue;
                    // position filter
                    if (negRef[hrp.rid] == false && quickRef[hrp.rid] == false)
                        candidates.push_back(hrp.rid);

                    // We need to let them be int in case of negative minus result
                    auto const hrp_pos = (int)hrp.pos;
                    if (pid_pos - hrp_pos > Ha || hrp_pos - pid_pos > Hb)
                        negRef[hrp.rid] = true;
                    else
                        quickRef[hrp.rid] = true;
                }
            }

            // maintain heap
            // Here is to find the next pair that insert to the heap
            // Basically it is to consider the situation of the 1-deletion of the current part
            int v2 = 0;

            // search if the 1-deletion
            auto oneit = lower_bound(od_index[indexLenGrp].begin(), od_index[indexLenGrp].end(), HashOfsCnt(pkey, 0, 0));
            // auto oneit = oneIndex.find(PACK(lenPart, hashValues[pid]));
            if (oneit != od_index[indexLenGrp].end() && oneit->hashvalue == pkey) {
                intPtr[pid] = *oneit;
                v2 -= oneit->cnt;
            } else {
                intPtr[pid].hashvalue = 0;
            }

            // if (oneHashValues[pid].size() == 0) {
            //     int mhv = 0, hv = hashValues[pid];
            //     auto &sq = subquery[pid];
            //     for (int idx = 0; idx < sq.size(); idx++) {
            //         int chv = hv + mhv * prime_exp[sq.size() - 1 - idx];
            //         mhv = mhv * PRIME + sq[idx] + 1;
            //         chv -= mhv * prime_exp[sq.size() - 1 - idx];
            //         oneHashValues[pid].push_back(chv);
            //     }
            // }
            auto const &id_st = odk_st[pid];
            auto const &id_ed = odk_st[pid + 1];
            for (auto id = id_st; id < id_ed; id++) {
                auto const &odk = od_keys[id].hashvalue;
                auto invit = lower_bound(parts_index[indexLenGrp].begin(), parts_index[indexLenGrp].end(), HashOfsCnt(odk, 0, 0));
                if (invit != parts_index[indexLenGrp].end() && invit->hashvalue == odk) {
                    onePtr[pid].push_back(*invit);
                    v2 -= invit->cnt;
                } else {
                    onePtr[pid].push_back(HashOfsCnt(0, 0, 0));
                }
            }

            values[heap_cnt - 1].first = v2;
            push_heap(values.begin(), values.begin() + heap_cnt);
        } else {
            auto ov_st = LogTime();
            // add candidates
            if (intPtr[pid].hashvalue != 0) {
                auto ofs_st = intPtr[pid].ofs;
                auto ofs_ed = intPtr[pid].ofs + intPtr[pid].cnt;
                for (auto ofs = ofs_st; ofs < ofs_ed; ofs++) {
                    // Attention here is the ods_arr
                    auto const &hrp = ods_arr[indexLenGrp][ofs];
                    if(hrp.rid >= rid) break;
                    unsigned int rLen = dataset[hrp.rid].size();
                    int Ha = floor((len - det * rLen) / (1 + det) + EPS);
                    int Hb = floor((rLen - det * len) / (1 + det) + EPS);
                    unsigned int H = Ha + Hb; // maximum allowable difference
                    // If the current iteration index i is greater than this maximum allowable difference H
                    // current entry in the inverted list is skipped,
                    if (i > H) continue;
                    // position filter
                    if (negRef[hrp.rid] == false && quickRef[hrp.rid] == false)
                        candidates.push_back(hrp.rid);

                    // We need to let them be int in case of negative minus result
                    auto const hrp_pos = (int)hrp.pos;
                    if (pid_pos - hrp_pos > Ha || hrp_pos - pid_pos > Hb)
                        negRef[hrp.rid] = true;
                    else
                        quickRef[hrp.rid] = true;
                }
            }

            for (int id = 0; id < (int)onePtr[pid].size(); ++id) {
                if (onePtr[pid][id].hashvalue != 0) {
                    auto ofs_st = onePtr[pid][id].ofs;
                    auto ofs_ed = onePtr[pid][id].ofs + onePtr[pid][id].cnt;
                    for (auto ofs = ofs_st; ofs < ofs_ed; ofs++) {
                        auto const &hrp = parts_arr[indexLenGrp][ofs];
                        if(hrp.rid >= rid) break;
                        unsigned int rLen = dataset[hrp.rid].size();
                        int Ha = floor((len - det * rLen) / (1 + det) + EPS);
                        int Hb = floor((rLen - det * len) / (1 + det) + EPS);
                        unsigned int H = Ha + Hb; // maximum allowable difference
                        // If the current iteration index i is greater than this maximum allowable difference H
                        // current entry in the inverted list is skipped,
                        if (i > H) continue;
                        // position filter
                        if (negRef[hrp.rid] == false && quickRef[hrp.rid] == false)
                            candidates.push_back(hrp.rid);

                        // We need to let them be int in case of negative minus result
                        auto const hrp_pos = (int)hrp.pos;
                        if (pid_pos - hrp_pos > Ha || hrp_pos - pid_pos > Hb)
                            negRef[hrp.rid] = true;
                        else
                            quickRef[hrp.rid] = true;
                    }
                }
            }
            // overlap_cost += RepTime(ov_st);
            // maintain heap
            --heap_cnt;
        }
    }

    // listlens += cost;

    // clear candidates
    for (int idx = 0; idx < candidates.size(); idx++) {
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
}

void SetJoinParelled::findSimPairs() {
    auto find_st = LogTime();
    cout << "Start finding similar pairs " << endl;

    int maxIndexPartNum = floor(2 * coe * maxSize + EPS) + 1;
    vector<int> hashValues[MAXTHREADNUM];
    vector<vector<int>> subquery[MAXTHREADNUM];
    vector<HashRidPos> p_keys[MAXTHREADNUM];
    vector<HashRidPos> od_keys[MAXTHREADNUM];
    vector<unsigned short> odk_st[MAXTHREADNUM];
    
    for(int i = 0; i < MAXTHREADNUM; i++) {
        hashValues[i].resize(maxIndexPartNum);
        subquery[i].resize(maxIndexPartNum);
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

        const int tid = omp_get_thread_num();
        // We only need to access the indexLenGrp that current record belongs to and the previous group
        // But the previous group we have not created the partition key,etc for the current record
        // In this case we need generate them now
        for (int indexLenGrp = max(0, range_id[rid] - 1); indexLenGrp <= range_id[rid]; ++indexLenGrp) {
            // split the query into multiple parts if prevIndexPartNum != indexPartNum
            // it means if the indexLenGrp's first range is not change
            if (indexLenGrp != range_id[rid]) {
                const int &indexLen = range[indexLenGrp].first;
                const int indexPartNum = floor(2 * coe * indexLen + EPS) + 1;
                p_keys[tid].resize(indexPartNum);
                od_keys[tid].resize(len);
                odk_st[tid].resize(indexPartNum + 1);

                // clear subquery oneHashValues  hashValues
                for (int pid = 0; pid < indexPartNum; ++pid) {
                    subquery[tid][pid].clear();
                    hashValues[tid][pid] = 0;
                }

                // allocate the tokens into subquery and hash each part
                // That is the way how the query is splitted into indexPartNum
                for (auto &token : dataset[rid]) {
                    int pid = token % indexPartNum;
                    subquery[tid][pid].push_back(token);
                    hashValues[tid][pid] = PRIME * hashValues[tid][pid] + token + 1;
                }

                unsigned short pos = 0;
                unsigned short tmp_cnt = 0;
                for (int pid = 0; pid < indexPartNum; ++pid) {
                    int64_t lenPart = indexLen + pid * (maxSize + 1);
                    int mhv = 0, hv = hashValues[tid][pid];
                    auto &subrec = subquery[tid][pid];
                    
                    // get the keys of partitions
                    auto part_key = PACK(lenPart, hv);

                    p_keys[tid][pid] = HashRidPos(part_key, rid, pos);

                    // we store the keys into one array for each record.
                    // so we need to know the boundary of keys for onedeletions of each partition
                    odk_st[tid][pid] = tmp_cnt;
                    for (int idx = 0; idx < subrec.size(); idx++) {
                        int chv = hv + mhv * prime_exp[subrec.size() - 1 - idx];
                        mhv = mhv * PRIME + subrec[idx] + 1;
                        chv -= mhv * prime_exp[subrec.size() - 1 - idx];

                        // get the keys of one-deletion neighbors from the current partition
                        auto odkey = PACK(lenPart, chv);
                        od_keys[tid][tmp_cnt++] = HashRidPos(odkey, rid, pos);
                    }
                    pos += subrec.size();
                }
                odk_st[tid][indexPartNum] = tmp_cnt;

                // Now all the information we have got
                GreedyFindCandidateAndSimPairs(tid, indexLenGrp, rid, p_keys[tid], od_keys[tid], odk_st[tid]);
            } else {
                // We have calculated them if current indexLenGroup is the group this record belongs to the
                const vector<HashRidPos> &existing_p_keys = parts_keys[rid];
                const vector<HashRidPos> &existing_od_keys = onedelete_keys[rid];
                const vector<unsigned short> &existing_odk_st = odkeys_st[rid];
                GreedyFindCandidateAndSimPairs(tid, indexLenGrp, rid, existing_p_keys, existing_od_keys, existing_odk_st);
            }
        }
    }

    fprintf(stderr, "%lu %lu %lu \n", resultNum, candidateNum, listlens);
    cout << "Finding Similar Pairs Time Cost: " << RepTime(find_st) << endl;

    // release memory
    delete[] prime_exp;
    delete[] parts_index;
    delete[] od_index;
    delete[] parts_arr;
    delete[] ods_arr;

    for(int i = 0; i < MAXTHREADNUM; ++i){
        delete [] quickRef2D[i];
        delete[] negRef2D[i];
    }
    delete[] quickRef2D;
    delete[] negRef2D;
}