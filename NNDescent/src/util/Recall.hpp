#ifndef RECALL_H
#define RECALL_H

#include <iostream>
#include <vector>

#include "BruteForce.hpp"
#include "Util.hpp"
#include "../NNDescent/Heap.hpp"
#include "../NNDescent/SimpleHeap.hpp"
#include "../NNDescent/NNHeap.hpp"
using namespace std;

template <class T>
class Recall {
private:
    int recallNodesNum;
    vector<int> recallNodes;
    vector<vector<int>> recall_ans;

private:
    // Generate nodes for testing recall
    void generateRecallNodes(const unsigned &N);

    // get the standard answer (real KNN of recallNodes) of Recall
    void getRecallAns(int k, const vector<T> &nodes, function<double(T, T)> fn);

public:
    Recall(int node_size) :
        recallNodesNum(node_size){};

    //initialize this implementation and generate member variables
    void init(int k, const unsigned &N, const vector<T> &nodes, function<double(T, T)> fn);

    // Compute Recall
    double computeRecall(const vector<Heap> &neighborLists);
    double computeRecall(const vector<SimpleHeap> &neighborLists);
    double computeRecall(const vector<NNHeap> &neighborLists);
};

//generate the nodes needed in recall test
template <class T>
void Recall<T>::generateRecallNodes(const unsigned &N) {
    if (N < recallNodesNum) {
        cout << "Warning: the number of graph nodes should be used for large graphs more than "
             << recallNodesNum << " nodes" << endl;
    }

    int stride = std::max(1, int(N / recallNodesNum - 1));
    int i = 0;
    int cnt = 0;

    while (cnt < recallNodesNum) {
        recallNodes.push_back(i);
        i += stride;
        cnt++;
    }
    return;
}

//get the answer of recall test
template <class T>
void Recall<T>::getRecallAns(int k, const vector<T> &nodes, function<double(T, T)> fn) {
    cout << "Computing answer of recall test..." << endl;
    BruteForce<T> bf(k);
    recall_ans = bf.computePartialGraph(nodes, recallNodes, fn);
}

//initialize this implementation and generate member variables
template <class T>
void Recall<T>::init(int k, const unsigned &N, const vector<T> &nodes, function<double(T, T)> fn) {
    generateRecallNodes(N);
    getRecallAns(k, nodes, fn);
}

//Compute recall
template <class T>
double Recall<T>::computeRecall(const vector<Heap> &neighborLists) {
    vector<vector<int>> exp(recallNodesNum, vector<int>());
    for (int i = 0; i < recallNodesNum; i++) {
        exp[i] = neighborLists[recallNodes[i]].pickAll();
    }
    return knn_recall(exp, recall_ans);
}

//Compute recall
template <class T>
double Recall<T>::computeRecall(const vector<SimpleHeap> &neighborLists) {
    vector<vector<int>> exp(recallNodesNum, vector<int>());
    for (int i = 0; i < recallNodesNum; i++) {
        exp[i] = neighborLists[recallNodes[i]].pickAll();
    }
    return knn_recall(exp, recall_ans);
}

//Compute recall
template <class T>
double Recall<T>::computeRecall(const vector<NNHeap> &neighborLists) {
    vector<vector<int>> exp(recallNodesNum, vector<int>());
    for (int i = 0; i < recallNodesNum; i++) {
        exp[i] = neighborLists[recallNodes[i]].pickAllId();
    }
    return knn_recall(exp, recall_ans);
}
#endif