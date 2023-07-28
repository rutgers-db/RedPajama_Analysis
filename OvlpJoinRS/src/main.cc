#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
#include "../src/overlap_rsjoin/OvlpRSJoin.h"
using namespace std;

// It is the id map of sorted bottom_k to original_bottomk
vector<int> idmap_bottomk;

int main(int argc, char *argv[]) {

    // string variables
    const string root_dir =  "/research/projects/zp128/RedPajama_Analysis/OvlpJoinRS";
    const string bottomK_dir = "/research/projects/zp128/RedPajama_Analysis/OverlapJoin/bottomK_bins/";
    const string bottomK_fileName = string(argv[1]);
    const string bottomK_path = bottomK_dir + bottomK_fileName; //arxiv_bottom_64.bin;
    const string dataset = extract_prefix(bottomK_path);
    const string sortedset_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin/sorted_sets/";
    const string sortedset_path = sortedset_dir + dataset + "_sortedsets.bin";
    const string sortedset_idmap_path = sortedset_dir + dataset + "_idmap.bin";
    
    // OverlapJoin Parameters
    int K = 32;
    srand(0); // set seed for random generator
    int c = 29;

    // Load Bottom k
    // Input bottom_k and shrink their size to the specified K
    vector<vector<unsigned short>> bottomks;
    loadShortBin(bottomK_path, bottomks);
    for (auto &bottom_k : bottomks) {
        if (bottom_k.size() > K)
            bottom_k.resize(K);

        assert(bottom_k.size() > 0);
    }

    // Sort bottomks based on their length, elements
    idmap_bottomk.clear();
    for (auto i = 0; i < bottomks.size(); i++)
        idmap_bottomk.emplace_back(i);

    // sort dataset by size first, element second, id third
    sort(idmap_bottomk.begin(), idmap_bottomk.end(), [&bottomks](const unsigned short &id1, const unsigned short &id2) {
        int dsize1 = bottomks[id1].size();
        int dsize2 = bottomks[id2].size();
        if (dsize1 < dsize2)
            return true;
        else if (dsize1 > dsize2)
            return false;
        else {
            for (int i = 0; i < dsize1; i++)
            {
                if (bottomks[id1][i] < bottomks[id2][i])
                    return true;
                else if (bottomks[id1][i] > bottomks[id2][i])  
                    return false;
            }
            if (id1 < id2)
                return true;
            else
                return false;
        } });

    vector<vector<unsigned short>> sorted_bottomKs;
    for (int i = 0; i < idmap_bottomk.size(); i++)
        sorted_bottomKs.emplace_back(bottomks[idmap_bottomk[i]]);
    bottomks.clear();

    // Load sorted_sets (the sets of each documents)
    // Also we need to hash them cause the bottomk has been hashed
    // The seed should be 1
    vector<int> sortedsets_idmap;
    loadBin2vec(sortedset_idmap_path, sortedsets_idmap);
    vector<vector<unsigned short>> sorted_sets;
    loadShortBin(sortedset_path, sorted_sets);
    generateHF(1);
    hashDocuments(sorted_sets);
    
    // For debug
    // sorted_bottomKs.resize(100000);
    // sorted_sets.resize(100000);

    // Use overlapJoin
    OvlpRSJoin joiner(sorted_bottomKs, sorted_sets);
    const string simP_dirpath = root_dir + "/similar_pairs/"+dataset+"_simPair_K" + to_string(K) + "_C" + to_string(c) + "/";
    if(createDirectory(simP_dirpath) == false)
        return 0;
    // joiner.set_external_store(simP_dirpath);
    joiner.overlapjoin(c);

    // Investigate the result
    printf("joiner.result_pairs have %lu pairs\n", joiner.result_pairs.size());
    printf("The amount of document that occur in the pairs is %lu\n", getUniqueInts(joiner.result_pairs).size());

    // Print the result pairs
    int early_stop = 0;
    for(auto& pair : joiner.result_pairs){
        printf("%d %d\n",idmap_bottomk[pair.first],sortedsets_idmap[pair.second]);
        early_stop++; if(early_stop>5) break;
    }

    // Write the similar Pair
    if(joiner.if_external_IO == false){
        const string simP_file_path = simP_dirpath + "sim_pairs.bin";
        writeSimilarPair(simP_file_path, joiner.result_pairs);
        const string idmap_file_path = simP_dirpath + "idmap.bin";
        writeVec2Bin(idmap_file_path, idmap_bottomk);
    }
    
}