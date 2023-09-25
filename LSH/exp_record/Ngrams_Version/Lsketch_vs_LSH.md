# LSH vs Setjoin
Mention: The LSH here is the C++ version.

# Experiment Record

##  Threshold 0.8
1. Table of LSH (K 196, B14, R14)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    |
| Arxiv    |
| stackexchange | 300+| 21486 | 0.3135 | 0.1585|0.7561|33456|0.2291|0.1153|
| wikipedia    |

2.  Table of SetJoin + Lsketch(M8 Average Length 202)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| Book          |  | |   |
| Arxiv         |  |  |   | 
| StackExchange |999.85 | 17670 | 0.0637 | 0.0562|0.9401|29507|0.0611|0.0497|
| Wikipedia     |  | |
