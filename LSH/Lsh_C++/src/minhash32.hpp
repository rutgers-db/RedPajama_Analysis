#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include "util.hpp"
class MinHash32 {
public:
    int K;                                            // number of hash functions
    std::vector<pair<unsigned int, unsigned int>> hf; // hash functions
    double read_t = 0, hash_t = 0;

public:
    MinHash32(const int _K) :
        K(_K) {
        // the seeds are 1 to K
        for (int i = 1; i <= _K; i++) {
            generateHashFunc(i, hf);
        }
    }

    std::vector<unsigned int> getMinHashes(const std::vector<unsigned int> &doc) {
        if (doc.size() == 0) {
            return vector<unsigned int>();
        }

        std::vector<unsigned int> min_hashes(K, std::numeric_limits<unsigned int>::max());

        for (auto element : doc) {
            for (int i = 0; i < K; ++i) {
                unsigned int hash_val = hval(hf[i], element);
                min_hashes[i] = std::min(min_hashes[i], hash_val);
            }
        }

        return min_hashes;
    }

    void getMinHashesByBuffer(const string &binFileName, vector<vector<unsigned int>> &minhashes) {
        cout << "Bufferly Reading " << binFileName << endl;
        read_t = 0;

        ifstream ifs(binFileName, ios::binary);
        if (!ifs) {
            cout << "error open bin file" << endl;
            return;
        }

        unsigned int bufferSize = 1e6;       // Define your buffer size
        vector<vector<unsigned int>> buffer; // Buffer to hold a chunk of data

        int size;
        while (!ifs.eof()) {
            auto read_st = LogTime();
            buffer.clear();
            for (int i = 0; i < bufferSize && ifs.read((char *)&size, sizeof(int)); ++i) {
                vector<unsigned int> vec(size);
                ifs.read((char *)&vec[0], sizeof(unsigned int) * size);
                buffer.emplace_back(vec);
            }
            auto ori_size = minhashes.size();
            minhashes.resize(ori_size + buffer.size());
            read_t += RepTime(read_st);
            // Process the current buffer
            auto hash_st = LogTime();
#pragma omp parallel for
            for (unsigned int i = 0; i < buffer.size(); i++) {
                minhashes[ori_size + i] = getMinHashes(buffer[i]);
            }
            hash_t += RepTime(hash_st);
        }
        ifs.close();
    }

    void getMinHashesOnce(const string &binFileName, vector<vector<unsigned int>> &minhashes) {
        // read the sets
        cout << "Direct Reading " << binFileName << endl;
        auto read_st = LogTime();
        vector<vector<unsigned int>> sorted_sets;
        loadIntBin(binFileName, sorted_sets);
        read_t = RepTime(read_st);
        minhashes.resize(sorted_sets.size());
        
        // Get MinHashes
        auto hash_st = LogTime();
#pragma omp parallel for
        for (unsigned int i = 0; i < minhashes.size(); i++) {
            minhashes[i] = getMinHashes(sorted_sets[i]);
        }
        hash_t = RepTime(hash_st);
    }
};
