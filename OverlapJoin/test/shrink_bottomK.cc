#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"

using namespace std;

int main(){
    const string bottomK_path = "/research/projects/zp128/RedPajama_Analysis/OverlapJoin/bottomK_bins/c4_bottomK_1024.bin";
    int K = 32;
    const string shrinked_path = "/research/projects/zp128/RedPajama_Analysis/OverlapJoin/bottomK_bins/c4_bottomK_"+to_string(K)+ ".bin";

     // Input bottom_k and shrink their size to the specified K
    vector<vector<unsigned short>> bottomks;
    cout <<"Reading "<< bottomK_path<<endl;
    ifstream ifs(bottomK_path, ios::binary);
    if (!ifs) {
        cout << "error open bin file" << endl;
        return 0;
    }
    int size;
    while (ifs.read((char *)&size, sizeof(int))) {
        vector<unsigned short> vec(size);
        ifs.read((char *)&vec[0], sizeof(unsigned short) * size);
        if (vec.size() > K)
            vec.resize(K);
        bottomks.emplace_back(vec);
    }
    ifs.close();

    ofstream ofs(shrinked_path, ios::binary);
    for(auto& vec:bottomks){
        int bottomK_len = vec.size();
        // Write the bottomk
        ofs.write((char*)&bottomK_len, sizeof(int));
        ofs.write((char*)&vec[0], sizeof(unsigned short)*bottomK_len);
    }
    cout<<shrinked_path<<" writed\n";
    ofs.close();
}