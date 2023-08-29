#!/bin/bash
#SBATCH --mem=800g
#SBATCH --output=log.txt 
for dataset_name in book arxiv wikipedia github
do
    ./bin/sort_ngrams.exe $dataset_name
done
