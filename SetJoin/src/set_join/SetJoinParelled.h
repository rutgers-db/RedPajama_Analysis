
#ifndef _SETJOINPARELLED_H_
#define _SETJOINPARELLED_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <string.h>
#include <queue>
#include <inttypes.h>
#include <sys/time.h>
#include <cmath>
#include <cstdio>

using namespace std;

#define PACK(x, y) ((x << 32) + y) // 32

#define PRIME 2017
#define EPS 1e-5

#define MAXTHREADNUM 128

class SetJoinParelled {
public:
    double det;
    uint64_t resultNum = 0;
    uint64_t candidateNum = 0;
    uint64_t listlens = 0;

    vector<vector<unsigned short>> dataset;
    vector<pair<int, int>> result_pairs[MAXTHREADNUM];

    // parameters about caculation and dataset
    double coe;
    const double ALPHA = 1.12;
    unsigned int n;       // the amount of records
    unsigned int maxSize; // the max size of the records


    SetJoinParelled(vector<vector<unsigned short>> &sorted_records) {
        dataset = sorted_records;
    }

    ~SetJoinParelled() {
    }

    unsigned int getResultPairsAmount(){
        unsigned int pairs_amount = 0;
        for(int i = 0;i<MAXTHREADNUM;i++){
            pairs_amount += result_pairs[i].size();
        }
        return pairs_amount;
    }
    bool overlap(int x, int y, int posx = 0, int posy = 0, int current_overlap = 0);
    void setjoin(double threshold);

    void index(double threshold);
    void GreedyFindCandidateAndSimPairs(const int & tid, const int indexLenGrp, const unsigned int rid, const vector<unsigned int> &p_keys, const vector<unsigned int> &od_keys, const vector<unsigned short> &odk_st);
    void findSimPairs();

};

#endif