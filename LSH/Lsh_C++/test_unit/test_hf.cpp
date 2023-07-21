#include <vector>
#include <utility>
#include <unordered_set>
#include "../src/util.hpp"

using namespace std;

bool checkUnique(const std::vector<std::pair<unsigned short, unsigned short>>& vec) {
    set<std::pair<unsigned short, unsigned short>> pairs_set(vec.begin(), vec.end());
    return pairs_set.size() == vec.size();
}

int main() {
    std::vector<std::pair<unsigned short, unsigned short>> testVec;
    for(int i = 1 ;i<=128; i++){
        generateHashFunc(i, testVec);
    }
    if (checkUnique(testVec)) {
        std::cout << "All pairs are unique.\n";
    } else {
        std::cout << "There are duplicate pairs.\n";
    }
    return 0;
}
