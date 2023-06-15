#!/bin/bash

repeat=30

for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 0 0 0  >> results_noopt.txt
done

for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 0 0 1  >> results_cf.txt
done

for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 1 1 0 >> results_cse.txt
done

for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 1 1 1 >> results_cse_cf.txt
done

for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 1 1 1 >> results_cse_cf.txt
done


max_iter=800000

# depth
ruleset=0
# bottom_up
rewrite_heuristic=0
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> results_depth_bu.txt
done
# top_down
rewrite_heuristic=1
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> results_depth_td.txt
done

# ops_cost
ruleset=1
# bottom_up
rewrite_heuristic=0
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> results_ops_bu.txt
done
# top_down
rewrite_heuristic=1
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> results_ops_td.txt
done

# joined
ruleset=2
# bottom_up
rewrite_heuristic=0
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> results_joined_bu.txt
done
# top_down
rewrite_heuristic=1
for ((i = 0; i < $repeat; i++))
do
../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1 >> results_joined_td.txt
done
