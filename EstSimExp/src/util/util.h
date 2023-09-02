
#ifndef _UTIL_H_
#define _UTIL_H_
#include <bits/stdc++.h>
#include <sys/sysinfo.h>
#include <omp.h>
using namespace std;
void print_memory();
std::vector<unsigned int> getUniqueInts(const std::vector<std::pair<unsigned int,unsigned int>>& pairs);
void mergeArrays(std::vector<std::vector<pair<int,int>>>* input, int arr_len, std::vector<std::vector<pair<int,int>>> & result);
std::chrono::_V2::system_clock::time_point LogTime();
double RepTime(const std::chrono::_V2::system_clock::time_point &start);
double shrinkBottomk(vector<vector<unsigned short>>&  bottom_ks, double ratio);
double jaccard_similarity(const std::vector<unsigned short>& text1, const std::vector<unsigned short>& text2);
double jaccard_similarity(const std::vector<unsigned int>& text1, const std::vector<unsigned int>& text2);
void printHowManyThreads();
double bottomKJaccard(const std::vector<unsigned short>& A, const std::vector<unsigned short>& B);
double bottomKJaccard(const std::vector<unsigned int>& A, const std::vector<unsigned int>& B);
bool bottomKJaccard_2(const std::vector<unsigned int>& A, const std::vector<unsigned int>& B, double thres);
size_t difference(const std::vector<unsigned int>& A, const std::vector<unsigned int>& B);
#endif
  