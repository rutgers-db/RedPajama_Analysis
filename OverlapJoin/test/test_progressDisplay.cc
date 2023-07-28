#include <fstream>
#include <vector>
#include <utility>
#include <cassert>

void writeAllAtOnce(const std::string& filename, const std::vector<std::pair<int, int>>& data) {
    std::ofstream out(filename, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(std::pair<int, int>));
}

void writeIndividually(const std::string& filename, const std::vector<std::pair<int, int>>& data) {
    std::ofstream out(filename, std::ios::binary);
    for (const auto& pair : data) {
		out.write(reinterpret_cast<const char*>(&pair.first), sizeof(int));
        out.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
    }
}

bool filesAreEqual(const std::string& filename1, const std::string& filename2) {
    std::ifstream file1(filename1, std::ios::binary);
    std::ifstream file2(filename2, std::ios::binary);

    std::string content1((std::istreambuf_iterator<char>(file1)), std::istreambuf_iterator<char>());
    std::string content2((std::istreambuf_iterator<char>(file2)), std::istreambuf_iterator<char>());

    return content1 == content2;
}

int main() {
    std::vector<std::pair<int, int>> data = {{1, 2}, {3, 4}, {5, 6}};

    std::string filename1 = "file1.bin";
    std::string filename2 = "file2.bin";

    writeAllAtOnce(filename1, data);
    writeIndividually(filename2, data);

    assert(filesAreEqual(filename1, filename2));

    return 0;
}
