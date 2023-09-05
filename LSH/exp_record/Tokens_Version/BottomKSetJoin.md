# SetJoin + Bottomk Formula

## Attention

The implementation does not strictly follow the bound of bottomk formula. We first calculate the bound based on bottomk+jaccard formula (It has a little bias) and in verification, we use true jaccard formula

## Experiment Result

### Arxiv

| Paramaters| Pairs | Pairs FP rate| Pairs FN rate| Pairs F1| Distinct Docs | Docs FP rate| Docs FN rate|
|----------|----------|----------|----------|----------|----------|----------|----------|
| T0.8 + R0.1 (98.77)     | 9015057 | 0.63258 | 0.00215|0.53708|12002|0.15706|0.05404|
| T0.8 + R0.2 (196)    | 8286133 | 0.60078 | 0.00344|0.57007|11219|0.08771|0.04301|
| T0.8 + R0.4 (393)     | 6695149 | 0.50731 | 0.00625|0.65877|10886|0.04915|0.03216|

### github