from datasketch import lsh
threshold = 0.8
num_perm = 128
print(lsh._optimal_param(threshold, num_perm, 0.95, 0.05))

# 0.9 32 -> (2, 16)
# 0.8 32 -> (3, 10)
# 0.8 512 -> (28, 18)
# 0.8 1024 -> (51, 20)
# 0.8 128 0.2 0.8 (12, 10) 
# 0.8 128 0.2 0.8 (7, 18)
# 0.8 128 0.9 0.1 (6, 21) 
# 0.8 128 0.95 0.05 (5, 25) 