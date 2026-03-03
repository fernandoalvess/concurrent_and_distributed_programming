#!/bin/bash

for p in 1 2
do
    echo -e "\nRodando com $p processos"
    for i in {1..5}
    do
        mpiexec -n $p ./mpi_trap4_time < entrada.txt
    done
done
