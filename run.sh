#!/bin/bash

# Compile the programs
gcc -o serial serial.c -lpthread -lm
gcc -o mutex mutex.c -lpthread -lm
gcc -o readwrite readwrite.c -lpthread -lm

# Define test cases
cases=(
    "1 1000 10000 0.99 0.005 0.005"
    "2 1000 10000 0.90 0.05 0.05"
    "3 1000 10000 0.50 0.25 0.25"
)

# Loop through cases
for case in "${cases[@]}"; do
    echo "" | tee -a results.txt

    # Print the case
    echo "------------------------------------------------" | tee -a results.txt
    echo "| Running case: $case  |" | tee -a results.txt
    echo "------------------------------------------------" | tee -a results.txt

    echo "" | tee -a results.txt

    # Print table header
    printf "%-15s | %-10s | %-15s | %-10s | %-10s\n" "Implementation" "Threads" "Mean Time (s)" "Std Dev" "Samples" | tee -a results.txt
    printf "%-15s-+------------+-----------------+-------------+-------------\n" "--------------" | tee -a results.txt

    # Parse the case
    IFS=' ' read -r m_member n m m_member_fraction m_delete_fraction m_insert_fraction <<< "$case"

    sleep 0.2  # Sleep for a bit to allow the CPU to cool down otherwise serial gives -nan
    
    # Run the Serial Linked List
    output=$(./serial 1 "$n" "$m" "$m_member_fraction" "$m_delete_fraction" "$m_insert_fraction")
    mean=$(echo "$output" | grep "Mean execution time:" | awk '{print $4}')
    std=$(echo "$output" | grep "std:" | awk '{print $6}')
    samples=$(echo "$output" | grep "samples:" | awk '{print $8}')
    printf "%-15s | %-10s | %-15s | %-10s | %-10s\n" "Serial" "1" "$mean" "$std" "$samples" | tee -a results.txt
    printf "%-15s-+------------+-----------------+-------------+-------------\n" "--------------" | tee -a results.txt

    # Run the Mutex Linked List
    for threads in 1 2 4 8; do
        output=$(./mutex "$threads" "$n" "$m" "$m_member_fraction" "$m_delete_fraction" "$m_insert_fraction")
        mean=$(echo "$output" | grep "Mean execution time:" | awk '{print $4}')
        std=$(echo "$output" | grep "std:" | awk '{print $6}')
        samples=$(echo "$output" | grep "samples:" | awk '{print $8}')
        printf "%-15s | %-10s | %-15s | %-10s | %-10s\n" "Mutex" "$threads" "$mean" "$std" "$samples" | tee -a results.txt
    done
    printf "%-15s-+------------+-----------------+-------------+-------------\n" "--------------" | tee -a results.txt

    # Run the RW Lock Linked List
    for threads in 1 2 4 8; do
        output=$(./readwrite "$threads" "$n" "$m" "$m_member_fraction" "$m_delete_fraction" "$m_insert_fraction")
        mean=$(echo "$output" | grep "Mean execution time:" | awk '{print $4}')
        std=$(echo "$output" | grep "std:" | awk '{print $6}')
        samples=$(echo "$output" | grep "samples:" | awk '{print $8}')
        printf "%-15s | %-10s | %-15s | %-10s | %-10s\n" "RW Lock" "$threads" "$mean" "$std" "$samples" | tee -a results.txt
    done
done
