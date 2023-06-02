#!/bin/bash

# https://stackoverflow.com/questions/54920113/calculate-average-execution-time-of-a-program-using-bash
avg_time() {
    #
    # usage: avg_time n command ...
    #
    n=$1; shift
    (($# > 0)) || return                   # bail if no command given
    for ((i = 0; i < n; i++)); do
        { time -p "$@" &>/dev/null; } 2>&1 # ignore the output of the command
                                           # but collect time's output in stdout
    done | awk '
        /real/ { real = real + $2; nr++ }
        END    {
                 if (nr>0) printf("real %f\n", real/nr);
               }'
}

# for i in {0..150000..10000}
# do
#   time ../../build/benchmarks/cryptonets/cryptonets $i >> a.out
# done

repeat=10


# noopt
../../build/benchmarks/cryptonets/cryptonets 1 0 0 0 0 0 0 0
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 0 0 0
# cf
../../build/benchmarks/cryptonets/cryptonets 1 0 0 0 0 0 0 1
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 0 0 1
# cse
../../build/benchmarks/cryptonets/cryptonets 1 0 0 0 0 1 1 0
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 1 1 0
# cse + cf
../../build/benchmarks/cryptonets/cryptonets 1 0 0 0 0 1 1 1
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 0 0 0 0 1 1 1


max_iter=500000

# depth
ruleset=0
# bottom_up
rewrite_heuristic=0
# no_cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 0
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 1
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1
# top_down
rewrite_heuristic=1
# no_cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 0
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 1
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1

# ops_cost
ruleset=1
# bottom_up
rewrite_heuristic=0
# no_cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 0
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 1
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1
# top_down
rewrite_heuristic=1
# no_cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 0
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 1
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1

# joined
ruleset=2
# bottom_up
rewrite_heuristic=0
# no_cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 0
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 1
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1
# top_down
rewrite_heuristic=1
# no_cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 0
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 0
# cse
../../build/benchmarks/cryptonets/cryptonets 1 $ruleset $rewrite_heuristic $max_iter 1 1
avg_time $repeat ../../build/benchmarks/cryptonets/cryptonets 0 $ruleset $rewrite_heuristic $max_iter 1 1
