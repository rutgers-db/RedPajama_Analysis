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



