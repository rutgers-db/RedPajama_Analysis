##  Experiment Record
Randomly Choose 100000 pairs and then calculate each (average) error with true jaccard similarity
- Fixed K: 128
- Adaptive K: orignal length * 0.4

### Mean Error 
| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Book    | 0.028     |  0.058 | 0.006 | 0.049|
| Arxiv    | 0.025   | 0.045 | 0.020 | 0.041|
| stackexchange    | 0.015    |  0.024 | 0.051 | 0.043 |
| wikipedia    | 0.013     | 0.014| 0.016 | 0.015 |
| github    | 0.015     | 0.018| 0.020 | 0.020|
| c4    | 0.015     | 0.016| 0.021 | 0.018|

### Mean and Variance of jaccard similarity

- Arixv

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Mean    | 0.1823   | 0.168 | 0.1382 | 0.178| 0.147 |
| Variance    |0.0013  | 0.0020 | 0.0019| 0.0016| 0.00187|

- Book

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
| Mean    | 0.254 | 0.243 | 0.198 | 0.254 | 0.205 |
| Variance| 0.008 | 0.009 | 0.009 | 0.008 | 0.008 |

- Stackexchange

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
| Mean    | 0.108 | 0.103 | 0.086 | 0.097 | 0.085 |
| Variance| 0.001 | 0.001 | 0.001 | 0.002 | 0.001 |
-wikipeida

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
| Mean    | 0.059 | 0.060 | 0.046 | 0.058 | 0.048 |
| Variance| 0.004 | 0.004 | 0.003 | 0.005 | 0.003 |

- github

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
| Mean    | 0.077 | 0.076 | 0.060 | 0.075 | 0.061 |
| Variance| 0.001 | 0.001 | 0.001 | 0.002 | 0.001 |