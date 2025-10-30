#!/bin/bash

# This script automates the process of generating CDF graphs for RTT data
# from RACK ON and RACK OFF simulation scenarios.

# Exit immediately if a command exits with a non-zero status.
set -e

# 1. PREPARATION
# ==============================================================================
echo "--- Starting RTT Analysis ---"

# Check if gnuplot is installed
if ! command -v gnuplot &> /dev/null
then
    echo "Error: gnuplot is not installed. Please install it to proceed."
    exit 1
fi

# Create a directory to store the final graphs
OUTPUT_DIR="cdf_plots"
mkdir -p "$OUTPUT_DIR"
echo "Created output directory: $OUTPUT_DIR"

# 2. DATA PROCESSING AND PLOTTING
# ==============================================================================
# Loop through each of the 5 clients
for i in {1..5}; do
    echo ""
    echo "--- Processing Client $i ---"

    # Define temporary file names for the averaged data
    RACK_ON_AVG_FILE="tmp_rack_on_client_${i}_avg.dat"
    RACK_OFF_AVG_FILE="tmp_rack_off_client_${i}_avg.dat"
    
    # --- Step a & b: Average RTTs for RACK ON and RACK OFF scenarios ---
    
    # Find all relevant files for the current client in both scenarios
    RACK_ON_FILES=(rack_on/rng*/rtt/client-RTT-${i}.plotme)
    RACK_OFF_FILES=(rack_off/rng*/rtt/client-RTT-${i}.plotme)
    
    # Check if files were found
    if [ ${#RACK_ON_FILES[@]} -eq 0 ] || [ ! -f "${RACK_ON_FILES[0]}" ]; then
        echo "Warning: No data files found for Client $i in rack_on. Skipping."
        continue
    fi
    if [ ${#RACK_OFF_FILES[@]} -eq 0 ] || [ ! -f "${RACK_OFF_FILES[0]}" ]; then
        echo "Warning: No data files found for Client $i in rack_off. Skipping."
        continue
    fi
    
    echo "Averaging RTT data for ${#RACK_ON_FILES[@]} 'RACK ON' simulations..."
    # Use 'paste' to combine all files column by column.
    # Then use 'awk' to calculate the average of the RTT columns (2, 4, 6, etc.) for each row.
    paste "${RACK_ON_FILES[@]}" | awk '{
        sum=0;
        # The number of RTT columns is half the total number of columns (NF).
        num_files = NF/2;
        # Loop through only the RTT columns (every second column starting from 2).
        for(col=2; col<=NF; col+=2) {
            sum += $col;
        }
        print sum/num_files;
    }' > "$RACK_ON_AVG_FILE"

    echo "Averaging RTT data for ${#RACK_OFF_FILES[@]} 'RACK OFF' simulations..."
    paste "${RACK_OFF_FILES[@]}" | awk '{
        sum=0;
        num_files = NF/2;
        for(col=2; col<=NF; col+=2) {
            sum += $col;
        }
        print sum/num_files;
    }' > "$RACK_OFF_AVG_FILE"
    
    # --- Step c: Generate CDF graph using gnuplot ---
    
    OUTPUT_PNG="${OUTPUT_DIR}/client_${i}_RTT_CDF.png"
    echo "Generating CDF graph: $OUTPUT_PNG"
    
    # Use a 'here document' to pass plotting commands to gnuplot
    gnuplot <<-EOF
        # Set the output file type and name
        set terminal pngcairo enhanced font 'Verdana,10' size 800,600
        set output '$OUTPUT_PNG'
        
        # Set graph titles and labels
        set title "CDF of Average RTT for Client ${i}"
        set xlabel "Average RTT (ms)"
        set ylabel "Probability"
        
        # Configure the grid and legend
        set grid
        set key top left
        
        # The 'smooth cumulative' option automatically calculates and plots the CDF.
        # It requires sorting the data first, which we do on-the-fly.
        # We calculate the total number of lines (N) to correctly normalize the y-axis.
        
        # Get line counts for normalization
        rack_on_lines = system("wc -l < '$RACK_ON_AVG_FILE'")
        rack_off_lines = system("wc -l < '$RACK_OFF_AVG_FILE'")

        plot '< sort -n $RACK_ON_AVG_FILE' u 1:(1.0/rack_on_lines) smooth cumulative title 'RACK ON' with lines lw 2, \
             '< sort -n $RACK_OFF_AVG_FILE' u 1:(1.0/rack_off_lines) smooth cumulative title 'RACK OFF' with lines lw 2
EOF
done

# 3. CLEANUP
# ==============================================================================
echo ""
echo "--- Cleaning up temporary files ---"
rm -f tmp_*.dat
echo "Done."