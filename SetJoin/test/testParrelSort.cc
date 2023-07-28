#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <execution>
#include <chrono> // include this header

int main()
{
    std::vector<int> vec(100000000);
    
    // fill vector with random numbers
    std::srand(unsigned(std::time(nullptr)));
    std::generate(vec.begin(), vec.end(), std::rand);
    
    // start timer
    auto start = std::chrono::high_resolution_clock::now();
    
    // sort vector
    // std::sort(std::execution::par_unseq, vec.begin(), vec.end());
    std::sort( vec.begin(), vec.end());
    // stop timer
    auto end = std::chrono::high_resolution_clock::now();

    // compute and print the elapsed time in milliseconds
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time elapsed: " << elapsed.count() << " ms\n";
    
    return 0;
}
