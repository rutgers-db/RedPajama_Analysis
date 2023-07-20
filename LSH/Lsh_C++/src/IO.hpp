#pragma once
#include <bits/stdc++.h>
using namespace std;
// Function to write similar pairs into a binary file from a vector of pairs
void writeSimilarPair(const string &binFileName, const vector<pair<unsigned int, unsigned int>> &result_pairs) {
    // Similar to writeVec2Bin, but for a vector of pairs
    cout << "Writing " << binFileName << endl;
    ofstream ofs(binFileName, ios::binary);
    unsigned long long size = result_pairs.size();
    cout << binFileName << " has " << size << endl;
    ofs.write((char *)&size, sizeof(unsigned long long));
    ofs.write((char *)&result_pairs[0], size * sizeof(pair<int, int>));
    ofs.close();
}

// Function to load binary file into a 2D vector (documents)
void loadShortBin(const string &binFileName, vector<vector<unsigned short>> &docs) {
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    int size;                                                     // Initialize a variable to store the size of each vector
    while (ifs.read((char *)&size, sizeof(int))) {                // Read the size of the vector
        vector<unsigned short> vec(size);                         // Create a vector of the read size
        ifs.read((char *)&vec[0], sizeof(unsigned short) * size); // Read the data into the vector
        docs.emplace_back(vec);                                   // Add the vector to the documents
    }
    ifs.close(); // Close the file stream after reading

    printf("There are %lu documents in %s\n",docs.size(),  binFileName.c_str());
}

// Function to load binary file into a vector
void loadBin2vec(const string &binFileName, vector<int> &vec) {
    // Similar to loadShortBin, but for a single-dimension vector
    cout << "Reading " << binFileName << endl;
    ifstream ifs(binFileName, ios::binary);
    if (!ifs) {
        cout << "error open bin file " << binFileName << endl;
        return;
    }
    int size;
    ifs.read((char *)&size, sizeof(int));
    vec.resize(size);
    ifs.read((char *)&vec[0], sizeof(int) * size);
    ifs.close();
}

void outputVector(const std::vector<unsigned short>& vec) {
    for (const auto& elem : vec) {
        std::cout << elem << ' ';
    }
    std::cout << '\n';
}
