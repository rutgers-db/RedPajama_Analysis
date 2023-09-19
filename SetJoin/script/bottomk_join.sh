#!/bin/bash
#SBATCH --mem=500g
#SBATCH --output=script/bkjoin_script_2.txt 
for dataset_name in book #arxiv stackexchange wikipedia github
do
for thres in 0.9 0.8
do
for ratio in 0.1 0.2 0.4
do
    ./bin/sketch_join.exe $dataset_name $thres $ratio
done 
done
done

# The script that sort the bottomks by their size