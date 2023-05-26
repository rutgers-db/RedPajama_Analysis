#include "io.h"
#include <filesystem>

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
    for(const auto & p : result_pairs){
        ofs.write((char*)&p.first, sizeof(int));
        ofs.write((char*)&p.second, sizeof(int));
    }
    ofs.close();
}



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