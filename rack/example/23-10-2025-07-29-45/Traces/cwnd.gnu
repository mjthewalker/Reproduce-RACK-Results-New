# ===============================
# Plot .plotme data using gnuplot
# ===============================

set terminal pngcairo size 1200,600 enhanced font 'Verdana,10'
set output 'cwnd.png'

set title 'TCP Congestion Window'
set xlabel 'Time (s)'
set ylabel 'CWND (segments)'
set grid

# Plot column 1 vs column 2 with line + points
plot 'cwnd.plotme' using 1:2 with lines lw 2 lc rgb "blue" title 'CWND'
