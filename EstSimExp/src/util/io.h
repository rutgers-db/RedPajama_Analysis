#pragma once
#include <bits/stdc++.h>
using namespace std;

// load the vector<int> of a bin file and push back to docs
void loadTokensBasedOnIDX(const string & dir_path, vector<vector<unsigned short>> &docs);
void loadShortBin(const string &binFileName, vector<vector<unsigned short>> &docs);
void loadIntBin(const string &binFileName, vector<vector<unsigned int>> &docs);
void loadBin2vec(const string &binFileName, vector<int> &vec);
void writeVec2Bin(const string &binFileName, vector<int> &vec);
void readDividedList(const string &binFileName, vector<vector<pair<int,int>>> &res_lists);
void readSimilarPair(const string &binFileName,  vector<pair<int, int>> &sim_pairs);
void readSimilarPair(const string &binFileName, vector<pair<unsigned int, unsigned int>> &sim_pairs);
void writeSimilarPair(const string &binFileName, const vector<pair<int, int>> &result_pairs);
std::string extract_prefix(const std::string& filepath);
void getFiles(string path, vector<string> &files);
void writeEstJaccardCSV(const std::string &filename, const std::vector<std::vector<double>> &data);
std::string getDirectoryName(const std::string& path);