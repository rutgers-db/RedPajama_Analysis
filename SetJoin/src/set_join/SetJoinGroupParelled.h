//
// Created by 86183 on 2023/9/2.
//

#ifndef REDPAJAMA_ANALYSIS_SETJOINGROUPPARELLED_H
#define REDPAJAMA_ANALYSIS_SETJOINGROUPPARELLED_H

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

// Macro to define the version of the algorithm.
// If VERSION is set to 2, the bottomk variant is used.
#define VERSION 1

// Macros for hashing
#define PACK(x, y) ((x << 32) + y) // 32
#define PRIME 2017

#define EPS 1e-5

// Maximum number of threads that can be used
#define MAXTHREADNUM 128

// Type alias for token length.
// Use unsigned int for ngram, unsigned short otherwise.
using TokenLen = unsigned int;

class SetJoinGroupParelled {
public:
    double det{};
    uint64_t resultNum = 0;    // Number of result pairs found
    uint64_t candidateNum = 0; // Number of candidate pairs considered
    uint64_t listlens = 0;
    TokenLen maxIndexPartNum{};  // Maximum index partition number
    double threshold{};

    string dataset_path{};

    //information
    vector<int> dataset_info;

    //temp dataset
    unordered_map<unsigned int, vector<TokenLen>> temp_dataset;

    // Array to store result pairs for each thread
    vector<pair<int, int>> result_pairs[MAXTHREADNUM];

    // Parameters related to calculation and dataset
    double coe{};
    double ALPHA{};
    unsigned int n{};       // Number of records in the dataset
    unsigned int maxSize{}; // Maximum size of the records

    explicit SetJoinGroupParelled(vector<int> &sorted_records_info) {
        dataset_info = sorted_records_info;
    }

    ~SetJoinGroupParelled() = default;

    // Output the Parameters
    void showPara() {
        printf("det: %f coe: %f ALPHA: %f maxSize: %u maxIndexPartNum: %hu \n", det, coe, ALPHA, maxSize,
               maxIndexPartNum);
    }

    // Function to get the total number of result pairs found by all threads
    unsigned long long getResultPairsAmount() {
        unsigned long long pairs_amount = 0;
        for (int i = 0; i < MAXTHREADNUM; i++) {
            pairs_amount += result_pairs[i].size();
        }
        return pairs_amount;
    }

    //init range information
    void init();

    // Function to find similar pairs
    void findSimPairs();


private:
    int *prime_exp{};    // Array for storing prime numbers, presumably for hashing
    bool **quickRef2D{}; // 2D quick reference array
    bool **negRef2D{};   // 2D negative reference array

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


    // Function to build index for two range
    void index(int left_range_id, int right_range_id);


    // Function to find candidate and similar pairs using a greedy approach
    void GreedyFindCandidateAndSimPairs(const int &tid, const int indexLenGrp, const unsigned int rid,
                                        const vector<unsigned int> &p_keys, const vector<unsigned int> &od_keys,
                                        const vector<TokenLen> &odk_st);

    // Function to check overlap between two sets
    bool overlap(unsigned int x, unsigned int y, int posx = 0, int posy = 0, int current_overlap = 0);

    // Main function to perform set join
    void setjoin(double threshold);
};


#endif //REDPAJAMA_ANALYSIS_SETJOINGROUPPARELLED_H
