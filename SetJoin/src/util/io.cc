    // Include necessary headers
#include "io.h"       // The custom I/O functions defined in io.h
#include <filesystem> // Filesystem library for performing operations on files and directories
#include <dirent.h>   // For directory management (such as open directory, close directory)
#include <sys/stat.h> // For stat function which returns information about a file
#include <unistd.h>   // For POSIX operating system API

// Function to load binary file based on idx file into a 2D vector (documents)
// Used specifically in loading REdPajama Dataset
// You should make sure there are two files called "tokenized_text_document.bin" and "tokenized_text_document.idx" in the specified directory.
void loadTokensBasedOnIDX(const string &dir_path, vector <vector<unsigned short>> &docs) {
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
    idx_ifs.read((char *) &tmp, 8);
    long long N = tmp;
    printf("There are total %lld documents in this %s\n", N, bin_file_path.c_str());
    idx_ifs.read((char *) &tmp, 8);

    for (long long i = 0; i < N; i++) {
        int len;
        idx_ifs.read((char *) &len, 4);

        // Read the file
        vector<unsigned short> entity(len);
        bin_ifs.read((char *) &entity[0], sizeof(unsigned short) * len);

        // Add the document into docs
        docs.emplace_back(entity);
    }
    printf("%s read\n", bin_file_path.c_str());
}

// Function to load binary file into a 2D vector (documents)
void loadShortBin(const string &binFileName, vector <vector<unsigned short>> &docs) {
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    int size;                                                     // Initialize a variable to store the size of each vector
    while (ifs.read((char *) &size, sizeof(int))) {                // Read the size of the vector
        vector<unsigned short> vec(size);                         // Create a vector of the read size
        ifs.read((char *) &vec[0], sizeof(unsigned short) * size); // Read the data into the vector
        docs.emplace_back(vec);                                   // Add the vector to the documents
    }
    ifs.close(); // Close the file stream after reading

    printf("There are %lu documents in %s\n", docs.size(), binFileName.c_str());
}

// Function to load binary file into a 2D vector (documents)
void loadShortBin(const string &binFileName, vector <vector<unsigned int>> &docs) {
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    int size;                                                     // Initialize a variable to store the size of each vector
    while (ifs.read((char *) &size, sizeof(int))) {                // Read the size of the vector
        vector<unsigned short> vec(size);                         // Create a vector of the read size
        ifs.read((char *) &vec[0], sizeof(unsigned short) * size); // Read the data into the vector
        vector<unsigned int> uint_vec(size);
        copy(vec.begin(), vec.end(), uint_vec.begin());
        docs.emplace_back(uint_vec);                                   // Add the vector to the documents
    }
    ifs.close(); // Close the file stream after reading

    printf("There are %lu documents in %s\n", docs.size(), binFileName.c_str());
}

// Function to load binary file into a 2D vector (documents)
void loadIntBin(const string &binFileName, vector <vector<unsigned int>> &docs) {
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    int size;                                                     // Initialize a variable to store the size of each vector
    while (ifs.read((char *) &size, sizeof(int))) {                // Read the size of the vector
        vector<unsigned int> vec(size);                         // Create a vector of the read size
        ifs.read((char *) &vec[0], sizeof(unsigned int) * size); // Read the data into the vector
        docs.emplace_back(vec);                                   // Add the vector to the documents
    }
    ifs.close(); // Close the file stream after reading
    printf("There are %lu documents in %s\n", docs.size(), binFileName.c_str());
}

// Function to load binary file into a 2D vector (documents)
void loadBinSize(const string &binFileName, vector<unsigned int>& docs_size) {
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    int size;                                                     // Initialize a variable to store the size of each vector
    while (ifs.read((char *) &size, sizeof(int))) {                // Read the size of the vector
        ifs.seekg(sizeof(unsigned int) * size, std::ios::cur);    // Skip the data portion of the vector
        docs_size.emplace_back(size);                                   // Add the size to the documents
    }
    ifs.close(); // Close the file stream after reading
    printf("There are %lu documents in %s\n", docs_size.size(), binFileName.c_str());
}

void readDataAtIndexRange(const string &binFileName, unordered_map<unsigned int, vector<unsigned int>>& docs,
                     vector<unsigned int> docs_size, int left_index, int right_index) {
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }

    if (right_index >= docs_size.size()) {
        return;
    }

    std::streamoff offset = 0;
    for (int i = 0; i < left_index; ++i) {
        offset += sizeof(int) + sizeof(unsigned int) * docs_size[i];
    }
    ifs.seekg(offset + sizeof(int), std::ios::beg);

    for (int index = left_index; index <= right_index; ++index) {
        if (index >= docs_size.size()) {
            break;
        }

        int size = docs_size[index];

        std::vector<unsigned int> vec(size);
        ifs.read((char*)&vec[0], sizeof(unsigned int) * size);

        docs[index] = vec;
        ifs.seekg(sizeof(int), std::ios::cur);
    }

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
    ifs.read((char *) &size, sizeof(int));
    vec.resize(size);
    ifs.read((char *) &vec[0], sizeof(int) * size);
    ifs.close();
}

// Function to write a vector into a binary file
void writeVec2Bin(const string &binFileName, vector<int> &vec) {
    cout << "Writing " << binFileName << endl;
    ofstream ofs;
    ofs.open(binFileName.c_str(), ios::binary| ios::out);
    int size = vec.size();
    ofs.write((char *) &size, sizeof(int));                                     // Write the size of the vector
    ofs.write(reinterpret_cast<const char *>(vec.data()), size * sizeof(int)); // Write the vector data
    ofs.close();
}

// Function to read similar pairs from a binary file into a vector of pairs
void readSimilarPair(const string &binFileName, vector <pair<int, int>> &sim_pairs) {
    // Similar to loadBin2vec, but for a vector of pairs
    cout << "Reading " << binFileName << endl;
    ifstream ifs(binFileName, ios::binary);
    if (!ifs) {
        cout << "error open bin file" << binFileName << endl;
        return;
    }
    unsigned long long size;
    ifs.read((char *) &size, sizeof(unsigned long long));
    sim_pairs.resize(size);
    ifs.read((char *) &sim_pairs[0], size * sizeof(pair < int, int >));
    ifs.close();
}

void readSimilarPair(const string &binFileName, vector <pair<unsigned int, unsigned int>> &sim_pairs) {
    // Similar to loadBin2vec, but for a vector of pairs
    cout << "Reading " << binFileName << endl;
    ifstream ifs(binFileName, ios::binary);
    if (!ifs) {
        cout << "error open bin file" << binFileName << endl;
        return;
    }
    unsigned long long size;
    ifs.read((char *) &size, sizeof(unsigned long long));
    sim_pairs.resize(size);
    ifs.read((char *) &sim_pairs[0], size * sizeof(pair < unsigned int, unsigned int >));
    ifs.close();
}

// Function to write similar pairs into a binary file from a vector of pairs
void writeSimilarPair(const string &binFileName, const vector <pair<int, int>> &result_pairs) {
    // Similar to writeVec2Bin, but for a vector of pairs
    cout << "Writing " << binFileName << endl;
    ofstream ofs(binFileName, ios::binary);
    unsigned long long size = result_pairs.size();
    cout << binFileName << " has " << size << endl;
    ofs.write((char *) &size, sizeof(unsigned long long));
    ofs.write((char *) &result_pairs[0], size * sizeof(pair < int, int >));
    ofs.close();
}

// Function to write similar pairs into a binary file from a vector of pairs (Multithread version)
void writeSimilarPairs(const string &binFileName, vector <pair<int, int>> *result_pairs) {
    // Similar to writeVec2Bin, but for a vector of pairs
    cout << "Writing " << binFileName << " using paralled setjoin" << endl;
    ofstream ofs(binFileName, ios::binary);
    unsigned long long total_pairs_amount = 0;
    for (std::size_t i = 0; i < MAXTHREADNUM; ++i) {
        unsigned long long pairs_amount = result_pairs[i].size();
        total_pairs_amount += pairs_amount;

    }
    cout << binFileName << " has " << total_pairs_amount << endl;

    // write the amount
    ofs.write((char *) &total_pairs_amount, sizeof(unsigned long long));

    // write the pairs
    for (std::size_t i = 0; i < MAXTHREADNUM; ++i) {
        auto &pairs_arr = result_pairs[i];
        unsigned long long pairs_amount = pairs_arr.size();
        ofs.write((char *) &pairs_arr[0], pairs_amount * sizeof(pair < int, int >));

    }
    ofs.close();
}

void readDividedList(const string &binFileName, vector <vector<pair < int, int>>

> &res_lists) {
cout << "Reading " << binFileName <<
endl; // Print the name of the file being read
ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
cout << "error open bin file" <<
endl;
return; // Exit the function
}
int size;                                                     // Initialize a variable to store the size of each vector
while (ifs.read((char *)&size, sizeof(int))) {                // Read the size of the vector
vector <pair<int, int>> vec(size);                         // Create a vector of the read size
ifs.read((char *)&vec[0], sizeof(pair<int, int>) * size); // Read the data into the vector
res_lists.
emplace_back(vec);                              // Add the vector to the documents
}
ifs.

close(); // Close the file stream after reading
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
void getFiles(string path, vector <string> &files) {
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


std::string getDirectoryName(const std::string &path) {
    return std::filesystem::path(path).filename().string();
}
unsigned int countFilesAmountInOneDir(string path){
    std::vector<std::string> filenames;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(path))
            filenames.push_back(entry.path().filename().string());
    }
    catch (std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
    }

    return filenames.size();
}
void createDirectoryIfNotExists(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            if (std::filesystem::create_directories(path)) {
                std::cout << "Directory created successfully at: " << path << '\n';
            } else {
                std::cout << "Failed to create directory: " << path << '\n';
            }
        } else {
            std::cout << "Directory already exists: " << path << '\n';
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory: " << e.what() << '\n';
    }
}
