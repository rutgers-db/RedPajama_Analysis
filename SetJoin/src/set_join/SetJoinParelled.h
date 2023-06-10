
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

#define google_unordered_map unordered_map
#define google_unordered_set unordered_set

#define PACK(x, y) ((x << 32) + y) // 32

#define PRIME 2017
#define EPS 1e-5
#define CACHE_SIZE 5

// extern vector<pair<int, int>> cacheVec;
// extern vector<vector<pair<int, int>>> indexVecs;

class SetJoinParelled {
public:
    double det;
    uint64_t resultNum = 0;
    uint64_t candidateNum = 0;
    uint64_t listlens = 0;

    vector<vector<unsigned short>> dataset;
    vector<pair<int, int>> result_pairs;

    // parameters about caculation and dataset
    double coe;
    const double ALPHA = 1.12;
    unsigned int n;       // the amount of records
    unsigned int maxSize; // the max size of the records

    struct invertedList {
        int vec_no, cnt;
        pair<int, int> cache[CACHE_SIZE];

        // vector<pair<int, int>>& getVector() const {
        //     if (cnt <= CACHE_SIZE) {
        //         cacheVec.assign(cache, cache + cnt);
        //         return cacheVec;
        //     } else
        //         return indexVecs[vec_no];
        // }

        // void add(pair<int, int> data) {
        //     if (cnt < CACHE_SIZE) cache[cnt++] = data;
        //     else {
        //         if (CACHE_SIZE == cnt) {
        //             indexVecs.push_back(vector<pair<int, int>>());
        //             vec_no = indexVecs.size() - 1;
        //             indexVecs[vec_no].assign(cache, cache + CACHE_SIZE);
        //         }
        //         ++cnt;
        //         indexVecs[vec_no].push_back(data);
        //     }
        // }
    };

    struct invIndexStruct {
        int list_no;
        // vector<int> oneList;

        invIndexStruct() :
            list_no(0) {
        }
    };

    vector<invertedList> indexLists;

    SetJoinParelled(vector<vector<unsigned short>> &sorted_records) {
        dataset = sorted_records;
        // indexVecs.clear();
        // cacheVec.clear();
        // cacheVec.resize(CACHE_SIZE);
    }

    ~SetJoinParelled() {
        // cacheVec.clear();
        // indexVecs.clear();
    }

    bool overlap(int x, int y, int posx = 0, int posy = 0, int current_overlap = 0);
    void setjoin(double threshold);

    void index(double threshold);
    void findSimPairs();
};

#endif