
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
#include "../util/util.h"

using namespace std;

// Macro to define the version of the algorithm.
// If VERSION is set to 2, the bottomk variant is used.
#define VERSION 1

// Macros for hashing
#define PACK(x, y) ((x << 32) + y) // 32
#define PRIME 2017
#define EPS 1e-5

// Type alias for token length.
// Use unsigned int for ngram, unsigned short otherwise.
using TokenLen = unsigned int;

class SetJoinParelled {
public:
    double det;
    uint64_t resultNum = 0;    // Number of result pairs found
    uint64_t candidateNum = 0; // Number of candidate pairs considered
    uint64_t listlens = 0;
    TokenLen maxIndexPartNum; // Maximum index partition number

    // Dataset containing records to be joined
    vector<vector<TokenLen>> dataset;

    // Array to store result pairs for each thread
    vector<pair<int, int>> result_pairs[MAXTHREADNUM];

    // Parameters related to calculation and dataset
    double coe;
    double ALPHA;
    unsigned int n;       // Number of records in the dataset
    unsigned int maxSize; // Maximum size of the records

    // Recording time cost of different part
    double index_cost;
    double search_cost;
    double hashInFind_cost[MAXTHREADNUM];
    double mem_cost[MAXTHREADNUM];
    double find_cost[MAXTHREADNUM];
    double alloc_cost[MAXTHREADNUM];
    double verif_cost[MAXTHREADNUM];

    SetJoinParelled(vector<vector<TokenLen>> &sorted_records) {
        dataset = sorted_records;
    }

    ~SetJoinParelled() {
    }

    // Output the Parameters
    void showPara() {
        printf("det: %f coe: %f ALPHA: %f maxSize: %u maxIndexPartNum: %hu \n", det, coe, ALPHA, maxSize,
               maxIndexPartNum);
    }

    void reportTimeCost() {
        double total_hash_cost = 0;
        double total_memeory_cost = 0;
        double total_find_cost = 0;
        double total_alloc_cost = 0;
        double total_verif_cost = 0;
        double sum;
        for (int i = 0; i < MAXTHREADNUM; i++) {
            total_hash_cost += hashInFind_cost[i];
            total_memeory_cost += mem_cost[i];
            total_find_cost += find_cost[i];
            total_alloc_cost += alloc_cost[i];
            total_verif_cost += verif_cost[i];
        }
        sum = total_hash_cost + total_memeory_cost + total_find_cost + total_alloc_cost + total_verif_cost;
        total_hash_cost = total_hash_cost / sum * search_cost;
        total_memeory_cost = total_memeory_cost / sum * search_cost;
        total_find_cost = total_find_cost / sum * search_cost;
        total_alloc_cost = total_alloc_cost / sum * search_cost;
        total_verif_cost = total_verif_cost / sum * search_cost;

        printf("|index_cost| total_hash_cost| total_memeory_cost| find_cost| alloc_cost| verif_cost|\n");
        printf("|%f|%f|%f|%f|%f|%f|\n", index_cost, total_hash_cost, total_memeory_cost, total_find_cost, total_alloc_cost, total_verif_cost);
    }

    void reportLargestGroup() {
        vector<unsigned long long> range_size(range.size());
        for (unsigned int i = 0; i < range_id.size(); i++) {
            range_size[range_id[i]] += dataset[i].size();
        }

        double total_size = 0;
        double max_size = 0;
        for (auto &size : range_size) {
            total_size += size;
            max_size = max(max_size, (double)size);
        }

        printf("Average Range size: %.3f Maximum Range size ratio %.3f \n", total_size / range_size.size(), max_size / total_size);
    }
    
    // Function to get the total number of result pairs found by all threads
    unsigned long long getResultPairsAmount() {
        unsigned long long pairs_amount = 0;
        for (int i = 0; i < MAXTHREADNUM; i++) {
            pairs_amount += result_pairs[i].size();
        }
        return pairs_amount;
    }

    // Function to check overlap between two sets
    bool overlap(unsigned int x, unsigned int y, int posx = 0, int posy = 0, int current_overlap = 0);

    // Main function to perform set join
    void setjoin(double threshold);

    // Function to build index
    void index(double threshold);

    // Function to find candidate and similar pairs using a greedy approach
    void GreedyFindCandidateAndSimPairs(const int &tid, const int indexLenGrp, const unsigned int rid, const vector<unsigned int> &p_keys, const vector<unsigned int> &od_keys, const vector<TokenLen> &odk_st);

    // Function to find similar pairs
    void findSimPairs();

private:
    int *prime_exp;    // Array for storing prime numbers, presumably for hashing
    bool **quickRef2D; // 2D quick reference array
    bool **negRef2D;   // 2D negative reference array

    // Vectors for storing range information(the groups that based on the size of documents)
    vector<pair<unsigned int, unsigned int>> range;
    vector<unsigned int> range_st;
    vector<int> range_id;

    // vectors needed when allocate by greedy heap method
    vector<unsigned int> invPtrArr[MAXTHREADNUM];
    vector<unsigned int> intPtrArr[MAXTHREADNUM];
    vector<vector<unsigned int>> onePtrArr[MAXTHREADNUM];
    vector<pair<int, TokenLen>> valuesArr[MAXTHREADNUM]; // <value, loc>
    vector<TokenLen> scoresArr[MAXTHREADNUM];
};

#endif