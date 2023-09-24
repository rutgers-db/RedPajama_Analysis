
#ifndef SETJOINGROUPPARELLED_H
#define SETJOINGROUPPARELLED_H

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <cstring>
#include <queue>
#include <inttypes.h>
#include <sys/time.h>
#include <cmath>
#include <cstdio>
#include "../util/util.h"
#include "group_paralled/PartitionHasher.h"
#include "group_paralled/PartitionIndex.h"
#include "group_paralled/RangeLoader.h"

using namespace std;

/**
 * SetJoinGroupParelled: Class to perform set joins in parallel
 * It manages the joining of datasets based on certain criteria, 
 * especially focusing on overlapping criteria.
 */
class SetJoinGroupParelled {
public:
    string dataset_path; // Path to the dataset being processed

    // Array to store result pairs for each thread
    vector<pair<int, int>> result_pairs[MAXTHREADNUM];

    // Parameters related to calculation and dataset
    double det{};
    double coe{};
    double ALPHA{};

    // Recording time cost of different part
    double index_cost;
    double setjoin_cost;
    double hashInFind_cost[MAXTHREADNUM];
    double alloc_cost[MAXTHREADNUM];
    double verif_cost[MAXTHREADNUM];

    /**
     * Constructor
     * @param path: Path to the dataset to be processed
     */
    explicit SetJoinGroupParelled(string path) : dataset_path(std::move(path)) {}

    // Default destructor
    ~SetJoinGroupParelled() = default;

    /**
     * Get the total number of result pairs found by all threads
     * @return Total number of result pairs found
     */
    unsigned long long getResultPairsAmount();

    /**
     * Main function to perform set join
     * @param threshold: Overlapping threshold to decide joins
     */
    void setjoin(double threshold);

    void reportTimeCost();

private:
    // Vectors for storing range information (groups based on the size of documents)

    /**
     * Initialize class parameters
     * @param threshold: Overlapping threshold
     */
    void initializeParameters(double threshold);

    /**
     * Processes the dataset and manages the set joins
     */
    void processDataset();

    /**
     * Process the current dataset and manage its hashing and indexing
     * @param currentDataset: Current dataset to be processed
     * @param currentHasher: Hasher object for the current dataset
     * @param currentIndex: Index object for the current dataset
     * @param previousDocId: ID for previous document
     */
    void processCurrentDataset(
        const vector<vector<unsigned int>>& currentDataset,
        PartitionHasher*& currentHasher,
        PartitionIndex*& currentIndex,
        const unsigned int& previousDocId);

    /**
     * Process the previous dataset and manage comparisons with the current dataset
     * @param previousDataset: Previous dataset
     * @param currentDataset: Current dataset
     * @param previousHasher: Hasher object for the previous dataset
     * @param previousIndex: Index object for the previous dataset
     * @param beforePreviousDocId: ID for before previous document
     * @param previousDocId: ID for previous document
     */
    void processPreviousDataset(
        const vector<vector<unsigned int>>& previousDataset,
        const vector<vector<unsigned int>>& currentDataset,
        PartitionHasher& previousHasher,
        PartitionIndex& previousIndex,
        const unsigned int& beforePreviousDocId,
        const unsigned int& previousDocId);

    /**
     * Verifies pairs for overlaps
     * @param datasetA: Dataset A for verification
     * @param datasetB: Dataset B for verification
     * @param docId: Document ID for referencing
     * @param candidates: Candidate set IDs for verification
     * @param datasetAStartId: Start ID for Dataset A
     * @param datasetBStartId: Start ID for Dataset B
     */
    void verifyPairs(
        const vector<vector<unsigned int>>& datasetA,
        const vector<vector<unsigned int>>& datasetB,
        unsigned int docId,
        const vector<unsigned int>& candidates,
        unsigned int datasetAStartId, 
        unsigned int datasetBStartId);

    /**
     * Check if two sets overlap based on the defined criteria
     * @param x: Set X
     * @param y: Set Y
     * @return True if the sets jaccard's similarity is over the det, otherwise False
     */
    bool overlap(const vector<unsigned int>& x, const vector<unsigned int>& y);
};

#endif // SETJOINGROUPPARELLED_H
