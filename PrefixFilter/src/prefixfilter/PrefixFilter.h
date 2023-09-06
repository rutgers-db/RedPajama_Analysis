
#ifndef _PREFIXFILTER_H_
#define _PREFIXFILTER_H_

#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

// Maximum number of threads that can be used
#define MAXTHREADNUM 128

class PrefixFilter{
public:
    double det; // Similarity Threshold

    vector<vector<unsigned int>> dataset;  // Sorted Bottomk (Order by their Size)
    vector<unsigned int> ori_lens;         // Their Original Length

    // Array to store result pairs for each thread
    vector<pair<int, int>> result_pairs[MAXTHREADNUM];

    PrefixFilter(vector<vector<unsigned int>> &sorted_bks, vector<unsigned int> & orignal_records_sizes){
        dataset = sorted_bks;
        ori_lens = orignal_records_sizes;
    }

    // Function to check overlap between two bottomk
    bool overlap(unsigned int x, unsigned int y);

    void group(double thres);
    void find_simp();

};
#endif
