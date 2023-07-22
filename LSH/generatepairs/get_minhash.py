import os
import gc
import struct
from datasketch import MinHash
from collections import defaultdict
from multiprocessing import Process, Queue, cpu_count, Pool, Value
from itertools import repeat
import time
import argparse
import pickle
import numpy as np
from tqdm import tqdm

def output_results(output_dir, results, chunk_id, iter):
    gc.collect()
    with open(
        f"{output_dir}/minhash_tokens/{iter}-{chunk_id}.pickle", "wb"
    ) as fout:
        pickle.dump(results, fout)

def generate_minhash(args):
# def generate_minhash(dataset_name,chunk_id, num_perm):
    dataset_name,chunk_id, num_perm = args
    gc.collect()
    parent_dir = "/common/users/zp128/RedPajama_Tokenized/" + dataset_name
    chunk_file_path = f"{parent_dir}/chunk_{chunk_id}.pickle"
    
    # Load the chunk file into memory
    with open(chunk_file_path, 'rb') as chunk_file:
        chunk_data = pickle.load(chunk_file)

    gc.collect()
    buckets = []
    for doc in tqdm(chunk_data, desc=f"Processing documents in chunk {chunk_id}", total= len(chunk_data)):
    # for doc in chunk_data:
        # Read the text length
        
        doc_id = doc["doc_id"]
        tokens = doc["text"]

        minhash = MinHash(num_perm=num_perm)
        for token in tokens:
            minhash.update(str(token).encode('utf8'))

        buckets.append(
            {"doc_id": doc_id, "hash": minhash,}
        )

    # write the buckets into the file
    # return buckets
    output_results(parent_dir, buckets, chunk_id, 0)


if __name__ == "__main__":
    # ParseS
    parser = argparse.ArgumentParser()
    parser.add_argument("-dataset")
    args = parser.parse_args()
    dataset_name = args.dataset # "stackexchange"

    # MinHash output directory
    parent_dir = "/common/users/zp128/RedPajama_Tokenized/" + dataset_name
    minh_dir = f"{parent_dir}/minhash_tokens/"

    if not os.path.exists(minh_dir):
        os.mkdir(minh_dir)

    processes = []

    start_time = time.time()

    chunk_amount = 71
    k = 10000
    num_perm = 128
    chunk_id = 0

    with Pool(processes=chunk_amount) as pool:
        for i, chunks in enumerate(
            tqdm(
                pool.imap(
                    generate_minhash,
                    zip(
                        repeat(dataset_name),
                        range(chunk_amount),
                        repeat(num_perm),
                    ),
                ),
                total=chunk_amount,
            )
        ):

            for chunk in chunks:
                if len(results) == k:
                    output_results(
                        parent_dir, results, chunk_id, 0
                    )
                    del results
                    gc.collect()
                    results = []
                    chunk_id += 1
                results.append(chunk)

    if results:
        output_results(parent_dir, results, chunk_id, 0)

    #Producer
    # for chunk_id in (1, 2, 4, 9, 12, 32, 34, 35, 36, 41, 51, 52, 53, 54, 55, 56, 58, 59, 60, 61, 65, 66, 67, 69, 70): #(2,35,41): #
    #     # Producer
    #     tmp_args = [dataset_name, chunk_id, num_perm]
    #     p = Process(
    #             target=generate_minhash,
    #             args=(dataset_name, chunk_id, num_perm ),
    #         )
    #     processes.append(p)
    #     p.start()
    
    print("All start")

    for p in processes:
        p.join()
    
    elapsed_time = time.time() - start_time
    print(f"Elapsed time: {elapsed_time} seconds")