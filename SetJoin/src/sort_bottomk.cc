/*
This is a file that extract the set (unqiue elements and sort them) of the Redpajama datasets
Also will sort them based on their sizes

And then output the sorted sets in the datasets directory and their idmap.
*/

#include <bits/stdc++.h>
#include "../src/util/io.h"
#include <omp.h>

using namespace std;

int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]);
    const string bottomk_path = "/research/projects/zp128/RedPajama_Analysis/OverlapJoin/bottomK_bins/" + dataset_name + "_bottomK_1024.bin"; // arxiv_bottom_64.bin;

    string idmap_file_path = root_dir + "/sorted_bottomk/" + dataset_name + "_idmap.bin";
    string sortedbottomk_file_path = root_dir + "/sorted_bottomk/" + dataset_name + "_sortedbottomk.bin";

    // Input bottom_k and sort them
    vector<vector<unsigned short>> bottomks;
    loadShortBin(bottomk_path, bottomks);
    // Sort bottomks based on their length, elements
    vector<int> idmap_bottomk;
    for (int i = 0; i < bottomks.size(); i++)
        idmap_bottomk.emplace_back(i);

    // sort dataset by size first, element second, id third
    cout << "Generating idmap " << endl;
    sort(idmap_bottomk.begin(), idmap_bottomk.end(), [&bottomks](const unsigned int &id1, const unsigned int &id2) {
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

    // write the idmap and the sorted set
    cout << "Writing idmap and the sorted bottomks" << endl;
    writeVec2Bin(idmap_file_path, idmap_bottomk);

    ofstream ofs;
    ofs.open(sortedbottomk_file_path.c_str(), ios::binary);
    for (int i = 0; i < idmap_bottomk.size(); i++) {
        const auto &vec = bottomks[idmap_bottomk[i]];
        int size = vec.size();

        ofs.write((char *)&size, sizeof(int));
        ofs.write((char *)&vec[0], size * sizeof(unsigned short)); // Write the vector data
    }
    ofs.close();
}