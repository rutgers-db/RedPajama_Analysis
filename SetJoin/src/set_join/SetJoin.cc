
#include "SetJoin.h"
#include "../util/util.h"

vector<pair<int, int>> cacheVec;
vector<vector<pair<int, int>>> indexVecs;
double overlap_cost = 0;
double allocation_cost = 0;
double index_cost = 0;
bool SetJoin::overlap(int x, int y, int posx, int posy, int current_overlap) 
{
    int require_overlap = ceil(det / (1 + det) * (int)(dataset[x].size() + dataset[y].size()) - EPS);
    while (posx < (int)dataset[x].size() && posy < (int)dataset[y].size()) {

        if ((int)dataset[x].size() - posx + current_overlap < require_overlap || 
                (int)dataset[y].size() - posy + current_overlap < require_overlap) return false; 

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

void SetJoin::setjoin(double threshold) 
{
    print_memory();
    timeval allstart, allend;
    gettimeofday(&allstart, NULL);

    cout << "Writing " << simP_file_path << endl;
    ofstream simp_ofs(simP_file_path, ios::binary);

    prime_exp[0] = 1;
    for (int i = 1; i < MAX_LINE_LENGTH; ++i)
        prime_exp[i] = prime_exp[i - 1] * PRIME;

    string str;
    
    // for (auto &vec : dataset) sort(vec.begin(), vec.end());

    det = threshold;
    double coe = (1 - det) / (1 + det);

    double ALPHA = 1.8;
    
    bool print_result = false;
    
    int n = dataset.size();

    int maxSize = dataset.back().size();

    int tokenNum = 0;
    for (int i = 0; i < dataset.size(); i++)
        if (tokenNum < dataset[i].back()) tokenNum = dataset[i].back();
    tokenNum += 1;

    // the universe U: 1st dimension: length + part, 2nd dimension, hashvalue
    google_unordered_map<int64_t, invIndexStruct> invIndex;
    google_unordered_map<int64_t, unsigned long long> oneIndex;
    // invIndex.set_empty_key(-1);
    // oneIndex.set_empty_key(-1);
    int invIndexSize = (1 - det) / det * lengthSum, oneIndexSize =  lengthSum;
    // invIndex.resize(invIndexSize);
    // oneIndex.resize(oneIndexSize);
    indexLists.resize(1);
    indexVecs.resize(1);
    indexLists.reserve(oneIndexSize + invIndexSize);
    indexVecs.reserve(n);

    int partNum;  // partition num
    int prevLen = 0;
    bool * quickRef = new bool[n];
    memset(quickRef, 0, sizeof(bool) * n);
    bool * negRef = new bool[n];
    memset(negRef, 0, sizeof(bool) * n);

    vector<int> candidates;

    int maxIndexPartNum = floor(2 * coe * maxSize + EPS) + 1; 
    vector<int> hashValues;
    vector<int> positions;
    vector<vector<int>> subquery;  // first part id second tokens
    vector<vector<int>> oneHashValues;
    vector<unsigned long long> invPtr;
    vector<unsigned long long> intPtr;
    vector<vector<unsigned long long>> onePtr;
    vector<pair<int, int>> values;	// <value, loc>
    vector<int> scores;

    subquery.resize(maxIndexPartNum);
    hashValues.resize(maxIndexPartNum);
    positions.resize(maxIndexPartNum);
    oneHashValues.resize(maxIndexPartNum);
    invPtr.resize(maxIndexPartNum);
    intPtr.resize(maxIndexPartNum);
    onePtr.resize(maxIndexPartNum);
    values.resize(maxIndexPartNum);
    scores.resize(maxIndexPartNum);

    int low = 0, high = 0;
    vector<pair<int, int>> range;

    for (int rid = 0; rid < n; rid++) {
        int len = dataset[rid].size();
        if(len==0)
            continue;

        int indexPartNum;
        int prevIndexPartNum = 0; 
        int pos = 0;

        int indexLenLow = ceil(det * len - EPS);
        for (int indexLenGrp = 0; indexLenGrp < (int)range.size(); ++indexLenGrp) {
            if (range[indexLenGrp].second < indexLenLow) continue;

            int indexLen = range[indexLenGrp].first;
            indexPartNum = floor(2 * coe * indexLen + EPS) + 1;

            // split the query into multiple parts if prevIndexPartNum != indexPartNum
            // it means if the indexLenGrp's first range is not change
            // But does it possible?
            if (prevIndexPartNum != indexPartNum) {

                // clear subquery oneHashValues  hashValues
                for (int pid = 0; pid < indexPartNum; ++pid) {
                    subquery[pid].clear();
                    oneHashValues[pid].clear();
                    hashValues[pid] = 0;
                }
                
                // allocate the tokens into subquery and hash each part
                // That is the way how the query is splitted into indexPartNum
                for (auto &token : dataset[rid]) {
                    int pid = token % indexPartNum;
                    subquery[pid].push_back(token);
                    hashValues[pid] = PRIME * hashValues[pid] + token + 1;
                }

                // Dont know the position meaning?
                pos = 0;
                for (int pid = 0; pid < indexPartNum; pid++) {
                    positions[pid] = pos;
                    pos = pos + subquery[pid].size();
                }

                prevIndexPartNum = indexPartNum;
            }

            // Initialize onePtr and reserve each space
            for (int pid = 0; pid < indexPartNum; ++pid) {
                onePtr[pid].clear();
                onePtr[pid].reserve(subquery[pid].size());
            }

            auto alloc_st = LogTime();
            // Iterate each part find each part if there is identical part already exits in inverted list
            // Based on the above result, initialize the value and scores
            for (int pid = 0; pid < indexPartNum; pid++) {
                
                // A hash 
                int64_t lenPart = indexLen + pid * (maxSize + 1);

                int v1 = 0;
                auto invit = invIndex.find(PACK(lenPart, hashValues[pid]));
                if (invit != invIndex.end()) {
                    invPtr[pid] = invit->second.list_no;
                    v1 = -indexLists[invit->second.list_no].cnt;
                } else {
                    invPtr[pid] = 0;
                }
                values[pid] = make_pair(v1, pid);
                scores[pid] = 0;
            }
            int heap_cnt = indexPartNum;
            make_heap(values.begin(), values.begin() + heap_cnt);


            int cost = 0;
            int rLen = min(range[indexLenGrp].second, len);
            int Ha = floor((len - det * rLen) / (1 + det) + EPS);
            int Hb = floor((rLen - det * len) / (1 + det) + EPS);
            int maxH = Ha + Hb;
            maxH = floor(coe * (len + rLen) + EPS);

            // We need use greedy selection in maxH + 1 times
            for (int i = 0; i < maxH + 1; ++i) {
                auto sel = values.front();
                pop_heap(values.begin(), values.begin() + heap_cnt);
                int pid = sel.second;
                ++scores[pid];
                cost -= sel.first;
                int64_t lenPart = indexLen + pid * (maxSize + 1);

                if (scores[sel.second] == 1) {
                    if (invPtr[pid] != 0) {
                        auto &vec = indexLists[invPtr[pid]].getVector();
                        for (auto lit = vec.begin(); lit != vec.end(); lit++) {
                            int rLen = dataset[lit->first].size();
                            int Ha = floor((len - det * rLen) / (1 + det) + EPS);
                            int Hb = floor((rLen - det * len) / (1 + det) + EPS);
                            int H = Ha + Hb; // maximum allowable difference

                            // If the current iteration index i is greater than this maximum allowable difference H
                            // current entry in the inverted list is skipped,
                            if (i > H) continue;
                            // position filter
                            if (negRef[lit->first] == false && quickRef[lit->first] == false)
                                candidates.push_back(lit->first);
                            if (positions[pid] - lit->second > Ha || lit->second - positions[pid] > Hb)
                                negRef[lit->first] = true;
                            else
                                quickRef[lit->first] = true;
                        }
                    }

                    // maintain heap
                    // Here is to find the next pair that insert to the heap
                    // Basically it is to consider the situation of the 1-deletion of the current part
                    int v2 = 0;

                    // search if the 1-deletion 
                    auto oneit = oneIndex.find(PACK(lenPart, hashValues[pid]));
                    if (oneit != oneIndex.end()) {
                        intPtr[pid] = oneit->second;
                        v2 -= indexLists[oneit->second].cnt;
                    } else {
                        intPtr[pid] = 0;
                    }


                    if (oneHashValues[pid].size() == 0) {
                        int mhv = 0, hv = hashValues[pid];
                        auto &sq = subquery[pid];
                        for (int idx = 0; idx < sq.size(); idx++) {
                            int chv = hv + mhv * prime_exp[sq.size() - 1 - idx];
                            mhv = mhv * PRIME + sq[idx] + 1;
                            chv -= mhv * prime_exp[sq.size() - 1 - idx];
                            oneHashValues[pid].push_back(chv);
                        }
                    }
                    for (int id = 0; id < (int)oneHashValues[pid].size(); ++id) {
                        auto invit = invIndex.find(PACK(lenPart, oneHashValues[pid][id]));
                        if (invit != invIndex.end()) {
                            onePtr[pid].push_back(invit->second.list_no);
                            v2 -= indexLists[invit->second.list_no].cnt;
                        } else {
                            onePtr[pid].push_back(0);
                        }
                    }

                    values[heap_cnt - 1].first = v2;
                    push_heap(values.begin(), values.begin() + heap_cnt);
                } else {
                    auto ov_st = LogTime();
                    // add candidates
                    if (intPtr[pid] != 0) {
                        auto &vec = indexLists[intPtr[pid]].getVector();
                        for (auto lit = vec.begin(); lit != vec.end(); lit++) {
                            int rLen = dataset[lit->first].size();
                            int Ha = floor((len - det * rLen) / (1 + det) + EPS);
                            int Hb = floor((rLen - det * len) / (1 + det) + EPS);
                            int H = Ha + Hb;
                            if (i > H) continue;
                            if (negRef[lit->first] == false && quickRef[lit->first] == false)
                                candidates.push_back(lit->first);
                            if (positions[pid] - lit->second > Ha || lit->second - positions[pid] > Hb)
                                negRef[lit->first] = true;
                            else
                                quickRef[lit->first] = true;
                        }
                    }

                    for (int id = 0; id < (int)onePtr[pid].size(); ++id) {
                        if (onePtr[pid][id] != 0) {
                            auto &vec = indexLists[onePtr[pid][id]].getVector();
                            for (auto lit = vec.begin(); lit != vec.end(); lit++) {
                                int rLen = dataset[lit->first].size();
                                int Ha = floor((len - det * rLen) / (1 + det) + EPS);
                                int Hb = floor((rLen - det * len) / (1 + det) + EPS);
                                int H = Ha + Hb;
                                if (i > H) continue;
                                if (negRef[lit->first] == false && quickRef[lit->first] == false)
                                    candidates.push_back(lit->first);
                                if (positions[pid] - lit->second > Ha || lit->second - positions[pid] > Hb)
                                    negRef[lit->first] = true;
                                else
                                    quickRef[lit->first] = true;
                            }
                        }
                    }
                    overlap_cost += RepTime(ov_st);
                    // maintain heap
                    --heap_cnt;
                }
            }
            allocation_cost += RepTime(alloc_st);
            listlens += cost;

            // clear candidates
            for (int idx = 0; idx < candidates.size(); idx++) {
                if (negRef[candidates[idx]] == false && quickRef[candidates[idx]] == true) {
                    if (overlap(candidates[idx], rid) == true) {
                        resultNum++;
                        pair<int, int> sim_pair = make_pair(rid, candidates[idx]);
                        // result_pairs.emplace_back(rid, candidates[idx]);
                        simp_ofs.write((char *)&sim_pair, sizeof(pair<int, int>));
                        
                        // if (print_result)
                        //    printf("%d %d\n", rid, candidates[idx]);
                    }
                    candidateNum++;
                }
                quickRef[candidates[idx]] = false;
                negRef[candidates[idx]] = false;
            }
            candidates.clear();
        }

        // indexing

        auto index_st = LogTime();
        // Here is to create a new group 
        if (len > high) {
            low = len;
            high = len * ALPHA;
            partNum = floor(2 * coe * low + EPS) + 1;
            range.push_back(make_pair(low, high));

            hashValues.clear();
            oneHashValues.clear();
            subquery.clear();
            hashValues.resize(partNum);
            oneHashValues.resize(partNum);
            subquery.resize(partNum);
            for (auto &token : dataset[rid]) {
                int pid = token % partNum;
                auto &subrec = subquery[pid];
                subrec.push_back(token);
                int hv = hashValues[pid] = PRIME * hashValues[pid] + token + 1;
            }
        } 

        for (int pid = 0; pid < partNum; ++pid) {
            if (oneHashValues[pid].size() == 0) {
                int mhv = 0, hv = hashValues[pid];
                auto &subrec = subquery[pid];
                for (int idx = 0; idx < subrec.size(); idx++) {
                    int chv = hv + mhv * prime_exp[subrec.size() - 1 - idx];
                    mhv = mhv * PRIME + subrec[idx] + 1;
                    chv -= mhv * prime_exp[subrec.size() - 1 - idx];
                    oneHashValues[pid].push_back(chv);
                }
            }
        }

        pos = 0;
        for (int pid = 0; pid < partNum; pid++) {
            int hv = hashValues[pid];
            auto &subrec = subquery[pid];
            int64_t lenPart = low + pid * (maxSize + 1);
            auto &inv = invIndex[PACK(lenPart, hv)];
            if (inv.list_no == 0) {
                indexLists.push_back(invertedList());
                inv.list_no = indexLists.size() - 1;
            }
            indexLists[inv.list_no].add(make_pair(rid, pos));
            // build one inverted index
            for (auto &chv : oneHashValues[pid]) {
                auto &one = oneIndex[PACK(lenPart, chv)];
                if (one == 0) {
                    indexLists.push_back(invertedList());
                    one = indexLists.size() - 1;
                }
                indexLists[one].add(make_pair(rid, pos));
            }

            pos = pos + subrec.size();
        }

        index_cost += RepTime(index_st);
    }
    gettimeofday(&allend, NULL);
    double all = allend.tv_sec - allstart.tv_sec + (allend.tv_usec - allstart.tv_usec) / 1e6;

    // print
    int v[6] = { 0 };
    for (auto &vec : indexVecs) {
        if (vec.size() < 6) ++v[vec.size()];
    }
    //fprintf(stderr, "vec len: ");
    //for (int i = 1; i < 6; ++i) fprintf(stderr, "(%d: %d) ", i, v[i]);
    fprintf(stderr, "total time: %.3fs\n", allend.tv_sec - allstart.tv_sec + (allend.tv_usec - allstart.tv_usec) / 1e6);
    fprintf(stderr, "%lu %lu %lu %.3f\n", resultNum, candidateNum, listlens, all);
    fprintf(stderr, "AllocCost %.3f  IndexCost: %.3f OverlapCost : %.3f\n",allocation_cost, index_cost, overlap_cost);

    unsigned long long pairs_amount = 0;
    for(auto const & vec : indexVecs){
        pairs_amount+=vec.size();
    }
    fprintf(stderr, "The amount of pairs in the indexVecs is: %llu\n", pairs_amount);
    print_memory();

    simp_ofs.close();

    delete[] quickRef;
    delete[] negRef;
} 