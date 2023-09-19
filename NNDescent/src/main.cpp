
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
    const string sortedsets_file_path = "./data/samples.bin";
    vector<vector<unsigned int>> sorted_sets;
    loadIntBin(sortedsets_file_path, sorted_sets);
    printf("Loaded Data\n");
    // Convert it into bottomk

    auto timer_st = LogTime();

    // Early termination coefficient
    double delta = 0.25*1e-3;
    // Sampling coefficient
    double rho = 1;
    // max iterations
    int maxIterations = 50;

    // Initialize NNDescent
    NNDescent<vector<unsigned int>> builder(rho, delta, maxIterations, k);

    auto start = chrono::high_resolution_clock::now();
    auto exp = builder.computeGraph(sorted_sets, Jaccard<unsigned int>);

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "NNDescent:  " << duration.count() / 1000000.0 << " seconds" << endl;
    return 0;
}
