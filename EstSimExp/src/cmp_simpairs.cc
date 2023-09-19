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
unordered_map<unsigned int, vector<unsigned int>> Lsketches;

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

unsigned int load_docs(vector<unsigned int> &doc_ids, const string binFileName, bool tokenOrngrams, const unsigned int min_k)
{
    sort(doc_ids.begin(), doc_ids.end());

    cout << "Reading " << binFileName << endl; // Print the name of the file being read
    ifstream ifs(binFileName, ios::binary);    // Open the binary file for reading
    if (!ifs)
    { // If the file cannot be opened or does not exist, print an error message
        cout << "error open bin file" << endl;
        return 0; // Exit the function
    }
    unsigned int short_docs_cnt = 0;
    unsigned int cur_docid = 0;
    size_t cnt = 0;
    int size; // Initialize a variable to store the size of each vector

    if(tokenOrngrams == false){
        while (ifs.read((char *)&size, sizeof(int)))
        { 
            // Read the size of the vector
            if (cnt >= doc_ids.size())
                break;

            if (cur_docid == doc_ids[cnt])
            {
                cnt++;
                if(size >= min_k){
                    docs[cur_docid].resize(size);
                    ifs.read((char *)&docs[cur_docid][0], sizeof(unsigned int) * size); // Read the data into the vector
                }
                else{
                    ifs.seekg(sizeof(unsigned int) * size, std::ios::cur);
                    short_docs_cnt ++;
                }
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
                cnt++;
                if(size >= min_k){
                    docs[cur_docid].resize(size);
                    vector<unsigned short> vec(size);                         // Create a vector of the read size
                    ifs.read((char *)&vec[0], sizeof(unsigned short) * size); // Read the data into the vector
                    copy(vec.begin(), vec.end(), docs[cur_docid].begin());  
                }else{
                    ifs.seekg(sizeof(unsigned short) * size, std::ios::cur);
                    short_docs_cnt ++;
                }
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
    if (short_docs_cnt>0)
        printf("There are %u documents in the ground truth similar docs are shorter then %d min_k", short_docs_cnt,  min_k);
    
    return short_docs_cnt;
}
// The hash value function
inline unsigned int hval(const pair<unsigned int, unsigned int> &hf, unsigned int &word)
{
    return hf.first * word + hf.second;
}

unsigned int find_maxElement(const vector<unsigned int> &doc_ids){
    unsigned int tmp_elements[128]={0};
#pragma omp parallel for
    for (int i = 0; i < doc_ids.size(); i++)
    {
        auto id = doc_ids[i];
        auto tid = omp_get_thread_num();
        // build bottom k for this document
        for (auto &ele : docs[id])
        {   
            
            tmp_elements[tid] = max(tmp_elements[tid], ele);
        }
    }

    return (*max_element(tmp_elements,tmp_elements+128));
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
    for(int i = 0; i< 5;i++){
        doc_fN[i] = (difference(docIds,choose_docIds[i])*1.0) / docIds.size();
    }
}



int main(int argc, char *argv[])
{
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]); //
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
    double ratio = 0.25;
    unsigned int M = 4;

    //  Cause the document length of data is mostly more than 200-13
    // assert(min_k <= 183);

    // Load the similar pairs
    vector<pair<unsigned int, unsigned int>> pairs;
    readSimilarPair(simP_file_path, pairs);

    // Load sortedsets
    vector<unsigned int> doc_ids = getUniqueInts(pairs);
    unsigned int short_cnt = load_docs(doc_ids, sortedsets_file_path,tokensOrngrams, min_k);

    // Due to the mink may be filter some short documents we need regenerate the doc_ids
    if(short_cnt != 0){
        doc_ids.clear();
        for(auto it : docs){
            doc_ids.emplace_back(it.first);
        }
        sort(doc_ids.begin(), doc_ids.end());

        // erase those pairs having short documents
        size_t orignal_pairsamount = pairs.size();
        pairs.erase(
        std::remove_if(pairs.begin(), pairs.end(), 
                       [&](const std::pair<int, int>& p) {
                           return docs.find(p.first) == docs.end() || 
                                  docs.find(p.second) == docs.end();
                       }), 
        pairs.end());
        printf("We remove %lu pairs\n", orignal_pairsamount - pairs.size());
    }
    printf("Parameter shown: ratio %f M %u \n", ratio, M);
    
    
    
    // building their bottom ks
    // size_t max_ele = find_maxElement(doc_ids);
    // printf("Max elemenet of it is: %lu\n", max_ele);
    
    // build the permutation
    // vector<unsigned int> perm(max_ele+1);
    // iota(perm.begin(), perm.end(), 0);
    // perm = randomPermutation(perm);
    // build the sketch from function L
for (int i = 0; i < doc_ids.size(); i++)
    {   
        auto id = doc_ids[i];
        Lsketches[id] = vector<unsigned int> ();
    }

    pair<unsigned int, unsigned int> hf; // hash functions
    generateHashFunc(1, hf);
    
    unsigned long long lsketch_sizes[128] = {0};
    unsigned long long origal_sizes[128] = {0};
#pragma omp parallel for
    for (int i = 0; i < doc_ids.size(); i++)
    {   
        auto id = doc_ids[i];
        auto & lsketch = Lsketches[id];
        auto tid = omp_get_thread_num();
        // build bottom k for this document
        for (auto ele : docs[id])
        {   
            // ele = perm[ele];            
            ele = hval(hf, ele);
            if(ele%M == 0){
                lsketch.emplace_back(ele);
            }
        }
        origal_sizes[tid] += docs[id].size();
        lsketch_sizes[tid] += lsketch.size();
    }
    
    build_bks(doc_ids);

    cout << "Iterating each pair" << endl;

    // Iterate each pair and calculate its estimated jaccards
    vector<double> true_jaccard(pairs.size());
    vector<double> fixk_bottomk(pairs.size());
    vector<double> fixk_jaccard(pairs.size());
    vector<double> adpativek_bottomk(pairs.size());
    vector<double> adpativek_jaccard(pairs.size());
    vector<double> lsketch_jaccard(pairs.size());

    vector<unsigned int> adaptiveKs(pairs.size());
    double error[5] = {0, 0, 0, 0,0};
    double fp[5] = {0, 0, 0, 0,0};
    double fp_docs[5] = {0, 0, 0, 0,0};
    vector<vector<double>> data(pairs.size(), vector<double>(6));

    int myJaccardNum = 0;
#pragma omp parallel for
    for (auto i = 0; i < pairs.size(); i++)
    {
        true_jaccard[i] = jaccard_similarity(docs[pairs[i].first], docs[pairs[i].second]);

        vector<unsigned int> bk_A(docs[pairs[i].first].begin(), docs[pairs[i].first].begin() + min_k);
        vector<unsigned int> bk_B(docs[pairs[i].second].begin(), docs[pairs[i].second].begin() + min_k);

        fixk_bottomk[i] = bottomKJaccard(bk_A, bk_B);
        fixk_jaccard[i] = jaccard_similarity(bk_A, bk_B);

        unsigned int min_adpativek = min(docs[pairs[i].first].size() * ratio, docs[pairs[i].second].size() * ratio);
        adaptiveKs[i] = min_adpativek;
        vector<unsigned int> adpbk_A(docs[pairs[i].first].begin(), docs[pairs[i].first].begin() + min_adpativek);
        vector<unsigned int> adpbk_B(docs[pairs[i].second].begin(), docs[pairs[i].second].begin() + min_adpativek);

        adpativek_bottomk[i] = bottomKJaccard(adpbk_A, adpbk_B);
        adpativek_jaccard[i] = jaccard_similarity(adpbk_A, adpbk_B);

        lsketch_jaccard[i]= jaccard_similarity(Lsketches[pairs[i].first], Lsketches[pairs[i].second]);
        // record data
        data[i][0] = true_jaccard[i];
        data[i][1] = fixk_bottomk[i];
        data[i][2] = fixk_jaccard[i];
        data[i][3] = adpativek_bottomk[i];
        data[i][4] = adpativek_jaccard[i];
        data[i][5] = lsketch_jaccard[i];

        if(bottomKJaccard_2(adpbk_A, adpbk_B, thres) == false)
            myJaccardNum ++ ;
    }

    cout << "Writing the csv data" << endl;

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
    vector<unsigned int> * choose_docIds = new vector<unsigned int>[5];
    for (int i = 0; i < pairs.size(); i++)
    {
        error[0] += abs(fixk_bottomk[i] - true_jaccard[i]);
        error[1] += abs(fixk_jaccard[i] - true_jaccard[i]);
        error[2] += abs(adpativek_bottomk[i] - true_jaccard[i]);
        error[3] += abs(adpativek_jaccard[i] - true_jaccard[i]);
        error[4] += abs(lsketch_jaccard[i] - true_jaccard[i]);

        fp[0] += fixk_bottomk[i] < thres ? 1 : 0;
        fp[1] += fixk_jaccard[i] < thres ? 1 : 0;
        fp[2] += adpativek_bottomk[i] < thres ? 1 : 0;
        fp[3] += adpativek_jaccard[i] < thres ? 1 : 0;
        fp[4] += lsketch_jaccard[i] < thres ? 1 : 0;

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
        if(lsketch_jaccard[i] >= thres){
            choose_docIds[4].emplace_back(pairs[i].first);
            choose_docIds[4].emplace_back(pairs[i].second);
        }
    }

    // Unique the choose doc ids and then calculate the FP ratio of documents
    for(int i =0; i<5;i++){
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

    unsigned long long total_sketchsizes = 0;
    unsigned long long total_aveBottomksizes = 0;
    for (unsigned int i = 0; i< 128;i++){
        total_sketchsizes += lsketch_sizes[i];
        total_aveBottomksizes += origal_sizes[i];
    }
    total_aveBottomksizes /= docs.size();
    total_aveBottomksizes = (unsigned long long) total_aveBottomksizes * ratio;
    total_sketchsizes /= Lsketches.size();
    // print out the result
    // printf("Now print out errors:\n |fixk_bottomk|fixk_jaccard| adpativek_bottomk| adpativek_jaccard|\n");
    printf("|%s|%.3f |%.3f|%.3f| %.3f | %.3f |\n", dataset_name.c_str(), error[0], error[1], error[2], error[3], error[4]);
    printf("|%s|%.3f |%.3f|%.3f| %.3f | %.3f |\n", dataset_name.c_str(), fp[0], fp[1], fp[2], fp[3], fp[4]);
    printf("|%s|%.3f |%.3f|%.3f| %.3f | %.3f |\n", dataset_name.c_str(), fp_docs[0], fp_docs[1], fp_docs[2], fp_docs[3], fp_docs[4]);
    printf("Average adpative k on Data(%s): %llu \n ", tokensOrngrams ? "tokens" : "ngrams", ave_adpk);
    printf("Average L sketch: %llu and Average Bottomk size: %llu \n ", total_sketchsizes, total_aveBottomksizes);
    cout << (myJaccardNum*1.0/true_jaccard.size());
    return 0;
}