#include <bits/stdc++.h>
#include "./src/IO.hpp"
#include "./src/util.hpp"
#include "./src/minhash.hpp"
#include "./src/LSH.hpp"
using namespace std;
int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    // const string dataset_name = "stackexchange";
    const string dataset_name = string(argv[1]);
    const string sortedsets_file_path = root_dir + "/sorted_sets/" + dataset_name + "_sortedsets.bin";
    const string idmap_file_path = root_dir + "/sorted_sets/" + dataset_name + "_idmap.bin";

    const double thres = 0.8;
    int K = 128;
    int band = 6;
    int range = 21;

    for (int i = 1; i < argc; i++) {
        const string arg = argv[i];
        if (arg == "-K") {
            K = atoi(argv[i + 1]);
        }

        if (arg == "-band") {
            band = atoi(argv[i + 1]);
        }

        if (arg == "-range") {
            range = atoi(argv[i + 1]);
        }

    }

    const string simP_file_path = "./similar_pairs/" + dataset_name + "_sim_pairs_" + "K" + to_string(K) + "B" + to_string(band) + "R" + to_string(range) + ".bin";

    // Load Idmap and sortedsets
    vector<int> idmap;
    loadBin2vec(idmap_file_path, idmap);
    vector<vector<unsigned short>> sorted_sets;
    loadShortBin(sortedsets_file_path, sorted_sets);

    auto timer_st = LogTime();
    cout << "Getting MinHash" << endl;

    // for(int i = 50000 ;i<50300;i++){
    //     outputVector(sorted_sets[i]);
    // }
    // Get MinHashes
    vector<vector<unsigned short>> minhashes(sorted_sets.size());
    MinHash minHasher(K);

#pragma omp parallel for
    for (unsigned int i = 0; i < minhashes.size(); i++) {
        minhashes[i] = minHasher.getMinHashes(sorted_sets[i]);
    }

    // Free sorted sets
    vector<vector<unsigned short>>().swap(sorted_sets);
    printf("MinHash Opertation Finished\n");

    // LSH
    LSH lsh(K, band, range);
    cout << "Start LSH" << endl;
    lsh.run(minhashes);

    // Free MinHashes
    vector<vector<unsigned short>>().swap(minhashes);

    writeSimilarPair(simP_file_path, lsh.result_pairs);
    printf("Parameters Setting:%s %d %d %d\n",dataset_name.c_str(), K, band, range);
    printf("At last the total time cost is: %f\n", RepTime(timer_st));
    return 0;
}