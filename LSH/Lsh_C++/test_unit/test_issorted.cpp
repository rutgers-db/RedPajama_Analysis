#include "../src/IO.hpp"
#include <bits/stdc++.h>
using namespace std;

int main(){
    string file_path = "/research/projects/zp128/RedPajama_Analysis/LSH/Lsh_C++/similar_pairs/wikipedia_sim_pairs_K128B16R8.bin";
    vector<pair<unsigned int, unsigned int>> result_pairs;
    readSimilarPair(file_path, result_pairs);
    // readPairsFromFile(file_path, result_pairs);
    if (std::is_sorted(result_pairs.begin(), result_pairs.end())) {
        std::cout << "result_pairs is sorted.\n";
    } else {
        std::cout << "result_pairs is not sorted.\n";
    }
}   