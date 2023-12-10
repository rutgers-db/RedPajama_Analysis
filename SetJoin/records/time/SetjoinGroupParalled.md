# Time Analysis

This file records the time cost of Setjoin whose parallism is based on group level paralled methods.

## Thres:0.8 Ngrams
| Dataset| Dataset Size (GB) | Used Memory| Docs Amount| Found Pairs | IO time | Index Time | Online Hashing Time| Allocation Time | Verification Time|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| stackexchange (M8 Lsketches Ngrams)    | 23G | 38G | 29825086 |17670 |36.645798|469.507394|13.577611|399.362348|0.816552|         
| stackexchange (Pure Ngrams)    | 180G | 231G | 29825086 |17530 |392.203862|3605.487832|54.278302|5937.728584|4.007801|
| sample (M8 Lsketches Ngrams) | 29G |  |13574695 |12981838 |22.753296|337.708222|18.984678|297.869289|3.141675|
| sample (Pure Ngrams)    | 225G | 158G | 13574695 |10504035 |603.912620|2200.245378|79.701442|6389.613357|32.369095|
| arxiv (Pure Ngrams) | 200G | 319G |1548620 |15934 |65.810232|1703.214803|66.637473|3371.995345|28.637102| 
|github  (Pure Ngrams) | 281G| 194G |25539436 |3141160507 |702.799256|3458.912061|131.147406|14095.799139|2425.894869|
|wikipedia  (Pure Ngrams)| 233G|  |27146652 |83274739 |579.789081|2963.837383|151.914790|4338.319437|123.588843|
|book (Pure Ngrams) |308G | |205645 |689 |590.343581|1264.603302|372.453968|2948.310357|553.282922|


## Thres:0.9 Ngrams
| Dataset| Dataset Size (GB) | Used Memory| Docs Amount| Found Pairs | IO time | Index Time | Online Hashing Time| Allocation Time | Verification Time|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| arxiv (Pure Ngrams)    | 200G | 157G | 1548620 |11113|570.655023|1190.078953|57.311368|3015.344264|0.566219|
|wikipedia (Pure Ngrams) | 233G| 105GB |27146652 |18455485 |434.758662|2087.204587|53.135962|4647.180056|3.917412|
|gihub | 281G| 110G |25539436 |393251283 |769.935467|2300.508319|84.031356|7034.427743|283.438409|
| sample (Pure Ngrams)    | 225G | 82G |13574695 |2452114 |417.217665|1401.418946|58.377853|3684.730249|2.536970|
| book (M8 Lsketches NGrams) | 39G | |205645 |273 |90.646460|180.885536|44.155980|218.456019|0.113344| 


### For Counting LowerBound Cost
| Dataset| Dataset Size (GB) | Used Memory| Docs Amount| Found Pairs | IO time | Index Time | Online Hashing Time| Allocation Time | Verification Time| Lowerbound Time|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| sample (M8 Lsketches Ngrams) | 29G |  |13574695 |12981838 |21.870466|308.962318|15.947846|294.390033|2.418080|131.235968|