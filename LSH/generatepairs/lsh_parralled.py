import os
import struct
from datasketch import MinHash
from collections import defaultdict
from multiprocessing import Process, Queue, cpu_count, Pool, Value
import queue
import time
import argparse
import pickle
import numpy as np
from tqdm import tqdm

# Create a shared counter
counter = Value('i', 0)

def _H(hs):
    return bytes(hs.byteswap().data)

doc_queues = []

def generate_minhash(dataset_name, doc_queues, chunk_id, r, num_perm=128):
    parent_dir = "/common/users/zp128/RedPajama_Tokenized/" + dataset_name
    chunk_file_path = f"{parent_dir}/chunk_{chunk_id}.pickle"
    
    # Load the chunk file into memory
    with open(chunk_file_path, 'rb') as chunk_file:
        chunk_data = pickle.load(chunk_file)

    for doc in tqdm(chunk_data, desc=f"Processing documents in chunk {chunk_id}", total= len(chunk_data)):
    # for doc in chunk_data:
        # Read the text length
        
        doc_id = doc["doc_id"]
        tokens = doc["text"]

        minhash = MinHash(num_perm=num_perm)
        for token in tokens:
            minhash.update(str(token).encode('utf8'))

        # LSH
        if minhash.is_empty() == False:
            for i, doc_queue in enumerate(doc_queues):
                H = _H(minhash.hashvalues[i * r : (i + 1) * r])
                doc_queue.put((doc_id, H))


def find_similar_pairs(simp_file_path, doc_queue, idx):
    similar_pairs = []
    print(f"Consumer {idx} starts")
    # Create dictionaries for each hash slice
    lsh_dict = defaultdict(int)
    f = open(simp_file_path.replace(".txt", f"-{idx}.txt"), "w")
    while True:
        try:
            doc_id, H = doc_queue.get(timeout=20)
            cand = lsh_dict.get(H, -1)
            if cand != -1:
                f.write(f'{doc_id} :: {cand}\n')
            else:
                lsh_dict[H] = doc_id
            # with counter.get_lock():
            #     counter.value += 1
        except queue.Empty:
            break
    
        # if counter.value % 1000 == 0:
        #     print(counter.value)

if __name__ == "__main__":
    # ParseS
    parser = argparse.ArgumentParser()
    parser.add_argument("-dataset")
    args = parser.parse_args()
    dataset_name = args.dataset # "stackexchange"

    # Similar pairs output directory
    threshold = 0.9
    simP_dir = f"../similar_pairs/{dataset_name}_{threshold}/"
    if not os.path.exists(simP_dir):
        os.mkdir(simP_dir)
    simP_file = simP_dir + dataset_name + "_simp_lsh.txt"

    
    if threshold == 0.9:
        bands = 5 
        ranges = 25
    if threshold ==0.8:
        bands = 9
        ranges = 13

    chunk_amount = 71
    doc_queues = [Queue(10000000) for _ in range(bands)]
    processes = []

    start_time = time.time()

    #Producer
    for chunk_id in range(chunk_amount):
        # Producer
        p = Process(
                target=generate_minhash,
                args=(dataset_name, doc_queues, chunk_id, ranges),
            )
        processes.append(p)
        p.start()
    
    # Consumer
    for process_id in range(bands):
        p = Process(
            target=find_similar_pairs,
            args=(simP_file, doc_queues[process_id], process_id,),
        )
        processes.append(p)
        p.start()
    
    print("All start")

    for p in processes:
        p.join()
    
    elapsed_time = time.time() - start_time
    print(f"Elapsed time: {elapsed_time} seconds")