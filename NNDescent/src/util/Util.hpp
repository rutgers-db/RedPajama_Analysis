/**
 * Utility function
 * 
 * Author: Weiqi Feng
 * Date:   Jan 30, 2020
 * Email:  fengweiqi@sjtu.edu.cn
 */

#ifndef UTIL_H
#define UTIL_H
#include <iostream>
#include <unordered_set>
#include <vector>
#include <chrono>
 #include <fstream> 
#include <random>
#include <set>
#include <omp.h>

using namespace std;

#define INIT_VISIT_TIME 0
#define INIT_GLOBAL_TIME 1

static short CLOCK = INIT_GLOBAL_TIME;
double FACTOR = 3.0;

vector<std::mt19937> generators;

void InitRandom(int num_thread) {
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < num_thread; i++) {
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
        generators.emplace_back(duration.count());
    }
}

int intRand(const int &min = 0, const int &max = std::numeric_limits<int>::max()) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generators[omp_get_thread_num()]);
}

std::chrono::_V2::system_clock::time_point LogTime() {
    return chrono::high_resolution_clock::now();
}

double RepTime(const std::chrono::_V2::system_clock::time_point &start) {
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    return duration.count() / 1000000.0;
}

// Calculate the recall based on exp output and standard answer
// (notice the second argument should be standard answer)
double recall(vector<vector<int>> exp, vector<vector<int>> ans) {
    int totalCnt = 0;
    int correct = 0;
    for (int i = 0; i < exp.size(); i++) {
        totalCnt += ans[i].size();
        for (int j = 0; j < exp[i].size(); j++) {
            for (auto val : ans[i]) {
                if (val == exp[i][j]) {
                    correct += 1;
                    break;
                }
            }
        }
    }

    return double(correct) / totalCnt;
}

// Calculate the recall based on exp output and standard answer
// (notice the second argument should be standard answer)
// the answers have no particular order
double knn_recall(vector<vector<int>> exp, vector<vector<int>> ans) {
    int totalCnt = 0;
    int correct = 0;

    for (int i = 0; i < ans.size(); i++) {
        totalCnt += ans[i].size();

        unordered_set<int> res;
        for (auto r : ans[i])
            res.insert(r);

        for (auto e : exp[i])
            if (res.find(e) != res.end())
                correct += 1;
    }

    return double(correct) / totalCnt;
}

// Return the union of two vectors
vector<int> unionVecs(const vector<int> &vec1, const vector<int> &vec2, int omit) {
    vector<int> r;
    for (auto v : vec1) {
        if (v != omit && find(r.begin(), r.end(), v) == r.end()) {
            r.push_back(v);
        }
    }
    for (auto v : vec2) {
        if (v != omit && find(r.begin(), r.end(), v) == r.end()) {
            r.push_back(v);
        }
    }
    return r;
}

// Return the intersection of two vectors
vector<int> intersectVecs(const vector<int> &vec1, const vector<int> &vec2) {
    set<int> s;
    vector<int> r;

    if (vec1.empty() || vec2.empty())
        return r;

    for (auto v : vec1) {
        s.insert(v);
    }

    for (auto v : vec2) {
        if (s.find(v) != s.end())
            r.push_back(v);
    }

    return r;
}

template<class T>
void print_vector(const vector<T> &vt) {
    auto it = vt.begin();
    for (; it != vt.end(); it++) {
        cout << (*it) << " ";
    }
    cout << endl;
}

// save a vector to a csv file
template<class T>
void saveCsv_vector(string file_name, const vector<T> &vt){
    if(vt.size()==0)
        return;
        
    ofstream outputfile;
    outputfile.open (file_name);
    if (outputfile.is_open()) {
       for(int i=0;i<vt.size();i++){
           if(i!=vt.size()-1){
               outputfile<<vt[i]<<",";
           }else{
               outputfile<<vt[i]<<endl;
           }
       }
       outputfile.close();
    }
}

// save vectors to a csv file
template<class T>
void saveCsv_vecs(string file_name, const vector<vector<T>> &vectors){
    ofstream outputfile;
    outputfile.open (file_name);

    if (outputfile.is_open()) {
       for(int i=0;i<vectors.size();i++){
           for(int j=0;j<vectors[i].size();j++){
               if(j!=vectors[i].size()-1){
                    outputfile<<vectors[i][j]<<",";
                }else{
                    outputfile<<vectors[i][j]<<endl;
                }
           }
       }
       outputfile.close();
    }
}

template<class T>
double bottomKJaccard(const std::vector<T>& A, const std::vector<T>& B){
    // Adpative K
    unsigned int k = min(A.size(),B.size());

    unsigned int posx =0;
    unsigned int posy = 0;
    
    unsigned int current_overlap = 0;
    unsigned int missing_ele = 0;
    while (current_overlap + missing_ele < k && posx < k && posy < k) {

        if (A[posx] == B[posy]) { 
            current_overlap++;
            posx++;
            posy++;
        } else if (A[posx] < B[posy]) {
            posx++;
            missing_ele++;
        } else {
            posy++;
            missing_ele++;
        }
    }
    return 1 - double(current_overlap)/k;
}

template<class T>
double Jaccard(const std::vector<T>& A, const std::vector<T>& B){

    unsigned int posx =0;
    unsigned int posy = 0;
    
    unsigned int current_overlap = 0;
    unsigned int missing_ele = 0;
    while (posx < A.size() && posy < B.size() ) {

        if (A[posx] == B[posy]) { 
            current_overlap++;
            posx++;
            posy++;
        } else if (A[posx] < B[posy]) {
            posx++;
            missing_ele++;
        } else {
            posy++;
            missing_ele++;
        }
    }

    missing_ele += A.size() - posx + B.size() - posy;

    return 1 - double(current_overlap)/(current_overlap + missing_ele);
}

inline unsigned int hval(const pair<unsigned int, unsigned int> &hf, unsigned int &word)
{
    return hf.first * word + hf.second;
}

void build_bks(vector<unsigned int> & doc, const pair<unsigned int, unsigned int> &hf)
{
    for (auto &ele : doc)
    {
        ele = hval(hf, ele);
    }
    sort(doc.begin(), doc.end());
}

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




#endif
