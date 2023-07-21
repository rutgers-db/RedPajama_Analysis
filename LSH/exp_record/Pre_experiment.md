# Preliminary Experiment

## Dataset
- Tokenizer: 20B_tokenizer From RedPajama
- Datasets:
    - Arxiv 
        - Docs: 1558306 
        - Average Length (tokens) per Document: 18030
    - Stackexchange
        - Docs: 29825086 
        - Average Length (tokens) per Document: 688


## Exp Record
- Arxiv
    - Exp Setting: Similarity Threshold 0.8
        1. SetJoin (Ground Truth): 
            - Time: 582.339s
            - Found Similar Pairs: 3319410 
            - Contained Documents: 10695 
        2. OverlapJoin (K:32 C:29):
            - Time:  6229.71s
            - Found Similar Pairs: 30903  
            - False Positve Pairs: 2873  
            - Contained Documents: 6034 
            - False Positive Document: 1969
        3. LSH: (K:128 B:9 R:13)
            - Time: Build MinHash 5347s LSH: 107s
            - Found Similar Pairs: 20819     
            - False Positve Pairs: 9808   
            - Contained Documents: 25387  
            - False Positive Document: 15463 

    - Exp Setting: Similarity Threshold 0.9
        1. SetJoin (Ground Truth): 
            - Time: 191.97s
            - Found Similar Pairs: 3285654 
            - Contained Documents: 8333  
        2. OverlapJoin (K:32 C:29):
            - Time: 306.49s
            - Found Similar Pairs: 15745  
            - False Positve Pairs: 3360   
            - Contained Documents: 2886 
            - False Positive Document: 617 
        3. LSH: (K:128 B:5 R:25)
            - Time: Build MinHash 5347s LSH: 81s
            - Found Similar Pairs: 8010 (The pairs amount is so low because in the slimpajama they only make the pair with the center of the cluster with other members)  
            - False Positve Pairs: 304
            - Contained Documents: 8293 
            - False Positive Document: 277 
- Stackexchange
    - Exp Setting: Similarity Threshold 0.9
        
