/**
 * Heap data structure (A wrapped priority queue)
 * 
 * Author: Weiqi Feng
 * Date: Jan 27, 2019
 * Email: fengweiqi@sjtu.edu.cn
 */ 
#ifndef HEAP_H
#define HEAP_H

#include <iostream>
#include <utility>
#include <functional>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_set>
using namespace std;

class Heap {
private:
    int _size;
    vector<pair<int, double>> v;
    unordered_set<int> set;
    // Decalre compare function
    struct cmp {
        bool operator()(const pair<int, double> &lhs, const pair<int, double> &rhs) {
            return lhs.second < rhs.second;
        }
    };
public:

    // Initialize heap with size
    Heap(int size): _size(size){
    };

    // Insert new elements into heap
    int insert(const pair<int, double> &p) {
        // Already full and insert element is larger or equal than top element
        if (v.size() == _size && p.second >= v[0].second)  {
            return 0;
        }

        // Check for duplication
        for (int i = 0; i < v.size(); i++) {
            if (v[i].first == p.first) {
                return 0;
            }
        }

        // Insert into heap
        v.push_back(pair<int, double>(p.first, p.second));
        push_heap(v.begin(), v.end(), cmp());
        // Only keep the smallest "_size" elements
        if (v.size() > _size) {
            pop_heap(v.begin(), v.end(), cmp());
            v.pop_back();
        }
        // Insert into the heap => return 1
        // Else => return 0
        return !(p.second > v[0].second);
    }

    // Pop the larget elements
    pair<int, double> pop() {
        pop_heap(v.begin(), v.end(), cmp());
        // Copy the largest elements
        auto tmp = v.back();
        v.pop_back();

        return pair<int, double>(tmp.first, tmp.second);
    }

    // Return whether heap is empty or not
    bool empty() const {
        return v.size() == 0;
    }

    // Return the size of heap
    int size() const {
        return v.size();
    }

    // Print the heap
    void print() const {
        cout << "The heap has following (index, distance) pairs" << endl;
        for (auto it = v.begin(); it < v.end(); it++) {
            cout << "(" << it->first << ", " << it->second << ") ";
        }
        cout << endl;
    }

    // Pick all falses elements in the heap
    vector<int> pickFalses() {
        vector<int> res;
        for (auto it = v.begin(); it < v.end(); it++) {
            if (set.find(it->first) != set.end()) {
                res.push_back(it->first);
            }
        }
        return res;
    }

    // Pick all true elements in the heap without marking
    vector<int> pickTrues() {
        vector<int> res;
        for (auto it = v.begin(); it < v.end(); it++) {
            if (set.find(it->first) == set.end()) {
                res.push_back(it->first);
            }
        }
        return res;
    }

    // Mark all true elements in the heap
    void markTrues() {
        for (auto it = v.begin(); it < v.end(); it++) {
            if (set.find(it->first) == set.end()) {
                set.insert(it->first);
            }
        }
    }

    // Pick true elements and mark them as false
    vector<int> pickTruesAndMark(double rho) {
        vector<int> res;
        for (auto it = v.begin(); it < v.end(); it++) {
            if (set.find(it->first) == set.end()) {
                double p = double(rand()) / RAND_MAX;
                if (p < rho) {
                    res.push_back(it->first);
                    set.insert(it->first);
                }
            }
        }
        return res;
    }

    // Pick all elements 
    vector<int> pickAll() const{
        vector<int> res;
        for (auto it = v.begin(); it < v.end(); it++){
                    res.push_back(it->first);
        }
        return res;
    }
};
#endif