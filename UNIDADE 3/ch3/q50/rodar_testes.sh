#!/bin/bash

for p in 1 2
do
    echo -e "\nRodando com $p processos"
    for i in {1..5}
    do
        mpiexec -n $p ./q50 g 100000000
    done
done
