#!/bin/bash
#SBATCH -w rlab1
#SBATCH --mem=700g
#SBATCH --output=log/gpjoinParalledManyDataset.txt 
for dataset_name in  book # arxiv wikipedia github wikipedia sample #sample_full
do
for thres in 0.8
do
    /usr/bin/time -v ./bin/groupParelledJoin.exe $dataset_name $thres
done
done
# /usr/bin/time -v ./bin/groupParelledJoin.exe sample 0.9