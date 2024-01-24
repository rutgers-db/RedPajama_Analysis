# Time Analysis

This file records the time cost of Setjoin whose parallism is based on group level paralled methods.

## Thres:0.8 Ngrams
| Dataset| Dataset Size (GB) | Used Memory| Docs Amount| Found Pairs | IO time | Index Time | Online Hashing Time| Allocation Time | Verification Time| Total Time|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|     
| stackexchange (Pure Ngrams)    | 180G | 231G | 29825086 |17530 |392.203862|3605.487832|54.278302|5937.728584|4.007801|
| sample (Pure Ngrams)    | 225G | 252G |13574695 |10504035 |305.354680|806.821841|169.558715|1132.062272|97.355984|2205.798812|
| arxiv (Pure Ngrams) | 200G | 469G |1548620 |15934 |236.559965|862.991002|147.372082|986.827670|34.737289|2031.928043|
|github  (Pure Ngrams) | 281G| 331G |25539436 |3141160510 |387.233125|1034.004630|249.874771|6485.274495|10736.921525|18506.075420|
|wikipedia  (Pure Ngrams)| 233G| 246G |27146652 |83274739 |368.993826|911.283809|154.514276|1528.709336|637.938220|3232.445641|
|book (Pure Ngrams) |308G | 719G |205645 |689 |332.972263|801.445494|376.246746|1054.767058|265.778930|2498.238228|


## Thres:0.9 Ngrams
| Dataset| Dataset Size (GB) | Used Memory| Docs Amount| Found Pairs | IO time | Index Time | Online Hashing Time| Allocation Time | Verification Time|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| arxiv (Pure Ngrams)    | 200G | 157G | 1548620 |11113|570.655023|1190.078953|57.311368|3015.344264|0.566219|
|wikipedia (Pure Ngrams) | 233G| 105GB |27146652 |18455485 |434.758662|2087.204587|53.135962|4647.180056|3.917412|
|gihub | 281G| 110G |25539436 |393251283 |769.935467|2300.508319|84.031356|7034.427743|283.438409|
| sample (Pure Ngrams)    | 225G | 82G |13574695 |2452114 |417.217665|1401.418946|58.377853|3684.730249|2.536970|
| book (M8 Lsketches NGrams) | 39G | |205645 |273 |90.646460|180.885536|44.155980|218.456019|0.113344| 
| book (Pure NGrams) | 305G | 286G|205645 |273 |360.147514|824.177096|205.646046|1792.314978|2.225705|

### For Counting LowerBound Cost
| Dataset| Dataset Size (GB) | Used Memory| Docs Amount| Found Pairs | IO time | Index Time | Online Hashing Time| Allocation Time | Verification Time| Lowerbound Time|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| sample (M8 Lsketches Ngrams) | 29G |  |13574695 |12981838 |21.870466|308.962318|15.947846|294.390033|2.418080|131.235968|