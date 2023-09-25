# Time Analysis

This file records the time cost of Setjoin whose parallism is based on original methods.

## Thres:0.8 Tokens
| Dataset| Dataset Size (GB) | Docs Amount| Found Pairs | Index Time | Online Hashing Time| Memory Time| Finding Time | Allocation Time | Verification Time| 
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 3.4G     | 205743 | 1735 |57.253090|0.507968|0.505353|10.297710|92.340763|1181.474485|
| Arxiv    | 4.4G | 1558306 | 3319410 |120.353089|0.772291|0.527146|35.751856|186.408094|173.887044|
| stackexchange    | 11G | 29825086 |31950| |818.203153|3.062184|2.999524|89.290258|412.534489|26.530092|
| wikipedia    | 14G | 29834171 | 15232525114 |298.376542|2.747399|2.738803|120.009173|4156.532375|6678.474743|
| github    | 9.8G | 29834171 | 42116929003 |229.354525|8.486114|13.236269|445.278449|4935.511701|7811.699075|


## Thres:0.9 Tokens
| Dataset| Dataset Size (GB) | Docs Amount| Found Pairs | Index Time | Online Hashing Time| Memory Time| Finding Time | Allocation Time | Verification Time| 
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| Book    | 3.4G     | 205743 | 396 |26.405836|0.807895|0.184349|14.042030|47.990974|2.103604|
| Arxiv    | 4.4G | 1558306 | 3285654 |55.415054|0.782348|0.332483|26.089001|107.674366|0.323778|
| stackexchange    | 11G | 29825086 |11014 |364.726819|3.261139|2.324803|38.954612|177.801583|0.444563|
| wikipedia    | 14G | 29834171 | 141574669 |163.552565|5.430413|5.188511|148.004664|817.892723|161.020637|
| github    | 9.8G | 29834171 | 39181707284 |137.317448|6.285783|6.501419|146.362220|853.343808|327.684258|

The result shown before has bugs, the time of index is not correct and the index of one deletion neghbors is not built well.
Therefore, it will impact all the result. The result shown below is debugged already. And that makes the time in indexing significantly increases.


## Thres:0.8 Ngrams
| Dataset| Dataset Size (GB) | Docs Amount| Found Pairs | Index Time | Online Hashing Time| Memory Time| Finding Time | Allocation Time | Verification Time|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| stackexchange (M8 Lsketches Ngrams)    | 23G | 29825086 |17670 |1669.860246|4.366444|5.096554|116.583303|487.957136|0.731031|