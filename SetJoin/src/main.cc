#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
#include "../src/set_join/SetJoin.h"
#include "../src/set_join/SetJoinParelled.h"
#include "set_join/SetJoinGroupParelled.h"

using namespace std;

//int main(int argc, char *argv[]) {
//    // show how many threads can be used in the current machine
//    printHowManyThreads();
//
//    // Parameters
//    const string dataset_name = string(argv[1]);
//    const double thres = 0.8;
//
//    // File Paths
//    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
//    const string sortedsets_file_path = root_dir + "/sorted_sets/" + dataset_name +"_sortedsets.bin";
//    const string idmap_file_path = root_dir + "/sorted_sets/" +  dataset_name + "_idmap.bin";
//    const string simP_file_path = root_dir + "/similar_pairs/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
//    cout << "The Saved Path is " << simP_file_path<<endl;
//
//    // Load Idmap
//    // vector<int> idmap;
//    // loadBin2vec(idmap_file_path, idmap);
//
//    // Load SortedSets(Documents that have been sorted based on their length)
//    vector<vector<TokenLen>> sorted_sets;
//    loadShortBin(sortedsets_file_path, sorted_sets);
//
//    // For Debug, shrink the size of dataset so that we can run through the code rapidly
//    if(argc > 2) {
//        const unsigned int shrink_size = stoul(argv[2]);
//        sorted_sets.resize(shrink_size);
//
//        printf("For Debug using the dataset amount has been changed into %u \n", shrink_size);
//    }
//
//    auto timer_st = LogTime();
//    // Use setjoin
//    cout<<"Start Set Join"<<endl;
//
//    // Single Thread Version
//    // SetJoin joiner(sorted_sets, simP_file_path);
//    // joiner.setjoin(thres);
//
//    // Paralled Version
//    SetJoinParelled joiner(sorted_sets);
//    joiner.index(thres);
//    joiner.findSimPairs();
//
//    // Investigate the result
//    printf("joiner.result_pairs have %llu pairs\n", joiner.getResultPairsAmount());
//    // printf("The amount of document that occur in the pairs is %lu\n", getUniqueInts(joiner.result_pairs).size());
//
//    // Output the similar pairs
//    writeSimilarPairs(simP_file_path, joiner.result_pairs);
//    printf("At last the total time cost is: %f\n", RepTime(timer_st));
//}

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
    const string simP_file_path = root_dir + "/similar_pairs/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
    cout << "The Saved Path is " << simP_file_path<<endl;

    // Load Idmap
    // vector<int> idmap;
    // loadBin2vec(idmap_file_path, idmap);

    // Load SortedSets(Documents that have been sorted based on their length)
    vector<int> sorted_sets_size_info;
    loadBinSize(sortedsets_file_path, sorted_sets_size_info);

    auto timer_st = LogTime();
    // Use setjoin
    cout<<"Start Set Join"<<endl;

    // Single Thread Version
    // SetJoin joiner(sorted_sets, simP_file_path);
    // joiner.setjoin(thres);

    // Paralled Version
    SetJoinGroupParelled joiner(sorted_sets_size_info);
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