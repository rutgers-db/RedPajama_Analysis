#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"

using namespace std;

int main(){
    string common_crawl_dirpath = "/research/projects/zp128/RedPajama_Analysis/OverlapJoin/bottomK_bins/common_crawl/";
    int K = 64;
    const string shrinked_path = "/research/projects/zp128/RedPajama_Analysis/OverlapJoin/bottomK_bins/intergrated_commoncrawl_"+to_string(K)+ ".bin";

     // Input bottom_k of common_crawl and shrink their size to the specified K and intergrate into one file
    vector<vector<unsigned short>> bottomks;
    vector<string> cc_divided_filenames;
    getFiles(common_crawl_dirpath, cc_divided_filenames);

    for(auto cc_file: cc_divided_filenames){
        cout <<"Reading "<< cc_file<<endl;
        ifstream ifs(cc_file, ios::binary);
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
    }

    // Write the intergrated common_crawl
    ofstream ofs(shrinked_path, ios::binary);
    for(auto& vec:bottomks){
        int bottomK_len = vec.size();
        // Write the bottomk
        ofs.write((char*)&bottomK_len, sizeof(int));
        ofs.write((char*)&vec[0], sizeof(unsigned short)*bottomK_len);
    }
    cout<<shrinked_path<<" writed\n";
    cout<<"total intergrated common crawls have "<<bottomks.size()<< "bottomks"<<endl;
    ofs.close();
}