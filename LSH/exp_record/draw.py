import matplotlib.pyplot as plt
import numpy as np

# Data
time_lsh = [615, 485, 498, 977, 2392]
time_lsketch = [85, 125, 215, 394, 803]
memory_lsh = [56, 64, 77, 104, 158]
memory_lsketch = [7, 14, 28, 53, 108]
pairs_f1_lsh = [0.67649, 0.72200, 0.71899, 0.79753, 0.79948]
pairs_f1_lsketch = [0.68254, 0.81407, 0.85035, 0.92252, 0.95287]
docs_fn_rate_lsh = [0.06189, 0.04119, 0.04361, 0.03235, 0.03247]
docs_fn_rate_lsketch = [0.03447, 0.02579, 0.02053, 0.01555, 0.01083]
group_labels = ['K 128 & M 32', 'K 256 & M 16', 'K 512 & M 8', 'K 1024 & M 4', 'K 2048 & M 2']
index = np.arange(len(group_labels))
bar_width = 0.35

# Function for creating and saving grouped bar chart with larger font sizes
def create_grouped_bar_chart(data1, data2, y_label, title, filename):
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.bar(index, data1, bar_width, label='LSH', alpha=0.7)
    ax.bar(index + bar_width, data2, bar_width, label='Lsketch', alpha=0.7)
    
    # Setting font sizes
    plt.xlabel('K & M values', fontsize=25)
    plt.ylabel(y_label, fontsize=25)
    plt.title(title, fontsize=25)
    plt.xticks(index + bar_width / 2, group_labels, fontsize=17)
    plt.yticks(fontsize=25)
    plt.legend(fontsize=25)
    
    plt.savefig(filename, format='eps')

# Example usage for time cost comparison
create_grouped_bar_chart(memory_lsh, memory_lsketch, 
                         'Memory (GB)', 
                         'Memory Usage between LSH and Lsketch', 
                         'figures/memory.eps')
