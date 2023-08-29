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

// Function to read similar pairs from a binary file into a vector of pairs
void readSimilarPair(const string &binFileName, vector<pair<unsigned int, unsigned int>> &result_pairs) {
    cout << "Reading " << binFileName << endl;
    ifstream ifs(binFileName, ios::binary);

    // Read the size
    unsigned long long size;
    ifs.read((char *)&size, sizeof(unsigned long long));
    cout << binFileName << " has " << size << endl;

    // Read the pairs
    result_pairs.resize(size);
    ifs.read((char *)&result_pairs[0], size * sizeof(pair<unsigned int, unsigned int>));

    ifs.close();
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

// Function to load binary file into a 2D vector (documents)
void loadIntBin(const string &binFileName, vector<vector<unsigned int>> &docs){
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    int size;                                                     // Initialize a variable to store the size of each vector
    while (ifs.read((char *)&size, sizeof(int))) {                // Read the size of the vector
        vector<unsigned int> vec(size);                         // Create a vector of the read size
        ifs.read((char *)&vec[0], sizeof(unsigned int) * size); // Read the data into the vector
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

template<typename T>
void writeVecInOfs(ofstream & ofs, vector<T>& vec){
    ofs.write((char *)&vec[0], vec.size() * sizeof(T));
}

void readPairsFromFile(const std::string& filename, std::vector<std::pair<unsigned int, unsigned int>>& vec) {
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    if (!ifs) {
        throw std::runtime_error("Could not open file " + filename);
    }

    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    vec.resize(size / sizeof(std::pair<unsigned int, unsigned int>));
    ifs.read(reinterpret_cast<char*>(vec.data()), size);
}

void loadTokensBasedOnIDX(const string &dir_path, vector<vector<unsigned short>> &docs) {
    string idx_file_path = dir_path + "/tokenized_text_document.idx";
    string bin_file_path = dir_path + "/tokenized_text_document.bin";
    ifstream idx_ifs(idx_file_path.c_str(), ios::in | ios::binary);
    ifstream bin_ifs(bin_file_path.c_str(), ios::in | ios::binary);
    if (!idx_ifs || !bin_ifs) { // If the file cannot be opened or does not exist, print an error message
        printf("%s or %s does not exist", idx_file_path.c_str(), bin_file_path.c_str());
        return; // Exit the function
    }

    // Start Reading
    idx_ifs.seekg(18, ios::beg);
    long long tmp;
    idx_ifs.read((char *)&tmp, 8);
    long long N = tmp;
    printf("There are total %lld documents in this %s\n", N, bin_file_path.c_str());
    idx_ifs.read((char*)&tmp, 8);

    for (long long i = 0; i < N; i++) {
        int len;
        idx_ifs.read((char *)&len, 4);

        // Read the file
        vector<unsigned short> entity(len);
        bin_ifs.read((char *)&entity[0], sizeof(unsigned short) * len);

        // Add the document into docs
        docs.emplace_back(entity);
    }
    printf("%s read\n", bin_file_path.c_str());
}