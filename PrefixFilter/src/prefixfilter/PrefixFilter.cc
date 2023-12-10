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
    for (unsigned int rid = 0; rid < dataset.size(); rid++) {
        auto &bottomk = dataset[rid];

        unsigned int prefix_size = floor((1-det) * bottomk.size()) + 1;

        ele_invlist_pos[rid].resize(prefix_size);
        for (unsigned int i = 0; i < prefix_size; i++) {
            auto &ele = bottomk[i];
            inv_lists[ele].emplace_back(rid);
            ele_invlist_pos[rid][i] = inv_lists[ele].size() - 1;
        }
    }
    printf("Grouping Costs: %f\n", RepTime(timer_st));
}

void PrefixFilter::init() {
    // calculate tokenFrequency
    vector<pair<unsigned int, unsigned int>> eles;
    unordered_map<unsigned int, vector<unsigned int>> ele;

    for (unsigned int i = 0; i < dataset.size(); i++) {
        for (unsigned int j = 0; j < dataset[i].size(); j++) // build inverted index
            ele[dataset[i][j]].push_back(i);
        dataset[i].clear();
    }

    eles.reserve(ele.size());
    for (auto it = ele.begin(); it != ele.end(); it++)
        eles.push_back(make_pair(it->first, it->second.size())); // build element frequency table

    // get global order: frequency increasing order
    // sort the elements
    cout<<"Sorting"<<endl;
    sort(eles.begin(), eles.end(), [](const pair<unsigned int, unsigned int> &p1, const pair<unsigned int, unsigned int> &p2) { return p1.second < p2.second; });

    // sort elements by its global order: frequence increasing order
    // remove widow word
    // encode elements in decreasing order
    // so the dataset is the same as the record, only the element is encoded to 0~ total_eles-1.
    // The encoding way is the less frequency of the element is, the large number it gets
    cout<<"Sorted"<<endl;
    auto total_eles = eles.size();
    for (unsigned int i = 0; i < eles.size(); ++i) {
        // if (eles[i].second < 2)
        //     continue;
        for (auto j = ele[eles[i].first].begin(); j != ele[eles[i].first].end(); j++)
            dataset[*j].push_back(total_eles - i - 1);
    }

    printf("Rearranging Elements Done\n");
}

bool **quickRef2D; // 2D quick reference array

void PrefixFilter::find_simp() {
    auto timer_st = LogTime();

    // the length bound of compared bottomk's original length
    double coe = (1 - det) / (1 + det);

    // Initialize quick reference arrays
    quickRef2D = new bool *[MAXTHREADNUM];
    for (unsigned int i = 0; i < MAXTHREADNUM; ++i) {
        quickRef2D[i] = new bool[dataset.size()];
        memset(quickRef2D[i], 0, sizeof(bool ) * dataset.size());
    }
#pragma omp parallel for
    for (unsigned int rid = 0; rid < dataset.size(); rid++) {
        auto tid = omp_get_thread_num();
        auto quickRef = quickRef2D[tid];
        auto prefix_size = ele_invlist_pos[rid].size();
        unsigned int len_limit = floor(coe * ori_lens[rid] - 0.0001);
        std::vector<unsigned int> candidates;

        for (unsigned int i = 0; i < prefix_size; i++) {
            auto pos = ele_invlist_pos[rid][i];
            auto ele = dataset[rid][i];

                for (unsigned int j = 0; j < pos; j++) {
                    auto bk_id = inv_lists[ele][j];

                    if (ori_lens[bk_id] <= len_limit)
                        continue;

                    if (quickRef[bk_id])
                        continue;

                    candidates.push_back(bk_id); // Add the candidate to the vector
                    quickRef[bk_id] = true; // Mark the candidate as processed
                }
            }

        // Verify the candidates
        for (const auto &cand_id: candidates) {
            if (overlap(cand_id, rid)) {
                {
                    result_pairs[tid].emplace_back(cand_id, rid);
                }
            }
        }
    }
    std::cout << result_pairs->size() << std::endl;
    printf("Finding Simpairs Costs: %f\n", RepTime(timer_st));
}
