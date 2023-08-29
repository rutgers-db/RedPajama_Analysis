#  Experiment Record
Choose similar pairs and then calculate each (average) error with true jaccard and then calculate its False Negative.
The data is from NGrams
- Fixed K: 128
- Adaptive K: orignal length * 0.1

## Threshold 0.8

### Mean Error
- K: 128
- Average Adpative K: 406(0.1 ratio)

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Book    ||
| Arxiv    |
| stackexchange    |0.041 |0.041|0.036| 0.038 |
| wikipedia    | |||  |
| github    | 
| c4    |


### False Positive
- K: 128
- Average Adpative K: 406 (0.1 ratio)

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Book    ||
| Arxiv    |
| stackexchange    |0.315 |0.299|0.271| 0.288 |
| wikipedia    | |||  |
| github    | 
| c4    |

## Threshold 0.9

### Mean Error
- K: 128
- Average Adpative K: 199 (0.1 ratio)
| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Book    ||
| Arxiv    |
| stackexchange    |0.019 |0.019|0.018| 0.019 |
| wikipedia    | |||  |
| github    | 
| c4    |


### False Positive
- K: 128
- Average Adpative K: 199 (0.1 ratio)

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Book    |
| Arxiv   |
| stackexchange    |0.203 |0.215|0.190| 0.199 |
| wikipedia    | |||  |
| github    | 
| c4    |

## Threshold 0.8

### Mean Error
- K: 64
- Average Adpative K: 1627 (0.4 ratio)

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| stackexchange    |0.047 |0.047|0.029| 0.031 |

### False Positive
| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| stackexchange    |0.326 |0.283|0.200| 0.268 |

## Threshold 0.9

### Mean Error
- K: 64
- Average Adpative K: 1627 (0.4 ratio)

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| stackexchange    |0.024 |0.024|0.012| 0.014 |

### False Positive
| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| stackexchange    |0.192 |0.199|0.150| 0.181 |


## Threshold 0.8

Tokens 
### Mean Error
- K: 128
- Average Adpative K: 179 (0.4 ratio)

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| stackexchange    |0.056 |0.095|0.052| 0.052 |

### False Positive
| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| stackexchange    |0.366 |0.391|0.323| 0.331 |

