#ifndef _PARTITIONHASER_H_
#define _PARTITIONHASER_H_

#include <vector>
#include <iostream>
#include <fstream>
#include <omp.h>
#include "../../util/util.h"

using namespace std;

#define PACK(x, y) ((x << 32) + y) // Combine two 32-bit values into a 64-bit value
#define PRIME 2017 // A prime constant for hash calculations
#define EPS 1e-5   // Small epsilon value, potentially for floating-point comparisons

const unsigned int PRIMEEXP_SPACE = 3e7;
extern vector<int> prime_exp;    // Array for storing prime numbers, presumably for hashing

class PartitionHasher {
private:
    vector<vector<unsigned int>> subquery[MAXTHREADNUM]; // Array of vectors containing token lengths for each thread
   
public:
    vector<vector<unsigned int>> parts_keys; // Keys for each partition
    vector<vector<unsigned int>> onedelete_keys; // Reference to keys after a single deletion
    vector<vector<unsigned int>> odkeys_st; // Stores the start positions of one deletion information for each partition

    static void init_primeArr(){
        prime_exp.resize(PRIMEEXP_SPACE + 1);
        prime_exp[0] = 1;
        for (unsigned int i = 1; i <= PRIMEEXP_SPACE; ++i)
            prime_exp[i] = prime_exp[i - 1] * PRIME;
    } 

    // Constructor
    // to do: the reason that I refer the dataset is that to save the memory space, but the orignal content of dataset
    // will be lost
    PartitionHasher(const vector<vector<unsigned int>> & dataset) {

        // If the dataset's max_len is too long
        // reinitialize prime exponential array for hashing
        unsigned max_len = dataset[dataset.size() - 1].size();
        if(max_len > PRIMEEXP_SPACE){
            prime_exp.resize(max_len + 1);
            for (unsigned int i = PRIMEEXP_SPACE + 1; i <= max_len; ++i)
                prime_exp[i] = prime_exp[i - 1] * PRIME;
        }
        
    }

    /**
     * @brief Hash a dataset into multiple partitions.
     * 
     * @param dataset The dataset to be hashed.
     * @param partNum The number of partitions.
     * @return Elapsed time of hashing.
     */
    
    double hash(const vector<vector<unsigned int>> & dataset, const unsigned int partNum) {
        auto time_st = LogTime();

        unsigned int n = dataset.size();
        parts_keys.resize(n);
        odkeys_st.resize(n);
        onedelete_keys.resize(n);

        // Initialize subquery and hash value vectors
        for (unsigned int i = 0; i < MAXTHREADNUM; i++) {
            subquery[i].resize(partNum);
        }

#pragma omp parallel for
        for (unsigned int rid = 0; rid < n; rid++) {
            unsigned int len = dataset[rid].size();
            unsigned int tid = omp_get_thread_num(); // Get the current thread number
            subquery[tid].clear();
            parts_keys[rid].resize(partNum);
            subquery[tid].resize(partNum);

            // Assign tokens to partitions and compute initial hash values
            for (auto &token : dataset[rid]) {
                unsigned int pid = token % partNum;
                auto &subrec = subquery[tid][pid];
                subrec.push_back(token);

                // Update the hash value for the partition
                parts_keys[rid][pid] = PRIME * parts_keys[rid][pid] + token + 1;
            }

            onedelete_keys[rid].resize(len);
            odkeys_st[rid].resize(partNum + 1);
            unsigned int tmp_cnt = 0;
            for (unsigned int pid = 0; pid < partNum; ++pid) {
                unsigned int mhv = 0, hv = parts_keys[rid][pid];
                auto &subrec = subquery[tid][pid];

                // Track the start position of one-deletion keys for the partition
                odkeys_st[rid][pid] = tmp_cnt;

                // Calculate one-deletion hash values for each token in the partition
                for (unsigned int idx = 0; idx < subrec.size(); idx++) {
                    int chv = hv + mhv * prime_exp[subrec.size() - 1 - idx];
                    mhv = mhv * PRIME + subrec[idx] + 1;
                    chv -= mhv * prime_exp[subrec.size() - 1 - idx];

                    // Update the one-deletion keys for the record
                    onedelete_keys[rid][tmp_cnt++] = chv;
                }
            }
            odkeys_st[rid][partNum] = tmp_cnt; // Track the end position of one-deletion keys for the last partition
        }

        return RepTime(time_st); // Return elapsed time
    }

    double hash(const vector<unsigned int> & vec, const unsigned int partNum, int& tid, vector<unsigned int>& p_keys,
        vector<unsigned int>& od_keys, vector<unsigned int>& odk_st) {
        auto time_st = LogTime();

        subquery[tid].clear();
        p_keys.resize(partNum);
        subquery[tid].resize(partNum);

        // Assign tokens to partitions and compute initial hash values
        for (auto &token : vec) {
            unsigned int pid = token % partNum;
            auto &subrec = subquery[tid][pid];
            subrec.push_back(token);

            // Update the hash value for the partition
            p_keys[pid] = PRIME * p_keys[pid] + token + 1;
        }

        od_keys.resize(vec.size());
        odk_st.resize(partNum + 1);
        unsigned int tmp_cnt = 0;
        for (unsigned int pid = 0; pid < partNum; ++pid) {
            unsigned int mhv = 0, hv = p_keys[pid];
            auto &subrec = subquery[tid][pid];

            // Track the start position of one-deletion keys for the partition
            odk_st[pid] = tmp_cnt;

            // Calculate one-deletion hash values for each token in the partition
            for (unsigned int idx = 0; idx < subrec.size(); idx++) {
                int chv = hv + mhv * prime_exp[subrec.size() - 1 - idx];
                mhv = mhv * PRIME + subrec[idx] + 1;
                chv -= mhv * prime_exp[subrec.size() - 1 - idx];

                // Update the one-deletion keys for the record
                od_keys[tmp_cnt++] = chv;
            }
        }
        odk_st[partNum] = tmp_cnt; // Track the end position of one-deletion keys for the last partition
    

        return RepTime(time_st); // Return elapsed time
    }

};

#endif
