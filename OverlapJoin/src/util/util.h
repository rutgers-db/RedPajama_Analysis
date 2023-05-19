#include <bits/stdc++.h>
 #include <sys/sysinfo.h>
using namespace std;
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