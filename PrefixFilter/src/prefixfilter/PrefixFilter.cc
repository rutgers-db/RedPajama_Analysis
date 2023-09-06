#include "PrefixFilter.h"
#include "../util/util.h"

// Function to check if two sets overlap based on a certain condition
bool PrefixFilter::overlap(unsigned int x, unsigned int y) {
    return bottomKJaccard(dataset[x], dataset[y], det);
}

unordered_map<unsigned int, vector<unsigned int>> inv_lists;
vector<vector<unsigned int>> ele_invlist_pos;
void PrefixFilter::group(double thres) {
    det = thres;
    auto timer_st = LogTime();
    ele_invlist_pos.resize(dataset.size());
    for(unsigned int rid = 0; rid< dataset.size(); rid ++){
        auto& bottomk = dataset[rid];

        unsigned int prefix_size = floor(det * bottomk.size() ) + 1;

        ele_invlist_pos[rid].resize(prefix_size);
        for(unsigned int i = 0; i < prefix_size; i++){
            auto & ele = bottomk[i];
            inv_lists[ele].emplace_back(rid);
            ele_invlist_pos[rid][i] = inv_lists[ele].size() - 1;
        }
    }
    printf("Grouping Costs: %f\n", RepTime(timer_st));
}

bool **quickRef2D; // 2D quick reference array

void PrefixFilter::find_simp(){
    auto timer_st = LogTime();

    // the length bound of compared bottomk's original length
    double coe = (1 - det) / (1 + det);

    // Initialize quick reference arrays
    quickRef2D = new bool*[MAXTHREADNUM];
    for(unsigned int i = 0; i < MAXTHREADNUM; ++i){
        quickRef2D[i] = new bool[dataset.size()];
        memset(quickRef2D[i], 0, sizeof(bool) * dataset.size());
    }

#pragma omp parallel for
    for(unsigned int rid = 0; rid< dataset.size(); rid ++){
        auto tid = omp_get_thread_num();
        auto quickRef = quickRef2D[tid]; 
        auto prefix_size =  ele_invlist_pos[rid].size();
        unsigned int len_limit = floor(coe* ori_lens[rid] - 0.0001);

        vector<unsigned int> candidates;

        for(unsigned int i = 0; i<prefix_size; i++){
            auto pos = ele_invlist_pos[rid][i];
            auto ele = dataset[rid][i];
            for(unsigned int j = 0; j<pos; j++){
                auto & bk_id = inv_lists[ele][j];

                // skip too short document compared with current document
                if(ori_lens[bk_id] <= len_limit)
                    continue;
                
                if(quickRef[bk_id] == true)
                    continue;
                
                quickRef[bk_id] = true;
                candidates.push_back(bk_id);
            }
        }

        // verify the candidates
        for(auto & cand_id: candidates){
            quickRef[cand_id] = false;
            if(overlap(cand_id, rid)){
                result_pairs[tid].emplace_back(cand_id, rid);
            }
        }
    }

    printf("Finding Simpairs Costs: %f\n", RepTime(timer_st));
}
