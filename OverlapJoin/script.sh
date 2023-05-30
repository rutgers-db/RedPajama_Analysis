# This is a script that generate the similar pairs and write them into similar_pairs (directory)

for bottomK_bin in 'arxiv_bottom_64.bin' 'book_bottomK_1024.bin' 'stackexchange_bottomK_1024.bin' #'wikipedia_bottomK_1024.bin'
do      
        srun --mem=999g /usr/bin/time -v ./bin/main.exe ${bottomK_bin} >> script_log.txt
        echo "${bottomK_bin} done"
done