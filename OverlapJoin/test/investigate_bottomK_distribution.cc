/*
    This a test unit that test the bottomk's size distribution.
    Although the bottom k bin files limit the maximum size of the bottomk.
    But there are small document that don't have so much length.
    Therefore, we wanna know what the length distribution of these bottomk
*/

#include <bits/stdc++.h>
#include "../src/util/io.h"
using namespace std;

// This function creates a frequency distribution array, where each index corresponds to a size range of vectors. 
// Each range covers 16 units, and the final range covers up to 1024.
vector<int> sizeDistribution(const vector<vector<unsigned short>>& vecs, const int& MAX_SIZE) {
    const int RANGE = 16;
    const int BINS = MAX_SIZE / RANGE;

    // Initialize the distribution vector with zeros
    vector<int> distribution(BINS, 0);

    // Calculate the distribution
    for (const auto& v : vecs) {
        int index = v.size() / RANGE;
        if (v.size() % RANGE == 0 && v.size() != 0) {
            index -= 1;
        }
        if (index >= 0 && index < BINS) {
            distribution[index]++;
        }else{
            cout<<index<<endl;
        }
    }

    return distribution;
}

int main(){
    const string bottomK_path = "../bottomK_bins/arxiv_bottomK_1024.bin";
    vector<vector<unsigned short>> bottomks;
    loadShortBin(bottomK_path, bottomks);
    printf("There are total %u documents in this file\n", bottomks.size());
    auto distribution = sizeDistribution(bottomks, 1024);
    for(auto len: distribution){
        cout<<len<<",";
    }
}

/*
    Investigation Result:
    Size Distribution:
        1. Arxiv: 6156,1090,1232,1278,1348,1120,1001,969,717,553,455,335,341,278,310,266,244,178,211,165,118,142,122,145,110,99,159,131,107,124,121,99,120,111,97,91,108,100,97,87,81,107,96,92,99,91,110,103,109,106,105,101,98,129,131,109,111,137,127,123,118,139,150,1535199,
        2. 
*/