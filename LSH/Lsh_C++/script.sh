#!/bin/bash
#SBATCH --output=log/compare.txt
#SBATCH --mem=800g
for dataset_name in wikipedia
do
for K in 128 256 512
do 
for bands in 4 8 16
do
    if [[ $K -eq 128 && $bands -eq 16 ]]
    then
        echo "Jump the 128 16 8"
    else
        ./bin/compare $dataset_name -K $K -band $bands -range $((K/bands))
    fi
done
done
done

# for dataset_name in  wikipedia #book arxiv stackexchange
# do
# for K in 128 256 512
# do 
# for bands in 4 8 16
# do
#     ./main $dataset_name -K $K -band $bands -range $((K/bands))
# done
# done
# done

# for dataset_name in book arxiv stackexchange
# do
#     ./bin/main $dataset_name -K 9000 -band 20 -range 450
# done