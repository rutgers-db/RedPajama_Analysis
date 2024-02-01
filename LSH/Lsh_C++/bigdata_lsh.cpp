// If the loading bin file is too big and the similar pairs are too many
// Use this version help you reduce Memory Usage

#include <bits/stdc++.h>
#include "./src/IO.hpp"
#include "./src/util.hpp"
#include "./src/minhash32.hpp"
#include "./src/External_LSH64.hpp"
using namespace std;
int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]);
    const string sortedsets_file_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name + "_sortedngram.bin";

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

    const string simP_file_path = "./ngram_simps/" + dataset_name + "_sim_pairs_" + "K" + to_string(K) + "B" + to_string(band) + "R" + to_string(range) + ".bin";

    auto timer_st = LogTime();
    // Get MinHashes
    vector<vector<unsigned int>> minhashes;
    MinHash32 minHasher(K);
    minHasher.getMinHashesByBuffer(sortedsets_file_path, minhashes);
    // minHasher.getMinHashesOnce(sortedsets_file_path, minhashes);
    printf("MinHash Opertation Finished\n");

    External_LSH64 lsh(K, band, range);
    cout << "Start External LSH" << endl;
    lsh.run(minhashes);

    writeSimilarPair(simP_file_path, lsh.result_pairs);
    printf("Parameters Setting:%s %d %d %d\n", dataset_name.c_str(), K, band, range);
    printf("Read Cost: %f MinHash + LSH cost: %f At last the total time cost is: %f\n", minHasher.read_t, minHasher.hash_t+lsh.lsh_t, RepTime(timer_st));
    return 0;
}