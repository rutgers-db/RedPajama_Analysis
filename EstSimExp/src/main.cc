/*
    Randomly extract 1e7 pairs in the dataset
*/
#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
using namespace std;

unordered_map<unsigned int, vector<unsigned short>> bottomk_mp;

std::vector<std::pair<unsigned int, unsigned int>> 
randomlyChoosePairs(const std::vector<std::vector<unsigned short>>& docs, 
                    unsigned int Pairs_NUM, size_t len_limit) {

    // Create a list of valid IDs
    std::vector<unsigned int> valid_ids;
    for (unsigned int i = 0; i < docs.size(); ++i) {
        if (docs[i].size() >= len_limit) {
            valid_ids.push_back(i);
        }
    }

    // Vector to hold the chosen pairs
    std::vector<std::pair<unsigned int, unsigned int>> chosen_pairs;

    // Randomly select pairs
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, valid_ids.size() - 1);

    while (Pairs_NUM > 0 && valid_ids.size() > 1) {
        unsigned int idx1 = dist(gen);
        unsigned int idx2 = dist(gen);
        while (idx2 == idx1) {  // ensure they are distinct
            idx2 = dist(gen);
        }

        chosen_pairs.push_back({valid_ids[idx1], valid_ids[idx2]});
        Pairs_NUM--;

        // Optional: To ensure the same pair isn't chosen again, you could remove 
        // these indices from valid_ids, but that will decrease the pool size quickly.
    }
    return chosen_pairs;
}

void generateHashFunc(unsigned int seed, pair<unsigned short, unsigned short> &hf) {
    srand(seed);
    unsigned short a = 0;
    while (a == 0)
        a = rand();
    unsigned short b = rand();
    hf.first = a;
    hf.second = b;
}

// The hash value function
inline unsigned short hval(const pair<unsigned short, unsigned short> &hf, unsigned short &word) {
    return hf.first * word + hf.second;
}

// build bottomks
void build_bks(const vector<vector<unsigned short>>& docs, const vector<pair<unsigned int, unsigned int>> &pairs){
    pair<unsigned short, unsigned short> hf; // hash functions
    generateHashFunc(1, hf);

    auto vec_ids = getUniqueInts(pairs);
    for(int i = 0 ;i< vec_ids.size(); i++){
        auto id = vec_ids[i];
        // build bottom k for this document
        bottomk_mp[id] = docs[id];
    }

#pragma omp parallel for
    for(int i = 0 ;i< vec_ids.size(); i++){
        auto id = vec_ids[i];
        // build bottom k for this document
        for(auto& ele: bottomk_mp[id]){
            ele = hval(hf,ele);
        }
        sort(bottomk_mp[id].begin(), bottomk_mp[id].end());
    }
}

int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]); 
    const string sortedsets_file_path = root_dir + "/sorted_sets/" + dataset_name +"_sortedsets.bin";
    const unsigned int min_k = stoul(argv[2]);
    double ratio = 0.4;
    // Load sortedsets
    vector<vector<unsigned short>> sorted_sets;
    loadShortBin(sortedsets_file_path, sorted_sets);

    // randomly choose paris
    unsigned int pairs_num = 1e6;
    auto pairs = randomlyChoosePairs(sorted_sets, pairs_num, min_k);

    // building their bottom ks
    build_bks(sorted_sets, pairs);

    cout<<"Iterating each pair"<<endl;

    // Iterate each pair and calculate its estimated jaccards
    vector<double> true_jaccard(pairs.size());
    vector<double> fixk_bottomk(pairs.size());
    vector<double> fixk_jaccard(pairs.size());
    vector<double> adpativek_bottomk(pairs.size());
    vector<double> adpativek_jaccard(pairs.size());
    vector<unsigned int> adaptiveKs(pairs.size());
    double error[4]={0,0,0,0};

    vector<vector<double>> data(pairs.size(), vector<double>(5));
#pragma omp parallel for
    for(int i = 0 ;i<pairs.size();i++){
        true_jaccard[i] = jaccard_similarity(sorted_sets[pairs[i].first], sorted_sets[pairs[i].second]);
        
        vector<unsigned short> bk_A(bottomk_mp[pairs[i].first].begin(), bottomk_mp[pairs[i].first].begin() + min_k );
        vector<unsigned short> bk_B(bottomk_mp[pairs[i].second].begin(), bottomk_mp[pairs[i].second].begin() + min_k );

        fixk_bottomk[i] = bottomKJaccard(bk_A, bk_B);
        fixk_jaccard[i] = jaccard_similarity(bk_A, bk_B);

        auto min_adpativek = int(min(bottomk_mp[pairs[i].first].size()*ratio,  bottomk_mp[pairs[i].second].size()*ratio));
        adaptiveKs[i] = min_adpativek;
        vector<unsigned short> adpbk_A(bottomk_mp[pairs[i].first].begin(), bottomk_mp[pairs[i].first].begin() + min_adpativek );
        vector<unsigned short> adpbk_B(bottomk_mp[pairs[i].second].begin(), bottomk_mp[pairs[i].second].begin() + min_adpativek );

        adpativek_bottomk[i] = bottomKJaccard(adpbk_A, adpbk_B);
        adpativek_jaccard[i] = jaccard_similarity(adpbk_A, adpbk_B);

        // record data
        data[i][0] = true_jaccard[i];
        data[i][1] = fixk_bottomk[i];
        data[i][2] = fixk_jaccard[i];
        data[i][3] = adpativek_bottomk[i];
        data[i][4] = adpativek_jaccard[i];

    }

    const string csv_path = "./csv/" + dataset_name + "_" + to_string(min_k) + ".csv";
    writeEstJaccardCSV(csv_path, data);

    // Record error
    unsigned long long ave_adpk = 0;
    for(int i = 0 ;i<pairs.size();i++){
        error[0] += abs(fixk_bottomk[i] - true_jaccard[i]);
        error[1] += abs(fixk_jaccard[i] - true_jaccard[i]);
        error[2] += abs(adpativek_bottomk[i] - true_jaccard[i]);
        error[3] += abs(adpativek_jaccard[i] - true_jaccard[i]);
        ave_adpk += adaptiveKs[i];
    }

    // Average errors
    ave_adpk /= true_jaccard.size();
    for(auto & e: error){
        e = e/true_jaccard.size();
    }
    // print out the result
    // printf("Now print out errors:\n |fixk_bottomk|fixk_jaccard| adpativek_bottomk| adpativek_jaccard|\n");
    printf("|%s|%.3f |%.3f|%.3f| %.3f |\n", dataset_name.c_str(), error[0], error[1], error[2], error[3]);
    // printf("Average adpative k: %llu \n ", ave_adpk);
    return 0;
}