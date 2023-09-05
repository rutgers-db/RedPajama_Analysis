##  Experiment Record
Randomly Choose 100000 pairs and then calculate each (average) error with true jaccard similarity
- Fixed K: 32 or 128
- Adaptive K: orignal length * 0.4

If we find the errors of adaptive k is larger than that of fixed bottom k, maybe that is because the former k is adaptive and lower than latter on average.
### Mean Error

- K: 32 

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Book    |0.056 |0.070|0.006| 0.049 |
| Arxiv    |0.046 |0.047|0.012| 0.034 |
| stackexchange    |  0.049   |  0.045 | 0.030 | 0.030 |
| wikipedia    |0.025 |0.020|0.021| 0.018 |
| github    | 0.031     | 0.026| 0.027 |0.023|
| c4    |0.033 |0.026|0.029| 0.023 |

- K: 64 

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Book    |0.040 |0.065|0.006| 0.049 |
| Arxiv    |0.032 |0.045|0.012| 0.034 |
| stackexchange    |0.027 |0.028|0.029| 0.029 |
| wikipedia    |0.018 |0.015|0.019| 0.017 |
| github    |0.021 |0.021|0.024| 0.021 |
| c4    |0.022 |0.018|0.026| 0.021 |

- K: 128 

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Book    | 0.028     |  0.058 | 0.006 | 0.049|
| Arxiv    | 0.025   | 0.045 | 0.020 | 0.041|
| stackexchange    | 0.015    |  0.024 | 0.051 | 0.043 |
| wikipedia    | 0.013     | 0.014| 0.016 | 0.015 |
| github    | 0.015     | 0.018| 0.020 | 0.020|
| c4    | 0.015     | 0.016| 0.021 | 0.018|

### Mean and Variance of jaccard similarity

#### K: 32

- Book

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.253 | 0.236 | 0.192 | 0.254 | 0.204 |
| Variance| 0.008 | 0.011 | 0.009 | 0.008 | 0.008 |

- Arixv

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.180 | 0.180 | 0.145 | 0.177 | 0.146 |
| Variance| 0.002 | 0.005 | 0.004 | 0.002 | 0.002 |

- Stackexchange

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.103 | 0.062 | 0.062 | 0.086 | 0.076 |
| Variance| 0.001 | 0.002 | 0.001 | 0.002 | 0.001 |

- wikipeida

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.048 | 0.043 | 0.035 | 0.045 | 0.039 |
| Variance| 0.003 | 0.004 | 0.003 | 0.004 | 0.002 |

- github

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.065 | 0.058 | 0.046 | 0.061 | 0.049 |
| Variance| 0.001 | 0.002 | 0.001 | 0.002 | 0.001 |

- c4

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.069 | 0.063 | 0.053 | 0.068 | 0.057 |
| Variance| 0.001 | 0.002 | 0.001 | 0.002 | 0.001 |

#### K:64

- book 

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.253 | 0.237 | 0.191 | 0.254 | 0.204 |
| Variance| 0.008 | 0.010 | 0.009 | 0.008 | 0.008 |

- arxiv 

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.181 | 0.175 | 0.140 | 0.177 | 0.147 |
| Variance| 0.002 | 0.003 | 0.002 | 0.002 | 0.002 |

- stackexchange 

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.104 | 0.090 | 0.079 | 0.088 | 0.078 |
| Variance| 0.001 | 0.002 | 0.001 | 0.002 | 0.001 |

- wikipedia 

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.052 | 0.052 | 0.041 | 0.049 | 0.042 |
| Variance| 0.003 | 0.004 | 0.002 | 0.004 | 0.002 |

- github 

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.070 | 0.067 | 0.052 | 0.066 | 0.053 |
| Variance| 0.001 | 0.002 | 0.001 | 0.002 | 0.001 |

- c4 

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.074 | 0.078 | 0.061 | 0.074 | 0.062 |
| Variance| 0.001 | 0.001 | 0.001 | 0.002 | 0.001 |

#### K: 128 

- Book

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.254 | 0.243 | 0.198 | 0.254 | 0.205 |
| Variance| 0.008 | 0.009 | 0.009 | 0.008 | 0.008 |

- Arixv

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.1823   | 0.168 | 0.1382 | 0.178| 0.147 |
| Variance    |0.0013  | 0.0020 | 0.0019| 0.0016| 0.00187|

- Stackexchange

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.108 | 0.103 | 0.086 | 0.097 | 0.085 |
| Variance| 0.001 | 0.001 | 0.001 | 0.002 | 0.001 |

- wikipeida

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.059 | 0.060 | 0.046 | 0.058 | 0.048 |
| Variance| 0.004 | 0.004 | 0.003 | 0.005 | 0.003 |

- github

| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|----------|
| Mean    | 0.077 | 0.076 | 0.060 | 0.075 | 0.061 |
| Variance| 0.001 | 0.001 | 0.001 | 0.002 | 0.001 |

