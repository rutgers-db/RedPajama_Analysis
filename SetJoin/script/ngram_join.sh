#!/bin/bash
#SBATCH --mem=1000g
#SBATCH --output=script/ngramjoin_script.txt 
for dataset_name in book arxiv wikipedia github wikipedia
do
for thres in 0.9 0.8
do
    ./bin/ngram_join.exe $dataset_name $thres
done
done

# The script that sort the bottomks by their size