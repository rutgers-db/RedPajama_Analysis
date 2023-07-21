# LSH vs Setjoin
Mention: The LSH here is the C++ version.

# Experiment Record

##  Threshold 0.8
1. Table of LSH

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 12.2     |  1557407 | 0.9991 | 0.1954|106069|0.9852|0.1086|
| Arxiv    | 24.19333   | 3401118 | 0.0281 | 0.0041|72115|0.8618|0.0681|
| stackexchange    | 94.84    |  212881 | 0.8882 | 0.2548|97949|0.6252|0.1805|
| wikipedia    | 4135     | 

2.  Table of SetJoin (K128, B9, R13)

| Dataset       | Time(s) | Pairs | Distinct Docs |
|---------------|------|-------|---------------|
| Book          | 2802 | 1735  | 1758          |
| Arxiv         | 582.339s |  3319410 | 10695  |
| StackExchange | 1465 |  31950 | 44793  |
| Wikipedia     | 

##  Threshold 0.9
1. Table of LSH

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    |  13| 1150 | 0.7678 | 0.3258|1263|0.7641|0.2819|
| Arxiv    |28| 3285963 | 0.0008 | 0.0007|8310|0.0357|0.0384|
| stackexchange | 73 | 11112 | 0.1968 | 0.1897|19696|0.1516|0.1849|
| wikipedia    | 404.32 | 

2.  Table of SetJoin (K128, B5, R25)

| Dataset       | Time(s) | Pairs | Distinct Docs |
|---------------|------|-------|---------------|
| Book          | 121 |396 | 415  |
| Arxiv         | 191.97 | 3285654 | 8333  | 
| StackExchange | 640 | 11014 | 20501  |
| Wikipedia     |  1134.6 | 