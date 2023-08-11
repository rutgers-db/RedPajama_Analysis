#!/bin/bash
#SBATCH --mem=500g
#SBATCH --output=simp_sort.txt 

# Define the directory paths
source_directory="/research/projects/zp128/RedPajama_Analysis/SetJoin/bottomk/similar_pairs"
destination_directory="/research/projects/zp128/RedPajama_Analysis/SetJoin/bottomk/sorted_simp"

# Iterate through the files in the source directory
for file in "$source_directory"/*
do
  # Check if it's a file and not a directory
  if [ -f "$file" ]; then
    filename=$(basename -- "$file")
    destination_path="$destination_directory/$filename"
    
    # Execute your program with the source file as the first parameter
    # and the destination path as the second parameter
    echo Sorting "$file"
    ./bin/sort.exe "$file" "$destination_path"
  fi
done