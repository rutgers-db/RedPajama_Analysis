// Reference Command:
// ./bin/merge_simp stackexchange ./ngram_simps -K 9000 -band 20 -range 450

#include <bits/stdc++.h>

typedef std::pair<unsigned int, unsigned int> Pair;
const unsigned long long BUFFER_SIZE = 10000;

using namespace std;

struct PairWithIndex {
    Pair pair;
    int fileIndex;

    PairWithIndex(const Pair& p, int index) : pair(p), fileIndex(index) {}
};

struct Compare {
    bool operator()(const PairWithIndex& a, const PairWithIndex& b) {
        return a.pair > b.pair;
    }
};

void mergeFiles(const std::vector<std::string>& filenames, const std::string& outputFilename) {
    std::priority_queue<PairWithIndex, std::vector<PairWithIndex>, Compare> pq;
    vector<ifstream> streams(filenames.size());
    vector<vector<pair<unsigned int, unsigned int>>> buffers(filenames.size(), vector<pair<unsigned int, unsigned int>>(BUFFER_SIZE));
    vector<unsigned long long> bufferIndices(filenames.size(), 0);
    vector<unsigned long long> sizes(filenames.size());
    
    // Open all files and load the first BUFFER_SIZE pairs of each file into buffers
    for (int i = 0; i < filenames.size(); ++i) {
        streams[i].open(filenames[i], ios::binary);
        // First, read the size of pairs in this file
        streams[i].read((char *)&sizes[i], sizeof(unsigned long long));
        
        // Read pairs into buffer
        auto readSize = min(sizes[i], BUFFER_SIZE);
        streams[i].read((char *)&buffers[i][0], readSize * sizeof(pair<unsigned int, unsigned int>));
        
        pq.push(PairWithIndex(buffers[i][0], i));
        bufferIndices[i]++;
        sizes[i] -= readSize;

    }

    Pair lastPair = {0, 0};
    bool isFirstPair = true;

    ofstream ofs(outputFilename, ios::binary);
    vector<pair<unsigned int, unsigned int>> writing_buffer;
    pair<unsigned int, unsigned int> lastWritten = make_pair(0, 0);

    while (!pq.empty()) {
        PairWithIndex node = pq.top();
        pq.pop();

        // Skip duplicates
        if (isFirstPair || node.pair != lastPair) {
            isFirstPair = false;
            lastPair = node.pair;
            writing_buffer.emplace_back(node.pair);
            if(writing_buffer.size()>= BUFFER_SIZE){
                ofs.write((char *)&writing_buffer[0], writing_buffer.size() * sizeof(Pair));
                writing_buffer.clear();
            }
        }

        // If there are remaining pairs in the buffer of this node, add it to the pq
        if (bufferIndices[node.fileIndex] < buffers[node.fileIndex].size()) {
            pq.push(PairWithIndex(buffers[node.fileIndex][bufferIndices[node.fileIndex]], node.fileIndex));
            bufferIndices[node.fileIndex]++;
        } else if (sizes[node.fileIndex] > 0) { // If buffer is empty, load next pairs from file
            auto readSize = min(sizes[node.fileIndex], BUFFER_SIZE);
            streams[node.fileIndex].read((char *)&buffers[node.fileIndex][0], readSize * sizeof(pair<unsigned int, unsigned int>));
            buffers[node.fileIndex].resize(readSize);

            pq.push(PairWithIndex(buffers[node.fileIndex][0], node.fileIndex));
            bufferIndices[node.fileIndex] = 1;
            sizes[node.fileIndex] -= readSize;
        }
    }

    ofs.write((char *)&writing_buffer[0], writing_buffer.size() * sizeof(Pair));
    writing_buffer.clear();
    ofs.close();

    // Close all files
    for (auto& stream : streams) {
        stream.close();
    }

}

int main() {
    // default argument
    int K = 128;
    int band = 16;
    int range = 8;
    const string dataset_name = string(argv[1]);
    const string output_dir = string(argv[2]);
    // load arguments
    for (int i = 1; i < argc; i++) {
        const string arg = argv[i];
        if (arg == "-K") {
            K = atoi(argv[i + 1]);
        }

        if (arg == "-band") {
            band = atoi(argv[i + 1]);
        }

        if (arg == "-range") {
            range = atoi(argv[i + 1]);
        }
    }

    std::vector<std::string> filenames;
    for(int i = 0 ;i<band;i++){
        filenames.push_back("./tmp/sorted_"+to_string(i)+".bin");
    }
    
    const string simP_file_path = output_dir +"/"+ dataset_name + "_sim_pairs_" + "K" + to_string(K) + "B" + to_string(band) + "R" + to_string(range) + ".bin";
    mergeFiles(filenames, simP_file_path);
    return 0;
}
