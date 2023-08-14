#!/bin/bash
#SBATCH --mem=400g
#SBATCH --output=script/analysis.txt 
for dataset_name in book arxiv stackexchange wikipedia github c4
do
    python3 ./csv/analysis.py -dataset $dataset_name -K 64
done

# The script that sort the bottomks by their size