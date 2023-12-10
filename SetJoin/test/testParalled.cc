#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include <tbb/parallel_sort.h>

// Constants for our test
const size_t NUM_ARRAYS = 10;
const size_t ARRAY_SIZE = 100000; // Size of each array

std::vector<std::vector<int>> generateRandomData() {
    std::vector<std::vector<int>> data(NUM_ARRAYS, std::vector<int>(ARRAY_SIZE));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(INT_MIN, INT_MAX);
    for (auto &arr : data) {
        for (auto &elem : arr) {
            elem = dist(gen);
        }
    }
    return data;
}

void serialSort(std::vector<std::vector<int>> &data) {
    for (auto &arr : data) {
        std::sort(arr.begin(), arr.end());
    }
}

void openmpSort(std::vector<std::vector<int>> &data) {
    #pragma omp parallel for
    for (size_t i = 0; i < data.size(); i++) {
        std::sort(data[i].begin(), data[i].end());
    }
}

void tbbSort(std::vector<std::vector<int>> &data) {
    for (auto &arr : data) {
        tbb::parallel_sort(arr.begin(), arr.end());
    }
}

void nestedParallelSort(std::vector<std::vector<int>> &data) {
    #pragma omp parallel for
    for (size_t i = 0; i < data.size(); i++) {
        tbb::parallel_sort(data[i].begin(), data[i].end());
    }
}

template <typename Func>
double measureTime(Func &&f, std::vector<std::vector<int>> &data) {
    auto start = std::chrono::high_resolution_clock::now();
    f(data);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

int main() {
    // Generate random data
    auto data = generateRandomData();
// #pragma omp parallel for
    std::cout << omp_get_num_threads() << std::endl;
    // Serial sorting
    auto copy1 = data;
    double serialTime = measureTime(serialSort, copy1);
    std::cout << "Serial sorting: " << serialTime << " seconds" << std::endl;

    // OpenMP sorting
    auto copy2 = data;
    double ompTime = measureTime(openmpSort, copy2);
    std::cout << "OpenMP sorting: " << ompTime << " seconds" << std::endl;

    // TBB sorting
    auto copy3 = data;
    double tbbTime = measureTime(tbbSort, copy3);
    std::cout << "TBB sorting: " << tbbTime << " seconds" << std::endl;

    // Nested parallel sorting
    auto copy4 = data;
    double nestedTime = measureTime(nestedParallelSort, copy4);
    std::cout << "Nested parallel sorting: " << nestedTime << " seconds" << std::endl;

    return 0;
}
