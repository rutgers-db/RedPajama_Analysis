#pragma once
#include <vector>
#include <execution>
#include <functional>
#include <omp.h>
#include "../../../SetJoin/emhash/hash_table7.hpp"

typedef emhash7::HashMap<size_t, unsigned int> hash_t;

#define PRIME 2017

using namespace std;
// External_LSH64: A class for performing Locality-Sensitive Hashing on large datasets.
class External_LSH64 {
private:
    int K;     // Number of hash functions used to compute MinHashes.
    int Band;  // Number of bands. Each band will have a portion of the MinHashes.
    int Range; // Size of each band (i.e., number of hash functions per band).

    // vector_hash: A functor for hashing a vector of unsigned integers.
    // Uses a simple rolling hash with a prime base.
    struct vector_hash {
        size_t operator()(const vector<unsigned int> &vec) const {
            size_t seed = 0;
            for (auto i : vec) {
                seed = PRIME * seed + i + 1;
            }
            return seed;
        }
    };

    vector_hash hasher; // Instance of the vector_hash functor.

public:
    // vector<map<size_t, vector<unsigned int>>> band_buckets;
    vector<pair<unsigned int, unsigned int>> result_pairs;

public:
    External_LSH64() {
    }

    External_LSH64(int _K, int _Band, int _Range) :
        K(_K), Band(_Band), Range(_Range) {
    }

    // run: Performs the LSH process on a set of MinHashes.
    // minhashes: A vector of vectors, each containing MinHashes for a document.
    void run(vector<vector<unsigned int>> &minhashes) {
        assert(Band * Range <= K); // Ensure that the total number of hash functions does not exceed K.

        // pairs: An array of vectors, each vector storing a pair of hash value and document ID for each band.
        vector<pair<size_t, unsigned int>> *pairs = new vector<pair<size_t, unsigned int>>[Band];
        for (int i = 0; i < Band; i++) {
            pairs[i].resize(minhashes.size());
        }

        auto timer_st = LogTime();

#pragma omp parallel for
        for (unsigned int docId = 0; docId < minhashes.size(); ++docId) {
            auto &minhash = minhashes[docId];
            if (minhash.size() == 0)
                continue;

            for (int b = 0; b < Band; ++b) {
                int i = b * Range;

                if (i + Range > K) {
                    break;
                }
                vector<unsigned int> slice(minhash.begin() + i, minhash.begin() + i + Range);
                size_t hashValue = hasher(slice);

                pairs[b][docId] = make_pair(hashValue, docId);
            }
        }

        // Free MinHashes memory
        vector<vector<unsigned int>>().swap(minhashes);
        cout << "Slice and Hash Done cost: " << RepTime(timer_st) << endl;

        vector<hash_t> group_index(Band);
        timer_st = LogTime();
        vector<pair<unsigned int, unsigned int>> *temp_pairs;
        temp_pairs = new vector<pair<unsigned int, unsigned int>>[Band];

        // Process each band to identify potential pairs of similar documents.
#pragma omp parallel for
        for (int i = 0; i < Band; i++) {
            temp_pairs[i].reserve(100000);
            auto &cur_gindex = group_index[i];
            cur_gindex.reserve(pairs[i].size());
            vector<unsigned int> tmp_vec(pairs[i].size());

            for (unsigned int j = 0; j < pairs[i].size(); j++) {
                cur_gindex[pairs[i][j].first]++;
            }

            unsigned int cnt = 0;

            for (auto &it : cur_gindex) {
                unsigned int pre_cnt = cnt;
                cnt += it.second;
                it.second = pre_cnt;
            }

            for (unsigned int j = 0; j < pairs[i].size(); j++) {
                tmp_vec[cur_gindex[pairs[i][j].first]++] = pairs[i][j].second;
                
            }

            unsigned int pre_ed = 0;
            unsigned int cur_ed;
            for (auto &it : cur_gindex) {
                cur_ed = it.second;
                for (auto j = pre_ed; j < cur_ed; j++) {
                    for (auto k = j + 1; k < cur_ed; k++) {
                        temp_pairs[i].emplace_back(tmp_vec[j], tmp_vec[k]);
                    }
                }
                pre_ed = cur_ed;
            }

            // Now lets writing pairs into a temp file

            // string outfile_name = "./tmp/" + to_string(i) + ".bin";
            // ofstream ofs(outfile_name.c_str(), ios::binary);
            // vector<pair<unsigned int, unsigned int>> buffer(100000);

            // unsigned int pre_ed = 0;
            // unsigned int cur_ed;
            // for(auto & it : cur_gindex){
            //     cur_ed = it.second;
            //     const auto & st = pre_ed;
            //     const auto & ed = cur_ed;
            //     pre_ed = cur_ed;
            //     for(auto j = st; j<= ed;j++){
            //         for(auto k = j+1; k <= ed;k++){
            //             buffer.emplace_back(tmp_vec[j], tmp_vec[k]);
            //         }
            //     }
            //     if(buffer.size()>100000){
            //         writeVecInOfs(ofs, buffer);
            //         buffer.clear();
            //     }
            // }
            // writeVecInOfs(ofs, buffer);
            // buffer.clear();
            // ofs.close();
        }
        cout << "LSH Done Now remove duplicate pairs cost: " << RepTime(timer_st) << endl;
        group_index.clear();
        timer_st = LogTime();
        delete[] pairs;
        for (int b = 0; b < Band; ++b) {
            result_pairs.insert(result_pairs.end(), temp_pairs[b].begin(), temp_pairs[b].end());
        }
        delete[] temp_pairs;
        // deduplicate the result_pairs
        deduplicate_vec(result_pairs);

        cout << "Merge and Sort Cost:" << RepTime(timer_st) << endl;
    }
};
