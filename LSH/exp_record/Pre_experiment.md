# Preliminary Experiment

## Dataset
- Arxiv
- Docs: 1558306 Average Length (tokens) per Document: 18030
- Tokenizer: 20B_tokenizer From RedPajama

## Exp Record
- Exp Setting: Similarity Threshold 0.8
    1. SetJoin (Ground Truth): 
        - Found Similar Pairs: 3319410 
        - Contained Documents: 10695 
    2. OverlapJoin (K:32 C:29): 
        - Found Similar Pairs: 30903  
        - False Positve Pairs: 2873  
        - Contained Documents: 6034 
        - False Positive Document: 1969
    3. LSH: (K:128 B:9 R:13)
        - Found Similar Pairs: 20819     
        - False Positve Pairs: 9808   
        - Contained Documents: 25387  
        - False Positive Document: 15463 

- Exp Setting: Similarity Threshold 0.9
    1. SetJoin (Ground Truth): 
        - Found Similar Pairs: 3285654 
        - Contained Documents: 8333  
    2. OverlapJoin (K:32 C:29): 
        - Found Similar Pairs: 15745  
        - False Positve Pairs: 3360   
        - Contained Documents: 2886 
        - False Positive Document: 617 
    3. LSH: (K:128 B:5 R:25)
        - Found Similar Pairs: 8010 (The pairs amount is so low because in the slimpajama they only make the pair with the center of the cluster with other members)  
        - False Positve Pairs: 304
        - Contained Documents: 8293 
        - False Positive Document: 277 
    
