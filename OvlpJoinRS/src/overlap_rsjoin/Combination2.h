#ifndef _COMBINAION2_H_
#define _COMBINAION2_H_
#include "OvlpRSJoin.h"
#include "Combination1.h"
#include <bits/stdc++.h>
// extern vector<vector<int>> dataset2;
struct combination2{
    int N;
    int id;
    bool completed;
    vector<int> curr;

    combination2(int d, int beg):
        completed(false), id(d), N(dataset2[d].size()) {
        if (N < 1 || c > N)
            completed = true;
        for (auto i = 0; i < c; ++i)
            curr.push_back(beg + 1 + i);
    }

    inline int getlastcurr(){
        assert(curr[c-1]<N);
        return curr[c-1];
    }

    // compute next combination2
    void next();

    void print() const;

    bool stepback(const int i);

    void binary(const combination2 &value);
    void binary(const combination1 &value);

};
#endif