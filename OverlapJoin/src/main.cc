#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/overlap_join/OvlpJoin.h"
using namespace std;

int main()
{
    // global variables
    const string bottomK_path = "/research/projects/zp128/RedPajama_Analysis/OverlapJoin/bottomK_bins/arxiv_bottomK_1024.bin";
    const int max_k = 1024;
    int K = 64;
    srand(0); // set seed for random generator
    // OverlapJoin Parameters
    int c = 58;

    // Input bottom_k and shrink their size to the specified K
    vector<vector<unsigned short>> bottomks;
    loadShortBin(bottomK_path, bottomks);
    for (auto &bottom_k : bottomks)
    {

        // // If should be deleted later just for debug. Unique Operation
        // auto uniq_it = unique(bottom_k.begin(), bottom_k.end());
        // bottom_k.resize(distance(bottom_k.begin(), uniq_it)); // 10 20 30 20 10
        if (bottom_k.size() > K)
            bottom_k.resize(K);
    }

    // // choose only 100 elements delete later
    bottomks.resize(100);
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

    // Use overlapJoin
    OvlpJoin joiner(sorted_bottomKs, false);
    joiner.overlapjoin(c);

    // Investigate the result
    joiner.get_results();
    printf("joiner.result_pairs have %lu pairs\n", joiner.result_pairs.size());
}