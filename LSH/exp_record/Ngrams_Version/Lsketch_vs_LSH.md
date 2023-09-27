# LSH vs Setjoin
Mention: The LSH here is the C++ version.

# Experiment Record

##  Threshold 0.8
1. Table of LSH 

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    |
| Arxiv    |
| stackexchange  (K 196, B14, R14) | 300+| 21486 | 0.3135 | 0.1585|0.7561|33456|0.2291|0.1153|
| wikipedia    |
| sample  (K 556, B30, R18)   | 2656 | 9524467 | 0.2355 | 0.3068|0.7271|783201|0.0925|0.0397|

2.  Table of SetJoin + 

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| Book          |  | |   |
| Arxiv         |  |  |   | 
| StackExchange  Lsketch(M8 Average Length 202) |999.85 | 17670 | 0.0637 | 0.0562|0.9401|29507|0.0611|0.0497|
| Wikipedia     |  | |
| sample  Lsketch(M8 Average Length 556)  | 657.552721 |12981838 | 0.231 | 0.0496|0.8502|766815|0.0545|0.0205|