#!/bin/bash
#SBATCH --output=log.txt
#SBATCH --mem=800g
for dataset_name in  wikipedia #book arxiv stackexchange
do
for K in 128 256 512
do 
for bands in 4 8 16
do
    ./main $dataset_name -K $K -band $bands -range $((K/bands))
done
done
done
