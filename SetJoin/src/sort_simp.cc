#include "util/io.h"
#include <bits/stdc++.h>
using namespace std;

void correctPairOrder(vector<pair<int, int>> & pairs){
    for(auto & pair: pairs){
        if(pair.first>= pair.second){
            auto tmp =pair.first;
            pair.first = pair.second;
            pair.second = tmp;
        }
    }
    sort(pairs.begin(), pairs.end());
}

int main(int argc, char *argv[]){
    const string dataset_name = string(argv[1]); 
    const double thres = 0.8;
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string simP_file_path = root_dir + "/similar_pairs/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
    const string sortedsimP_file_path = root_dir + "/sorted_simp/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
    vector<pair<int,int>> pairs;
    readSimilarPair(simP_file_path, pairs);
    
    correctPairOrder(pairs);

    writeSimilarPair(sortedsimP_file_path, pairs);
}