#!/bin/bash
testdir=/mnt/nvme/ted/kvell/scratch0
k=23
v=128
nitems=100000000
nops=1000000000
#nitems=10000000
#nops=100000000
step=$nitems
crash=()
for ((i=$nitems;i<=$nops;i+=$step)); do
    crash+=($i)
done
for w in a-uniform a-zipfian; do
for c in "${crash[@]}"; do
    rm -rf "$testdir"
    mkdir -p "$testdir"
    file="kvell-recovery-k${k}v${v}n${nitems}m${nops}c${c}-${w}.log"
    echo "=== Crashing Run ===" > "$file"
    ./main 1 1 -k "$k" -v "$v" -n "$nitems" -m "$nops" -c "$c" -w "$w" >> "$file"
    /usr/bin/du -s "$testdir" >> "$file"
    echo "=== Recovery Run ===" >> "$file"
    ./main 1 1 -k "$k" -v "$v" -n 0 -m 0 -c 0 -w "$w" >> "$file"
done
done
