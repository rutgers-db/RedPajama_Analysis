#!/bin/bash
#SBATCH --mem=500g
#SBATCH --output=./script/lsketchsorting.txt 
for dataset_name in book arxiv wikipedia github sample
do
    ./bin/sort_lsketch.exe $dataset_name
done
