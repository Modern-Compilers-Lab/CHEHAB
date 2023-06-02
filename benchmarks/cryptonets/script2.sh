#!/bin/bash


# noopt
../../build/benchmarks/cryptonets/cryptonets_checks 1 0 0 0 0 0 0 0
# cf
../../build/benchmarks/cryptonets/cryptonets_checks 1 0 0 0 0 0 0 1
# cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 0 0 0 0 1 1 0
# cse + cf
../../build/benchmarks/cryptonets/cryptonets_checks 1 0 0 0 0 1 1 1


max_iter=500000

# depth
ruleset=0
# bottom_up
rewrite_heuristic=0
# no_cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 1
# top_down
rewrite_heuristic=1
# no_cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 1

# ops_cost
ruleset=1
# bottom_up
rewrite_heuristic=0
# no_cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 1
# top_down
rewrite_heuristic=1
# no_cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 1

# joined
ruleset=2
# bottom_up
rewrite_heuristic=0
# no_cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 1
# top_down
rewrite_heuristic=1
# no_cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets_checks 1 $ruleset $rewrite_heuristic $max_iter 1 1
