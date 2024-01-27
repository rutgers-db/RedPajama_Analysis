#include "util/io.h"
#include <execution>
#include <bits/stdc++.h>
using namespace std;

void correctPairOrder(vector<pair<int, int>> &pairs) {
#pragma omp parallel for
    for (unsigned long long i = 0; i < pairs.size(); i++) {
        auto &pair = pairs[i];
        if (pair.first >= pair.second) {
            auto tmp = pair.first;
            pair.first = pair.second;
            pair.second = tmp;
        }
    }

    sort(execution::par_unseq, pairs.begin(), pairs.end());
}

int main(int argc, char *argv[]) {
    // const string dataset_name = string(argv[1]);
    // const double thres = 0.8;
    // const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    // const string simP_file_path = root_dir + "/similar_pairs/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
    // const string sortedsimP_file_path = root_dir + "/sorted_simp/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";

    const string simP_file_path = string(argv[1]);
    const string sortedsimP_file_path = string(argv[2]);
    vector<pair<int, int>> pairs;
    readSimilarPair(simP_file_path, pairs);
    correctPairOrder(pairs);
    writeSimilarPair(sortedsimP_file_path, pairs);
}