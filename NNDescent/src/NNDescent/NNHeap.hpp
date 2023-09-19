/**
 * Simple Heap data structure (A wrapped priority queue) distinguishing the old and the new
 */
#ifndef NNHEAP
#define NNHEAP

#include <iostream>
#include <utility>
#include <functional>
#include <vector>
#include <algorithm>

using namespace std;

class NNHeap {
public:
    int size_;
    class Neighbor {
    public:
        int node_id_;     // the node id of this neighbor
        double distance_; // distance to the neighbor
        short cal_times_;       // how many times this neighbor has been calculated in this partition
        short rev_cal_times_; // how many times this neighbor has been calculated in a reverse partition
        Neighbor() {
        }
        Neighbor(int node_id, double distance, short cal_times, short rev_cal_times) :
            node_id_(node_id), distance_(distance), cal_times_(cal_times),  rev_cal_times_(rev_cal_times){
        }
    };
    vector<Neighbor> neighbors_;
    vector<Neighbor> rev_neighbors_;

    // the node id and cal_times of the partition based on this node
    vector<pair<int,short>> partition;

    // Decalre compare function based on distance
    struct cmp {
        bool operator()(const Neighbor &lhs, const Neighbor &rhs) {
            return lhs.distance_ < rhs.distance_;
        }
    };

public:
    // Initialize heap with size
    NNHeap(int size) :
        size_(size){};

    // Insert new elements into heap
    int insert(int node_id, double distance) {
        // Already full and insert element is larger or equal than top element
        if (neighbors_.size() == size_ && distance >= neighbors_[0].distance_) {
            return 0;
        }

        // Check for duplication
        for (int i = 0; i < neighbors_.size(); i++) {
            if (neighbors_[i].node_id_ == node_id) {
                return 0;
            }
        }

        // Insert into heap
        neighbors_.emplace_back(node_id, distance, 0, 0);
        push_heap(neighbors_.begin(), neighbors_.end(), cmp());
        // Only keep the smallest "size_" elements
        if (neighbors_.size() > size_) {
            pop_heap(neighbors_.begin(), neighbors_.end(), cmp());
            neighbors_.pop_back();
        }
        // Insert into the heap => return 1
        // Else => return 0
        return !(distance > neighbors_[0].distance_);
    }

    // Pop the larget elements
    Neighbor pop() {
        pop_heap(neighbors_.begin(), neighbors_.end(), cmp());
        // Copy the largest elements
        auto tmp = neighbors_.back();
        neighbors_.pop_back();

        return tmp;
    }

    // Return whether heap is empty or not
    bool empty() const {
        return neighbors_.size() == 0;
    }

    // Return the size of heap
    int size() const {
        return neighbors_.size();
    }

    // get all id of the neighbors
    vector<int> pickAllId() const {
        vector<int> res;
        for (auto it = neighbors_.begin(); it != neighbors_.end(); it++) {
            res.push_back(it->node_id_);
        }
        return res;
    }

    // get all id of the neighbors
    vector<double> pickAllDis() const {
        vector<double> res;
        for (auto it = neighbors_.begin(); it != neighbors_.end(); it++) {
            res.push_back(it->distance_);
        }
        return res;
    }
        
    void clearRevNeighbors() {
        rev_neighbors_.clear();
    }

    void clearPartition() {
        partition.clear();
    }
};
#endif