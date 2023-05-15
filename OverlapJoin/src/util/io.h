#pragma once
#include <bits/stdc++.h>
using namespace std;

// load the vector<int> of a bin file and push back to docs
void loadShortBin(const string &binFileName, vector<vector<unsigned short>> &docs) {
    ifstream ifs(binFileName, ios::binary);
    if (!ifs) {
        cout << "error open bin file" << endl;
        return;
    }
    int size;
    while (ifs.read((char *)&size, sizeof(int))) {
        vector<unsigned short> vec(size);
        ifs.read((char *)&vec[0], sizeof(unsigned short) * size);
        docs.emplace_back(vec);
    }
    ifs.close();
}
