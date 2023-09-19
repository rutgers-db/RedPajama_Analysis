/**
 * Bruteforce algorithm
 * 
 * Author: Weiqi Feng
 * Date:   Jan 30, 2020
 * Email:  fengweiqi@sjtu.edu.cn
 */  
#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

#include <iostream>
#include <vector>
#include <functional>
#include <utility>
#include <algorithm>

using namespace std;
template <class T>
class BruteForce
{
private:
    int _k;
    struct cmp {
        bool operator()(const pair<int, double> &lhs, const pair<int, double> &rhs) {
            return lhs.second < rhs.second;
        }
    };
public:
    BruteForce(int k): _k(k){};
    vector<vector<int>> computeGraph(const vector<T> &nodes, function<double(T, T)> fn);
    vector<vector<int>> computePartialGraph(const vector<T> &nodes, const vector<int> &tests, function<double(T, T)> fn);
};


template <class T>
vector<vector<int>> BruteForce<T>::computeGraph(const vector<T> &nodes, function<double(T, T)> fn) {
    vector<vector<pair<int, double> >> dis(nodes.size(), vector<pair<int, double> >());

    for (int i = 0; i < nodes.size(); i++) {
        for (int j = 0; j < nodes.size(); j++) {
            if (i != j) {
                // Insert into distance matrix
                dis[i].push_back(pair<int, double>(j, fn(nodes[i], nodes[j])));
            }
        }
        sort(dis[i].begin(), dis[i].end(), cmp());
    }
    vector<vector<int>> res(nodes.size(), vector<int>());
    for (int j = 0; j < nodes.size(); j++) {
        for (int i = _k - 1; i >= 0; --i) {
            res[j].push_back(dis[j][i].first);
        }
    }
    return res;
}




template <class T>
vector<vector<int>> BruteForce<T>::computePartialGraph(const vector<T> &nodes, const vector<int> &tests, function<double(T, T)> fn) {

    vector<vector<pair<int, double> >> dis(tests.size(), vector<pair<int, double> >());

    #pragma omp parallel for
    for (int i = 0; i < tests.size(); i++) {
        for (int j = 0; j < nodes.size(); j++) {
            if (tests[i] != j) {
                // Insert into distance matrix
                dis[i].push_back(pair<int, double>(j, fn(nodes[tests[i]], nodes[j])));
            }
        }
        sort(dis[i].begin(), dis[i].end(), cmp());
    }
    
    vector<vector<int>> res(tests.size(), vector<int>());
    #pragma omp parallel for
    for (int j = 0; j < tests.size(); j++) {
        for (int i = _k - 1; i >= 0; --i) {
            res[j].push_back(dis[j][i].first);
        }
    }
    return res;
}

#endif