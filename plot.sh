#!/bin/bash

ROOT="rack"

find "$ROOT" -type f -name "cwnd.plotme" -o -name "rtt.plotme" | while read plotfile; do
    dir=$(dirname "$plotfile")  
    base=$(basename "$plotfile" .plotme) 
    output="${dir}/${base}.png" 

    echo "Plotting $plotfile -> $output"

    if [ "$base" == "rtt" ]; then
        gnuplot <<EOF
set terminal pngcairo size 1200,600 enhanced font 'Verdana,10'
set output '${output}'
set title 'TCP RTT'
set xlabel 'Time (s)'
set ylabel 'RTT (s)'
set grid
plot '${plotfile}' using 1:2 with lines lw 2 lc rgb "blue" title 'RTT'
EOF
    else
        gnuplot <<EOF
set terminal pngcairo size 1200,600 enhanced font 'Verdana,10'
set output '${output}'
set title 'TCP Congestion Window'
set xlabel 'Time (s)'
set ylabel 'CWND (segments)'
set grid
plot '${plotfile}' using 1:2 with lines lw 2 lc rgb "blue" title 'CWND'
EOF
    fi

done

echo "✅ All plots generated successfully."
