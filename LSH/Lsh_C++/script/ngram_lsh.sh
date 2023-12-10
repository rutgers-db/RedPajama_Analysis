#!/bin/bash
#SBATCH --output=/research/projects/zp128/RedPajama_Analysis/LSH/Lsh_C++/log/ngram_lsh.txt
#SBATCH --mem=500g

K=128
bands=6
for dataset_name in book arxiv stackexchange wikipedia github sample
do
    echo ./bin/ngram_LSH $dataset_name -K $K -band $bands -range $((K/bands))
    /usr/bin/time -v ./bin/ngram_LSH $dataset_name -K $K -band $bands -range $((K/bands))
done