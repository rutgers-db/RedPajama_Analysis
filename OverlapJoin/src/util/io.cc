#include "io.h"
#include <filesystem>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void loadShortBin(const string &binFileName, vector<vector<unsigned short>> &docs) {
    cout <<"Reading "<< binFileName<<endl;
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

void writeSimilarPair(const string &binFileName, const vector<pair<int, int>> &result_pairs){
    cout <<"Writing "<< binFileName<<endl;
    ofstream ofs(binFileName, ios::binary);

    int size = result_pairs.size();
    ofs.write((char*)&size, sizeof(int));
    ofs.write(reinterpret_cast<const char*>(result_pairs.data()), size * sizeof(pair<int, int>));
    ofs.close();
}

// Get the filename from a given path. 
// Then use the std::string::find and std::string::substr functions to extract the substring before the first underscore in the filename.
std::string extract_prefix(const std::string& filepath) {
    // Extract the filename from the path
    std::filesystem::path path(filepath);
    std::string filename = path.filename().string();

    // Find the position of the first underscore
    std::size_t pos = filename.find('_');

    // If an underscore was found, extract the prefix
    if (pos != std::string::npos) {
        return filename.substr(0, pos);
    }

    // If no underscore was found, return the whole filename
    return filename;
}

// get all the file names in path and put them in a vector
void getFiles(string path, vector<string> &files) {
    DIR *dr;
    struct dirent *en;
    string file_path;
    dr = opendir(path.c_str()); // open all directory
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            // ignore hidden files and folders
            if (en->d_name[0] != '.') {
                const char end = path.back();
                if (end == '/')
                    file_path = path + en->d_name;
                else
                    file_path = path + '/' + en->d_name;
                files.push_back(file_path);
            }
        }
        closedir(dr); // close all directory
    }
}
