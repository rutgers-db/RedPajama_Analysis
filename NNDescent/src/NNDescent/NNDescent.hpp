/**
 * NN Descent algorithm
 *
 * Author: Weiqi Feng
 * Date:   Jan 30, 2020
 * Email:  fengweiqi@sjtu.edu.cn
 */
#ifndef NNDESCENT_H
#define NNDESCENT_H

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "Heap.hpp"
#include "SuperDescent.hpp"

#include "../util/BruteForce.hpp"
#include "../util/Util.hpp"
#include "../util/Recall.hpp"

using namespace std;

template <class T>
class NNDescent : public SuperDescent<T> {
    // swtich improvement for independent split
    bool improvement_switch = false;
    
    // Number of comparisons
    int cmps = 0;

    // Number of updates
    int c = 0;

public:
    // Default constructor for NN-Descent
    NNDescent() :
        SuperDescent<T>(){};

    // Initialize NN-Descent with given parameters
    NNDescent(double rho, double delta, int maxIterations, int k) :
        SuperDescent<T>(rho, delta, maxIterations, k){};

    // Compute K-NN Graph
    vector<vector<int>> computeGraph(const vector<T> &nodes,
                                     function<double(T, T)> fn);

    // switch improvement
    void switch_improvement(bool flag) {
        improvement_switch = flag;
        cout << "switch improvement : " << flag << endl;
    }
    // local join
    inline void localJoin(const vector<T> &nodes,function<double(T, T)> fn,const vector<vector<int>> &oldLists,const vector<vector<int>> &newLists,vector<Heap> &neighborLists,unsigned id);

    // Generate random neighbor list of node v
    Heap randomNeighborList(const vector<T> &nodes, int node1, function<double(T, T)> fn);

    // Reverse neighborhood lists
    vector<vector<int>> reverse(const vector<T> &nodes,
                                vector<vector<int>> lists);

    // Sample "count" items fron nodeIndice vector
    vector<int> sample(vector<int> nodeIndice, double rate);

    // Return the union of two vectors
    vector<int> unionVecs(vector<int> vec1, vector<int> vec2);

};

// Compute K-NN Graph
template <class T>
vector<vector<int>> NNDescent<T>::computeGraph(const vector<T> &nodes,
                                               function<double(T, T)> fn) {

    if (nodes.size() < SuperDescent<T>::getMinSize()) {
        cout << "Warning: NNDescent should be used for large graphs more than "
                "500 nodes"
             << endl;
    }

    // vector implementations
    vector<vector<int>> oldLists(nodes.size(), vector<int>());
    vector<vector<int>> newLists(nodes.size(), vector<int>());
    vector<Heap> neighborLists;
    vector<vector<int>> adjacencyList(nodes.size(), vector<int>());

    // For each node, create a random neighbor list
    for (int i = 0; i < nodes.size(); ++i) {
        neighborLists.push_back(randomNeighborList(nodes, i, fn));
    }

    // Number of iterations
    unsigned iterations = 0;
    
    // time cost of split
    double split_cost=0;

    // time cost of local join
    double join_cost=0;

    // Generate nodes for calculating recall
    unsigned recall_nodesNum=100;

    Recall<T> recall(recall_nodesNum);
    recall.init(SuperDescent<T>::getK(),nodes.size(), nodes, fn);

    while (true) {
        cout << "Iteration " << iterations << endl;
        iterations++;
        // Have reached max iterations
        if (iterations > SuperDescent<T>::getMaxIterations()) {
            cout << "Reach Max Iteration: " << iterations << endl;
            break;
        }

        // Initialize c
        c = 0;
        // for v ∈ V do
        // old[v]←− all items in B[v] with a false flag
        // new[v]←− ρK items in B[v] with a true flag
        // Mark sampled items in B[v] as false;
        for (int i = 0; i < nodes.size(); i++) {
            oldLists[i] = neighborLists[i].pickFalses();
            //newLists[i] = neighborLists[i].pickTrues();
            adjacencyList[i] = neighborLists[i].pickAll();
            // for debug
            newLists[i] =neighborLists[i].pickTruesAndMark(SuperDescent<T>::getRho());
        }

        // old' = reverse(old)
        // new' = reverse(new)
        vector<vector<int>> oldListsPrime = reverse(nodes, oldLists);
        vector<vector<int>> newListsPrime = reverse(nodes, newLists);
        vector<vector<int>> rev_adLists = reverse(nodes, adjacencyList);

        double rate = SuperDescent<T>::getRho();
        for (int i = 0; i < nodes.size(); i++) {
            vector<int> oldPrimeSample = sample(oldListsPrime[i], rate);
            vector<int> newPrimeSample = sample(newListsPrime[i], rate);

            oldLists[i] = unionVecs(oldLists[i], oldPrimeSample);
            newLists[i] = unionVecs(newLists[i], newPrimeSample);
            adjacencyList[i] = unionVecs(adjacencyList[i], rev_adLists[i]);
        }
        
        auto start = chrono::high_resolution_clock::now();
        
        for (unsigned i = 0; i < nodes.size(); i++) {
            localJoin(nodes,fn,oldLists,newLists,neighborLists,i);
        }   
        
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
        join_cost+= duration.count()/ 1000000.0;
        
        double recallNum = recall.computeRecall(neighborLists);
        //cout << "total calculated num: " << calNum << endl;
        //cout << "unique calculate num: " << caled.size() << endl;
        // cout << "ratio: " << caled.size() * 1.0 / calNum << endl;
        // cout << "new calculated in this round: " << newCalNum << endl;
        cout << "recall: " << recallNum << endl;
        
        cout << "Number of updates: " << c << endl;

        if (recallNum >= 0.9) {
            cout<<"split cost: "<<split_cost <<"seconds" <<endl;
            break;
        }
    }
    cout<<"join cost: "<<join_cost<<"seconds"<<endl;

    // Compute result
    vector<vector<int>> res(nodes.size(), vector<int>());
    for (int i = 0; i < nodes.size(); i++) {
        while (!neighborLists[i].empty()) {
            auto tmp = neighborLists[i].pop();
            res[i].push_back(tmp.first);
        }
    }
    cout << "total cmps: " << cmps << endl;
    cout << "NNDescent scan rate: "
         << double(cmps) * 2 / (nodes.size() * (nodes.size() - 1)) << endl;
    return res;
}

// local join
template <class T>
inline void NNDescent<T>::localJoin(const vector<T> &nodes,function<double(T, T)> fn,const vector<vector<int>> &oldLists,const vector<vector<int>> &newLists,vector<Heap> &neighborLists, unsigned id){
    // Mark those new neighbors to old neighbor Debug
    neighborLists[id].markTrues();

    for (int j = 0; j < newLists[id].size(); j++) {
        // for u1,u2 ∈ new[v], u1 < u2 do
        for (int k = j + 1; k < newLists[id].size(); k++) {
            int u1 = newLists[id][j];
            int u2 = newLists[id][k];
            cmps += 1;

            double s = fn(nodes[u1], nodes[u2]);
            c += neighborLists[u1].insert(pair<int, double>(u2, s));
            c += neighborLists[u2].insert(pair<int, double>(u1, s));
        }
        // or u1 ∈ new[v], u2 ∈ old[v] do
        for (int k = 0; k < oldLists[id].size(); k++) {
            int u1 = newLists[id][j];
            int u2 = oldLists[id][k];
            if (u1 == u2) {
                continue;
            }

            cmps += 1;

            double s = fn(nodes[u1], nodes[u2]);
            c += neighborLists[u1].insert(pair<int, double>(u2, s));
            c += neighborLists[u2].insert(pair<int, double>(u1, s));
        }
    }
}

// Generate random neighbor list of node v
template <class T>
Heap NNDescent<T>::randomNeighborList(const vector<T> &nodes, int node1, function<double(T, T)> fn) {
    Heap hp(SuperDescent<T>::getK());
    while (hp.size() < SuperDescent<T>::getK()) {
        int node2 = random() % nodes.size();
        if (node2 == node1) {
            continue;
        }
        int distance = fn(nodes[node1], nodes[node2]);
        hp.insert(pair<int, double>(node2, distance));
    }
    return hp;
}

// Reverse neighborhood lists
// If j is i 's K-NN, i will be j's reverse K-NN
template <class T>
vector<vector<int>> NNDescent<T>::reverse(const vector<T> &nodes,
                                          vector<vector<int>> lists) {
    vector<vector<int>> res(nodes.size(), vector<int>());
    // Reverse the lists
    for (int i = 0; i < nodes.size(); i++) {
        for (auto it = lists[i].begin(); it < lists[i].end(); it++) {
            res[*it].push_back(i);
        }
    }
    return res;
}

// Sample "count" items fron nodeIndice vector
template <class T>
vector<int> NNDescent<T>::sample(vector<int> nodeIndice, double rate) {
    int count = int(rate * nodeIndice.size());
    while (nodeIndice.size() > count) {
        int offset = random() % nodeIndice.size();
        nodeIndice.erase(nodeIndice.begin() + offset);
    }
    return nodeIndice;
}

// Return the union of two vectors
template <class T>
vector<int> NNDescent<T>::unionVecs(vector<int> vec1, vector<int> vec2) {
    vector<int> r;
    for (auto v : vec1) {
        if (find(r.begin(), r.end(), v) == r.end()) {
            r.push_back(v);
        }
    }
    for (auto v : vec2) {
        if (find(r.begin(), r.end(), v) == r.end()) {
            r.push_back(v);
        }
    }
    return r;
}
#endif
