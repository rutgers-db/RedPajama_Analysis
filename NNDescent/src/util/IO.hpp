/**
 * Basic input and output utility function
 * 
 * Author: Weiqi Feng
 * Date:   Jan 30, 2020
 * Email:  fengweiqi@sjtu.edu.cn
 */
#ifndef IO_H
#define IO_H
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;


uint32_t readUint32(ifstream &in) {
    uint8_t bytes[4];
    // Read each byte
    for (int i = 0; i < 4; i++) {
        // Can't use >> (otherwise ignore certian chars)
        in.read((char *)bytes + i, 1);
    }
    uint32_t res = 0;
    // From little endian -> big endian
    for (int i = 0; i < 4; i++) {
        res = ((uint32_t)bytes[i] << (8 * i)) | res;
    }
    return res;
}

int readInt(ifstream &in) {
    uint32_t res = readUint32(in);
    return reinterpret_cast<int&>(res);
}

float readFloat(ifstream &in) {
    uint32_t res = readUint32(in);
    return reinterpret_cast<float&>(res);
}

uint8_t readUChar(ifstream &in) {
    uint8_t byte;
    in.read((char *)&byte, 1);
    return byte;
}

int fileBytes(ifstream &in) {
    streampos begin = in.tellg();
    // Move to the end of stream
    in.seekg(0, ios::end);
    streampos end = in.tellg();
    // Move to the beginning of stream
    in.seekg(0, ios::beg);
    return end - begin;
}

int vecDim(ifstream &in) {
    in.seekg(0, ios::beg);
    int dim = readInt(in);
    in.seekg(0, ios::beg);
    return dim;
}

vector<vector<int>> ivecsRead(string path) {
    ifstream infile;
    // Open the file
    infile.open(path, ios::in | ios::binary);

    if (!infile.is_open()) {
        cerr << "Open File Error!" << endl;
    }

    int fileSize = fileBytes(infile);
    // The dimension of vector
    int dim = vecDim(infile);
    int atomSize = 4 + dim * 4;
    // The number of vectros
    int num = fileSize / atomSize;
    vector<vector<int>> iMatrix(num, vector<int>(dim, 0));
    for (int i = 0; i < num; i++) {
        // Just for move the read pointer
        readInt(infile);
        for (int j = 0; j < dim; j++) {
            iMatrix[i][j] = readInt(infile);
        }
    }
    // Close the file
    infile.close();

    return iMatrix;
}

vector<vector<float>> fvecsRead(string path) {
    ifstream infile;
    // Open the file
    infile.open(path, ios::in | ios::binary);

    if (!infile.is_open()) {
        cerr << "Open File Error!" << endl;
    }

    int fileSize = fileBytes(infile);
    // The dimension of vector
    int dim = vecDim(infile);
    int atomSize = 4 + dim * 4;
    // The number of vectros
    int num = fileSize / atomSize;
    vector<vector<float>> fMatrix(num, vector<float>(dim, 0.0));
    for (int i = 0; i < num; i++) {
        // Just for move the read pointer
        readInt(infile);
        for (int j = 0; j < dim; j++) {
            fMatrix[i][j] = readFloat(infile);
        }
    }
    // Close the file
    infile.close();

    return fMatrix;
}


vector<vector<float>> fvecsReadN(string path, int cnt) {
    ifstream infile;
    // Open the file
    infile.open(path, ios::in | ios::binary);

    if (!infile.is_open()) {
        cerr << "Open File Error!" << endl;
    }

    int fileSize = fileBytes(infile);
    // The dimension of vector
    int dim = vecDim(infile);
    vector<vector<float>> fMatrix(cnt, vector<float>(dim, 0.0));
    for (int i = 0; i < cnt; i++) {
        // Just for move the read pointer
        readInt(infile);
        for (int j = 0; j < dim; j++) {
            fMatrix[i][j] = readFloat(infile);
        }
    }
    // Close the file
    infile.close();

    return fMatrix;
}


vector<vector<float>> bvecsRead(string path) {
    ifstream infile;
    // Open the file
    infile.open(path, ios::in | ios::binary);

    if (!infile.is_open()) {
        cerr << "Open File Error!" << endl;
    }

    int fileSize = fileBytes(infile);
    // The dimension of vector
    int dim = vecDim(infile);
    int atomSize = 4 + dim;
    // The number of vectros
    int num = fileSize / atomSize;
    vector<vector<float>> fMatrix(num, vector<float>(dim, 0.0));
    for (int i = 0; i < num; i++) {
        // Just for move the read pointer
        readInt(infile);
        for (int j = 0; j < dim; j++) {
            fMatrix[i][j] = readUChar(infile);
        }
    }
    // Close the file
    infile.close();

    return fMatrix;
}


vector<vector<float>> bvecsReadN(string path, int cnt) {
    ifstream infile;
    // Open the file
    infile.open(path, ios::in | ios::binary);

    if (!infile.is_open()) {
        cerr << "Open File Error!" << endl;
    }

    int fileSize = fileBytes(infile);
    // The dimension of vector
    int dim = vecDim(infile);
    cout << "dim " << dim << endl;
    vector<vector<float>> fMatrix(cnt, vector<float>(dim, 0.0));
    for (int i = 0; i < cnt; i++) {
        // Just for move the read pointer
        readInt(infile);
        for (int j = 0; j < dim; j++) {
            uint8_t num = readUChar(infile);
            fMatrix[i][j] = num;
        }
    }
    // Close the file
    infile.close();

    return fMatrix;
}

vector<vector<float>> myFDataRead(string path,int dim) {
    ifstream infile;
    // Open the file
    infile.open(path,ios::in);

    if (!infile.is_open()) {
        cerr << "Open File Error!" << endl;
    }

    int fileSize = fileBytes(infile);
    // The dimension of vector
    int atomSize = dim * 4;
    // The number of vectros
    int num = fileSize / atomSize;
    vector<vector<float>> fMatrix(num, vector<float>(dim, 0.0));
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < dim; j++) {
            fMatrix[i][j] = readFloat(infile);
        }
    }
    // Close the file
    infile.close();

    return fMatrix;
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


#endif
