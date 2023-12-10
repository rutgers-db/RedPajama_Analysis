#include<bits/stdc++.h>

using namespace std;

// Function to load binary file into a 2D vector (documents)
void loadIntBin(const string &binFileName, vector <unsigned int> & lens) {
    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs) {                                // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    unsigned int size;                                                     // Initialize a variable to store the size of each vector
    while (ifs.read((char *) &size, sizeof(unsigned int))) {                // Read the size of the vector
        ifs.seekg(sizeof(unsigned int) * size, std::ios::cur);
        lens.emplace_back(size);                                   // Add the vector to the documents
    }
    ifs.close(); // Close the file stream after reading
    printf("There are %lu documents in %s\n", lens.size(), binFileName.c_str());
}

double findMax(const std::vector<double>& arr) {
    
    auto max_value = arr[0];
    for(auto num : arr) {
        if(num > max_value) {
            max_value = num;
        }
    }
    return max_value;
}

int main(){
    string dataset_names[] = {"arxiv", "github", "stackexchange", "wikipedia","book", "sample"};
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin/data/ngram/sorted_ngrams/";
    
    double thres = 0.8;
    double ALPHA = ALPHA = 1.0 / thres + 0.0001;

    
    
    // const string sortedsets_file_path = root_dir + "/test/sample.bin";
    // loadIntBin(sortedsets_file_path, docs_length);

    // load all the lengths of the files above
    for(auto & dataset_name : dataset_names){
        const string sortedsets_file_path = root_dir + dataset_name +"_sortedngram.bin";
        vector<unsigned int> docs_length;
        loadIntBin(sortedsets_file_path, docs_length);
         vector<double> groups_size;
        unsigned int low = 0, high = 0;
        unsigned int group_len = 0;
        for (unsigned int rid = 0; rid < docs_length.size(); rid++) {
            unsigned int len = docs_length[rid];
            if (len > high) {
                low = len;
                high = low * ALPHA;
                groups_size.push_back((double)len);
                group_len++;
            }else{
                groups_size[group_len-1] += len;
            }
        }

        cout<< dataset_name<< ": "<<findMax(groups_size)/250000000<<endl;
    }
    
    // sort(docs_length.begin(),docs_length.end());

   
}