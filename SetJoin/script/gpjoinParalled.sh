#!/bin/bash
#SBATCH --mem=500g
#SBATCH --output=script/gpjoinParalled_script.txt 
for dataset_name in book arxiv wikipedia github wikipedia 
do
for thres in 0.9 0.8
do
    /usr/bin/time -v ./bin/groupParelledJoin.exe $dataset_name $thres
done
done
/usr/bin/time -v ./bin/groupParelledJoin.exe sample 0.9
# The script that sort the bottomks by their size