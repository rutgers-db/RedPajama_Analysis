#include "util/io.h"
#include <bits/stdc++.h>
using namespace std;

std::set<unsigned int> extract_unique_ids(const std::vector<std::pair<unsigned int, unsigned int>>& pairs) {
    std::set<unsigned int> unique_ids;

    for (const auto& p : pairs) {
        unique_ids.insert(p.first);
        unique_ids.insert(p.second);
    }

    return unique_ids;
}


double jaccard_similarity(const std::vector<unsigned short>& text1, const std::vector<unsigned short>& text2) {
    std::unordered_set<unsigned short> set1(text1.begin(), text1.end());
    std::unordered_set<unsigned short> set2(text2.begin(), text2.end());

    std::vector<unsigned short> intersection;
    std::vector<unsigned short> union_set;

    // calculate intersection
    for (auto& element : set1) {
        if (set2.count(element)) {
            intersection.push_back(element);
        }
    }

    // calculate union
    union_set = text1;
    union_set.insert(union_set.end(), text2.begin(), text2.end());

    std::sort(union_set.begin(), union_set.end());
    union_set.erase(std::unique(union_set.begin(), union_set.end()), union_set.end());

    return static_cast<double>(intersection.size()) / union_set.size();
}

bool verify_jaccard_similarity(std::vector<std::pair< unsigned int,  unsigned int>> text_pairs,
                               std::vector<std::vector<unsigned short>> texts, double threshold) {
    for (const auto& pair : text_pairs) {
        if (jaccard_similarity(texts[pair.first], texts[pair.second]) < threshold) {
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[]){
    const string dataset_name = string(argv[1]); 
    const double thres = 0.8;
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string sortedsets_file_path = root_dir + "/sorted_sets/" + dataset_name +"_sortedsets.bin";
    // const string idmap_file_path = root_dir + "/sorted_sets/" +  dataset_name + "_idmap.bin";
    const string sortedsimP_file_path = root_dir + "/sorted_simp/"+ dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";

    vector<pair<unsigned int,unsigned int>> pairs;
    // vector<int> idmap;
    vector<vector<unsigned short>> sorted_sets;

    loadShortBin(sortedsets_file_path, sorted_sets);
    // loadBin2vec(idmap_file_path, idmap);
    readSimilarPair(sortedsimP_file_path, pairs);

    printf("The amount of pairs is: %lu, the amount of unique documents is %lu\n ", pairs.size(), extract_unique_ids(pairs).size());
    if (verify_jaccard_similarity(pairs,sorted_sets, thres) == false){
        cout <<"Fail"<<endl;
    }else{
        cout<<"Success"<<endl;
    }
    
}