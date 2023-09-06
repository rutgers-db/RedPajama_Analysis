/*
    Join the similar sketch and output the result pairs
*/
#include <bits/stdc++.h>
#include "util/io.h"
#include "util/util.h"
#include "set_join/SetJoin.h"
#include "set_join/SetJoinParelled.h"
using namespace std;

// It is the id map of sorted bottom_k to original_bottomk

int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin/bottomk";
    const string dataset_name = string(argv[1]); 
    const string sortedbk_file_path = root_dir + "/sorted_bottomk/" + dataset_name +"_sortedbottomk.bin";
    const string idmap_file_path = root_dir + "/sorted_bottomk/" +  dataset_name + "_idmap.bin";
    
    char* end;
    const double thres = strtod(argv[2], &end);
    const double ratio = strtod(argv[3], &end);
    cout<<thres<<ratio;
    // const string simP_file_path = root_dir + "/similar_pairs/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  "_withoutSizes.bin";
    const string simP_file_path = root_dir + "/similar_pairs/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+ "_"+to_string(ratio)+".bin";
    cout << "The Saved Path is " << simP_file_path<<endl;

    vector<vector<unsigned int>> sorted_bks;
    loadShortBin(sortedbk_file_path, sorted_bks);

    double ave_k = shrinkBottomk(sorted_bks, ratio);
    
    auto timer_st = LogTime();
    // Use setjoin
    cout<<"Start Set Join For Bottomk"<<endl;

    // Execute SetJoin to Join out the similar pairs
    SetJoinParelled joiner(sorted_bks);
    joiner.index(thres);
    joiner.findSimPairs();

    // Investigate the result
    printf("joiner.result_pairs have %llu pairs\n", joiner.getResultPairsAmount());

    // Write the similar files into directory    
    writeSimilarPairs(simP_file_path, joiner.result_pairs);
    printf("At last the total time cost is: %f\n", RepTime(timer_st));
    printf("Average K of the bottomks is %.4f, under the ratio of %.2f\n",ave_k, thres);
}