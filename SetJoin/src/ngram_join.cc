#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
#include "../src/set_join/SetJoin.h"
#include "../src/set_join/SetJoinParelled.h"
using namespace std;

// It is the id map of sorted bottom_k to original_bottomk

int main(int argc, char *argv[]) {
    printHowManyThreads();

    // Parameters
    const string dataset_name = string(argv[1]); 
    const double thres = strtod(argv[2], nullptr);
    // Convert them into L sketches
    unsigned int M = 8;
    // File Paths
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string sortedsets_file_path = root_dir + "/data/ngram/sorted_lsketch/" + dataset_name +"_M"+to_string(M) + ".bin";
    const string simP_file_path = root_dir + "/data/ngram/lsketch_simp/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
    cout << "The Saved Path is " << simP_file_path<<endl;    

    // Load Sorted Ngrams
    vector<vector<TokenLen>> sorted_sets;
    loadIntBin(sortedsets_file_path, sorted_sets);

    

    auto timer_st = LogTime();
    // Use setjoin
    cout<<"Start Set Join"<<endl;
    SetJoinParelled joiner(sorted_sets);
    joiner.index(thres);
    joiner.findSimPairs();
    joiner.reportTimeCost();
    joiner.reportLargestGroup();
    // Investigate the result
    printf("joiner.result_pairs have %llu pairs\n", joiner.getResultPairsAmount());
    // printf("The amount of document that occur in the pairs is %lu\n", getUniqueInts(joiner.result_pairs).size());
    
    writeSimilarPairs(simP_file_path, joiner.result_pairs);
    printf("At last the total time cost is: %f\n", RepTime(timer_st));
}