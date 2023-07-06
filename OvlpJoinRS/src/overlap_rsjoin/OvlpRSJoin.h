#ifndef _OVLPJOINPARELL_H_
#define _OVLPJOINPARELL_H_

#include <bits/stdc++.h>
#include <sys/time.h>
#include <time.h>

const int MAX_THREAD = 128;

using namespace std;

struct combination;

extern int c;
extern vector<vector<unsigned short>> dataset1, dataset2; // datasets
/*
  This is a class that uses overlapjoin for two datasets (Set R and set S) to join them
  We gonna implement it in a parelled method
*/
class OvlpRSJoin {
public:
    int n1, n2; // R S sizes
    // int total_eles;
    // vector<vector<int>> heap;
    // vector<int> heap1, heap2; // heap
    vector<vector<unsigned short>> records1, records2; // two sets

    vector<pair<int, int>> idmap_records1, idmap_records2;
    vector<vector<pair<int, int>>> ele_lists1, ele_lists2;
    vector<pair<int, int>> result_pairs;

    int64_t candidate_num;
    int64_t result_num;

    void overlapjoin(int overlap_threshold);
    void small_case(int L1, int R1, int L2, int R2);

    bool if_external_IO = false;
    string resultPair_storePath;

    OvlpRSJoin(vector<vector<unsigned short>> &sorted_records_1, vector<vector<unsigned short>> &sorted_records_2) {
        // reset everything
        c = 0;

        records1 = sorted_records_1;
        records2 = sorted_records_2;
    }

    void set_external_store(string _resPair_path){
        if_external_IO = true;
        resultPair_storePath = _resPair_path;
    }

    // void save_idmap(string _resPair_path);
};
#endif
