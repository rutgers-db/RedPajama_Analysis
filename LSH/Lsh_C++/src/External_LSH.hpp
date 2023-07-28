#pragma once
#include <vector>
#include <execution>
#include <functional>
#include <omp.h>
#define PRIME 2017

using namespace std;
// const int mod_p = 2<<31 - 1;
class External_LSH {
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
    // vector<pair<unsigned int, unsigned int>> result_pairs;

public:
    External_LSH() {
    }

    External_LSH(int _K, int _Band, int _Range) :
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
        cout << "Slice and Hash Done" << endl;

#pragma omp parallel for
        for (int i = 0; i < Band; i++) {
            sort(pairs[i].begin(), pairs[i].end());
            printf("%d : sorted, its pairs size is : %lu", i, pairs[i].size());

            vector<pair<unsigned long long, unsigned long long>> groups;
            vector<vector<unsigned int>> group_ids(pairs[i].size());
            vector<unsigned int> prev_cal(pairs[i].size(), -1);

            // Now lets writing pairs into a temp file
            string outfile_name = "./tmp/" + to_string(i) + ".bin";
            ofstream ofs(outfile_name.c_str(), ios::binary);
            vector<pair<unsigned int, unsigned int>> buffer(100000);

            // Get the groups share the same keys
            auto prev_key = pairs[i][0].first;
            unsigned long long prev_loc = 0;
            for (unsigned long long j = 1; j < pairs[i].size(); j++) {
                auto key = pairs[i][j].first;
                if (key != prev_key) {
                    auto st = prev_loc;
                    auto ed = j;
                    if (ed - st > 1) {
                        groups.emplace_back(st, ed);
                    }
                    prev_key = key;
                    prev_loc = j;
                }
            }
            auto st = prev_loc;
            auto ed = pairs[i].size();
            groups.emplace_back(st, ed);
            // for (unsigned long long k = st; k < ed; k++){
            //     auto & docid_0 = pairs[i][k].second;
            //     for (unsigned long long p = k + 1; p < ed; p++) {
            //         auto & docid_1 = pairs[i][p].second;
            //     }
            // }

            // Iterate each group and get each doc, what group are they in
            for (auto gid = 0; gid < groups.size(); gid++) {
                auto st = groups[gid].first;
                auto ed = groups[gid].second;
                for (auto j = st; j < ed; j++) {
                    group_ids[pairs[i][j].second].emplace_back(gid);
                }
            }

            // Iterate each doc and generate its corresponding pair
            for (unsigned int did = 0; did < group_ids.size(); did++) {
                for (auto gid : group_ids[did]) {
                    auto st = groups[gid].first;
                    auto ed = groups[gid].second;
                    for (auto j = st; j < ed; j++) {
                        auto &doc_id = pairs[i][j].second;
                        if (doc_id <= did)
                            continue;

                        if (prev_cal[doc_id] != did) {
                            prev_cal[doc_id] = did;
                            buffer.emplace_back(did, doc_id);
                        }
                    }

                    if(buffer.size()>100000){
                        writeVecInOfs(ofs, buffer);
                        buffer.clear();
                    }
                }
            }
            writeVecInOfs(ofs, buffer);
            buffer.clear();

            ofs.close();
        }

        cout << "Writing into temp files done" << endl;
    }
};
