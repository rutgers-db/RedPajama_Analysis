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

    void getMinHashesByBuffer(const string &binFileName, vector<vector<unsigned int>>& minhashes ) {
        cout << "Reading " << binFileName << endl;
        ifstream ifs(binFileName, ios::binary);
        if (!ifs) {
            cout << "error open bin file" << endl;
            return;
        }

        
        unsigned int bufferSize = 1e6; // Define your buffer size
        vector<vector<unsigned int>> buffer; // Buffer to hold a chunk of data

        int size;
        while (!ifs.eof()) {
            buffer.clear();
            for (int i = 0; i < bufferSize && ifs.read((char *)&size, sizeof(int)); ++i) {
                vector<unsigned int> vec(size);
                ifs.read((char *)&vec[0], sizeof(unsigned int) * size);
                buffer.emplace_back(vec);
            }
            auto ori_size = minhashes.size();
            minhashes.resize(ori_size + buffer.size());
            // Process the current buffer
#pragma omp parallel for
            for (unsigned int i = 0; i < buffer.size(); i++) {
                minhashes[ori_size + i] = getMinHashes(buffer[i]);
            }
        }
        ifs.close();
        
    }
};
