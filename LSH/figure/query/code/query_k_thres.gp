# Set the terminal to PNG and output file
set terminal postscript eps color enhanced "Helvetica" 25
set xlabel "Similarity Threshold {/Symbol q}" offset 0 font "Helvetica, 25"
set ylabel "Elapsed Time (s)" offset 1 font "Helvetica, 25"
set key font ",25"

set terminal png


# Set style for histogram
set style data histogram
set style histogram cluster gap 1
set style fill pattern

# Change the location of the legend
set key left top

set xtics ("1.0" 1, "0.9" 2, "0.8" 3)
set xrange[-0.5:2.5] 
set yrange[0:1.8]

set xtics font "Helvetica,18"
set ytics font "Helvetica,18"
set ytics 0.6

# Plot the data
set output 'query_k_thres.png'
plot './data/query_k_thres.txt' using 2:xtic(1) title 'k=16' lt rgb "purple" fs pattern 1, '' using 3 title 'k=32' lt rgb "forest-green" fs pattern 2, '' using 4 title 'k=64' lt rgb "medium-blue" fs pattern 37
