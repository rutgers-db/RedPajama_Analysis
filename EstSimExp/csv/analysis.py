import pandas as pd
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-dataset", default="book")
parser.add_argument("-K", default=128)
args = parser.parse_args()

csv_path = "/research/projects/zp128/RedPajama_Analysis/EstSimExp/csv/%s_%s.csv" %(args.dataset, args.K) 
# Load the CSV file into a pandas DataFrame
df = pd.read_csv(csv_path)

# Compute the mean and variance for each column
means = df.mean()
variances = df.var()

# Format and print the results
mean_values = [f"{value:.3f}" for value in means]
variance_values = [f"{value:.3f}" for value in variances]
print("- %s \n" % (args.dataset))
print("| Statistical value | True Jaccard | Fixed bottom k + BottomK formula | Fixed bottom k + Jaccard formula| Adaptive bottom k + BottomK formula|Adaptive bottom k + Jaccard formula|")
print("|----------|----------|----------|----------|----------|----------|")
print("| Mean    |", " | ".join(mean_values), "|")
print("| Variance|", " | ".join(variance_values), "|")
print("")