# SetJoin

This is the code that compute the groundtruth of the Large Dataset "RedPajama".
It is to compute all the similar pairs whose jaccard similarity is over the threshold. (not the estimated jaccard similarity)

## Purpose
This algorithm aims to build the benchmark of all similar pairs in the dataset. Compared to the bottom-k, it can find all the similarpairs.

## Reference
[An Efficient Partition Based Method for Exact Set Similarity Joins](https://people.cs.rutgers.edu/~dd903/assets/papers/vldb16-setjoin.pdf)

## MultiThread Modification
To handle the large dataset, we need to use multithread strategy.
Also, original code occupies too much memory, so we need to modify the structure of codes.

## Project Description
### Main Idea
The main idea of the code is to first find some candidates documents based on a partition method, then we gonna verify whether the candidate documents are similar to the current document using Jaccard Similarity
The paper focuses on the filtering stage (how to partition the documents so that for those have the same partition, are similar documents)

### Input Type
There are two kinds of Input depending on data preprocessing
1. tokens (16bits), when loading the data using util.loadShortBin function
2. ngrams (32bits), when loading the data using util.loadIntBin function

Attention: Before loading the data, you need to define the InputDataSize in the set_join.SetJoinParelled.h (using TokenLen = unsigned int (unsigned short))

### Output Type

Similar pairs: pair<unsigned int, unsigned int> indicates the document id between the similar documents that are similar than the given threshold

### Instruction

To use the SetJoinParelled Class, After create a instance, you need to call two functions:
1. index: The index() function builds an inverted index for partitions and one-deletion neighbors. It uses OpenMP to parallelize this process.
2. findSimPairs: The findSimPairs() function is responsible for finding similar pairs based on the index created. It uses OpenMP for parallelization.

After finding similar pairs, the result will be stored at a member variable "result_pairs"