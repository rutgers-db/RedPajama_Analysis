#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
#include "../src/overlap_join/OvlpJoinParelled.h"
using namespace std;

int main()
{
    // global variables
    const string bottomK_path = "/research/projects/zp128/RedPajama_Analysis/OverlapJoin/bottomK_bins/c4_bottomK_32.bin";
    // const int max_k = 1024;
    int K = 32;
    srand(0); // set seed for random generator

    // OverlapJoin Parameters
    int c = 3;

    // Input bottom_k and shrink their size to the specified K
    vector<vector<unsigned short>> bottomks;
    loadShortBin(bottomK_path, bottomks);

    // just for debug
    bottomks.resize(int(1e7));
    print_memory();
    for (auto &bottom_k : bottomks)
    {
        if (bottom_k.size() > K)
            bottom_k.resize(K);

        assert(bottom_k.size() > 0);
    }

    print_memory();
    // Sort bottomks based on their length, elements
    vector<int> idmap;
    idmap.clear();
    for (auto i = 0; i < bottomks.size(); i++)
        idmap.emplace_back(i);

    // sort dataset by size first, element second, id third
    sort(idmap.begin(), idmap.end(), [&bottomks](const unsigned short &id1, const unsigned short &id2)
         {
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
    for (int i = 0; i < idmap.size(); i++)
        sorted_bottomKs.emplace_back(bottomks[idmap[i]]);
    bottomks.clear();
    print_memory();

    // Use overlapJoin
    OvlpJoinParelled joiner(sorted_bottomKs);
    joiner.overlapjoin(c);

    // Investigate the result
    joiner.get_results();
    printf("joiner.result_pairs have %lu pairs\n", joiner.result_pairs.size());
    printf("The amount of document that occur in the pairs is %lu\n", getUniqueInts(joiner.result_pairs).size());
    // Print the result pairs
    // for(auto& pair : joiner.result_pairs){
    //     printf("%d %d\n",idmap[pair.first],idmap[pair.second]);
    // }
}