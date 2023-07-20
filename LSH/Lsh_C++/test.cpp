#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <algorithm>
#include <execution>

int main() {
    // Generate random pairs
    std::vector<int> pairs(10000);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10000);

    for (auto& pair : pairs) {
        pair= dis(gen);
    }

    // Sort pairs in parallel
    std::sort(std::execution::par_unseq, pairs.begin(), pairs.end());

    // Check if the vector is sorted
    bool sorted = std::is_sorted(pairs.begin(), pairs.end());
    if (sorted) {
        std::cout << "The vector is sorted." << std::endl;
    } else {
        std::cout << "The vector is not sorted." << std::endl;
    }

    return 0;
}
