# Experiment of Comparing Similarity Estimation Ways
## Description

Compare four estimation ways:
1. Fixed bottom k + BottomK formula
2. Fixed bottom k + Jaccard formula
3. Adaptive bottom k + BottomK formula
4. Adaptive bottom k + Jaccard formula

We wanna compare different formula, but in our straightforward, the jaccard formula should be more accuracy, cause it uses more information.

Below are the two formula that estimate jaccard similarity
- BottomK formula
$$ \frac{{|S_k(A \cup B) \cap S_k(A) \cap S_k(B)|}}{k}. $$

- Jaccard formula
$$ \frac{{ |S_k(A) \cap S_k(B)|}}{|S_k(A) \cup S_k(B)|}. $$

## Workflow
1. Randomly generate 1000000 docId pairs
2. Calculate the estimated similarity of each definition
3. Calculate the real jaccard similarity
4. Calculate the error of each way and compare them 
