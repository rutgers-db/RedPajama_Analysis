#!/bin/bash
#SBATCH --mem=300g
#SBATCH --output=script/analyze_bks_2.txt 
for dataset_name in  wikipedia github # arxiv book wikipedia stackexchange
do
for thres in 0.8 0.9
do
for ratio in 0.1 0.2 0.4
do
    ./bin/compare.exe $dataset_name -t $thres -r $ratio
done 
done
done

# The script that sort the bottomks by their size