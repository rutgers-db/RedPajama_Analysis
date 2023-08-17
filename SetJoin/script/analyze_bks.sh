#!/bin/bash
#SBATCH --mem=400g
#SBATCH --output=script/analyze_bks.txt 
for dataset_name in arxiv stackexchange wikipedia github
do
for thres in 0.9 0.8
do
for ratio in 0.1 0.2 0.4
do
    ./bin/compare.exe $dataset_name -t $thres -r $ratio
done 
done
done

# The script that sort the bottomks by their size