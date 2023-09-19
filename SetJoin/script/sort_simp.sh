#!/bin/bash
#SBATCH --mem=400g
#SBATCH --output=script/simp_sort.txt 

# Define the directory paths
source_directory="/research/projects/zp128/RedPajama_Analysis/SetJoin/bottomk/similar_pairs"
destination_directory="/research/projects/zp128/RedPajama_Analysis/SetJoin/bottomk/sorted_simp"
# source_directory="/research/projects/zp128/RedPajama_Analysis/SetJoin/similar_pairs"
# destination_directory="/research/projects/zp128/RedPajama_Analysis/SetJoin/sorted_simp"

# Iterate through the files in the source directory
for file in "$source_directory"/*
do
  # Check if it's a file and not a directory
  if [ -f "$file" ]; then
    filename=$(basename -- "$file")
    destination_path="$destination_directory/$filename"
    
    # Check if the destination file already exists
    if [ -f "$destination_path" ]; then
      echo "Destination file $destination_path already exists. Skipping..."
      continue
    fi
    
    # Execute your program with the source file as the first parameter
    # and the destination path as the second parameter
    echo Sorting "$file"
    ./bin/sort.exe "$file" "$destination_path"
  fi
done