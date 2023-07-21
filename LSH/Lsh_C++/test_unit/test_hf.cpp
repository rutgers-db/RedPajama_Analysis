// #include <vector>
// #include <utility>
// #include <unordered_set>
// #include "../src/util.hpp"

// using namespace std;

// bool checkUnique(const std::vector<std::pair<unsigned short, unsigned short>>& vec) {
//     set<std::pair<unsigned short, unsigned short>> pairs_set(vec.begin(), vec.end());
//     return pairs_set.size() == vec.size();
// }

// int main() {
//     std::vector<std::pair<unsigned short, unsigned short>> testVec;
//     for(int i = 1 ;i<=128; i++){
//         generateHashFunc(i, testVec);
//     }
//     if (checkUnique(testVec)) {
//         std::cout << "All pairs are unique.\n";
//     } else {
//         std::cout << "There are duplicate pairs.\n";
//     }

    
//     return 0;
// }

// #include <cstdlib> // For rand(), RAND_MAX
// #include <iostream> // For std::cout

// int main() {
//     std::cout << "RAND_MAX is " << RAND_MAX << std::endl;
//     return 0;
// }

#include <iostream>

int main() {
    std::cout << "Size of size_t: " << sizeof(size_t) << " bytes\n";
    std::cout << "Size of unsigned int: " << sizeof(unsigned int) << " bytes\n";
    std::cout << "Size of unsigned long long: " << sizeof(unsigned long long) << " bytes\n";

    if (sizeof(size_t) == sizeof(unsigned int)) {
        std::cout << "size_t is the same size as unsigned int\n";
    } else if (sizeof(size_t) == sizeof(unsigned long long)) {
        std::cout << "size_t is the same size as unsigned long long\n";
    } else {
        std::cout << "size_t is neither the same size as unsigned int nor unsigned long long\n";
    }

    return 0;
}


