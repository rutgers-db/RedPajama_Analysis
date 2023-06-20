#ifndef _COMBINAION1_H_
#define _COMBINAION1_H_
#include <bits/stdc++.h>
#include "Combination2.h"

extern vector<vector<int>> dataset1, dataset2;
extern bool comp_int(const int a, const int b);
struct combination1 {
    int N;
    int id;
    bool completed;
    vector<int> curr;

    combination1(int d, int beg) :
        completed(false), id(d), N(dataset1[d].size()) {
        if (N < 1 || c > N)
            completed = true;
        for (auto i = 0; i < c; ++i)
            curr.push_back(beg + 1 + i);
    }

    inline int getlastcurr(){
        assert(curr[c-1]<N);
        return curr[c-1];
    }
    // compute next combination1
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
        cout << "combination1 from " << id << " : ";
        for (auto j = 0; j < c; j++)
            cout << dataset1[id][curr[j]] << " ";
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

    void binary(const combination1 &value) {
        auto it = dataset1[id].begin() + curr[0];
        for (int i = 0; i < c; i++) {
            // find the first one not larger than the value
            it = lower_bound(it, dataset1[id].end(), dataset1[value.id][value.curr[i]], comp_int);
            // if get the end, we will increase the last one by 1 and set the rest as max
            if (it == dataset1[id].end()) {
                completed = stepback(i);
                return;
                // if we get the same value, we fill in it
            } else if (*it == dataset1[value.id][value.curr[i]]) {
                curr[i] = distance(dataset1[id].begin(), it);
                // if we get the smaller value, we set the rest as max
            } else {
                curr[i] = distance(dataset1[id].begin(), it);
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

    void binary(const combination2 &value) {
        auto it = dataset1[id].begin() + curr[0];
        for (int i = 0; i < c; i++) {
            // find the first one not larger than the value
            it = lower_bound(it, dataset1[id].end(), dataset2[value.id][value.curr[i]], comp_int);
            // if get the end, we will increase the last one by 1 and set the rest as max
            if (it == dataset1[id].end()) {
                completed = stepback(i);
                return;
                // if we get the same value, we fill in it
            } else if (*it == dataset2[value.id][value.curr[i]]) {
                curr[i] = distance(dataset1[id].begin(), it);
                // if we get the smaller value, we set the rest as max
            } else {
                curr[i] = distance(dataset1[id].begin(), it);
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

#endif