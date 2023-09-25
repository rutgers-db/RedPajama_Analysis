# Time Analysis

This file records the time cost of Setjoin whose parallism is based on group level paralled methods.

## Thres:0.8 Ngrams
| Dataset| Dataset Size (GB) | Used Memory| Docs Amount| Found Pairs | IO time | Index Time | Online Hashing Time| Allocation Time | Verification Time|
|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| stackexchange (M8 Lsketches Ngrams)    | 23G | 38G | 29825086 |17670 |36.645798|469.507394|13.577611|399.362348|0.816552|         
| stackexchange (Pure Ngrams)    | 180G | 231G | 29825086 |17530 |392.203862|3605.487832|54.278302|5937.728584|4.007801|