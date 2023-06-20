// Include necessary headers
#include "io.h"       // The custom I/O functions defined in io.h
#include <filesystem> // Filesystem library for performing operations on files and directories
#include <dirent.h>   // For directory management (such as open directory, close directory)
#include <sys/stat.h> // For stat function which returns information about a file
#include <unistd.h>   // For POSIX operating system API

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

// Function to write a vector into a binary file
void writeVec2Bin(const string &binFileName, vector<int> &vec) {
    cout << "Writing " << binFileName << endl;
    ofstream ofs;
    ofs.open(binFileName.c_str(), ios::binary);
    int size = vec.size();
    ofs.write((char *)&size, sizeof(int));                                     // Write the size of the vector
    ofs.write(reinterpret_cast<const char *>(vec.data()), size * sizeof(int)); // Write the vector data
    ofs.close();
}

// Function to read similar pairs from a binary file into a vector of pairs
void readSimilarPair(const string &binFileName, vector<pair<int, int>> &sim_pairs) {
    // Similar to loadBin2vec, but for a vector of pairs
    cout << "Reading " << binFileName << endl;
    ifstream ifs(binFileName, ios::binary);
    if (!ifs) {
        cout << "error open bin file" << binFileName << endl;
        return;
    }
    unsigned long long size;
    ifs.read((char *)&size, sizeof(unsigned long long));
    sim_pairs.resize(size);
    ifs.read((char *)&sim_pairs[0], size * sizeof(pair<int, int>));
    ifs.close();
}

// Function to write similar pairs into a binary file from a vector of pairs
void writeSimilarPair(const string &binFileName, const vector<pair<int, int>> &result_pairs) {
    // Similar to writeVec2Bin, but for a vector of pairs
    cout << "Writing " << binFileName << endl;
    ofstream ofs(binFileName, ios::binary);
    unsigned long long size = result_pairs.size();
    cout<<binFileName << " has " << size <<endl;
    ofs.write((char *)&size, sizeof(unsigned long long));
    ofs.write((char *)&result_pairs[0], size * sizeof(pair<int, int>));
    ofs.close();
}

void readDividedList(const string &binFileName, vector<vector<pair<int, int>>> &res_lists) {
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    int size;                                                     // Initialize a variable to store the size of each vector
    while (ifs.read((char *)&size, sizeof(int))) {                // Read the size of the vector
        vector<pair<int, int>> vec(size);                         // Create a vector of the read size
        ifs.read((char *)&vec[0], sizeof(pair<int, int>) * size); // Read the data into the vector
        res_lists.emplace_back(vec);                              // Add the vector to the documents
    }
    ifs.close(); // Close the file stream after reading
}

// Function to extract the prefix of a filename from a filepath
std::string extract_prefix(const std::string &filepath) {
    // Similar to get_filename, but this function additionally finds the prefix before the first underscore ('_') in the filename
    std::filesystem::path path(filepath);
    std::string filename = path.filename().string();
    std::size_t pos = filename.find('_');
    if (pos != std::string::npos) {
        return filename.substr(0, pos);
    }
    return filename;
}

// Function to get all file names in a given directory and store them in a vector
void getFiles(string path, vector<string> &files) {
    DIR *dr;
    struct dirent *en;
    string file_path;
    dr = opendir(path.c_str()); // Open the directory
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            // Ignore hidden files and folders (those beginning with '.')
            if (en->d_name[0] != '.') {
                const char end = path.back();
                if (end == '/')
                    file_path = path + en->d_name;
                else
                    file_path = path + '/' + en->d_name;
                files.push_back(file_path);
            }
        }
        closedir(dr); // Close the directory
    }
}
