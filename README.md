# RedPajama_Analysis

This is a project that Analyze the Large Dataset "RedPajama".
Our purpose is to get the duplicated documents pair in the project and then remove most of them to get a deduplicated dataset.

## Analysis Tools
We use three thechniques to analyze it.
1. Overlapjoin
2. OvplJoinRS
3. Setjoin

### OvplJoinRS

OverlapJoin for two sets (R and S). In our implementation, we treat the bottom-k of corpus as set **R** and the unqiue set of the corpus as set **S**. So that we can find all the document pairs **<a,b>** such that there exists **a** segment in document **b** whose jaccard similarity with document a exceeds threshold **theta**


