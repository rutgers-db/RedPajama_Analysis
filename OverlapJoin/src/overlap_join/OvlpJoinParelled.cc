

#include "OvlpJoinParelled.h"
#include "../util/util.h"
#include "../util/io.h"
#include "omp.h"

// It is the id map of sorted bottom_k to original_bottomk
extern vector<int> idmap_bottomk;
int c;
int K;
vector<vector<unsigned short>> dataset;
vector<vector<combination>> combs;

class Comp_comb {
    int tid;

public:
    Comp_comb(int _tid) :
        tid(_tid) {
    }

    bool operator()(const int a, const int b) {
        combination &c1 = combs[tid][a];
        combination &c2 = combs[tid][b];
        for (int i = 0; i < c; i++) {
            if (dataset[c1.id][c1.curr[i]] > dataset[c2.id][c2.curr[i]])
                return false;
            else if (dataset[c1.id][c1.curr[i]] < dataset[c2.id][c2.curr[i]])
                return true;
        }
        return c1.id > c2.id;
    }
};

inline bool judgeMinHashJaccard(const int & pos_1, const int & pos_2)
{
    if(pos_1 - c + 1 + pos_2 - c + 1<= K-c){
        return true;
    }
    return false;
}

void OvlpJoinParelled::save_idmap(string _resPair_path){
    assert(idmap_records.size() == records.size() && idmap_records.size()== idmap_bottomk.size());

    //Combine the idmap_records and idmap_bottomk into one idmap
    vector<int> idmap(idmap_records.size());
    for(int i = 0;i<idmap.size();i++){
        idmap[i]=idmap_bottomk[idmap_records[i].first];
    }

    // Write the idmap
    const string idmap_file_path = _resPair_path + "idmap.bin";
    writeVec2Bin(idmap_file_path, idmap);
}

// Function to handle the small sets
void OvlpJoinParelled::small_case() {
    --c;

    // "Global" variables in this function
    timeval mid, mid1, end;
    vector<vector<pair<int, int>>> *res_lists;
    res_lists = new vector<vector<pair<int, int>>>[MAX_THREAD];
    auto lists_num = vector<unsigned long long>(MAX_THREAD, 0);
    ofstream *pairs_ofs;

    // Based on whether we need to write the disk or running on the memory all the time
    if(if_external_IO == true){
        pairs_ofs = new ofstream[MAX_THREAD];
        for(int i = 0 ;i<MAX_THREAD;i++){
            const string divided_pairs_path = resultPair_storePath + "/" + to_string(i)+".bin";
            pairs_ofs[i].open(divided_pairs_path.c_str(), ios::binary);
        }
    }
    
    // Loop over all elements in reverse order
    cout << "Now we are using multithread and the thread amount is " << omp_get_num_threads() << endl;
    
    gettimeofday(&mid, NULL);
#pragma omp parallel for num_threads(MAX_THREAD)
    for (auto idx = total_eles - 1; idx >= 0; idx--) {
        // cout<<"Current idx: "<<idx<<endl;
        if (ele_lists[idx].size() < 2)
            continue;
        vector<pair<int, int>> &vec = ele_lists[idx];
        // int size = distance(vec.begin(), lower_bound(vec.begin(), vec.end(), 0, comp_pair));
        if (vec.size() <= 1)
            continue;

        int thread_id = omp_get_thread_num();
        Comp_comb comp_comb = Comp_comb(thread_id);

        // initialize heap and combinations
        heap[thread_id].clear();
        combs[thread_id].clear();
        int heap_size = 0;
        for (auto i = 0 ; i < vec.size(); i++) {
            if ((int)(dataset[vec[i].first].size()) - 1 - vec[i].second < c)
                continue;
            heap[thread_id].push_back(heap_size++);
            combs[thread_id].push_back(combination(vec[i].first, vec[i].second));
        }

        if (heap_size < 2)
            continue;
        make_heap(heap[thread_id].begin(), heap[thread_id].end(), comp_comb);
        // cout << heap_size << endl;

        // pop heaps
        vector<pair<int,int>> inv_list;
        while (heap_size > 1) {
            inv_list.clear();
            do {
                // cout << heap_size << " " << heap_cost << endl;
                pop_heap(heap[thread_id].begin(), heap[thread_id].begin() + heap_size, comp_comb);
                --heap_size;
                inv_list.emplace_back(combs[thread_id][heap[thread_id][heap_size]].id,combs[thread_id][heap[thread_id][heap_size]].getlastcurr());
            } while (heap_size > 0 && is_equal(combs[thread_id][heap[thread_id][heap_size]], combs[thread_id][heap[thread_id].front()]));

            if (inv_list.size() > 1) {
                res_lists[thread_id].push_back(std::move(inv_list));
            }

            if (heap_size == 0)
                break;

            for (auto i = heap_size; i < heap[thread_id].size(); ++i) {
                combs[thread_id][heap[thread_id][i]].binary(combs[thread_id][heap[thread_id].front()]);
            }

            int comp_num = 0;
            for (auto i = heap_size; i < heap[thread_id].size(); ++i) {
                if (combs[thread_id][heap[thread_id][i]].completed)
                    ++comp_num;
                else if (comp_num > 0)
                    heap[thread_id][i - comp_num] = heap[thread_id][i];
            }

            for (auto i = heap_size; i < (int)heap[thread_id].size() - comp_num; i++) {
                push_heap(heap[thread_id].begin(), heap[thread_id].begin() + i + 1, comp_comb);
            }
            while (comp_num-- > 0)
                heap[thread_id].pop_back();
            heap_size = heap[thread_id].size();
        }
    
        if(if_external_IO == true && res_lists[thread_id].size() !=0){
            for(auto const & inv_l : res_lists[thread_id]){
                int size = inv_l.size();
                pairs_ofs[thread_id].write((char*)&size, sizeof(int));
                pairs_ofs[thread_id].write(reinterpret_cast<const char*>(inv_l.data()), inv_l.size() * sizeof(pair<int, int>));
            }
            
            // cout<< res_lists[thread_id].size()<<endl;
            lists_num[thread_id] += (unsigned long long)res_lists[thread_id].size();
            // clear this temporray res_list
            res_lists[thread_id].clear();
        }

        if(idx%MAX_THREAD == 0){
            printf("▉");
        }
    }

    gettimeofday(&mid1, NULL);

    if(if_external_IO == false){
        // Merge res_lists
        vector<vector<pair<int, int>>> merged_res_list;
        mergeArrays(res_lists, int(MAX_THREAD), merged_res_list);

        cout << "\nRes lists num: " << merged_res_list.size() << endl;

        vector<vector<int>> id_lists(n);
        for (auto i = 0; i < merged_res_list.size(); i++) {
            for (auto j = 0; j < merged_res_list[i].size(); j++)
                id_lists[merged_res_list[i][j].first].push_back(i);
        }

        vector<int> results(n, -1);
        for (auto i = n - 1; i >= 0; i--) {
            if (id_lists[i].empty())
                continue;
            for (auto j = 0; j < id_lists[i].size(); j++) {
                auto last_cur_1 = merged_res_list[id_lists[i][j]].back().second;
                merged_res_list[id_lists[i][j]].pop_back();
                for (auto k = 0; k < merged_res_list[id_lists[i][j]].size(); k++) {
                    if (results[merged_res_list[id_lists[i][j]][k].first] != i) {
                        // cout << idmap_records[i].first << " " << idmap_records[merged_res_list[id_lists[i][j]][k]].first << endl;

                        candidate_num++;
                        results[merged_res_list[id_lists[i][j]][k].first] = i;

                        auto last_cur_2 = merged_res_list[id_lists[i][j]][k].second;

                        if(judgeMinHashJaccard(last_cur_1, last_cur_2)){
                            int idd1 = idmap_records[i].first;
                            int idd2 = idmap_records[merged_res_list[id_lists[i][j]][k].first].first;
                            result_pairs.emplace_back(idd1, idd2);

                            ++result_num;
                        }
                    }
                }
            }
        }
    }
    else{
        cout << "\nRes lists num: " << accumulate(lists_num.begin(), lists_num.end(), 0LL) << endl;
        for(int i = 0 ;i<MAX_THREAD;i++){
            pairs_ofs[i].close();
        }
    }
    
    ++c;

    cout << "candidate number: "<< candidate_num<<endl;
    gettimeofday(&end, NULL);
    cout << " small p2 : " << mid1.tv_sec - mid.tv_sec + (mid1.tv_usec - mid.tv_usec) / 1e6 << endl;
    cout << " small p3 : " << end.tv_sec - mid1.tv_sec + (end.tv_usec - mid1.tv_usec) / 1e6 << endl;
}

void OvlpJoinParelled::overlapjoin(int overlap_threshold, int _k) {
    srand(time(NULL));

    timeval starting, ending, s1, t1, s2, t2;
    timeval time1, time2, time3, time4;

    gettimeofday(&starting, NULL);

    c = overlap_threshold;           // get threshold
    K = _k;                          // the k of bottom_k
    n = records.size();              // get number of records

    vector<pair<int, int>> eles;
    unordered_map<int, vector<int>> ele;

    for (int i = 0; i < records.size(); i++) {
        if (records[i].size() < c)
            continue;                               // remove records with size smaller than c
        for (int j = 0; j < records[i].size(); j++) // build inverted index
            ele[records[i][j]].push_back(i);
    }

    for (auto it = ele.begin(); it != ele.end(); it++)
        eles.push_back(make_pair(it->first, it->second.size())); // build element frequency table

    // get global order: frequency increasing order
    // sort the elements
    sort(eles.begin(), eles.end(), [](const pair<int, int> &p1, const pair<int, int> &p2) { return p1.second < p2.second; });
    
    // container initialize
    dataset.resize(n);

    // sort elements by its global order: frequence increasing order
    // remove widow word
    // encode elements in decreasing order
    // so the dataset is the same as the record, only the element is encoded to 0~ total_eles-1.
    // The encoding way is the less frequency of the element is, the large number it gets
    total_eles = eles.size();
    for (auto i = 0; i < int(eles.size()); ++i) {
        if (eles[i].second < 2)
            continue;
        for (auto j = ele[eles[i].first].begin(); j != ele[eles[i].first].end(); j++)
            dataset[*j].push_back(total_eles - i - 1);
    }

    gettimeofday(&time1, NULL);
    cout << "Initial Time: " << time1.tv_sec - starting.tv_sec + (time1.tv_usec - starting.tv_usec) / 1e6 << endl;

    // ****** cost model for prefix length selection ******
    // remove short records
    for (auto i = 0; i < n; i++)
        if (dataset[i].size() < c)
            dataset[i].clear();

    // create id mappings: from sorted to origin
    for (auto i = 0; i < n; i++)
        idmap_records.push_back(make_pair(i, dataset[i].size()));

    // sort records by length in decreasing order
    sort(idmap_records.begin(), idmap_records.end(), [](const pair<int, int> &a, const pair<int, int> &b) { return a.second > b.second; });
    // todo Writing idmap_records[idmap_records]
    sort(dataset.begin(), dataset.end(), [](const vector<unsigned short> &a, const vector<unsigned short> &b) { return a.size() > b.size(); });
    cout << " largest set: " << dataset.front().size() << " smallest set: " << dataset.back().size() << "It might be 0 cause some row in dataset, its length is smaller than c" << endl;

    if(if_external_IO == true){
        save_idmap(resultPair_storePath);
    }

    // build real inverted index
    ele_lists.resize(total_eles);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < dataset[i].size(); j++)
            ele_lists[dataset[i][j]].push_back(make_pair(i, j));

    gettimeofday(&time3, NULL);
    cout << "Transform Time: " << time3.tv_sec - time1.tv_sec + (time3.tv_usec - time1.tv_usec) / 1e6 << endl;

    // ****** cost model for boundary selection ******
    cout << " All are treated as small sets: " << n << endl;

    gettimeofday(&time4, NULL);
    // ****** conduct joining ******
    result_num = 0;

    gettimeofday(&s1, NULL);
    gettimeofday(&t1, NULL);

    gettimeofday(&s2, NULL);
    small_case();
    gettimeofday(&t2, NULL);

    gettimeofday(&ending, NULL);
    cout << "Join Time: " << ending.tv_sec - time4.tv_sec + (ending.tv_usec - time4.tv_usec) / 1e6 << endl;
    cout << "  small Time: " << t2.tv_sec - s2.tv_sec + (t2.tv_usec - s2.tv_usec) / 1e6 << endl;
    cout << "All Time: " << ending.tv_sec - starting.tv_sec + (ending.tv_usec - starting.tv_usec) / 1e6 << endl;
    cout << "Result Num: " << result_num << endl;
}


bool OvlpJoinParelled::is_equal(const combination &c1, const combination &c2) {
    for (int i = 0; i < c; i++) {
        if (dataset[c1.id][c1.curr[i]] != dataset[c2.id][c2.curr[i]])
            return false;
    }
    return true;
}

bool comp_int(const int a, const int b) {
    return a > b;
}