#!/bin/bash
#SBATCH --mem=400g
#SBATCH --output=eSort.txt 
for dataset_name in common_crawl/2020-05 common_crawl/2021-04 common_crawl/2020-05  common_crawl/2021-04 common_crawl/2022-05
do
    ./bin/eSortNgram.exe $dataset_name
    rm -r "/research/projects/zp128/RedPajama_Analysis/LSH/slimpajama/$dataset_name"
    echo "$dataset_name done"
done
