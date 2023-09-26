/*
This is a file that extract the set (unqiue elements and sort them) of the Redpajama datasets
Also will sort them based on their sizes

And then output the sorted sets in the datasets directory and their idmap.
*/

#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
#include <execution>
#include <omp.h>

using namespace std;

int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]);
    const string ngram_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name +"_sortedngram.bin";
    // Convert them into L sketches
    unsigned int M = 8;
    unsigned int bit_mv = 3;

    string idmap_file_path = root_dir + "/data/ngram/sorted_lsketch/" + dataset_name + "_idmap.bin";
    string sortedlsketch_file_path = root_dir + "/data/ngram/sorted_lsketch/" + dataset_name + "_M"+to_string(M) + ".bin";

    // Input bottom_k and sort them
    vector<vector<unsigned int>> ngrams;
    loadIntBin(ngram_path, ngrams);
    // Sort lsketches based on their length, elements
    vector<int> idmap_lsketch;
    for (int i = 0; i < ngrams.size(); i++)
        idmap_lsketch.emplace_back(i);

    pair<unsigned int, unsigned int> hf; // hash functions
    generateHashFunc(1, hf);
    unsigned long long lsketch_sizes[128] = {0};
    vector<vector<unsigned int>> lsketches(ngrams.size());
#pragma omp parallel for
    for (int i = 0; i < ngrams.size(); i++)
    {   
        auto& lsketch = lsketches[i];
        auto tid = omp_get_thread_num();
        // build bottom k for this document
        for (auto ele : ngrams[i])
        {   
            ele = hval(hf, ele);
            if(ele%M == 0){
                ele >>= bit_mv;
                lsketch.emplace_back(ele);
            }
        }
        sort(lsketch.begin(), lsketch.end());
        lsketch_sizes[tid] += lsketch.size();
    }
    unsigned long long total_sketchsizes=0;
    for (unsigned int i = 0; i< 128;i++){
        total_sketchsizes += lsketch_sizes[i];
    }

    // release the dataset of original ngrams
    std::vector<std::vector<unsigned int>>().swap(ngrams);

    cout<<"Lsketches Generated and their average size is "<<total_sketchsizes/lsketches.size()<<endl;;

    // sort dataset by size first, element second, id third
    cout << "Generating idmap " << endl;
    sort(execution::par_unseq, idmap_lsketch.begin(), idmap_lsketch.end(), [&lsketches](const unsigned int &id1, const unsigned int &id2) {
        int dsize1 = lsketches[id1].size();
        int dsize2 = lsketches[id2].size();
        if (dsize1 < dsize2)
            return true;
        else if (dsize1 > dsize2)
            return false;
        else {
            for (int i = 0; i < dsize1; i++)
            {
                if (lsketches[id1][i] < lsketches[id2][i])
                    return true;
                else if (lsketches[id1][i] > lsketches[id2][i])  
                    return false;
            }
            if (id1 < id2)
                return true;
            else
                return false;
        } });

    // write the idmap and the sorted set
    cout << "Writing idmap and the sorted lsketches" << endl;
    writeVec2Bin(idmap_file_path, idmap_lsketch);

    ofstream ofs;
    ofs.open(sortedlsketch_file_path.c_str(), ios::binary);
    for (int i = 0; i < idmap_lsketch.size(); i++) {
        const auto &vec = lsketches[idmap_lsketch[i]];
        unsigned int size = vec.size();

        ofs.write((char *)&size, sizeof(unsigned int));
        ofs.write((char *)&vec[0], size * sizeof(unsigned int)); // Write the vector data
    }
    ofs.close();
}