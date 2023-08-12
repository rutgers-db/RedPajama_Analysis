##  Experiment Record
Randomly Choose 100000 pairs and then calculate each (average) error with true jaccard similarity
- Fixed K: 128
- Adaptive K: orignal length * 0.4

| Dataset| Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|
|----------|----------|----------|----------|----------|
| Book    | 0.028     |  0.058 | 0.006 | 0.049|
| Arxiv    | 0.025   | 0.045 | 0.020 | 0.041|
| stackexchange    | 0.015    |  0.024 | 0.051 | 0.043 |
| wikipedia    | 0.013     | 0.014| 0.016 | 0.015 |
| github    | 0.015     | 0.018| 0.020 | 0.020|
| c4    | 0.015     | 0.016| 0.021 | 0.018|