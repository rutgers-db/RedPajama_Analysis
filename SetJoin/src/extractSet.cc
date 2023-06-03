/*
This is a file that extract the set (unqiue elements and sort them) of the Redpajama datasets
Also will sort them based on their sizes

And then output the sorted sets in the datasets directory and their idmap.
*/

#include <bits/stdc++.h>
#include "../src/util/io.h"
#include <omp.h>

using namespace std;

int main(int argc, char *argv[]){
    
     // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]);
    const string dataset_dir_path = "/research/projects/zp128/RedPajama-Data-1T/RedPajama-Data-1T/" + dataset_name; // arxiv_bottom_64.bin;

    const string idmap_file_path =  root_dir + "/sorted_sets/" +  dataset_name + "_idmap.bin";
    const string sortedsets_file_path =  root_dir + "/sorted_sets/" +  dataset_name + "_sortedsets.bin";
    // Input bottom_k and shrink their size to the specified K
    vector<vector<unsigned short>> docs;
    loadTokensBasedOnIDX(dataset_dir_path,docs);

    // remove duplicates
    cout<<"Removing Deplicates"<<endl;

    #pragma omp parallel for
    for(int i = 0; i < docs.size(); ++i){
        std::sort(docs[i].begin(), docs[i].end());
        docs[i].erase(std::unique(docs[i].begin(), docs[i].end()), docs[i].end());
    }

    // Sort docs based on their length, elements
    vector<int> idmap_docs;
    idmap_docs.clear();
    for (int i = 0; i < docs.size(); i++)
        idmap_docs.emplace_back(i);

    // sort dataset by size first, element second, id third
    cout<<"Generating idmap "<<endl;
    sort(idmap_docs.begin(), idmap_docs.end(), [&docs](const unsigned int &id1, const unsigned int &id2) {
        int dsize1 = docs[id1].size();
        int dsize2 = docs[id2].size();
        if (dsize1 < dsize2)
            return true;
        else if (dsize1 > dsize2)
            return false;
        else {
            for (int i = 0; i < dsize1; i++)
            {
                if (docs[id1][i] < docs[id2][i])
                    return true;
                else if (docs[id1][i] > docs[id2][i])  
                    return false;
            }
            if (id1 < id2)
                return true;
            else
                return false;
        } });

    // write the idmap and the sorted set
    cout<<"Writing idmap and the sorted sets"<<endl;
    writeVec2Bin(idmap_file_path, idmap_docs);
    
    ofstream ofs;
    ofs.open(sortedsets_file_path.c_str(), ios::binary);
    for (int i = 0; i < idmap_docs.size(); i++){
        const auto & vec = docs[idmap_docs[i]];
        int size = vec.size();
        
        // if(i<10){
        //     cout<<size<<endl;
        //     cout<<idmap_docs[i]<<endl;
        // }
        ofs.write((char *)&size, sizeof(int)); 
        ofs.write((char *)&vec[0], size * sizeof(unsigned short)); // Write the vector data
    }
    ofs.close();
}