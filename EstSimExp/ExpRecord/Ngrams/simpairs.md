#  Experiment Record
Choose similar pairs and then calculate each (average) error with true jaccard and then calculate its False Negative.
The data is from NGrams
Attention: the data in ngrams has been filtered based on length filter
## Experiment Process
1. Extract the ground truth of the Setjoin
2. Get the sketch of the documents in the true similar pairs
    - Fixed K sketch is generated using the fixed K such as 64 or 128 or 256
    - Adaptive K sketch used K as adaptive value (ratio * original length of doc)
3. calculate how many jaccard calculated from the sketches are lower than the threshold

Attention: if the documents that correspond to those pairs are shorter than fixed K, then it and its pairs will be removed ( not accounting for the experiment result)

## Records of Different Datasets

### Stackexchange
False Negative Rate:

| Parameters | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula| Lsketch|
|----------|----------|----------|----------|----------|----------|
| K256 AdapativeK 1024 (ratio 0.25)  Delta 0.8 (L mod 16)  |0.081 |0.308|0.035| 0.287 | 0.037 |
| K256 AdapativeK 505 (ratio 0.25)  Delta 0.9 (L mod 16)  |0.050 |0.189|0.037| 0.189 | 0.036 |
| K128 AdapativeK 406 (ratio 0.1) Delta 0.8 (L mod 8)  |0.108 |0.300|0.053| 0.288 | 0.056 |
| K128 AdapativeK 199 (ratio 0.1) Delta 0.9 (L mod 8) |0.072 |0.216|0.054| 0.197 | 0.049 |

### Arxiv
False Negative Rate:

| Parameters | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| K128 AdapativeK 224 (ratio 0.1) Delta 0.9 |0.011 |0.289|0.244| 0.049 | 0.005 |

