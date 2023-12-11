#!/bin/bash
#SBATCH --output=/research/projects/zp128/RedPajama_Analysis/LSH/Lsh_C++/log/bigdata_lsh.txt
#SBATCH --mem=500g

K=128
bands=6
for dataset_name in c4 common_crawl/2019-30 common_crawl/2020-05 common_crawl/2021-04 common_crawl/2022-05 common_crawl/2023-06
do
    echo ./bin/bigdata_lsh $dataset_name -K $K -band $bands -range $((K/bands))
    /usr/bin/time -v ./bin/bigdata_lsh $dataset_name -K $K -band $bands -range $((K/bands))
done