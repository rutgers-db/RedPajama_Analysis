#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include "util.hpp"
class MinHash32 {
public:
    int K; // number of hash functions
    std::vector<pair<unsigned int, unsigned int>> hf;  // hash functions

public:
    MinHash32(const int _K) : K(_K) {
        // the seeds are 1 to K
        for(int i = 1 ;i<=_K; i++){
            generateHashFunc(i, hf);
        }

        // for(int i = 1 ;i<=_K; i++){
        //     cout<<hf[i-1].first<<" "<< hf[i-1].second<<endl;
        // }
    }

    std::vector<unsigned int> getMinHashes(const std::vector<unsigned int>& doc) {
        if(doc.size() == 0){
            return vector<unsigned int>();
        }
            
            
        std::vector<unsigned int> min_hashes(K, std::numeric_limits<unsigned int>::max());

        for (auto element : doc) {
            for (int i = 0; i < K; ++i) {
                unsigned int hash_val = hval(hf[i], element);
                // cout<<"i"<<i<<" hash_val:" << hash_val<<endl;
                min_hashes[i] = std::min(min_hashes[i], hash_val);
            }
        }

        return min_hashes;
    }
};
