import numpy as np
import struct
from tqdm import tqdm
import pickle
import os
import argparse
def read_and_split_pajama_dataset(dataset_name, chunk_amount):
    parent_dir = "/common/users/zp128/RedPajama_Tokenized/" + dataset_name
    idx_file_path = parent_dir + "/tokenized_text_document.idx"
    bin_file_path = parent_dir + "/tokenized_text_document.bin"
    chunk_files_path = []
    for i in range(chunk_amount):
        chunk_files_path.append(f"{parent_dir}/chunk_{i}.pickle")
    
    # Intialize the pickle 
    # Load the full contents of the files into memory
    with open(idx_file_path, 'rb') as idx_file:
        idx_content = idx_file.read()
    with open(bin_file_path, 'rb') as bin_file:
        bin_content = bin_file.read()

    print(f"{dataset_name} Read into the memory")
    # Seek to the position where the document count is stored
    N = struct.unpack_from('q', idx_content, 18)[0]

    chunk_size = N // chunk_amount + 1

    # Set the initial position in the idx_content and bin_content
    idx_pos = 34
    bin_pos = 0

    # Store the temporary content of the current chunk
    chunk = []
    chunk_index = 0

    for doc_id in tqdm(range(N), desc="Processing documents", total=N):
        # Read the text length
        text_len = struct.unpack_from('i', idx_content, idx_pos)[0]

        # Read the binary data from memory and create a numpy array
        entity = np.frombuffer(bin_content[bin_pos:bin_pos+text_len*2], dtype=np.uint16).tolist() # convert numpy array to list for serialization

        # Update the positions for the next iteration
        idx_pos += 4
        bin_pos += text_len * 2

        chunk.append({"doc_id": doc_id, "text": entity})
        #Write the entity into the chunk_file as the format {"doc_id": doc_id, "text": entity,}
        if len(chunk) == chunk_size or doc_id == N - 1:
            chunk_file_path = chunk_files_path[chunk_index]
            with open(chunk_file_path, 'wb') as chunk_file:
                pickle.dump(chunk, chunk_file)
            
            chunk_index += 1
            chunk = []

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-dataset")
    args = parser.parse_args()
    read_and_split_pajama_dataset(args.dataset, 71);