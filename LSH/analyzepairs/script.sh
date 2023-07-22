#!/bin/bash
for dataset_name in book arxiv stackexchange #wikipedia
do
for K in 128 256 512
do 
for bands in 4 8 16
do
    python3 lshVsSetjion.py -dataset $dataset_name -K $K -band $bands -range $((K/bands))
done
done
done
