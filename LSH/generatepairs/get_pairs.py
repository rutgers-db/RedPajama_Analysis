import argparse
import pickle
import queue
import time
import os
from collections import defaultdict
from glob import glob
from multiprocessing import Process, Queue

from datasketch.lean_minhash import LeanMinHash
from more_itertools import divide


def _H(hs):
    return bytes(hs.byteswap().data)


def split_files(dataset_name, input_dir, n_proc):
    files = []
    files.extend(glob(f"{input_dir}/{dataset_name}/minhash_tokens/*"))
    files = sorted(files)
    parts = divide(n_proc, files)
    return [list(p) for p in parts]


def get_hashes(files, doc_queues, r):
    for fp in files:
        with open(fp, "rb") as fin:
            for item in pickle.load(fin):
                # key = f"{item['file_name']}@{item['doc_id']}"
                key = f"{item['doc_id']}"
                minhash = LeanMinHash(item["hash"])
                for i, doc_queue in enumerate(doc_queues):
                    H = _H(minhash.hashvalues[i * r: (i + 1) * r])
                    doc_queue.put((key, H))


def lsh(out_file, doc_queue, idx):
    lsh_dict = defaultdict(str)
    i = 0
    start_time = time.time()
    f = open(out_file.replace(".txt", f"-{idx}.txt"), "w")
    while True:
        try:
            key, H = doc_queue.get(timeout=30)
            cand = lsh_dict.get(H, "None")
            if cand != "None":
                f.write(f'{key} :: {cand}\n')
            else:
                lsh_dict[H] = key

            if i % 100000 == 0:
                print(
                    f"{idx}: Processed {i / 931361530 * 100}%.",
                    time.time() - start_time,
                )
            i += 1
        except queue.Empty:
            break

    print(f"Total number of documents: {i}")
    f.close()


def generate_pairs(bands, ranges, processes_num, dataset_name, input_dir, out_file):
    # size of the queue was tuned for optimal perf and memory constraints.
    doc_queues = [Queue(1000000) for _ in range(bands)]
    files = split_files(dataset_name, input_dir, processes_num)

    processes = []
    for process_id in range(processes_num):
        p = Process(
            target=get_hashes,
            args=(files[process_id], doc_queues, ranges,),
        )
        processes.append(p)
        p.start()

    for process_id in range(bands):
        p = Process(
            target=lsh,
            args=(out_file, doc_queues[process_id], process_id,),
        )
        processes.append(p)
        p.start()

    for p in processes:
        p.join()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-dataset")
    args = parser.parse_args()
    dataset_name = args.dataset  # "stackexchange"
    threshold = 0.8
    if threshold == 0.8:
        bands = 9
        ranges = 13
    if threshold == 0.9:
        bands = 5
        ranges = 25
    processes_num = 45
    input_dir = "/common/users/zp128/RedPajama_Tokenized/"
    out_dir = f"../similar_pairs/{dataset_name}_{threshold}/"
    if not os.path.exists(out_dir):
        os.mkdir(out_dir)
    out_file = f"{out_dir}{dataset_name}_simp_lsh.txt"

    start_time = time.time()

    generate_pairs(bands, ranges, processes_num,
                   dataset_name, input_dir, out_file)

    elapsed_time = time.time() - start_time
    print(f"Elapsed time: {elapsed_time} seconds")