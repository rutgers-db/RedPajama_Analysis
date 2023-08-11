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
std::vector<int> getUniqueInts(const std::vector<std::pair<int, int>>& pairs) {
    std::set<int> uniqueInts;
    for (const auto& p : pairs) {
        uniqueInts.insert(p.first);
        uniqueInts.insert(p.second);
    }

    // Convert set to vector
    std::vector<int> result(uniqueInts.begin(), uniqueInts.end());

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

// Function to find the k smallest unique numbers from the given vector of numbers.
// The function returns a vector of the k smallest unique numbers.
// Parameters:
//   - nums: Input vector of numbers.
//   - k: The number of smallest unique numbers to return.
std::vector<unsigned short> kMostMinUnique(const std::vector<unsigned short>& nums, int k) {
    
    // counts: An unordered_map that keeps track of the frequency of each number in the input vector.
    std::unordered_map<unsigned short, int> counts;
    
    // Populate the frequency map.
    for (unsigned short num : nums) {
        counts[num]++;
    }

    // minHeap: A priority_queue used as a min heap to keep track of the k smallest unique numbers.
    std::priority_queue<unsigned short> minHeap;

    // Iterate through the frequency map.
    for (auto& pair : counts) {
        // If the heap has less than k elements, push the current number into the heap.
        if (minHeap.size() < k) {
            minHeap.push(pair.first);
        } 
        // If the heap has k elements and the current number is less than the largest number in the heap,
        // remove the largest number and insert the current number.
        else if (pair.first < minHeap.top()) {
            minHeap.pop();
            minHeap.push(pair.first);
        }
    }

    // result: A vector to store the k smallest unique numbers.
    std::vector<unsigned short> result;
    
    // Populate the result vector with the numbers from the heap.
    while (!minHeap.empty()) {
        result.push_back(minHeap.top());
        minHeap.pop();
    }

    // The result vector is reversed as the numbers were retrieved from a min heap in descending order.
    std::reverse(result.begin(), result.end());

    return result;
}