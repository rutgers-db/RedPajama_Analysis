
import util
import argparse
def check_jaccard_similarity(dataset, simp, thres):
    # Go through each pair in simp_setjoin
    total_invalid_pairs_amount = 0
    for pair in simp:
        # Calculate the Jaccard similarity of the two documents in the pair
        jacc = util.jaccard_similarity(dataset[pair[0]], dataset[pair[1]])
        if jacc < thres:
#             print(jacc)
            total_invalid_pairs_amount  = total_invalid_pairs_amount + 1
    if total_invalid_pairs_amount == 0:
        print("All pairs have Jaccard similarity >= thres")
    else:
        print(f"There are {total_invalid_pairs_amount} pairs among {len(simp)} pairs' similarity lower than threshold.")


parser = argparse.ArgumentParser()
parser.add_argument("-dataset", default="book")
args = parser.parse_args()
dataset_name = args.dataset
# Parameters
# dataset_name = "arxiv"
thres = 0.8
if_check_similarity = False

# Load From lsketch result
simp_dir_path = "/research/projects/zp128/RedPajama_Analysis/SetJoin/data/ngram/lsketch_simp"
file_prefix = f"{dataset_name}_sim_pairs_{thres:.6f}"


# simpairs_bin_path = util.find_file(simp_dir_path, file_prefix)
simpairs_bin_path = "/research/projects/zp128/RedPajama_Analysis/SetJoin/test_simp.bin"
print(simpairs_bin_path)
idmap_bin_path = f"/research/projects/zp128/RedPajama_Analysis/SetJoin/data/ngram/sorted_lsketch/{dataset_name}_idmap.bin"

idmap = util.read_ints_from_binary(idmap_bin_path)
sim_pairs = util.read_pairs_from_binary(simpairs_bin_path)
simp_lsketch = util.map_elements(sim_pairs, idmap)
simp_lsketch = util.correct_pair_order(simp_lsketch)

# load from SetJoin
simpairs_bin_path = f"/research/projects/zp128/RedPajama_Analysis/SetJoin/data/ngram/sorted_simp/{dataset_name}_sim_pairs_{thres:.6f}.bin"
print(simpairs_bin_path)
idmap_bin_path = f"/research/projects/zp128/RedPajama_Analysis/SetJoin/data/ngram/sorted_ngrams/{dataset_name}_idmap.bin"
# idmap = util.read_ints_from_binary(idmap_bin_path)
sim_pairs = util.read_pairs_from_binary(simpairs_bin_path)

simp_setjoin = util.correct_pair_order(sim_pairs)

# Load the documents of simp in setjoin
# load the real documents
ids_setjoin = util.extract_elements(simp_setjoin)
# docs_setjoin = util.read_pajama_dataset_selected_docs(dataset_name,ids_setjoin)
ids_setjoin_size = len(ids_setjoin)
print(f"Setjoin finds {ids_setjoin_size} unique documents.")


ids_mylsh = util.extract_elements(simp_lsketch)
ids_mylsh_size = len(ids_mylsh)
print(f"Lsketch Method finds {ids_mylsh_size} unique documents.")
if if_check_similarity == True:
    docs_mylsh = util.read_pajama_dataset_selected_docs(dataset_name,ids_mylsh)
    check_jaccard_similarity(docs_mylsh,simp_lsketch, thres )


# print("Now Analyze the property of there similar pairs")
# print(f"There are {len(simp_setjoin)} pairs in simp_setjoin")
# print(f"There are {len(simp_lsketch)} pairs in simp_lsketch")

union_set = simp_setjoin.union(simp_lsketch)
union_size = len(union_set)
# print(f"The union of two sets includes {union_size} unique pairs.")

# Intersection
intersection_set = simp_setjoin.intersection(simp_lsketch)
intersection_pairs_size = len(intersection_set)
print(f"The intersection of two pairs set includes {intersection_pairs_size} common pairs.")

# Difference
difference_set = simp_setjoin.difference(simp_lsketch)  # B - A
FN_paris = len(difference_set)
# print(f"The difference  of two sets(B - A) includes {FN_paris} different pairs.")

# Difference
difference_set = simp_lsketch.difference(simp_setjoin)  # A - B
FP_paris = len(difference_set)
# print(f"The difference  of two sets(A - B) includes {FP_paris} different pairs.")

# print("Now Analyze the property of their documents")
# print(f"There are {len(ids_setjoin)} documents in simp_setjoin")
# print(f"There are {len(ids_mylsh)} documents in simp_lsketch")

union_set = ids_setjoin.union(ids_mylsh)
union_size = len(union_set)
# print(f"The union of two sets includes {union_size} documents.")

# Intersection
intersection_set = ids_setjoin.intersection(ids_mylsh)
intersection_size = len(intersection_set)
# print(f"The intersection of two sets includes {intersection_size} common documents.")

# Difference
difference_set = ids_setjoin.difference(ids_mylsh)  # B - A
FN_ids = len(difference_set)
print(f"The difference  of two sets(B - A) includes {FN_ids} different documents.")

# Difference
difference_set = ids_mylsh.difference(ids_setjoin)  # A - B
FP_ids = len(difference_set)
print(f"The difference  of two sets(A - B) includes {FP_ids} different documents.")


# print(f"Here is the comparison under  {dataset_name} Setjoin {thres} between LSH K{K}B{band}R{R}")
print(f"Now print out the exp record of Setjoin {thres} for Pairs and distinct Docs ")
print(f"| {len(simp_setjoin)} | {len(ids_setjoin)}  |")

print("Now print out the exp record of LSH for | Pairs |Pairs FP rate |Pairs FN rate|Distinct Docs|Docs FP rate|Docs FN rate")
pairs_fp_rate = round(FP_paris / len(simp_lsketch),4)
pairs_fn_rate = round(FN_paris / len(simp_setjoin),4)
precision = intersection_pairs_size / (intersection_pairs_size + FP_paris)
recall = intersection_pairs_size / (intersection_pairs_size + FN_paris);
F1 = round(2 * (precision * recall) / (precision + recall),4)

ids_fp_rate = round(FP_ids / len(ids_mylsh),4)
ids_fn_rate =round( FN_ids / len(ids_setjoin),4)
print(f"| {len(simp_lsketch)} | {pairs_fp_rate} | {pairs_fn_rate}|{F1}|{len(ids_mylsh)}|{ids_fp_rate}|{ids_fn_rate}|")
