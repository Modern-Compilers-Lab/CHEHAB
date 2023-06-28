#!/bin/bash

repeat=30

for ((i = 0; i < repeat; i++))
do
    ../../build/benchmarks/hamming_distance_porcupine/hamming_distance_porcupine 0 1  >> compile_times_ops.txt
done
