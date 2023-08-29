#include <bits/stdc++.h>
#include "./src/IO.hpp"
#include "./src/util.hpp"
using namespace std;

int main(){
    int band = 6;
    for (int i = 1; i < argc; i++) {
        const string arg = argv[i];
        if (arg == "-band") {
            band = atoi(argv[i + 1]);
        }
    }

    auto timer_st = LogTime();
    cout << "Starting deduplicate each file" << endl;
    // read and deduplicate each file
#pragma omp parallel for
    for(int i = 0 ; i<band;i++){
        cout<<"Doing "<<i<<endl;
        string file_name ="./tmp/"+ to_string(i)+".bin";
        vector<pair<unsigned int, unsigned int>> pairs;
        readPairsFromFile(file_name, pairs);
        deduplicate_vec(pairs);
        file_name ="./tmp/sorted_"+ to_string(i)+".bin";
        writeSimilarPair(file_name,pairs);
    }   
    printf("At last the total time cost is: %f\n", RepTime(timer_st));
}