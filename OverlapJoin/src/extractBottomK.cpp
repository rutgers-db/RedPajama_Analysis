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

// Function to find the k smallest unique numbers from the given vector of numbers.
// The function returns a vector of the k smallest unique numbers.
// Parameters:
//   - nums: Input vector of numbers.
//   - k: The number of smallest unique numbers to return.
std::vector<unsigned short> kMostMinUnique(const std::vector<unsigned short>& nums, int k) {
    
    // counts: An unordered_map that keeps track of the frequency of each number in the input vector.
    std::unordered_map<unsigned short, int> counts;
    
    // Populate the frequency map.
    for (unsigned short num : nums) {
        counts[num]++;
    }

    // minHeap: A priority_queue used as a min heap to keep track of the k smallest unique numbers.
    std::priority_queue<unsigned short> minHeap;

    // Iterate through the frequency map.
    for (auto& pair : counts) {
        // If the heap has less than k elements, push the current number into the heap.
        if (minHeap.size() < k) {
            minHeap.push(pair.first);
        } 
        // If the heap has k elements and the current number is less than the largest number in the heap,
        // remove the largest number and insert the current number.
        else if (pair.first < minHeap.top()) {
            minHeap.pop();
            minHeap.push(pair.first);
        }
    }

    // result: A vector to store the k smallest unique numbers.
    std::vector<unsigned short> result;
    
    // Populate the result vector with the numbers from the heap.
    while (!minHeap.empty()) {
        result.push_back(minHeap.top());
        minHeap.pop();
    }

    // The result vector is reversed as the numbers were retrieved from a min heap in descending order.
    std::reverse(result.begin(), result.end());

    return result;
}

int main(int argc, char *argv[]){

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