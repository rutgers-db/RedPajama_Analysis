
#ifndef _UTIL_H_
#define _UTIL_H_
#include <bits/stdc++.h>
#include <sys/sysinfo.h>

using namespace std;
void print_memory();
std::vector<int> getUniqueInts(const std::vector<std::pair<int, int>>& pairs);
void mergeArrays(std::vector<std::vector<pair<int,int>>>* input, int arr_len, std::vector<std::vector<pair<int,int>>> & result);

#endif
  