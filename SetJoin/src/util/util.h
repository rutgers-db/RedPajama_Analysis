
#ifndef _UTIL_H_
#define _UTIL_H_
#include <bits/stdc++.h>
#include <sys/sysinfo.h>
#include <omp.h>
using namespace std;
void print_memory();
std::vector<int> getUniqueInts(const std::vector<std::pair<int, int>>& pairs);
void mergeArrays(std::vector<std::vector<pair<int,int>>>* input, int arr_len, std::vector<std::vector<pair<int,int>>> & result);
std::chrono::_V2::system_clock::time_point LogTime();
double RepTime(const std::chrono::_V2::system_clock::time_point &start);
double shrinkBottomk(vector<vector<unsigned int>>&  bottom_ks, double ratio);
template<class T>
bool bottomKJaccard(const std::vector<T>& A, const std::vector<T>& B, double& thres){
    // Adpative K
    unsigned int k = min(A.size(),B.size());

    unsigned int posx =0;
    unsigned int posy = 0;
    
    unsigned int current_overlap = 0;
    unsigned int required_overlap = ceil(thres * k);
    unsigned int missing_limit = k - required_overlap;
    unsigned int missing_ele = 0;
    while (posx < k && posy < k) {

        // Check if the missing elements is more than the limit
        // Check if remaining elements are sufficient for required overlap
        if (missing_ele > missing_limit) return false;
        if (current_overlap >= required_overlap) return true;

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
    return current_overlap >= required_overlap;
}
void printHowManyThreads();
#endif
  