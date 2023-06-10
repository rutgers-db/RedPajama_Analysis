#include<bits/stdc++.h>
#include "../src/util/util.h"
using namespace std;

int main() {
    vector<int64_t> merged_vector = {5, 2, 3, 1, 4,5, 2, 3, 1, 4,5, 2, 3, 1, 4,5, 2, 3, 1, 4,5, 2, 3, 1, 4,5, 2, 3, 1, 4};

    // Parallel sort using OpenMP
    std::vector<int64_t> sorted_vector(merged_vector.size());

    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            int threads = omp_get_num_threads();
            int size = merged_vector.size();
            int chunk_size = (size + threads - 1) / threads;
            std::vector<int64_t>::iterator chunk_start = merged_vector.begin();

            std::vector<std::vector<int64_t>::iterator> ends;
            ends.reserve(threads);

            for (int i = 0; i < threads && chunk_start != merged_vector.end(); ++i) {
                std::vector<int64_t>::iterator chunk_end = chunk_start;
                std::advance(chunk_end, std::min(chunk_size, (int)std::distance(chunk_start, merged_vector.end())));

                #pragma omp task firstprivate(chunk_start, chunk_end)
                {
                    std::sort(chunk_start, chunk_end);
                }

                ends.push_back(chunk_end);
                chunk_start = chunk_end;
            }

            #pragma omp taskwait

            std::vector<int64_t>::iterator start = merged_vector.begin();
            std::vector<int64_t>::iterator out_it = sorted_vector.begin();
            for (auto end : ends) {
                auto next = end == merged_vector.end() ? end : std::upper_bound(end, merged_vector.end(), *(end - 1));
                std::merge(start, end, end, next, out_it);
                out_it += std::distance(start, next);
                start = next;
            }
        }
    }
    for(auto const x: sorted_vector){
        cout<< x <<" ";
    }

    return 0;
}
