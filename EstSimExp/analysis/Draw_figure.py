import pandas as pd


import pandas as pd
import matplotlib.pyplot as plt

dataset = "stackexchange"
k = 128
csv_path = f"../csv/{dataset}_{k}.csv" 

# Load the CSV file into a pandas DataFrame
df = pd.read_csv(csv_path)

type = ["Fixed bottom k + BottomK formula" , "Fixed bottom k + Jaccard formula", "Adaptive bottom k + BottomK formula", "Adaptive bottom k + Jaccard formula "]
# Assuming the first column is your reference column
ref_col = df.iloc[:, 0]

# Plot the histogram of differences for each of the next four columns
for i in range(1, 5):
    diff = ref_col - df.iloc[:, i]
    
    # Plot the histogram
    plt.figure()
    plt.hist(diff, bins=50, edgecolor='black', alpha=0.75) # You can adjust the number of bins if needed
    plt.title(f"Difference Histogram: {type[i-1]} K:{k}")
    plt.xlabel('Difference')
    plt.ylabel('Frequency')

    # Save the figure to a file
    plt.savefig(f'figure/{dataset}_histogram_{i}.png', dpi=300, bbox_inches='tight')
    
    # Close the current figure to free up memory
    plt.close()