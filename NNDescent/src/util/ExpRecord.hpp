#ifndef EXPRECORD
#define EXPRECORD

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <time.h>
#include <numeric>
#include <cstring>

#include "Util.hpp"
using namespace std;

// this class is used for recording information of experiments

class ExpRecord{
private:

    // -1 means this indicator hasn't been used
    vector<int> updates_num;
    vector<unsigned long long> cmps_num;
    vector<double> recall;
    vector<double> split_cost;
    vector<double> join_cost;
    vector<vector<int>> map_info;
    vector<int> nodes_cmp;
public:
    void record_updates(int _updates){
        updates_num.push_back(_updates);
    }

    void record_cmps(unsigned long long _cmp){
        cmps_num.push_back(_cmp);
    }

    void record_recall(double _recall){
        recall.push_back(_recall);
    }

    void record_split_cost(double _split_cost){
        split_cost.push_back(_split_cost);
    }

    void record_join_cost(double _join_cost){
        join_cost.push_back(_join_cost);
    }

    void record_nodes_cmp(vector<int> &_nodes_cmp){
        nodes_cmp = vector<int>(_nodes_cmp);
    }

    // record the the size of partitions
    void record_map_info(const vector<NNHeap> &neighborLists){
        vector<int> partitions_size(neighborLists.size(),0);
        for (auto &nnHeap : neighborLists) {
            partitions_size[nnHeap.partition.size()] += 1;
        }

        int size=partitions_size.size();
        for(int i=size-1;i--;i>=0){
            if(partitions_size[i]!=0){
                partitions_size.resize(i+1);
                break;
            }
        }

        map_info.push_back(partitions_size);
    }

    void print_record(){
        if(updates_num.size()!=0){
            cout<<"updates: "<<updates_num.back()<<endl;
        }
        if(cmps_num.size()!=0){
            cout<<"cmps_num: "<<cmps_num.back()<<endl;
        }
        if(recall.size()!=0){
            cout<<"recall: "<<recall.back()<<endl;
        }
        if(split_cost.size()!=0){
            cout<<"split_cost: "<<split_cost.back()<<endl;
        }
    }

    void print_total_record(){
        if(join_cost.size()!=0){
            double total_join_cost=0;
            for(auto tmp:join_cost)
                total_join_cost+=tmp;
            cout<<"total_join_cost: "<<total_join_cost<<endl;
        }
    }

    void save_ExpRecord(string root_path){
        time_t tt = time(NULL);//这句返回的只是一个timestamp
        tm* t= localtime(&tt);

        // input current time to a string and make a dir
        char dir_name[100];
        sprintf(dir_name, "%d_%02d_%02d_%02d_%02d", 
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min);
        
        char order[100]={"mkdir expRecord/"};
        strcat(order,dir_name);
        system(order);

        // those vectors'size is more than 0 can be stored
        saveCsv_vector(root_path+"/"+dir_name+"/Updates.csv",updates_num);
        saveCsv_vector(root_path+"/"+dir_name+"/Recall.csv",recall);
        saveCsv_vector(root_path + "/" + dir_name + "/nodes_cmp.csv", nodes_cmp);
        saveCsv_vector(root_path+"/"+dir_name+"/Cmps.csv",cmps_num);
        saveCsv_vector(root_path+"/"+dir_name+"/Split_cost.csv",split_cost);
        saveCsv_vecs(root_path+"/"+dir_name+"/Partition_info.csv",map_info);
    }
};

#endif