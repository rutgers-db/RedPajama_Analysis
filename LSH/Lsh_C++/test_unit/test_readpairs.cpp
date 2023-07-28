#include <fstream>
#include <iostream>
#include <vector>
#include <utility>

std::vector<std::pair<unsigned int, unsigned int>> readPairsFromFile(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    if (!ifs) {
        throw std::runtime_error("Could not open file " + filename);
    }

    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<std::pair<unsigned int, unsigned int>> vec(size / sizeof(std::pair<unsigned int, unsigned int>));
    ifs.read(reinterpret_cast<char*>(vec.data()), size);

    return vec;
}

int main() {
    // Write a binary file filled with pairs of unsigned integers
    // std::ofstream ofs("test.bin", std::ios::binary);
    // std::vector<std::pair<unsigned int, unsigned int>> vecOut{{1, 2}, {3, 4}, {5, 6}};
    // ofs.write(reinterpret_cast<const char*>(vecOut.data()), vecOut.size() * sizeof(std::pair<unsigned int, unsigned int>));
    // ofs.close();
    // Read the binary file
    std::vector<std::pair<unsigned int, unsigned int>> vecIn = readPairsFromFile("test.bin");

    // Print the pairs to confirm they match
    for (const auto& pair : vecIn) {
        std::cout << "{" << pair.first << ", " << pair.second << "}\n";
    }

    return 0;
}
