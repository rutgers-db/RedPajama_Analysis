import struct



# Function to read pairs of integers from a binary file and store them in a set.
def read_pairs_from_binary(filename):
    pairs = set()

    with open(filename, 'rb') as f:
        # read the first 8 bytes as a unsigned long long int to get the number of pairs
        # 'Q' is the format string for unsigned long long
        size = struct.unpack('Q', f.read(8))[0]

        # now read the rest of the file as pairs of ints
        for _ in range(size):
            # read 8 bytes and unpack them as two ints
            pair = struct.unpack('ii', f.read(8))
            pairs.add(pair)

    return pairs

# Function to read integers from a binary file and store them in a list.
def read_ints_from_binary(filename):
    ints = []

    with open(filename, 'rb') as f:
        # read the first 4 bytes as an int to get the number of ints
        # 'i' is the format string for int
        size = struct.unpack('i', f.read(4))[0]

        # now read the rest of the file as ints
        for _ in range(size):
            # read 4 bytes and unpack them as an int
            i = struct.unpack('i', f.read(4))[0]
            ints.append(i)

    return ints

# Function to map elements of pairs in a set to new values.
def map_elements(pair_set, idmap):
    # create a new set
    new_set = set()

    # iterate over each pair in the original set
    for pair in pair_set:
        # map each element of the pair using idmap
        new_pair = (idmap[pair[0]], idmap[pair[1]])

        # add the new pair to the new set
        new_set.add(new_pair)

    return new_set

# Function to correct the order of pairs in a set.
def correct_pair_order(pair_set):
    corrected_set = set()

    for pair in pair_set:
        # If the first element is greater than the second, reverse the pair
        if pair[0] > pair[1]:
            corrected_set.add((pair[1], pair[0]))
        else:
            corrected_set.add(pair)

    return corrected_set

# Function to extract individual elements from pairs in a set.
def extract_elements(pair_set):
    # Initialize two empty sets
    ids = set()

    for pair in pair_set:
        # Add the first and second elements of each pair to the corresponding sets
        ids.add(pair[0])
        ids.add(pair[1])

    return ids

# Function to calculate the Jaccard similarity between two lists.
def jaccard_similarity(list1, list2):
    s1 = set(list1)
    s2 = set(list2)
    return len(s1.intersection(s2)) / len(s1.union(s2))

# Function to read the whole dataset into the memory
def read_pajama_datset(dataset_name):
    parent_dir = "/research/projects/zp128/RedPajama-Data-1T/RedPajama-Data-1T/" + dataset_name
    idx_file_path = parent_dir + "/tokenized_text_document.idx"
    bin_file_path = parent_dir + "/tokenized_text_document.bin"
    docs = []
    # Open the files in binary mode
    with open(idx_file_path, 'rb') as idx_file, open(bin_file_path, 'rb') as bin_file:
        # Seek to the 18th byte
        idx_file.seek(18)

        # Read 8 bytes and interpret them as a long long integer
        N = struct.unpack('q', idx_file.read(8))[0]
        print(f"There are total {N} documents in this {idx_file_path}")

        # Read another 8 bytes
        idx_file.read(8)

        cnt = 0
        for i in range(N):
            # Read 4 bytes and interpret them as an integer
            text_len = struct.unpack('i', idx_file.read(4))[0]

            # Read the binary file
            entity = struct.unpack(f'{text_len}H', bin_file.read(text_len * 2))
            docs.append(entity)

# Function to read a dataset and select certain documents based on given indices.
def read_pajama_dataset_selected_docs(dataset_name, selected_indices):
    parent_dir = "/research/projects/zp128/RedPajama-Data-1T/RedPajama-Data-1T/" + dataset_name
    idx_file_path = parent_dir + "/tokenized_text_document.idx"
    bin_file_path = parent_dir + "/tokenized_text_document.bin"

    # Use a dictionary (map) instead of a list to store the documents
    docs = {}

    # Open the files in binary mode
    with open(idx_file_path, 'rb') as idx_file, open(bin_file_path, 'rb') as bin_file:
        # Seek to the 18th byte
        idx_file.seek(18)

        # Read 8 bytes and interpret them as a long long integer
        N = struct.unpack('q', idx_file.read(8))[0]
        print(f"There are total {N} documents in this {idx_file_path}")

        # Read another 8 bytes
        idx_file.read(8)

        for i in range(N):
            # Read 4 bytes and interpret them as an integer
            text_len = struct.unpack('i', idx_file.read(4))[0]

            if i in selected_indices:
                # Read the binary file
                entity = struct.unpack(
                    f'{text_len}H', bin_file.read(text_len * 2))

                # Add the entity to the docs dictionary
                docs[i] = entity
            else:
                # If the index is not in the selected_indices set, skip this document in the bin file
                bin_file.seek(bin_file.tell() + text_len * 2)

    return docs

# Function to check if all pairs in a set have Jaccard similarity greater than a given threshold.
def check_jaccard_similarity(dataset, simp, thres):
    # Go through each pair in simp_setjoin
    for pair in simp:
        # Calculate the Jaccard similarity of the two documents in the pair
        jacc = jaccard_similarity(dataset[pair[0]], dataset[pair[1]])
        if jacc < thres:
            print(jacc)
            return False  # If you want to stop at the first error
    print("All pairs have Jaccard similarity >= thres")
    return True  # If all pairs pass the check
