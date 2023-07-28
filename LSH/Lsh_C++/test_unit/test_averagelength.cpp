#include <bits/stdc++.h>
#include "../src/IO.hpp"
using namespace std;

int main(int argc, char *argv[]){
    // const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    // // const string dataset_name = "stackexchange";
    // const string dataset_name = string(argv[1]);
    // const string sortedsets_file_path = root_dir + "/sorted_sets/" + dataset_name + "_sortedsets.bin";
    
    // vector<vector<unsigned short>> sorted_sets;
    // loadShortBin(sortedsets_file_path, sorted_sets);
    
    const string dataset_name = string(argv[1]);
    const string dataset_dir_path = "/research/projects/zp128/RedPajama-Data-1T/RedPajama-Data-1T/" + dataset_name; // arxiv_bottom_64.bin;
    vector<vector<unsigned short>> docs;
    loadTokensBasedOnIDX(dataset_dir_path, docs);
    double total_len = 0;

    for(auto & vec: docs){
        total_len += vec.size();
    }

    printf("Documents Amount %lu\n", docs.size());
    printf("Average length of sorted sets is %f\n", total_len / docs.size());
}