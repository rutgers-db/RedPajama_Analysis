#include <bits/stdc++.h>
#include "./src/IO.hpp"
#include "./src/util.hpp"
using namespace std;

int main(){
    
    auto timer_st = LogTime();
    cout << "Starting deduplicate each file" << endl;
    // read and deduplicate each file
    for(int i = 0 ; i<16;i++){
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