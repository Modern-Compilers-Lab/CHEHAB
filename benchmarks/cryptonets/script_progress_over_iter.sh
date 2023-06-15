#!/bin/bash

for i in {0..340000..10000}
do
  ../../build/benchmarks/cryptonets/cryptonets 1 2 1 $i 1 1
done

# for i in {340100..346100..100}
# do
#   ../../build/benchmarks/cryptonets/cryptonets 1 2 0 $i 1 1
# done
