#pragma once
#include <bits/stdc++.h>
using namespace std;

// load the vector<int> of a bin file and push back to docs
void loadShortBin(const string &binFileName, vector<vector<unsigned short>> &docs);
void writeSimilarPair(const string &binFileName, const vector<pair<int, int>> &result_pairs);
std::string extract_prefix(const std::string& filepath);