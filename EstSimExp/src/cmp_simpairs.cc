/*
    Choose the simpairs from SetJoin and see what is the FN of those sketches
    Attention here we use simpairs from those generated from ngrams
    Therefore the size of token is 32 bits

    Cause the document length of data is mostly more than 200-13
*/
#include <bits/stdc++.h>
#include "../src/util/io.h"
#include "../src/util/util.h"
using namespace std;

unordered_map<unsigned int, vector<unsigned int>> docs;

void generateHashFunc(unsigned int seed, pair<unsigned int, unsigned int> &hf)
{
    srand(seed);
    unsigned int a = 0;
    while (a == 0)
        a = rand();
    unsigned int b = rand();
    hf.first = a;
    hf.second = b;
}

void load_docs(vector<unsigned int> &doc_ids, const string binFileName, bool tokenOrngrams)
{
    sort(doc_ids.begin(), doc_ids.end());

    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs)
    { // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return; // Exit the function
    }
    unsigned int cur_docid = 0;
    size_t cnt = 0;
    int size; // Initialize a variable to store the size of each vector

    if(tokenOrngrams == false){
        while (ifs.read((char *)&size, sizeof(int)))
        { // Read the size of the vector
            if (cnt >= doc_ids.size())
                break;

            if (cur_docid == doc_ids[cnt])
            {
                docs[cur_docid].resize(size);
                ifs.read((char *)&docs[cur_docid][0], sizeof(unsigned int) * size); // Read the data into the vector
                cnt++;
            }
            else
            {
                // skip
                ifs.seekg(sizeof(unsigned int) * size, std::ios::cur);
            }

            cur_docid++;
        }
        ifs.close(); // Close the file stream after reading
    }else{
        while (ifs.read((char *)&size, sizeof(int)))
        { // Read the size of the vector
            if (cnt >= doc_ids.size())
                break;

            if (cur_docid == doc_ids[cnt])
            {
                docs[cur_docid].resize(size);
                vector<unsigned short> vec(size);                         // Create a vector of the read size
                ifs.read((char *)&vec[0], sizeof(unsigned short) * size); // Read the data into the vector
                copy(vec.begin(), vec.end(), docs[cur_docid].begin());  
                cnt++;
            }
            else
            {
                // skip
                ifs.seekg(sizeof(unsigned short) * size, std::ios::cur);
            }

            cur_docid++;
        }
        ifs.close(); // Close the file stream after reading
    }
    
}
// The hash value function
inline unsigned int hval(const pair<unsigned int, unsigned int> &hf, unsigned int &word)
{
    return hf.first * word + hf.second;
}

// build bottomks
void build_bks(const vector<unsigned int> &doc_ids)
{
    pair<unsigned int, unsigned int> hf; // hash functions
    generateHashFunc(1, hf);

#pragma omp parallel for
    for (int i = 0; i < doc_ids.size(); i++)
    {
        auto id = doc_ids[i];
        // build bottom k for this document
        for (auto &ele : docs[id])
        {
            ele = hval(hf, ele);
        }
        sort(docs[id].begin(), docs[id].end());
    }
}

void getFNDocRate(double* doc_fN, const vector<unsigned int> & docIds,  vector<unsigned int> * choose_docIds){
    for(int i = 0; i< 4;i++){
        doc_fN[i] = (difference(docIds,choose_docIds[i])*1.0) / docIds.size();
    }
}

int main(int argc, char *argv[])
{
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]);
    const double thres = strtod(argv[2], nullptr);
    bool tokensOrngrams = false;
    string sortedsets_file_path, simP_file_path;
    if (tokensOrngrams == false)
    {
        sortedsets_file_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name + "_sortedngram.bin";
        simP_file_path = root_dir + "/data/ngram/ngrams_simp/" + dataset_name + "_sim_pairs_" + to_string(thres) + ".bin";
    }
    else
    {
        sortedsets_file_path = root_dir + "/sorted_sets/" + dataset_name + "_sortedsets.bin";
        simP_file_path = root_dir + "/sorted_simp/" + dataset_name + "_sim_pairs_" + to_string(thres) + ".bin";
    }
    const unsigned int min_k = stoul(argv[3]);
    double ratio = 0.4;

    //  Cause the document length of data is mostly more than 200-13
    assert(min_k <= 183);

    // Load the similar pairs
    vector<pair<unsigned int, unsigned int>> pairs;
    readSimilarPair(simP_file_path, pairs);

    // Load sortedsets
    vector<unsigned int> doc_ids = getUniqueInts(pairs);
    load_docs(doc_ids, sortedsets_file_path,tokensOrngrams);

    // building their bottom ks
    build_bks(doc_ids);

    cout << "Iterating each pair" << endl;

    // Iterate each pair and calculate its estimated jaccards
    vector<double> true_jaccard(pairs.size());
    vector<double> fixk_bottomk(pairs.size());
    vector<double> fixk_jaccard(pairs.size());
    vector<double> adpativek_bottomk(pairs.size());
    vector<double> adpativek_jaccard(pairs.size());
    vector<unsigned int> adaptiveKs(pairs.size());
    double error[4] = {0, 0, 0, 0};
    double fp[4] = {0, 0, 0, 0};
    double fp_docs[4] = {0, 0, 0, 0};
    vector<vector<double>> data(pairs.size(), vector<double>(5));
#pragma omp parallel for
    for (int i = 0; i < pairs.size(); i++)
    {
        true_jaccard[i] = jaccard_similarity(docs[pairs[i].first], docs[pairs[i].second]);

        vector<unsigned short> bk_A(docs[pairs[i].first].begin(), docs[pairs[i].first].begin() + min_k);
        vector<unsigned short> bk_B(docs[pairs[i].second].begin(), docs[pairs[i].second].begin() + min_k);

        fixk_bottomk[i] = bottomKJaccard(bk_A, bk_B);
        fixk_jaccard[i] = jaccard_similarity(bk_A, bk_B);

        auto min_adpativek = int(min(docs[pairs[i].first].size() * ratio, docs[pairs[i].second].size() * ratio));
        adaptiveKs[i] = min_adpativek;
        vector<unsigned short> adpbk_A(docs[pairs[i].first].begin(), docs[pairs[i].first].begin() + min_adpativek);
        vector<unsigned short> adpbk_B(docs[pairs[i].second].begin(), docs[pairs[i].second].begin() + min_adpativek);

        adpativek_bottomk[i] = bottomKJaccard(adpbk_A, adpbk_B);
        adpativek_jaccard[i] = jaccard_similarity(adpbk_A, adpbk_B);

        // record data
        data[i][0] = true_jaccard[i];
        data[i][1] = fixk_bottomk[i];
        data[i][2] = fixk_jaccard[i];
        data[i][3] = adpativek_bottomk[i];
        data[i][4] = adpativek_jaccard[i];
    }

    if (tokensOrngrams == false)
    {
        const string csv_path = "./csv/ngrams/similar_pairs/" + dataset_name + "_" + to_string(min_k) + "_" + to_string(thres).substr(0, 3) + "_" + to_string(ratio).substr(0, 3) + ".csv";
        writeEstJaccardCSV(csv_path, data);
    }
    else
    {
        const string csv_path = "./csv/tokens/similar_pairs/" + dataset_name + "_" + to_string(min_k) + "_" + to_string(thres).substr(0, 3) + "_" + to_string(ratio).substr(0, 3) + ".csv";
        writeEstJaccardCSV(csv_path, data);
    }
    // Record error and calculate false positive
    unsigned long long ave_adpk = 0;
    vector<unsigned int> * choose_docIds = new vector<unsigned int>[4];
    for (int i = 0; i < pairs.size(); i++)
    {
        error[0] += abs(fixk_bottomk[i] - true_jaccard[i]);
        error[1] += abs(fixk_jaccard[i] - true_jaccard[i]);
        error[2] += abs(adpativek_bottomk[i] - true_jaccard[i]);
        error[3] += abs(adpativek_jaccard[i] - true_jaccard[i]);
        fp[0] += fixk_bottomk[i] < thres ? 1 : 0;
        fp[1] += fixk_jaccard[i] < thres ? 1 : 0;
        fp[2] += adpativek_bottomk[i] < thres ? 1 : 0;
        fp[3] += adpativek_jaccard[i] < thres ? 1 : 0;
        ave_adpk += adaptiveKs[i];

        if(fixk_bottomk[i] >= thres){
            choose_docIds[0].emplace_back(pairs[i].first);
            choose_docIds[0].emplace_back(pairs[i].second);
        }
        if(fixk_jaccard[i] >= thres){
            choose_docIds[1].emplace_back(pairs[i].first);
            choose_docIds[1].emplace_back(pairs[i].second);
        }
        if(adpativek_bottomk[i] >= thres){
            choose_docIds[2].emplace_back(pairs[i].first);
            choose_docIds[2].emplace_back(pairs[i].second);
        }
        if(adpativek_jaccard[i] >= thres){
            choose_docIds[3].emplace_back(pairs[i].first);
            choose_docIds[3].emplace_back(pairs[i].second);
        }
    }

    // Unique the choose doc ids and then calculate the FP ratio of documents
    for(int i =0; i<4;i++){
        sort(choose_docIds[i].begin(), choose_docIds[i].end());
        auto it = unique(choose_docIds[i].begin(), choose_docIds[i].end());
        choose_docIds[i].resize(distance(choose_docIds[i].begin(), it));
    }
    getFNDocRate(fp_docs, doc_ids, choose_docIds);

    // Average errors
    ave_adpk /= true_jaccard.size();
    for (auto &e : error)
    {
        e = e / true_jaccard.size();
    }

    for (auto &rate : fp)
    {
        rate = rate / true_jaccard.size();
    }
    // print out the result
    // printf("Now print out errors:\n |fixk_bottomk|fixk_jaccard| adpativek_bottomk| adpativek_jaccard|\n");
    printf("|%s|%.3f |%.3f|%.3f| %.3f |\n", dataset_name.c_str(), error[0], error[1], error[2], error[3]);
    printf("|%s|%.3f |%.3f|%.3f| %.3f |\n", dataset_name.c_str(), fp[0], fp[1], fp[2], fp[3]);
    printf("|%s|%.3f |%.3f|%.3f| %.3f |\n", dataset_name.c_str(), fp_docs[0], fp_docs[1], fp_docs[2], fp_docs[3]);
    printf("Average adpative k on Data(%s): %llu \n ", tokensOrngrams ? "tokens" : "ngrams", ave_adpk);
    return 0;
}