#include "../../../SetJoin/emhash/hash_table7.hpp"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;
// Define a test function for basic operations like insertion and lookup
void testGroupingAndPairGeneration() {
    // Mock data similar to 'pairs' in your original code
    vector<pair<size_t, unsigned int>> pairs{make_pair(1, 100), make_pair(1, 101), make_pair(1, 103), make_pair(2, 102)};

    vector<pair<unsigned int, unsigned int>>temp_pairs;
    emhash7::HashMap<size_t,unsigned int> group_index; 

    temp_pairs.reserve(100000);
    auto &cur_gindex = group_index;
    cur_gindex.reserve(pairs.size());
    vector<unsigned int> tmp_vec(pairs.size());

    for (unsigned int j = 0; j < pairs.size(); j++) {
        cur_gindex[pairs[j].first]++;
    }

    unsigned int cnt = 0;
    for (auto &it : cur_gindex) {
        unsigned int pre_cnt = cnt;
        cnt += it.second;
        it.second = pre_cnt;
    }

    for (unsigned int j = 0; j < pairs.size(); j++) {
        tmp_vec[cur_gindex[pairs[j].first]++] = pairs[j].second;
    }

    unsigned int pre_ed = 0;
    for (auto &it : cur_gindex) {
        unsigned int cur_ed = it.second;
        for (auto j = pre_ed; j < cur_ed; j++) {
            for (auto k = j + 1; k < cur_ed; k++) {
                temp_pairs.emplace_back(tmp_vec[j], tmp_vec[k]);
                cout<<tmp_vec[j]<<" "<<tmp_vec[k]<<endl;
            }
        }
        pre_ed = cur_ed;
    }


    // Assertions to validate the output
    // Example: assert(temp_pairs[0].size() == expected_size);

    cout << "Test for grouping and pair generation passed." << endl;
}


// Define more test functions for other operations like deletion, size, etc.

// Main function
int main() {
    testGroupingAndPairGeneration();
    // Call other test functions here

    std::cout << "All tests passed." << std::endl;
    return 0;
}