#include "io.h"
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