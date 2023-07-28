# Set the terminal to PNG and output file
set terminal postscript eps color enhanced "Helvetica" 25
set key top left width 0
set xlabel "# of Texts" offset 0 font "Helvetica, 25"
set xtics ("1M" 1, "2M" 2, "4M" 3, "8M" 4)
set xrange[0.5:4.5] 
set terminal png

set key font ",25"

# Set the xtics to display your categories
set xtics ("1M" 1, "2M" 2, "4M" 3, "8M" 4)
set ylabel "Elapsed Time (s)" offset 1 font "Helvetica, 25"
set xtics font "Helvetica,18"
set ytics font "Helvetica,18"

# set yrange[0:25] 
set ytics 5
# Plot the data
set output 'time.png'
plot  "./data/time.txt"   u ($1-0.25):($2+90.8):(0.25) w boxes lt rgb "purple" fs pattern 2  t "t=25" , \
      "./data/time.txt"   u ($1+0):($3+90.8):(0.25) w boxes lt rgb "forest-green" fs pattern 4 t "t=50" , \
      "./data/time.txt"  u ($1+0.25):($4+90.8):(0.25) w boxes lt rgb "medium-blue" fs pattern 5  t "t=100"
