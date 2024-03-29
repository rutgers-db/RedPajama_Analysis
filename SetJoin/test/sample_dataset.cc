#include <bits/stdc++.h>
#include <execution>

using namespace std;

// g++ sample_dataset.cc -o sample.exe -I ../oneapi-tbb-2021.9.0/include -pthread -L ../oneapi-tbb-2021.9.0/lib/intel64/gcc4.8 -ltbb

const int rand_percent = 1;
bool probability10Percent() {
    return (rand() % 100) < rand_percent; // Returns true approximately 10% of the time
}

// Function to load binary file into a 2D vector (documents)
void randloadIntBin(const string &binFileName, vector <vector<unsigned int>> &docs) {
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    int size;                                                     // Initialize a variable to store the size of each vector
    while (ifs.read((char *) &size, sizeof(int))) {
        if(probability10Percent()){
            // Read the size of the vector
            vector<unsigned int> vec(size);                         // Create a vector of the read size
            ifs.read((char *) &vec[0], sizeof(unsigned int) * size); // Read the data into the vector
            docs.emplace_back(vec);          // Add the vector to the documents
        }
        else{
            ifs.seekg(sizeof(unsigned int) * size, std::ios_base::cur);
        }
    }
    ifs.close(); // Close the file stream after reading
    printf("There are %lu documents in docs now\n", docs.size() );
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

int main(){
    // string  dataset_names[] = {"arxiv", "book","stackexchange", "github", "wikipedia"};
    vector<vector<unsigned int>> docs;
    string ds_path = "../data/ngram/sorted_ngrams/sample_sortedngram.bin";
    randloadIntBin(ds_path, docs);

    // for(auto & name: dataset_names){
    //     string ds_path = "/research/projects/zp128/RedPajama_Analysis/SetJoin/data/ngram/sorted_ngrams/" + name +"_sortedngram.bin";
    //     randloadIntBin(ds_path, docs);
    // }

    // string c4_dir[] = {"2020-05/", "2021-04/", "2022-05/"};
    // // load some c4 files
    // for(int j = 0 ; j< 3 ;j++)
    //     for( int i = 0; i <40; i++){
    //         string commoncrawl_subfile_path = "/research/projects/zp128/RedPajama_Analysis/LSH/slimpajama/common_crawl/" + c4_dir[j] + to_string(i)+ ".bin";
    //         loadIntBin(commoncrawl_subfile_path, docs);
    //     }

    vector<int> idmap_docs;
    for (int i = 0; i < docs.size(); i++)
        idmap_docs.emplace_back(i);

    cout << "Generating idmap " << endl;
    sort(execution::par_unseq, idmap_docs.begin(), idmap_docs.end(), [&docs](const unsigned int &id1, const unsigned int &id2) {
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

    ofstream ofs;
    ofs.open("./sample_small.bin", ios::binary);
    for (int i = 0; i < idmap_docs.size(); i++) {
        const auto &vec = docs[idmap_docs[i]];
        unsigned int size = vec.size();

        ofs.write((char *)&size, sizeof(unsigned int));
        ofs.write((char *)&vec[0], size * sizeof(unsigned int)); // Write the vector data
    }
    ofs.close();

}