# LSH vs Setjoin
Mention: The LSH here is the C++ version.

# Experiment Record

##  Threshold 0.8
1. Table of LSH (K 128, B6, R21)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| Book (external Hashing)   | 464s| 328 | 0.01220 | 0.52975|0.63717|588|0.01020|0.44466|
| Arxiv    | 162.454954 | 14145 | 0.00170 | 0.11378|0.93893|1723|0.01103|0.24835|
| stackexchange |153.259485| 9181 | 0.0398 | 0.4971|0.6601|17190|0.0283|0.427|
| wikipedia    | 448.599124| 32427814 | 0.14268 | 0.66615|0.48056|992591|0.07948|0.36581|
| github| 858.364394| 709412120 | 0.07685 | 0.79151|0.34015|6873705|0.00951|0.23139|
| sample|650|| 3361276 | 0.05950 | 0.69904|0.45600|568636|0.01590|0.24395|
| c4|4847| 2689799380|
| common_crawl/2019-30 (external Hashing) |3306| 143441852|
| common_crawl/2020-05(external Hashing) |4119| 223899916|
| common_crawl/2021-04(external Hashing) |3883| 198424672|
| common_crawl/2022-05(external Hashing)|3489| 151713810|
| common_crawl/2023-06 (external Hashing)|3695| 207802987|

2. Table of LSH (K 128, B9, R14)
| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| Book (external Hashing)   | 464s|
| Arxiv    | 162.454954 | 15619 | 0.02132 | 0.04067|0.96891|2229|0.10049|0.11557|
| stackexchange |153.259485| 17843 | 0.25663 | 0.24335|0.74995|28863|0.18054|0.18869|
| wikipedia    | 448.599124| 164574230 | 0.65009 | 0.30848|0.46469|1957202|0.32337|0.08082|
| github| 858.364394| 2525872792 | 0.25726 | 0.40274|0.66210|8729334|0.06089|0.07453|
| sample|650| 9322572 | 0.27146 | 0.35340|0.68513|755561|0.09565|0.07682|

3. Table of LSH (K 128, B9, R13)
| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| Book (external Hashing)   | 927| 675 | 0.18667 | 0.20319|0.80499|987|0.12563|0.17653|
| Arxiv    | 515 | 17370 | 0.11180 | 0.03176|0.92650|2366|0.13187|0.09396|
| stackexchange |535| 20968 | 0.33647 | 0.20633|0.72279|31462|0.21744|0.15546|
| wikipedia    | 751| 311589098 | 0.79802 | 0.24424|0.31877|2139267|0.36698|0.06006|
| github| 1360| 2548960973 | 0.25709 | 0.39715|0.66559|8996544|0.07486|0.06038|
| sample|615| 9995430 | 0.30630 | 0.33989|0.67649|785195|0.11571|0.06189|

4. Table of LSH Running At Sample With Variant (K,B,R)
| (K,B,R)| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| (128,9,13)   | 615 | 9995430 | 0.30630 | 0.33989|0.67649|785195|0.11571|0.06189|
| (256,17,15)    | 485 | 11306251 | 0.30361 | 0.25043|0.72200|801083|0.11412|0.04119|
| (512,28,18) |498| 9226053 | 0.23122 | 0.32475|0.71899|777010|0.08897|0.04361|
| (1024,51,20)    | 977| 10540424 | 0.20385 | 0.20109|0.79753|784991|0.08762|0.03235|


All of those LSH running on common_crawl/2019-30, /2020-05 all use 300~500GB memory

3.  Table of SetJoin 

| Dataset       | Time(s) | Pairs | Distinct Docs |
|---------------|------|-------|---------------|
| Book          |  | |   |
| Arxiv         |  |  |   | 
| StackExchange |  | 17529 | 29151  |
| Wikipedia     |  | |

##  Threshold 0.9
1. Table of LSH (K 128, B5, R25)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|
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
