/*
This is a file that extract the set (unqiue elements and sort them) of the Redpajama datasets
Also will sort them based on their sizes

And then output the sorted sets in the datasets directory and their idmap.
*/

#include <bits/stdc++.h>
#include <execution>
#include "../src/util/io.h"
#include <omp.h>

using namespace std;

class Buffer {
public:
    Buffer(){}
    
    void openFile(const std::string& filename, std::size_t _batchSize){
        ifs.open(filename, std::ios::binary);
        batchSize = _batchSize;
        isEndOfFile = false;
        currentIndex = 0;
        if (!ifs) {
            std::cerr << "Error opening bin file" << std::endl;
            isEndOfFile = true; // Mark as end of file to prevent further reading
        } else {
            loadBatch();
        }
    }

    void loadIdmap(const std::string& filename){
        loadBin2vec(filename,idmap);
    }

    Buffer(const std::string& filename, std::size_t batchSize)
        : ifs(filename, std::ios::binary), batchSize(batchSize), isEndOfFile(false), currentIndex(0) {
        if (!ifs) {
            std::cerr << "Error opening bin file" << std::endl;
            isEndOfFile = true; // Mark as end of file to prevent further reading
        } else {
            loadBatch();
        }
    }

     bool getNextVector() {
        if (currentIndex == currentBatch.size() - 1) {
            if (isEndOfFile) {
                return false; // Return empty vector to signal end of file
            }
            loadBatch();
            if(currentBatch.size() == 0)
                return false;
            currentIndex = 0;
            cnt++;
        }else{
            currentIndex++;
            cnt++;
        }
        return true;
    }

    bool endOfFile() const {
        return isEndOfFile && currentIndex >= currentBatch.size();
    }

private:
    void loadBatch() {
        currentBatch.clear();
        for (std::size_t i = 0; i < batchSize; ++i) {
            int size;
            if (ifs.read(reinterpret_cast<char*>(&size), sizeof(int))) {
                std::vector<unsigned int> vec(size);
                ifs.read(reinterpret_cast<char*>(&vec[0]), sizeof(unsigned int) * size);
                currentBatch.emplace_back(vec);
            }
            else {
                isEndOfFile = true;
                ifs.close();
                break;
            }
        }
    }

public:
    std::ifstream ifs;
    std::size_t batchSize;
    bool isEndOfFile;
    std::size_t currentIndex = 0;
    std::vector<std::vector<unsigned int>> currentBatch;
    std::vector<int> idmap;
    unsigned int cnt = 0;
};

Buffer* buffers;
// Due to pq is max heap we need to let the bigger vector has more weight
struct BufferComparator {
    bool operator()(unsigned int i, unsigned int j) {
        auto & a = buffers[i].currentBatch[buffers[i].currentIndex];
        auto & b = buffers[j].currentBatch[buffers[j].currentIndex];

        if (a.size() < b.size()) return false;
        if (a.size() > b.size()) return true;

        for (size_t i = 0; i < a.size(); ++i) {
            if (a[i] < b[i]) return false;
            if (a[i] > b[i]) return true;
        }
        return buffers[i].idmap[buffers[i].cnt] > buffers[j].idmap[buffers[j].cnt]; // returning false when both vectors are equal
    }
};


int main(int argc, char *argv[]) {
    // global variables
    const string root_dir = "/research/projects/zp128/RedPajama_Analysis/SetJoin";
    const string dataset_name = string(argv[1]);
    const string ngram_dir_path = "/research/projects/zp128/RedPajama_Analysis/LSH/slimpajama/" + dataset_name; 
    const string ngram_tmp_path = "/research/projects/zp128/RedPajama_Analysis/LSH/slimpajama/" + dataset_name +"_tmp";
    const string sortedNgram_path = "/research/projects/zp128/RedPajama_Analysis/LSH/slimpajama/" + dataset_name +"_sortedngram.bin";
    const string ori_idmap_path = "/research/projects/zp128/RedPajama_Analysis/LSH/slimpajama/" + dataset_name +"_idmap.bin";
    const string sorted_idmap_path = "/research/projects/zp128/RedPajama_Analysis/LSH/slimpajama/" + dataset_name +"_sortidmap.bin";
    vector<int> ori_idmap;
    loadBin2vec(ori_idmap_path,ori_idmap);
    printf("There are %lu documents in it\n", ori_idmap.size());

    // load original idmap
    createDirectoryIfNotExists(ngram_tmp_path);
    // string idmap_file_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name + "_idmap.bin";
    // string sortedngram_path = root_dir + "/data/ngram/sorted_ngrams/" + dataset_name + "_sortedngram.bin";

    unsigned int id_st = 0;
    unsigned int files_amount = countFilesAmountInOneDir(ngram_dir_path);
    
    // // First Stage: Iterate each sub file and sort it.
    // for(auto i = 0 ;i<files_amount;i++){
    //     printf("Doing %u \n", i);
    //     // Input bottom_k and sort them
    //     string ngram_path = ngram_dir_path + "/" + to_string(i) + ".bin";
    //     string idmap_file_path = ngram_tmp_path + "/" + to_string(i) + "_idmap.bin";
    //     string sortedngram_path =  ngram_tmp_path + "/" + to_string(i) + "_sortedngram.bin";

    //     vector<vector<unsigned int>> ngrams;
    //     loadIntBin(ngram_path, ngrams);
    //     // Sort ngrams based on their length, elements
    //     vector<int> idmap_ngrams;
    //     for (int i = 0; i < ngrams.size(); i++)
    //         idmap_ngrams.emplace_back(i);

    //     // sort dataset by size first, element second, id third
    //     sort(execution::par_unseq, idmap_ngrams.begin(), idmap_ngrams.end(), [&ngrams](const unsigned int &id1, const unsigned int &id2) {
    //         auto dsize1 = ngrams[id1].size();
    //         auto dsize2 = ngrams[id2].size();
    //         if (dsize1 < dsize2)
    //             return true;
    //         else if (dsize1 > dsize2)
    //             return false;
    //         else {
    //             for (int i = 0; i < dsize1; i++)
    //             {
    //                 if (ngrams[id1][i] < ngrams[id2][i])
    //                     return true;
    //                 else if (ngrams[id1][i] > ngrams[id2][i])  
    //                     return false;
    //             }
    //             if (id1 < id2)
    //                 return true;
    //             else
    //                 return false;
    //         } });

        

    //     ofstream ofs;
    //     ofs.open(sortedngram_path.c_str(), ios::binary);    
    //     for (auto i = 0; i < idmap_ngrams.size(); i++) {
    //         const auto &vec = ngrams[idmap_ngrams[i]];
    //         unsigned int size = vec.size();

    //         ofs.write((char *)&size, sizeof(unsigned int));
    //         ofs.write((char *)&vec[0], size * sizeof(unsigned int)); // Write the vector data
    //     }
    //     ofs.close();

    //     // add the id_st for each element in the idmap
    //     for (auto & id: idmap_ngrams){
    //         id += id_st;
    //     }
    //     id_st += ngrams.size();
    //     // write the idmap and the sorted set
    //     writeVec2Bin(idmap_file_path, idmap_ngrams);

    // }
    
    // Second Stage: Merge this sub sorted files into one big file
    cout<< " Start Merging these files" <<endl;
    // Initialize Buffer
    buffers = new Buffer[files_amount];
    for(int i = 0 ;i<files_amount;i++){
        string sortedngram_path =  ngram_tmp_path + "/" + to_string(i) + "_sortedngram.bin";
        string idmap_file_path = ngram_tmp_path + "/" + to_string(i) + "_idmap.bin";
        buffers[i].openFile(sortedngram_path, 1500);
        buffers[i].loadIdmap(idmap_file_path);
    }

    // create a heap
    priority_queue<unsigned int, vector<unsigned int>, BufferComparator> pq;
    for(unsigned int i = 0 ;i<files_amount;i++){
        if(buffers[i].isEndOfFile == false)
            pq.push(i);
    }

    vector<int> sorted_idmap;
    // open sorted ngram file
    ofstream ofs(sortedNgram_path, ios::binary | ios::out);
    const unsigned int once_docs_limit = 6000;
    unsigned int docs_tmp_cnt = 0;
    vector<unsigned int> writing_buffer;

    cout<<"start pq queue"<< pq.size() << endl;
    // wirte the vector 
    while (!pq.empty()) {
        auto buff_id = pq.top();
        auto& buff = buffers[buff_id];
        if(docs_tmp_cnt == once_docs_limit){
            // clear buffer
            ofs.write(reinterpret_cast<const char *>(writing_buffer.data()), writing_buffer.size() * sizeof(unsigned int));
            writing_buffer.clear();
            docs_tmp_cnt = 0;
        }
        // add the vec into the buffer
        docs_tmp_cnt ++;
        auto & vec = buff.currentBatch[buff.currentIndex];
        if(vec.size() == 0){
            cout<< "error" << buff_id << " "<< buff.currentIndex<<endl;
            break;
        }
        writing_buffer.emplace_back(vec.size());
        for(auto ele: vec){
            writing_buffer.emplace_back(ele);
        }
        sorted_idmap.emplace_back(ori_idmap[buff.idmap[buff.cnt]]);

        // pop the top
        pq.pop();

        // if it has next just add it again
        if(buff.getNextVector())
            pq.push(buff_id);

        // cout<<buff_id<<endl;
    }

    // clear buffer
    ofs.write(reinterpret_cast<const char *>(writing_buffer.data()), writing_buffer.size() * sizeof(unsigned int));
    writing_buffer.clear();
    ofs.close();

    // write the idmap
    writeVec2Bin(sorted_idmap_path,sorted_idmap);

    delete[] buffers;
}