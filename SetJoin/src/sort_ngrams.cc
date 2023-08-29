/*
This is a file that extract the set (unqiue elements and sort them) of the Redpajama datasets
Also will sort them based on their sizes

And then output the sorted sets in the datasets directory and their idmap.
*/

#include <bits/stdc++.h>
#include <execution>
#include "../src/util/io.h"
#include <omp.h>

using namespace std;

int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]);
    const string ngram_path = "/data/local/ngrams/" + dataset_name + ".bin"; // arxiv_bottom_64.bin;

    string idmap_file_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name + "_idmap.bin";
    string sortedngram_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name + "_sortedngram.bin";

    // Input bottom_k and sort them
    vector<vector<unsigned int>> ngrams;
    loadIntBin(ngram_path, ngrams);
    // Sort ngrams based on their length, elements
    vector<int> idmap_ngrams;
    for (int i = 0; i < ngrams.size(); i++)
        idmap_ngrams.emplace_back(i);

    // sort dataset by size first, element second, id third
    cout << "Generating idmap " << endl;
    sort(execution::par_unseq, idmap_ngrams.begin(), idmap_ngrams.end(), [&ngrams](const unsigned int &id1, const unsigned int &id2) {
        auto dsize1 = ngrams[id1].size();
        auto dsize2 = ngrams[id2].size();
        if (dsize1 < dsize2)
            return true;
        else if (dsize1 > dsize2)
            return false;
        else {
            for (int i = 0; i < dsize1; i++)
            {
                if (ngrams[id1][i] < ngrams[id2][i])
                    return true;
                else if (ngrams[id1][i] > ngrams[id2][i])  
                    return false;
            }
            if (id1 < id2)
                return true;
            else
                return false;
        } });

    // write the idmap and the sorted set
    cout << "Writing idmap and the sorted ngrams" << endl;
    writeVec2Bin(idmap_file_path, idmap_ngrams);

    ofstream ofs;
    ofs.open(sortedngram_path.c_str(), ios::binary);
    for (auto i = 0; i < idmap_ngrams.size(); i++) {
        const auto &vec = ngrams[idmap_ngrams[i]];
        unsigned int size = vec.size();

        ofs.write((char *)&size, sizeof(unsigned int));
        ofs.write((char *)&vec[0], size * sizeof(unsigned int)); // Write the vector data
    }
    ofs.close();
}