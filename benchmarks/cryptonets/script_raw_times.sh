#!/bin/bash

repeat=30

for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 0 0 0  >> compile_times_noopt.txt
done

for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 0 0 1  >> compile_times_cf.txt
done

for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 1 1 0 >> compile_times_cse.txt
done

for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 1 1 1 >> compile_times_cf_cse.txt
done

max_iter=800000

# depth
ruleset=0
# bottom_up
rewrite_heuristic=0
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> compile_times_depth_bu.txt
done
# top_down
rewrite_heuristic=1
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> compile_times_depth_td.txt
done

# ops_cost
ruleset=1
# bottom_up
rewrite_heuristic=0
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> compile_times_ops_bu.txt
done
# top_down
rewrite_heuristic=1
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> compile_times_ops_td.txt
done

# joined
ruleset=2
# bottom_up
rewrite_heuristic=0
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> compile_times_joined_bu.txt
done
# top_down
rewrite_heuristic=1
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> compile_times_joined_td.txt
done
