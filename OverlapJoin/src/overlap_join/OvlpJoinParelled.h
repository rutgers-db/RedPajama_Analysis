#ifndef _OVLPJOINPARELL_H_
#define _OVLPJOINPARELL_H_

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <inttypes.h>
#include <queue>
#include <sys/time.h>
#include <time.h>
#include <unordered_map>
#include <unordered_set>
#include <cassert>


const int MAX_THREAD = 128;

using namespace std;
struct combination;

extern int c;
extern vector<vector<unsigned short>> dataset;
extern vector<vector<combination>> combs;
extern bool comp_comb(const int a, const int b);
extern bool comp_pair(const pair<int, int> &p1, const int val);
extern bool comp_int(const int a, const int b);
struct combination {
    int N;
    int id;
    bool completed;
    vector<int> curr;

    combination(int d, int beg) :
        completed(false), id(d), N(dataset[d].size()) {
        if (N < 1 || c > N)
            completed = true;
        for (auto i = 0; i < c; ++i)
            curr.push_back(beg + 1 + i);
    }

    inline int getlastcurr(){
        assert(curr[c-1]<N);
        return curr[c-1];
    }
    // compute next combination
    void next() {
        int i = c - 1;
        while (i >= 0 && curr[i] == N - c + i)
            --i;
        if (i < 0)
            completed = true;
        else {
            int temp = curr[i];
            for (int j = i; j <= c - 1; j++)
                curr[j] = temp + 1 + j - i;
        }
    }

    void print() const {
        cout << "combination from " << id << " : ";
        for (auto j = 0; j < c; j++)
            cout << dataset[id][curr[j]] << " ";
        cout << " ----> ";
        for (auto j = 0; j < c; j++)
            cout << curr[j] << " ";
        cout << endl;
    }

    bool stepback(const int i) {
        if (i == 0)
            return true;
        curr[i - 1]++;
        if (curr[i - 1] + c - 1 - i + 1 >= N)
            return stepback(i - 1);
        for (int j = i; j < c; j++)
            curr[j] = curr[i - 1] + j - i + 1;
        return false;
    }

    void binary(const combination &value) {
        auto it = dataset[id].begin() + curr[0];
        for (int i = 0; i < c; i++) {
            // find the first one not larger than the value
            it = lower_bound(it, dataset[id].end(), dataset[value.id][value.curr[i]], comp_int);
            // if get the end, we will increase the last one by 1 and set the rest as max
            if (it == dataset[id].end()) {
                completed = stepback(i);
                return;
                // if we get the same value, we fill in it
            } else if (*it == dataset[value.id][value.curr[i]]) {
                curr[i] = distance(dataset[id].begin(), it);
                // if we get the smaller value, we set the rest as max
            } else {
                curr[i] = distance(dataset[id].begin(), it);
                if (curr[i] + c - 1 - i >= N) {
                    completed = stepback(i);
                    return;
                }
                for (int j = i + 1; j < c; j++)
                    curr[j] = curr[i] + j - i;
                return;
            }
        }
        return;
    }
};

class OvlpJoinParelled {
public:
    int n;
    int total_eles;
    vector<vector<int>> heap;   
    vector<vector<unsigned short>> records;
    vector<pair<int, int>> idmap_records;
    vector<vector<pair<int, int>>> ele_lists;
    vector<pair<int, int>> result_pairs;

    int64_t candidate_num;
    int64_t result_num;

    void overlapjoin(int overlap_threshold, int _k);
    bool is_equal(const combination &c1, const combination &c2);
    void small_case();

    bool if_external_IO = false;
    string resultPair_storePath;

    OvlpJoinParelled(vector<vector<unsigned short>> &sorted_records) {
        // reset everything
        c = 0;
        n = 0;
        total_eles = 0;

        heap.clear();
        heap.resize(MAX_THREAD);
        dataset.clear();
        records.clear();
        idmap_records.clear();
        ele_lists.clear();
        result_pairs.clear();

        combs.clear();
        combs.resize(MAX_THREAD);

        candidate_num = 0;
        result_num = 0;

        records = sorted_records;
    }

    void set_external_store(string _resPair_path){
        if_external_IO = true;
        resultPair_storePath = _resPair_path;
    }

    void save_idmap(string _resPair_path);
};
#endif
