#ifndef _RANGELOADER_H_
#define _RANGELOADER_H_

#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class RangeLoader {
public:
    unsigned int docs_amount = 0;
    int range_num = 0;                 // Number of ranges (or groups) loaded so far
    unsigned int cur_gp_size;
    unsigned int range_low;
    unsigned int range_high;
    double load_cost = 0;

    string sortedsets_path;            // Path to the binary file
    bool if_eof = false;               // Flag indicating if the end of the file is reached
    unsigned int nxt_size;             // Size of the next document vector to be loaded
    unsigned int gp_max_len;           // Maximum length of vectors in the current group
    double alpha;                      // Multiplier to calculate `gp_max_len` from the size of the first vector of the group
    ifstream ifs;                      // File input stream for reading the binary file

    RangeLoader(string _path, double _alpha) : sortedsets_path(_path), alpha(_alpha), ifs(_path, ios::in | ios::binary) {
        if (!ifs) {
            // If the file cannot be opened or does not exist, print an error message and set the end-of-file flag
            printf("%s does not exist\n", sortedsets_path.c_str());
            if_eof = true;
            return;
        }
        printf("Start loading the %s group by group\n", sortedsets_path.c_str());
        // Load the size of the first vector
        ifs.read((char *)&nxt_size, sizeof(unsigned int));
        range_low = nxt_size;
        gp_max_len = static_cast<unsigned int>(nxt_size * alpha);
    }

    void showCurrentRangeInfo(){
        printf("Now No.%d range has been loaded,it has %u documents, from length %u to %u\n",
            range_num, cur_gp_size, range_low, range_high);
    }
    // Load the next range group of documents into docs
    bool loadNextRange(vector<vector<unsigned int>> &docs) {
        if (if_eof)
            return false;

        auto load_st = LogTime();

        range_low = nxt_size;
        while (!ifs.eof()) {
            // Load next document vector
            vector<unsigned int> tmp_vec(nxt_size);
            ifs.read((char *)&tmp_vec[0], sizeof(unsigned int) * nxt_size);
            docs.emplace_back(move(tmp_vec));

            // Load the size of the next vector
            if (!ifs.read((char *)&nxt_size, sizeof(unsigned int)))
                break;

            // If the size of the new vector is greater than gp_max_len, it's the start of a new group
            if (nxt_size > gp_max_len) {
                gp_max_len = static_cast<unsigned int>(nxt_size * alpha);
                break;
            }
            range_high = nxt_size;
        }

        cur_gp_size = docs.size();
        docs_amount += cur_gp_size;
        range_num++;

        if (ifs.eof()) {
            if_eof = true;
            printf("Reach End of File and the total range group is: %d \n", range_num);
        }

        load_cost += RepTime(load_st);
        return true;
    }
};

#endif
