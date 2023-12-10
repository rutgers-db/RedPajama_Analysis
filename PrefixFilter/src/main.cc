#include <bits/stdc++.h>
#include "util/io.h"
#include "util/util.h"
#include "prefixfilter/PrefixFilter.h"

// Command Example: ./bin/main.exe stackexchange 0.8 0.2
using namespace std;
int main(int argc, char *argv[]) {
    // Parameters
    // const string dataset_name = string(argv[1]); 
    // const double thres = strtod(argv[2], nullptr);
    // const double ratio = strtod(argv[3], nullptr);

    // File Paths
    // const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    // const string sortedsets_file_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name +"_sortedngram.bin";
    // const string simP_file_path = "/data/ngram/ngrams_simp/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+ "_"+to_string(ratio)+  ".bin";

    // vector<vector<unsigned int>> sorted_sets;
    // loadIntBin(sortedsets_file_path, sorted_sets);
    // vector<unsigned int> sets_size(sorted_sets.size());
    
    // Convert it into bottomk
    pair<unsigned int, unsigned int> hf; // hash function
    generateHashFunc(1, hf);

    auto timer_st = LogTime();

// #pragma omp parallel for
//     for(unsigned int i = 0; i<sorted_sets.size(); i++){
//         sets_size[i] = sorted_sets[i].size();
//         build_bks(sorted_sets[i], hf);
//         sorted_sets[i].resize((unsigned int) ceil(ratio*sets_size[i]));
//     }
//     printf("Hashing bottomk costs: %f\n", RepTime(timer_st));
    
    // // Sort hashes bottomk
    // timer_st = LogTime();
    // vector<unsigned int> idmap;
    // for (int i = 0; i < sorted_sets.size(); i++)
    //     idmap.emplace_back(i);
    // sort(idmap.begin(), idmap.end(), [&sorted_sets](const unsigned int &id1, const unsigned int &id2) {
    //     auto dsize1 = sorted_sets[id1].size();
    //     auto dsize2 = sorted_sets[id2].size();
    //     if (dsize1 < dsize2)
    //         return true;
    //     else if (dsize1 > dsize2)
    //         return false;
    //     else {
    //         for (auto i = 0; i < dsize1; i++)
    //         {
    //             if (sorted_sets[id1][i] < sorted_sets[id2][i])
    //                 return true;
    //             else if (sorted_sets[id1][i] > sorted_sets[id2][i])  
    //                 return false;
    //         }
    //         if (id1 < id2)
    //             return true;
    //         else
    //             return false;
    //     } });
    // vector<vector<unsigned int>> sorted_bks;
    // vector<unsigned int> bks_oriSize ;
    // for(auto& id: idmap){
    //     sorted_bks.emplace_back(sorted_sets[id]);
    //     bks_oriSize.emplace_back(sets_size[id]);
    // }
    // sorted_sets.clear();
    // sorted_sets.shrink_to_fit();
    // printf("Sorting bottomk costs: %f\n", RepTime(timer_st));

    timer_st = LogTime();
    
    ofstream ofs;
    string bottomk = "./sample.bin";
    string size_path = "./ori_size.bin";
    
    // Then do the prefix filter
    vector<vector<unsigned int>> sorted_bks;
    loadIntBin(bottomk, sorted_bks);
    vector<unsigned int> sets_size;
    loadBin2vec(size_path, sets_size);
    double thres = 0.8;

    cout<<"Start Prefix filtering"<<endl;
    PrefixFilter pfilter(sorted_bks, sets_size);
    pfilter.init();
    pfilter.group(thres);
    pfilter.find_simp();

    // Output the similar pairs
    const string simP_file_path = "sample_simp.bin";
    writeSimilarPairs(simP_file_path, pfilter.result_pairs);
    printf("At last the total time cost is: %f\n", RepTime(timer_st));
}