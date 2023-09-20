// Include necessary headers
#include "io.h"       // The custom I/O functions defined in io.h
#include <filesystem> // Filesystem library for performing operations on files and directories
#include <dirent.h>   // For directory management (such as open directory, close directory)
#include <sys/stat.h> // For stat function which returns information about a file
#include <unistd.h>   // For POSIX operating system API

// Function to load binary file based on idx file into a 2D vector (documents)
// Used specifically in loading REdPajama Dataset
// You should make sure there are two files called "tokenized_text_document.bin" and "tokenized_text_document.idx" in the specified directory.
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
    printf("There are %lu pairs in %s \n", sim_pairs.size(), binFileName.c_str());
    ifs.close();
}

void readSimilarPair(const string &binFileName, vector<pair<unsigned int, unsigned int>> &sim_pairs) {
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
    ifs.read((char *)&sim_pairs[0], size * sizeof(pair<unsigned int, unsigned int>));
    printf("There are %lu pairs in %s \n", sim_pairs.size(), binFileName.c_str());
    ifs.close();
}

// Function to write similar pairs into a binary file from a vector of pairs
void writeSimilarPair(const string &binFileName, const vector<pair<int, int>> &result_pairs) {
    // Similar to writeVec2Bin, but for a vector of pairs
    cout << "Writing " << binFileName << endl;
    ofstream ofs(binFileName, ios::binary);
    unsigned long long size = result_pairs.size();
    cout << binFileName << " has " << size << endl;
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


std::string getDirectoryName(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

void writeEstJaccardCSV(const std::string &filename, const std::vector<std::vector<double>> &data){
    std::ofstream file(filename);
    
    for (const auto &row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i != row.size() - 1) file << ",";  // Don't write a comma after the last element
        }
        file << "\n";
    }
}