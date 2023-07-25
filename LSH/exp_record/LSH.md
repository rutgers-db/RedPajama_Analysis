# LSH
Analysis of LSH (C++) Varied from different <K, R, B>

# Experiment Record

##  Compared with Setjoin Result under threshold 0.8
All the rate here is calculated based on the result of setjoin 0.8 threshold
1. Table of LSH (K128, B4, R32)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 10.31     | 365 | 0.1342 | 0.8179|366|0.0847|0.8094|
| Arxiv    | 17.15 | 3281343 | 0.0 | 0.0115|7904|0.0003|0.2612|
| stackexchange    | 63.18 | 8176 | 0.0065 | 0.7458|15102|0.0045|0.6644|
| wikipedia    | 184.91
2. Table of LSH (K128, B8, R16)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 9.51     | 166605 | 0.9928 | 0.313|42195|0.9684|0.2412|
| Arxiv    | 19.33 | 3353751 | 0.0158 | 0.0056|13708|0.312|0.1182|
| stackexchange    | 67.75 | 40117 | 0.5143 | 0.3901|44476|0.2925|0.2975|
| wikipedia    | 1784.05

3. Table of LSH (K128, B16, R8)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 28.23    | 141383731 | 1.0 | 0.0058|193994|0.9909|0.0006|
| Arxiv    | 38.62 | 120196706 | 0.9724 | 0.0|1427708|0.9925|0.0006|
| stackexchange    |  98.64 | 182175612 | 0.9998 | 0.0111|12310350|0.9964|0.004|
| wikipedia    | 

4. Table of LSH (K256, B4, R64)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 16.87    | 79 | 0.0 | 0.9545|95|0.0|0.946|
| Arxiv    | 33.19 | 3279429 | 0.0 | 0.012|7484|0.0|0.3002|
| stackexchange    |  119.21 | 5497 | 0.0 | 0.8279|10060|0.0|0.7754|
| wikipedia    | 200.87

5. Table of LSH (K256, B8, R32)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 18.06    | 536 | 0.2649 | 0.7729|541|0.1848|0.7491|
| Arxiv    | 34.40 | 3284011 | 0.0 | 0.0107|8166|0.0006|0.2369|
| stackexchange    |  129.8 | 10078 | 0.01 | 0.6877|18457|0.0063|0.5906|
| wikipedia    | 341.42

6. Table of LSH (K256, B16, R16)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 16.13    | 289442 | 0.9951 | 0.1833|59441|0.9744|0.1348|
| Arxiv    | 33.30 | 3368734 | 0.0179 | 0.0033|15769|0.3696|0.0705|
| stackexchange    |  128.23 | 72789 | 0.6624 | 0.231|65848|0.4302|0.1623|
| wikipedia    |  2930.25

7. Table of LSH (K512, B4, R128)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 36.42   | 26 | 0.0 | 0.985|43|0.0|0.9755|
| Arxiv    |  53.58 | 3277814 | 0.0 | 0.0125|7235|0.0|0.3235|
| stackexchange    |  222.2 | 4190 | 0.0 | 0.8689|7504|0.0|0.8325|
| wikipedia    | 

8. Table of LSH (K512, B8, R64)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 32.44   | 140 | 0.0 | 0.9193|138|0.0|0.9215| 
| Arxiv    | 63.08 | 3279905 | 0.0 | 0.0119|7602|0.0|0.2892|
| stackexchange    |  226.54 | 6285 | 0.0 | 0.8033|11602|0.0|0.741|
| wikipedia    |

9. Table of LSH (K512, B16, R32)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 34.78   | 739 | 0.2625 | 0.6859|773|0.2109|0.653|
| Arxiv    | 59.34 | 3286267 | 0.0 | 0.01|8436|0.002|0.2128|
| stackexchange    |  215.200 | 12040 | 0.016 | 0.6292|21808|0.0094|0.5177|
| wikipedia    | 

10. Table of LSH (K9000, B20, R450)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 719.72   | 6 | 0.0 | 0.9965|12|0.0|0.9932|
| Arxiv    | 1137.14 | 3277375 | 0.0 | 0.0127|7087|0.0|0.3374|
| stackexchange    |  

##  Compared with Setjoin Result under threshold 0.9
All the rate here is calculated based on the result of setjoin 0.9 threshold
1. Table of LSH (K128, B4, R32)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 10.31     | 365 | 0.4411 | 0.4848|366|0.3415|0.4193|
| Arxiv    | 17.15  | 3281343 | 0.0001 | 0.0014|7904|0.0084|0.0594|
| stackexchange  | 63.18  | 8176 | 0.0693 | 0.3092|15102|0.0597|0.3073|
| wikipedia    | 184.91
2. Table of LSH (K128, B8, R16)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 9.51     | 166605 | 0.9977 | 0.0404|42195|0.9904|0.0265|
| Arxiv    | 19.33 | 3353751 | 0.0204 | 0.0001|13708|0.395|0.0047|
| stackexchange  | 67.75  | 40117 | 0.7334 | 0.0291|44476|0.5516|0.0273| 
| wikipedia    | 1784.05

3. Table of LSH (K128, B16, R8)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 28.23    | 141383731 | 1.0 | 0.0|193994|0.9979|0.0|
| Arxiv    | 38.62 | 120196706 | 0.9727 | 0.0|1427708|0.9942|0.0|
| stackexchange    |  98.64 | 182175612 | 0.9999 | 0.0|12310350|0.9983|0.0|
| wikipedia    | 

4. Table of LSH (K256, B4, R64)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 16.87    | 79 | 0.1266 | 0.8258|95|0.0421|0.7807|
| Arxiv    | 33.19  | 3279429 | 0.0 | 0.0019|7484|0.0003|0.1021|
| stackexchange    |  119.21 | 5497 | 0.0024 | 0.5021|10060|0.0024|0.5105|
| wikipedia    | 200.87

5. Table of LSH (K256, B8, R32)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 18.06    | 536 | 0.5261 | 0.3586|541|0.4529|0.2867|
| Arxiv    | 34.40 | 3284011 | 0.0004 | 0.0009|8166|0.0201|0.0397|
| stackexchange    |  129.8 | 10078 | 0.1189 | 0.1938|18457|0.1021|0.1916|
| wikipedia    | 341.42

6. Table of LSH (K256, B16, R16)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 16.13    | 289442 | 0.9986 | 0.0025|59441|0.993|0.0| 
| Arxiv    | 33.30 | 3368734 | 0.0247 | 0.0|15769|0.4719|0.0007|
| stackexchange    |  128.23 | 72789 | 0.8491 | 0.0025|65848|0.6894|0.0024| 
| wikipedia    |  2930.25

7. Table of LSH (K512, B4, R128)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 36.42   | 26 | 0.0 | 0.9343|43|0.0|0.8964|
| Arxiv    |  53.58 | 3277814 | 0.0 | 0.0024|7235|0.0|0.1318| 
| stackexchange    |  222.2 | 4190 | 0.0 | 0.6196|7504|0.0|0.634| 
| wikipedia    | 

8. Table of LSH (K512, B8, R64)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 32.44   | 140 | 0.0786 | 0.6742|138|0.029|0.6771|
| Arxiv    | 63.08 | 3279905 | 0.0 | 0.0018|7602|0.0004|0.0881| 
| stackexchange    |  226.54 | 6285 | 0.004 | 0.4316|11602|0.0041|0.4364|
| wikipedia    |

9. Table of LSH (K512, B16, R32)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 34.78   | 739 | 0.5643 | 0.1869|773|0.5291|0.1229|
| Arxiv    | 59.34 | 3286267 | 0.0008 | 0.0006|8436|0.0371|0.0252| 
| stackexchange    |  215.200 | 12040 | 0.1772 | 0.1005|21808|0.1514|0.0973| 
| wikipedia    | 

10. Table of LSH (K9000, B20, R450)

| Dataset| Time(s) | Pairs | Pairs FP rate| Pairs FN rate| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 719.72   | 6 | 0.0 | 0.9848|12|0.0|0.9711|
| Arxiv    | 1137.14 | 3277375 | 0.0 | 0.0025|7087|0.0|0.1495|
| stackexchange    |  