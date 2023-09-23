#include <bits/stdc++.h>

using namespace std;

bool probability10Percent() {
    return (rand() % 100) < 5; // Returns true approximately 10% of the time
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
    printf("There are %lu documents now\n", docs.size(), binFileName.c_str());
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
    string  dataset_names[] = {"arxiv", "book","stackexchange", "github", "wikipedia"};
    vector<vector<unsigned int>> docs;
    for(auto & name: dataset_names){
        string ds_path = "/research/projects/zp128/RedPajama_Analysis/SetJoin/data/ngram/sorted_ngrams/" + name +"_sortedngram.bin";
        randloadIntBin(ds_path, docs);
    }

    // load some c4 files
    for( int i = 0; i <10; i++){
        string c4_subfile_path = "/research/projects/zp128/RedPajama_Analysis/LSH/slimpajama/c4/" + to_string(i)+ ".bin";
        loadIntBin(c4_subfile_path, docs);
    }

    vector<int> idmap_docs;
    for (int i = 0; i < docs.size(); i++)
        idmap_docs.emplace_back(i);

    cout << "Generating idmap " << endl;
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

    ofstream ofs;
    ofs.open("./sample.bin", ios::binary);
    for (int i = 0; i < idmap_docs.size(); i++) {
        const auto &vec = docs[idmap_docs[i]];
        unsigned int size = vec.size();

        ofs.write((char *)&size, sizeof(unsigned int));
        ofs.write((char *)&vec[0], size * sizeof(unsigned int)); // Write the vector data
    }
    ofs.close();

}