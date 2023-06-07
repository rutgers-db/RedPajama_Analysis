#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
// #include "../src/set_join/SetJoin.h"
#include "../src/set_join/SetJoinParelled.h"
using namespace std;

// It is the id map of sorted bottom_k to original_bottomk

int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]); 
    const string sortedsets_file_path = root_dir + "/sorted_sets/" + dataset_name +"_sortedsets.bin";
    const string idmap_file_path = root_dir + "/sorted_sets/" +  dataset_name + "_idmap.bin";

    const double thres = 0.9;
    // Load Idmap and sortedsets
    vector<int> idmap;
    loadBin2vec(idmap_file_path, idmap);
    vector<vector<unsigned short>> sorted_sets;
    loadShortBin(sortedsets_file_path, sorted_sets);

    // For debug
    if(argc > 2) {
        const unsigned int shrink_size = stoul(argv[2]);
        sorted_sets.resize(shrink_size);
        printf("For Debug using the dataset amount has been changed into %u \n", shrink_size);
    }

    // Use setjoin
    cout<<sorted_sets.back().size()<<endl;
    cout<<"Start Set Join"<<endl;
    // SetJoin joiner(sorted_sets);
    SetJoinParelled joiner(sorted_sets);
    joiner.setjoin(thres);

    // Investigate the result
    printf("joiner.result_pairs have %lu pairs\n", joiner.result_pairs.size());
    printf("The amount of document that occur in the pairs is %lu\n", getUniqueInts(joiner.result_pairs).size());
    // Print the result pairs
    for(auto i =0; i< 5; i ++){
        if(i>=joiner.result_pairs.size())
            break;
        auto& pair =  joiner.result_pairs[i];
        printf("%d %d\n",pair.first,pair.second);
        printf("%d %d\n",idmap[pair.first],idmap[pair.second]);
    }

    const string simP_file_path = root_dir + "/similar_pairs/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
    writeSimilarPair(simP_file_path, joiner.result_pairs);
}