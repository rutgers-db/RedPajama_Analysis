#  Experiment Record
Choose similar pairs and then calculate each (average) error with true jaccard and then calculate its False Negative.
The data is from NGrams

## Experiment Process
1. Extract the ground truth of the Setjoin
2. Get the sketch of the documents in the true similar pairs
    - Fixed K sketch is generated using the fixed K such as 64 or 128
    - Adaptive K sketch used K as adaptive value (ratio * original length of doc)
3. calculate how many jaccard calculated from the sketches are lower than the threshold

## Records of Different Datasets

### Stackexchange
False Negative Rate:

| Parameters | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| K128 AdapativeK 406 (ratio 0.1) Delta 0.8   |0.315 |0.299|0.271| 0.288 |
| K128 AdapativeK 199 (ratio 0.1) Delta 0.9  |0.203 |0.215|0.190| 0.199 |
| K64 AdapativeK 1627 (ratio 0.4)  Delta 0.8  |0.326 |0.283|0.200| 0.268 |
| K64 AdapativeK 1627  Delta 0.9  |0.192 |0.199|0.150| 0.181 |



