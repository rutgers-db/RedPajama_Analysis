#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
#include "../src/set_join/SetJoinGroupParelled.h"
using namespace std;

// It is the id map of sorted bottom_k to original_bottomk

int main(int argc, char *argv[]) {
    printHowManyThreads();

    // Parameters
    const string dataset_name = "stackexchange" ;// string(argv[1]); 
    const double thres = 0.8 ; // strtod(argv[2], nullptr);
    // Convert them into L sketches
    unsigned int M = 8;
    // File Paths
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string sortedsets_file_path = root_dir + "/data/ngram/sorted_lsketch/" + dataset_name +"_M"+to_string(M) + ".bin";
    // string sortedsets_file_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name + "_sortedngram.bin";
    const string simP_file_path = root_dir + "/test/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
    cout << "The Saved Path is " << simP_file_path<<endl;    

    auto timer_st = LogTime();
    // Use setjoin
    cout<<"Start Set Join"<<endl;
    SetJoinGroupParelled joiner(sortedsets_file_path);
    joiner.setjoin(thres);
    joiner.reportTimeCost();
    // Investigate the result
    printf("joiner.result_pairs have %llu pairs\n", joiner.getResultPairsAmount());
    // printf("The amount of document that occur in the pairs is %lu\n", getUniqueInts(joiner.result_pairs).size());
    
    writeSimilarPairs(simP_file_path, joiner.result_pairs);
    printf("At last the total time cost is: %f\n", RepTime(timer_st));
}