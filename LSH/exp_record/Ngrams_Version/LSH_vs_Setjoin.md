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
1. Table of LSH (K 128, B5, R25)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    |
| Arxiv    |  |13861 | 0.2201 | 0.0273|0.8657|1624|0.0302|0.0575|
| stackexchange | 173.045563| 7433 | 0.1235 | 0.1831|0.8457|14301|0.1076|0.1775|
| wikipedia    |

2.  Table of SetJoin 

| Dataset       | Time(s) | Pairs | Distinct Docs |
|---------------|------|-------|---------------|
| Book          |  | |   |
| Arxiv         |  | 11113 | 1671  |
| StackExchange | 5586 | 7975 | 15517  |
| Wikipedia     |  | |
