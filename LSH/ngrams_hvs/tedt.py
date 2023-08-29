import struct
import array

def read_binary_file(filename):
    with open(filename, 'rb') as f:
        # Read the size of the list
        size_bytes = f.read(4)
        size = struct.unpack('<I', size_bytes)[0]
        print(size)
        # Read the data
        data_bytes = f.read(4 * size)  # each int is 4 bytes

        # Convert the bytes to integers
        data = array.array('I')  # 'I' stands for unsigned int
        data.frombytes(data_bytes)

    return list(data)

# Test
filename = "./book_idmap.bin"
data = read_binary_file(filename)
print(data)
