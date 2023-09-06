#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
#include "../src/set_join/SetJoinGroupParelled.h"
using namespace std;

int main(int argc, char *argv[]) {
    // show how many threads can be used in the current machine
    printHowManyThreads();

    // Parameters
    const string dataset_name = string(argv[1]); 
    const double thres = 0.8;

    // File Paths
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string sortedsets_file_path = root_dir + "/sorted_sets/" + dataset_name +"_sortedsets.bin";
    const string idmap_file_path = root_dir + "/sorted_sets/" +  dataset_name + "_idmap.bin";
    const string simP_file_path = root_dir + "/data/token/tokens_simp/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
    cout << "The Saved Path is " << simP_file_path<<endl;

    // Load SortedSets(Documents that have been sorted based on their length)
    // vector<vector<TokenLen>> sorted_sets;
    // loadShortBin(sortedsets_file_path, sorted_sets);
    
    // For Debug, shrink the size of dataset so that we can run through the code rapidly

    auto timer_st = LogTime();
    // Use setjoin
    cout<<"Start Set Join"<<endl;

    // Paralled Version
    vector<unsigned int> sorted_records_info;
    loadBinSize(sortedsets_file_path, sorted_records_info);
    SetJoinGroupParelled joiner(sorted_records_info);
    joiner.dataset_path = sortedsets_file_path;
    joiner.threshold = thres;
    joiner.init();
    joiner.findSimPairs();

    // Investigate the result
    printf("joiner.result_pairs have %llu pairs\n", joiner.getResultPairsAmount());
    // printf("The amount of document that occur in the pairs is %lu\n", getUniqueInts(joiner.result_pairs).size());
    
    // Output the similar pairs
    writeSimilarPairs(simP_file_path, joiner.result_pairs);
    printf("At last the total time cost is: %f\n", RepTime(timer_st));
}