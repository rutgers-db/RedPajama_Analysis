/*
    Compare the pairs that generated by LSH and SetJoin
    Output performance metrics of LSH such as FP FN
*/
#include <iostream>
#include <fstream>
#include <set>
#include "src/IO.hpp"

using namespace std;
// Assume the size of buffer
const int bufferSize = 10000;
vector<pair<unsigned int, unsigned int>> buffer;
int buffer_id = 100000;
unsigned long long total_left;
bool getBuffer(ifstream &ifs, pair<unsigned int, unsigned int> & res) {
    if(buffer_id >= buffer.size()){
        // start load the left data into the buffer
        if( total_left <= 0){
            return false;
        }

        auto readSize = min(total_left, (unsigned long long)bufferSize);
        buffer.resize(readSize);
        ifs.read((char *)&buffer[0], readSize * sizeof(pair<unsigned int, unsigned int>));
        total_left -= readSize;
        buffer_id = 0;
    }

    res = buffer[buffer_id++];
    return true;
}

bool ifnextBuffer(){
     if(buffer_id >= buffer.size() && total_left == 0)
        return false;
    return true;
}
// fileA -> LSH
// fileB -> Setjoin
void compareFiles(const string &fileA, const string &fileB) {
    // Read the pairs from the binary files
    vector<pair<unsigned int, unsigned int>> pairsA;
    ifstream ifsA(fileA, ios::binary| std::ios::ate);
    cout<<fileA<<endl;
    std::streamsize total_bytes = ifsA.tellg();
    cout<<total_bytes<<endl;
    // for those file have amount first
    ifsA.seekg(sizeof(unsigned long long), std::ios::beg);
    // ifsA.seekg(0, std::ios::beg);
    total_bytes -= sizeof(std::pair<unsigned int, unsigned int>);

    total_left = total_bytes / sizeof(std::pair<unsigned int, unsigned int>);

    cout<<total_left<<endl;

    unsigned long long pairsA_size = total_left;
    if (!ifsA.is_open()) {
        cerr << "Failed to open the files." << endl;
        return;
    }

    // readSimilarPair(fileA, pairsA);
    vector<pair<unsigned int, unsigned int>> pairsB;
    readSimilarPair(fileB, pairsB);

    set<unsigned int> uniqueValuesA;
    set<unsigned int> uniqueValuesB;

    unsigned long long pairsInAnotInB = 0;
    unsigned long long pairsInBnotInA = 0;
    int valuesInAnotInB = 0;
    int valuesInBnotInA = 0;
    unsigned long long  intersectionSize = 0;
    size_t i = 0, j = 0;

    pair<unsigned int, unsigned int> pairA;
    getBuffer(ifsA, pairA);

    while (j < pairsB.size()) {
        pair<unsigned int, unsigned int> pairB = pairsB[j];

        uniqueValuesA.insert(pairA.first);
        uniqueValuesA.insert(pairA.second);
        uniqueValuesB.insert(pairB.first);
        uniqueValuesB.insert(pairB.second);

        if (pairA < pairB) {
            pairsInAnotInB++;
            if(getBuffer(ifsA, pairA) == false) break;
        } else if (pairB < pairA) {
            pairsInBnotInA++;
            j++;
        } else {
            intersectionSize ++;
            j++;
            if(getBuffer(ifsA, pairA) == false) break;
        }
    }

    cout<<"There out";
    while (ifnextBuffer()) {
        getBuffer(ifsA, pairA);
        uniqueValuesA.insert(pairA.first);
        uniqueValuesA.insert(pairA.second);
        pairsInAnotInB++;
    }

    while (j < pairsB.size()) {
        pair<unsigned int, unsigned int> pairB = pairsB[j];
        uniqueValuesB.insert(pairB.first);
        uniqueValuesB.insert(pairB.second);
        pairsInBnotInA++;
        j++;
    }

    // Calculate values
    for (const auto &value : uniqueValuesA)
        if (uniqueValuesB.count(value) == 0)
            valuesInAnotInB++;

    for (const auto &value : uniqueValuesB)
        if (uniqueValuesA.count(value) == 0)
            valuesInBnotInA++;

    cout << "Pairs in file A: " << pairsA_size << endl;
    cout << "Pairs in file B: " << pairsB.size() << endl;
    cout << "Unique values in file A: " << uniqueValuesA.size() << endl;
    cout << "Unique values in file B: " << uniqueValuesB.size() << endl;
    cout << "FP_pairs:  " << pairsInAnotInB << endl;
    cout << "FN_pairs: " << pairsInBnotInA << endl;
    cout << "FP_ids: " << valuesInAnotInB << endl;
    cout << "FN_ids: " << valuesInBnotInA << endl;
    double pairs_fp_rate = 1.0 * pairsInAnotInB / pairsA_size;
    double pairs_fn_rate = 1.0 * pairsInBnotInA / pairsB.size();
    double precision = 1.0 * intersectionSize /(intersectionSize + pairsInAnotInB);
    double recall = 1.0 * intersectionSize /(intersectionSize + pairsInBnotInA);
    double f1 = 2.0 * (precision * recall) / (precision + recall);
    double ids_fp_rate = 1.0 * valuesInAnotInB / uniqueValuesA.size();
    double ids_fn_rate = 1.0 * valuesInBnotInA / uniqueValuesB.size();

    printf("| %llu | %.5f | %.5f|%.5f|%lu|%.5f|%.5f|\n",pairsA_size, pairs_fp_rate, pairs_fn_rate,f1, uniqueValuesA.size(), ids_fp_rate, ids_fn_rate);

}

int main(int argc, char *argv[]){
    const string dataset_name = string(argv[1]);
    const double thres = 0.8;
    int K = 128;
    int band = 6;
    int range = 21;
    int M;
    for (int i = 1; i < argc; i++) {
        const string arg = argv[i];
        if (arg == "-K") {
            K = atoi(argv[i + 1]);
        }

        if (arg == "-band") {
            band = atoi(argv[i + 1]);
        }

        if (arg == "-range") {
            range = atoi(argv[i + 1]);
        }

    }

    const string fileA_simp_path = "./ngram_simps/" + dataset_name + "_sim_pairs_" + "K" + to_string(K) + "B" + to_string(band) + "R" + to_string(range) + ".bin";
    const string fileB_simp_path = "/research/projects/zp128/RedPajama_Analysis/SetJoin/data/ngram/sorted_simp/" + dataset_name+ "_sim_pairs_" + to_string(thres)+  ".bin";
    cout<< fileA_simp_path<<endl;
    // const string fileB_simp_path = "/research/projects/zp128/RedPajama_Analysis/SetJoin/sorted_testsimp.bin";
    // const string fileB_simp_path = "./test_simp.bin";
    compareFiles(fileA_simp_path, fileB_simp_path);

}
