#pragma once
#include <bits/stdc++.h>
#include <sys/sysinfo.h>
using namespace std;
// Generate random hash functions based on given seed
// ab: the output argument
void generateHashFunc(int seed, vector<pair<unsigned short, unsigned short>> &hf) {
    // TODO: knuth shuffling?
    // TODO: random_seed
    srand(seed);
    unsigned short a = 0;
    while (a == 0)
        a = (unsigned short)rand();
    unsigned short b =(unsigned short) rand();
    hf.emplace_back(a, b);
}

// The hash value function
inline unsigned short hval(const pair<unsigned short, unsigned short> &hf, unsigned short word) {
    return hf.first * word + hf.second;
}

// Turn on timer
std::chrono::_V2::system_clock::time_point LogTime() {
    return chrono::high_resolution_clock::now();
}

// Turn off timer
double RepTime(const std::chrono::_V2::system_clock::time_point &start) {
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    return duration.count() / 1000000.0;
}
