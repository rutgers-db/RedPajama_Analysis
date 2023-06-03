/*
This is a code that can read the similar pairs that generated by the overlap_joiner
However read the id of similar pairs should leverage two files, including idmap
*/
#include <bits/stdc++.h>
#include "../src/util/io.h"
using namespace std;

int main() {
    const int K = 64;
    const int c = 58;
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/OverlapJoin";
    const string dataset = "stackexchange";
    const string simP_dirpath = root_dir + "/similar_pairs/" + dataset + "_simPair_K" + to_string(K) + "_C" + to_string(c) + "/";

    const string idmap_file_path = simP_dirpath + "idmap.bin";
    const string simpair_file_path = simP_dirpath + "sim_pairs.bin";

    // Read the idmap and simpair
    vector<int> idmap;
    vector < pair<int, int>> sim_pair;
    loadBin2vec(idmap_file_path, idmap);
    readSimilarPair(simpair_file_path, sim_pair);
    
    // Output the similar pairs
    for (int i = 0; i < 5; i++) {
        printf("Similar pairs: %d %d\n", idmap[sim_pair[i].first], idmap[sim_pair[i].second]);
    }
}