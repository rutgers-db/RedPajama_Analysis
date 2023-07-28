#ifndef _COMBINAION1_H_
#define _COMBINAION1_H_
#include "OvlpRSJoin.h"
#include <bits/stdc++.h>
// extern vector<vector<int>> dataset1, dataset2;
// extern vector<vector<int>> dataset1;
struct combination2;
struct combination1{
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
    void next();

    void print() const;

    bool stepback(const int i);

    void binary(const combination1 &value);
    void binary(const combination2 &value);
};
#endif