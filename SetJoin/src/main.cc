#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
#include "../src/set_join/SetJoin.h"
using namespace std;

// It is the id map of sorted bottom_k to original_bottomk

int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = "book"; 
    const string sortedsets_file_path = root_dir + "/sorted_sets/" + dataset_name +"_sortedsets.bin";
    const string idmap_file_path = root_dir + "/sorted_sets/" +  dataset_name + "_idmap.bin";

    // Load Idmap and sortedsets
    vector<int> idmap;
    loadBin2vec(idmap_file_path, idmap);
    vector<vector<unsigned short>> sorted_sets;
    loadShortBin(sortedsets_file_path, sorted_sets);

    // sorted_sets.resize(10);
    // for(auto const & vec: sorted_sets){
    //     for(auto const & shor: vec){
    //         cout<< shor <<" ";
    //     }
    //     cout<<endl;
    //     cout<<vec.size()<<endl;
    // }
    // Use setjoin

    cout<<"Start Set Join"<<endl;
    SetJoin joiner(sorted_sets);
    joiner.setjoin(0.9);

    // Investigate the result
    printf("joiner.result_pairs have %lu pairs\n", joiner.result_pairs.size());
    printf("The amount of document that occur in the pairs is %lu\n", getUniqueInts(joiner.result_pairs).size());
    // Print the result pairs
    for(auto i =0; i< joiner.result_pairs.size(); i ++){
        auto& pair =  joiner.result_pairs[i];
        printf("%d %d\n",pair.first,pair.second);
        printf("%d %d\n",idmap[pair.first],idmap[pair.second]);
    }
}