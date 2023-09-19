#include "util.h"

void print_memory() {
#ifdef __linux__
  struct sysinfo memInfo;

  sysinfo(&memInfo);
  // long long totalVirtualMem = memInfo.totalram;
  // // Add other values in next statement to avoid int overflow on right hand
  // // side...
  // totalVirtualMem += memInfo.totalswap;
  // totalVirtualMem *= memInfo.mem_unit;

  // long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
  // // Add other values in next statement to avoid int overflow on right hand
  // // side...
  // virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
  // virtualMemUsed *= memInfo.mem_unit;
  // cout << "Total Virtual Memory: " << totalVirtualMem << endl;
  // cout << "Used Virtual Memory: " << virtualMemUsed << endl;

  long long totalPhysMem = memInfo.totalram;
  // Multiply in next statement to avoid int overflow on right hand side...
  totalPhysMem *= memInfo.mem_unit;

  long long physMemUsed = memInfo.totalram - memInfo.freeram;
  // Multiply in next statement to avoid int overflow on right hand side...
  physMemUsed *= memInfo.mem_unit;

  // cout << "Total Physical Memory: " << totalPhysMem << endl;
  cout << "Used Physical Memory: " << physMemUsed << endl;
#elif __APPLE__
  vm_size_t page_size;
  mach_port_t mach_port;
  mach_msg_type_number_t count;
  vm_statistics64_data_t vm_stats;

  mach_port = mach_host_self();
  count = sizeof(vm_stats) / sizeof(natural_t);
  if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
      KERN_SUCCESS == host_statistics64(mach_port, HOST_VM_INFO,
                                        (host_info64_t)&vm_stats, &count)) {
    long long free_memory = (int64_t)vm_stats.free_count * (int64_t)page_size;

    long long used_memory =
        ((int64_t)vm_stats.active_count + (int64_t)vm_stats.inactive_count +
         (int64_t)vm_stats.wire_count) *
        (int64_t)page_size;
    printf("free memory: %lld\nused memory: %lld\n", free_memory, used_memory);
  }
#endif
}

//retrieves all unique integers from a vector of pairs and sorts them in ascending order
std::vector<unsigned int> getUniqueInts(const std::vector<std::pair<unsigned int,unsigned int>>& pairs) {
    std::set<unsigned int> uniqueInts;
    for (const auto& p : pairs) {
        uniqueInts.insert(p.first);
        uniqueInts.insert(p.second);
    }

    // Convert set to vector
    std::vector<unsigned int> result(uniqueInts.begin(), uniqueInts.end());

    return result;
}


// takes as input a vector<vector<vector<int>>>. 
// It iterates over all the outer vector<vector<int>> elements in the array, and then for each of these, 
// it iterates over the vector<int> elements, adding each to the result vector<vector<int>>

void mergeArrays(std::vector<std::vector<pair<int,int>>>* input, int arr_len, std::vector<std::vector<pair<int,int>>> & result)
{
    for(int i = 0; i < arr_len; i++){
        result.insert(result.end(), input[i].begin(), input[i].end());
    }
}

// Turn on timer
std::chrono::_V2::system_clock::time_point LogTime() {
    return chrono::high_resolution_clock::now();
}

// Turn off timer
double RepTime(const std::chrono::_V2::system_clock::time_point &start) {
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    return duration.count() / 1000000.0;
}

void printHowManyThreads(){
    int thread_num = 0;

    #pragma omp parallel for
    for(int i = 0; i<200;i++){
        #pragma omp critical
            thread_num = max(omp_get_thread_num(),thread_num);
    }

    cout<<"max thread amount: "<<thread_num<<endl;
}

double shrinkBottomk(vector<vector<unsigned short>>&  bottom_ks, double ratio){
    double average_size = 0;
    for(auto & vec:bottom_ks){
        unsigned int size = ceil(vec.size()*ratio);
        average_size += size;
        vec.resize(size);
    }
    average_size/= bottom_ks.size();
    return average_size;
}

double jaccard_similarity(const std::vector<unsigned int>& text1, const std::vector<unsigned int>& text2) {
    if(text1.size() == 0 || text2.size() == 0)
        return 0;
    std::unordered_set<unsigned int> set1(text1.begin(), text1.end());
    std::unordered_set<unsigned int> set2(text2.begin(), text2.end());

    std::vector<unsigned int> intersection;
    std::vector<unsigned int> union_set;

    // calculate intersection
    for (auto& element : set1) {
        if (set2.count(element)) {
            intersection.push_back(element);
        }
    }

    // calculate union
    union_set = text1;
    union_set.insert(union_set.end(), text2.begin(), text2.end());

    std::sort(union_set.begin(), union_set.end());
    union_set.erase(std::unique(union_set.begin(), union_set.end()), union_set.end());

    return static_cast<double>(intersection.size()) / union_set.size();
}

double jaccard_similarity(const std::vector<unsigned short>& text1, const std::vector<unsigned short>& text2) {
    if(text1.size() == 0 || text2.size() == 0)
        return 0;
        
    std::unordered_set<unsigned short> set1(text1.begin(), text1.end());
    std::unordered_set<unsigned short> set2(text2.begin(), text2.end());

    std::vector<unsigned short> intersection;
    std::vector<unsigned short> union_set;

    // calculate intersection
    for (auto& element : set1) {
        if (set2.count(element)) {
            intersection.push_back(element);
        }
    }

    // calculate union
    union_set = text1;
    union_set.insert(union_set.end(), text2.begin(), text2.end());

    std::sort(union_set.begin(), union_set.end());
    union_set.erase(std::unique(union_set.begin(), union_set.end()), union_set.end());

    return static_cast<double>(intersection.size()) / union_set.size();
}

double bottomKJaccard(const std::vector<unsigned short>& A, 
                      const std::vector<unsigned short>& B) {
    
    int k = min(A.size(),B.size());

    // Create a union set of A and B
    std::unordered_set<unsigned short> unionSet(A.begin(), A.end());
    unionSet.insert(B.begin(), B.end());

    // If the union is smaller than k, then there are duplicates.
    // Therefore, extract only the smallest k values.
    if(unionSet.size() > k) {
        std::vector<unsigned short> unionVec(unionSet.begin(), unionSet.end());
        std::sort(unionVec.begin(), unionVec.end());
        unionVec.resize(k);
        unionSet = std::unordered_set<unsigned short>(unionVec.begin(), unionVec.end());
    }

    // Iterate the element in A and check how many of them in B and also in k mins (A union B)
    auto kmost_B = unordered_set<unsigned short>(B.begin(), B.end());
    int overlap = 0;
    for(auto & ele: A){
        if(kmost_B.find(ele) != kmost_B.end() && unionSet.find(ele) != unionSet.end()){
            overlap++;
        }
    }

    return static_cast<double>(overlap) / k;
}

double bottomKJaccard(const std::vector<unsigned int>& A, const std::vector<unsigned int>& B){
    int k = min(A.size(),B.size());

    // Create a union set of A and B
    std::unordered_set<unsigned int> unionSet(A.begin(), A.end());
    unionSet.insert(B.begin(), B.end());

    // If the union is smaller than k, then there are duplicates.
    // Therefore, extract only the smallest k values.
    if(unionSet.size() > k) {
        std::vector<unsigned int> unionVec(unionSet.begin(), unionSet.end());
        std::sort(unionVec.begin(), unionVec.end());
        unionVec.resize(k);
        unionSet = std::unordered_set<unsigned int>(unionVec.begin(), unionVec.end());
    }

    // Iterate the element in A and check how many of them in B and also in k mins (A union B)
    auto kmost_B = unordered_set<unsigned int>(B.begin(), B.end());
    int overlap = 0;
    for(auto & ele: A){
        if(kmost_B.find(ele) != kmost_B.end() && unionSet.find(ele) != unionSet.end()){
            overlap++;
        }
    }

    return static_cast<double>(overlap) / k;
}

bool bottomKJaccard_2(const std::vector<unsigned int>& A, const std::vector<unsigned int>& B, double thres){
    // Assuming A and B have the same size k
    int k = min(A.size(),B.size());

    int posx =0;
    int posy = 0;
    
    int current_overlap = 0;
    int required_overlap = int(ceil(thres * k));
    int missing_limit = k - required_overlap;
    int missing_ele = 0;
    while (posx < k && posy < k) {

        // Check if the missing elements is more than the limit
        // Check if remaining elements are sufficient for required overlap
        if (missing_ele > missing_limit) return false;
        if (current_overlap >= required_overlap) return true;

        if (A[posx] == B[posy]) { 
            current_overlap++;
            posx++;
            posy++;
        } else if (A[posx] < B[posy]) {
            posx++;
            missing_ele++;
        } else {
            posy++;
            missing_ele++;
        }
    }
    return current_overlap >= required_overlap;
}

// have two sorted vectors, A and B, and you want to find A - B (all elements in A that aren't in B)
// Output the amount of the difference
size_t difference(const std::vector<unsigned int>& A, const std::vector<unsigned int>& B) {
    size_t res = 0;
    
    auto itA = A.begin();
    auto itB = B.begin();
    
    while (itA != A.end() && itB != B.end()) {
        if (*itA < *itB) {
            res++;
            ++itA;
        } else if (*itA > *itB) {
            ++itB;
        } else { // they're equal
            ++itA;
            ++itB;
        }
    }
    
    // If there are more elements in A
    while (itA != A.end()) {
        res++;
        ++itA;
    }
    
    return res;
}

std::vector<unsigned int> randomPermutation(std::vector<unsigned int>& vec) {
    // Obtain a time-based seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // Shuffle elements randomly
    std::shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));

    return vec;
}
