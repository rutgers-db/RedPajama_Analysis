
#include <bits/stdc++.h>
#include "omp.h"
#include "util/IO.hpp"
#include "NNDescent/NNDescent.hpp"
#include "util/Util.hpp"
using namespace std;

int main(int argc, char **argv) {
    int k = 10;

    // File Paths
    const string dataset_name = "stackexchange";
    double ratio = 0.1;
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string sortedsets_file_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name +"_sortedngram.bin";
    vector<vector<unsigned int>> sorted_sets;
    loadIntBin(sortedsets_file_path, sorted_sets);
    printf("Loaded Data");
    // Convert it into bottomk
    pair<unsigned int, unsigned int> hf; // hash function
    generateHashFunc(1, hf);
    auto timer_st = LogTime();
#pragma omp parallel for
    for(unsigned int i = 0; i<sorted_sets.size(); i++){
        build_bks(sorted_sets[i], hf);
        sorted_sets[i].resize((unsigned int) ceil(ratio*sorted_sets[i].size()));
    }
    printf("Hashing bottomk costs: %f\n", RepTime(timer_st));
 
    const string sample_data_path = "./data/samples.bin";

    // Calculate the sample size
    double sample_ratio = 0.1;
    size_t sample_size = static_cast<size_t>(sorted_sets.size() * sample_ratio);
    // Create a vector of indices
    std::vector<size_t> indices(sorted_sets.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Initialize a random engine
    std::random_device rd;
    std::mt19937 engine(rd());

    // Shuffle the indices
    std::shuffle(indices.begin(), indices.end(), engine);

    // Use the shuffled indices to pick elements from the original vector
    // Write it
    ofstream ofs;
    ofs.open(sample_data_path.c_str(), ios::binary);

    for(size_t i = 0; i < sample_size; ++i) {
        auto & vec = sorted_sets[indices[i]];
        unsigned int size = vec.size();
        ofs.write((char *) &size, sizeof(unsigned int));                                     // Write the size of the vector
        ofs.write(reinterpret_cast<const char *>(vec.data()), size * sizeof(unsigned int)); // Write the vector data
    }

    cout << sample_data_path << " written";
    return 0;
}
