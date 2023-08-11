#!/bin/bash
#SBATCH --mem=300g
#SBATCH --output=log.txt 
for dataset_name in book arxiv stackexchange wikipedia github c4
do
    ./bin/sort_bottomks.exe $dataset_name
done

# The script that sort the bottomks by their size