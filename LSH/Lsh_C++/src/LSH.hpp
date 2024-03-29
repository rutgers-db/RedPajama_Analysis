#pragma once
#include <vector>
#include <execution>
#include <functional>
#include <omp.h>
#define PRIME 2017

using namespace std;
// const int mod_p = 2<<31 - 1;
class LSH {
private:
    int K;     // number of hash functions
    int Band;  // number of bands
    int Range; // size of each band

    struct vector_hash {
        size_t operator()(const vector<unsigned short> &vec) const {
            size_t seed = 0;
            for (unsigned short i : vec) {
                seed = PRIME * seed + i + 1;
            }
            return seed;
        }
    };

    vector_hash hasher;

public:
    // vector<map<size_t, vector<unsigned int>>> band_buckets;
    vector<pair<unsigned int, unsigned int>> result_pairs;

public:
    LSH() {
    }

    LSH(int _K, int _Band, int _Range) :
        K(_K), Band(_Band), Range(_Range) {
    }

    void run(vector<vector<unsigned short>> &minhashes) {
        assert(Band * Range <= K);
        vector<pair<size_t, unsigned int>> *pairs = new vector<pair<size_t, unsigned int>>[Band];
        for (int i = 0; i < Band; i++) {
            pairs[i].resize(minhashes.size());
        }

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
                vector<unsigned short> slice(minhash.begin() + i, minhash.begin() + i + Range);
                size_t hashValue = hasher(slice);

                pairs[b][docId] = make_pair(hashValue, docId);
                // #pragma omp critical
                // band_buckets[b][hashValue].push_back(docId);
            }
        }

        // Free MinHashes memory
        vector<vector<unsigned short>>().swap(minhashes);
        cout<<"Slice and Hash Done"<<endl;

        vector<pair<unsigned int, unsigned int>> *temp_pairs;
        temp_pairs = new vector<pair<unsigned int, unsigned int>>[Band];
#pragma omp parallel for
        for (int i = 0; i < Band; i++) {
            sort(pairs[i].begin(), pairs[i].end());
            printf("%d : sorted, its pairs size is : %lu",i, pairs[i].size());
            // Get the groups share the same keys
            auto prev_key = pairs[i][0].first;
            unsigned long long prev_loc = 0;
            for (unsigned long long j = 1; j < pairs[i].size(); j++) {
                auto key = pairs[i][j].first;
                if (key != prev_key) {
                    auto st = prev_loc;
                    auto ed = j;
                    if(ed-st > 1){
                        for (unsigned long long k = st; k < ed; k++){
                            auto & docid_0 = pairs[i][k].second;
                            for (unsigned long long p = k + 1; p < ed; p++) {
                                auto & docid_1 = pairs[i][p].second;
                                temp_pairs[i].emplace_back(docid_0, docid_1);
                            }
                        }
                    }
                    prev_key = key;
                    prev_loc = j;
                }
            }
            auto st = prev_loc;
            auto ed = pairs[i].size();
            for (unsigned long long k = st; k < ed; k++){
                auto & docid_0 = pairs[i][k].second;
                for (unsigned long long p = k + 1; p < ed; p++) {
                    auto & docid_1 = pairs[i][p].second;
                    temp_pairs[i].emplace_back(docid_0, docid_1);
                }
            }
        }

        cout<<"LSH Done Now remove duplicate pairs"<<endl;
        // free the thesetemporary vectors
        delete[] pairs;

        for (int b = 0; b < Band; ++b) {
            result_pairs.insert(result_pairs.end(), temp_pairs[b].begin(), temp_pairs[b].end());
        }
        delete[] temp_pairs;
        
        // deduplicate the result_pairs
        deduplicate_vec(result_pairs);
    }
};
