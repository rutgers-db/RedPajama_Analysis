#!/bin/bash
#SBATCH --mem=400g
#SBATCH --output=script/errors.txt 
for dataset_name in book arxiv stackexchange wikipedia github c4
do
    ./bin/main.exe $dataset_name 64
done

# The script that sort the bottomks by their size