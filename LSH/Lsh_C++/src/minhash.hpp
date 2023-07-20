#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include "util.hpp"
class MinHash {
public:
    int K; // number of hash functions
    std::vector<pair<unsigned short, unsigned short>> hf; // hash functions

public:
    MinHash(const int _K) : K(_K) {
        // the seeds are 1 to K
        for(int i = 1 ;i<=_K; i++){
            generateHashFunc(i, hf);
        }
    }

    std::vector<unsigned short> getMinHashes(const std::vector<unsigned short>& doc) {
        if(doc.size() == 0)
            return vector<unsigned short>();
            
        std::vector<unsigned short> min_hashes(K, std::numeric_limits<unsigned short>::max());

        for (unsigned short element : doc) {
            for (int i = 0; i < K; ++i) {
                unsigned short hash_val = hval(hf[i], element);
                min_hashes[i] = std::min(min_hashes[i], hash_val);
            }
        }

        return min_hashes;
    }
};
