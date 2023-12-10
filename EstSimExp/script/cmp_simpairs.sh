#!/bin/bash
#SBATCH --mem=300g
#SBATCH --output=script/cmpsimparis.txt 

./bin/cmp_simpairs.exe stackexchange 0.8 128
./bin/cmp_simpairs.exe stackexchange 0.9 128
./bin/cmp_simpairs.exe arxiv 0.9 128

# The script that sort the bottomks by their size