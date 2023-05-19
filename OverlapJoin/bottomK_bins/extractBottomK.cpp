/*
    This is a script that can extract the bottom k of all the bin file, reading the idx file.
    Then it will store it and output the average length of all document read.
    The token number of the 20B tokenizer is 50277
*/

#include <bits/stdc++.h>

using namespace std;

pair<unsigned short, unsigned short> hf; // hash functions
vector<bool> if_filtered_hashvalues; // Vector to store if a token is filtered (stopword or punctuation)

// Generate random hash functions based on given seed
// ab: the output argument
void generateHashFunc(unsigned int seed, pair<unsigned short, unsigned short> &hf) {
    srand(seed);
    unsigned short a = 0;
    while (a == 0)
        a = rand();
    unsigned short b = rand();
    hf.first = a;
    hf.second = b;
}

// The hash value function
inline unsigned short hval(const pair<unsigned short, unsigned short> &hf, unsigned short &word) {
    return hf.first * word + hf.second;
}

// load the vector<int> of a bin file and push back to docs
void loadBin(const string &binFileName, vector<vector<int>> &docs) {
    ifstream ifs(binFileName, ios::binary);
    if (!ifs) {
        cout << "error open bin file" << endl;
        return;
    }
    int size;
    while (ifs.read((char *)&size, sizeof(int))) {
        vector<int> vec(size);
        ifs.read((char *)&vec[0], sizeof(int) * size);
        docs.emplace_back(vec);
    }
    ifs.close();
}

// Load the filtered_tokens from a bin file and generate filtered_hash_values
void loadFilteredTokens(const string &filtered_tokens_file, int tokenNum) {
    vector<vector<int>> filtered_tokens;
    loadBin(filtered_tokens_file, filtered_tokens);
    if_filtered_hashvalues = vector<bool>(tokenNum, false);
    for (int i = 0; i < filtered_tokens[0].size(); i++)
        if_filtered_hashvalues[filtered_tokens[0][i]] = true;
}

// extract the bottom k
// In this function, we'll use a combination of a std::unordered_map to count the frequency of each number, 
// and a std::priority_queue (acting as a min heap) to keep track of the k smallest unique numbers.
std::vector<unsigned short> kMostMinUnique(const std::vector<unsigned short>& nums, int k) {
    std::unordered_map<unsigned short, int> counts;
    for (unsigned short num : nums) {
        counts[num]++;
    }

    std::priority_queue<unsigned short> minHeap;

    for (auto& pair : counts) {
        if (minHeap.size() < k) {
            minHeap.push(pair.first);
        } else if (pair.first < minHeap.top()) {
            minHeap.pop();
            minHeap.push(pair.first);
        }
    }

    std::vector<unsigned short> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top());
        minHeap.pop();
    }

    std::reverse(result.begin(), result.end());
    return result;
}

int main(int argc, char *argv[]){

    int token_num = 50277;
    const int k = 1024;

    //Initialize the hash function
    generateHashFunc(1, hf);

    string parent_dir = string(argv[1]);
    string idx_file_path =  parent_dir+"/tokenized_text_document.idx";
    string bin_file_path = parent_dir+"/tokenized_text_document.bin";
    string bottomK_file_path = parent_dir + "/bottom_" + to_string(k) + ".bin";
    
    // Reading the bin file using the idx file
    // Iterate the document and process them
    ifstream idxFile(idx_file_path.c_str(), ios::in | ios::binary);
    ifstream binFile(bin_file_path.c_str(), ios::in | ios::binary);
    ofstream bottomKFile(bottomK_file_path.c_str(), ios::out | ios::binary);

    idxFile.seekg(18, ios::beg);
    long long tmp;
    idxFile.read((char*)&tmp, 8);
    long long N = tmp;
    printf("There are total %lld documents in this %s\n", N, bottomK_file_path.c_str());
    idxFile.read((char*)&tmp, 8);

    double docLen_sum = 0;
    int min_bottomK_len = 0x7ffffff;
    int min_id = -1;
    for (long long i = 0; i < N; i++) {
        int len;
        idxFile.read((char*)&len, 4);
        docLen_sum+= (double) len;
        
        // Read the file
        vector<unsigned short> entity(len);
        binFile.read((char*)&entity[0], sizeof(unsigned short) * len);

        for(auto& ele: entity){
            ele = hval(hf,ele);
        }
        // Find the minmum length Just for debug
        auto bottomK = kMostMinUnique(entity, k);
        int bottomK_len = bottomK.size();
        if(min_bottomK_len >bottomK_len ){
            min_bottomK_len = bottomK_len;
            min_id = i;
        }

        // Write the bottomk
        bottomKFile.write((char*)&bottomK_len, sizeof(int));
        bottomKFile.write((char*)&bottomK[0], sizeof(unsigned short)*bottomK_len);
    }

    printf("The average size of the document is %f\n", docLen_sum/N);
    printf("The minimum length of those bottom %d is %d, its id is %d (In case there are some short documents)\n", k, min_bottomK_len,min_id);
    idxFile.close();
    binFile.close();
    bottomKFile.close();
}