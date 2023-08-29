# LSH vs Setjoin
Mention: The LSH here is the C++ version.

# Experiment Record

##  Threshold 0.8
1. Table of LSH (K 128, B6, R21)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    |
| Arxiv    |
| stackexchange | 358.281852| 9181 | 0.0398 | 0.4971|0.6601|17190|0.0283|0.427|
| wikipedia    |

2.  Table of SetJoin 

| Dataset       | Time(s) | Pairs | Distinct Docs |
|---------------|------|-------|---------------|
| Book          |  | |   |
| Arxiv         |  |  |   | 
| StackExchange | 5586 | 7975 | 15517  |
| Wikipedia     |  | |

##  Threshold 0.9
1. Table of LSH (K 128, B6, R21)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    |
| Arxiv    |
| stackexchange | 358.281852| 9181 | 0.2261 | 0.1091|0.8283|17190|0.1919|0.1047|
| wikipedia    |

2.  Table of SetJoin 

| Dataset       | Time(s) | Pairs | Distinct Docs |
|---------------|------|-------|---------------|
| Book          |  | |   |
| Arxiv         |  |  |   | 
| StackExchange | 5586 | 7975 | 15517  |
| Wikipedia     |  | |
