
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
// Define a preprocessor macro named VERSION. IF Version 2 that is for bottomk
#define VERSION 2
#define PACK(x, y) ((x << 32) + y) // 32

#define PRIME 2017
#define EPS 1e-5

#define MAXTHREADNUM 128

using TokenLen = unsigned int;

class SetJoinParelled {
public:
    double det;
    uint64_t resultNum = 0;
    uint64_t candidateNum = 0;
    uint64_t listlens = 0;
    TokenLen maxIndexPartNum;

    vector<vector<TokenLen>> dataset;
    vector<pair<int, int>> result_pairs[MAXTHREADNUM];

    // parameters about caculation and dataset
    double coe;
    double ALPHA;
    unsigned int n;       // the amount of records
    unsigned int maxSize; // the max size of the records


    SetJoinParelled(vector<vector<TokenLen>> &sorted_records) {
        dataset = sorted_records;
    }

    ~SetJoinParelled() {
    }

    // Output the Parameters
    void showPara(){
        printf("det: %f coe: %f ALPHA: %f maxSize: %u maxIndexPartNum: %hu \n", det, coe, ALPHA, maxSize, maxIndexPartNum);
    }

    unsigned long long getResultPairsAmount(){
        unsigned long long pairs_amount = 0;
        for(int i = 0;i<MAXTHREADNUM;i++){
            pairs_amount += result_pairs[i].size();
        }
        return pairs_amount;
    }
    bool overlap(unsigned int x, unsigned int y, int posx = 0, int posy = 0, int current_overlap = 0);
    void setjoin(double threshold);

    void index(double threshold);
    void GreedyFindCandidateAndSimPairs(const int & tid, const int indexLenGrp, const unsigned int rid, const vector<unsigned int> &p_keys, const vector<unsigned int> &od_keys, const vector<TokenLen> &odk_st);
    void findSimPairs();

};

#endif