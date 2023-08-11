
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
double shrinkBottomk(vector<vector<unsigned short>>&  bottom_ks, double ratio);
void printHowManyThreads();
#endif
  