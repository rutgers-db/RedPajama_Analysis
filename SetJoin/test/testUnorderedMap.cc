#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
#include "../src/set_join/SetJoinGroupParelled.h"

using namespace std;

int main(int argc, char *argv[]){
    const string dataset_name = string(argv[1]);//   // "stackexchange" ;
    const double thres = strtod(argv[2], nullptr); 
    unsigned int M = 8;

    // File Paths
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string sortedsets_file_path = root_dir + "/data/ngram/sorted_lsketch/" + dataset_name +"_M"+to_string(M) + ".bin";
    SetJoinGroupParelled joiner(sortedsets_file_path);
    joiner.testUnordered_map(thres);

    return 0;
}