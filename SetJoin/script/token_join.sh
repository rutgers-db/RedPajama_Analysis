#!/bin/bash
#SBATCH --mem=300g
#SBATCH --output=script/tokenJoin.txt 
for dataset_name in stackexchange # book arxiv wikipedia github
do
for thres in 0.8 0.9
do
    ./bin/main.exe $dataset_name $thres
done
done

# The script that sort the bottomks by their size