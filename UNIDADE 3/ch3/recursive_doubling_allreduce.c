/**
 * Arquivo:   recursive_doubling_allreduce.c
 *
 * Propósito: Implementa o `allreduce` usando o algoritmo otimizado de
 * duplicação recursiva (borboleta), como em bibliotecas MPI reais.
 *
 * NOTA:      Este código assume que o número de processos é uma potência de 2.
 *
 * Compilar:  mpicc -g -Wall -o recursive_doubling_allreduce recursive_doubling_allreduce.c -lm
 *
 * Executar:  mpirun -np <num_procs> ./recursive_doubling_allreduce
 *
 * Exemplo:   mpirun -np 8 ./recursive_doubling_allreduce
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h> // Para log2f

// Função para verificar se um número é potência de 2
int is_power_of_two(int x) {
    return (x > 0) && ((x & (x - 1)) == 0);
}

int main(int argc, char* argv[]) {
    int my_rank, comm_sz;
    int my_val;
    int sum;
    int received_val; // Buffer para receber o valor do parceiro

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (!is_power_of_two(comm_sz)) {
        if (my_rank == 0) {
            fprintf(stderr, "ERRO: O número de processos (%d) não é uma potência de 2. Este algoritmo requer uma potência de 2.\n", comm_sz);
        }
        MPI_Finalize();
        return 1;
    }

    // Valores iniciais do exemplo da imagem
    int initial_values[] = {5, 2, -1, -3, 6, 5, -7, 2};
    if (comm_sz == 8) {
        my_val = initial_values[my_rank];
    } else {
        // Valor padrão se não estiver usando 8 processos
        my_val = my_rank + 1;
    }

    // A soma inicial de cada processo é seu próprio valor.
    sum = my_val;

    // --- Lógica Principal: Duplicação Recursiva ---
    // O número de estágios é log2(comm_sz)
    int num_stages = log2f(comm_sz);
    for (int i = 0; i < num_stages; i++) {
        // A distância entre parceiros dobra a cada estágio
        int distance = 1 << i; // 2^i = 1, 2, 4, ...
        
        // Calcula o parceiro usando a operação XOR
        int partner = my_rank ^ distance;

        // Cada processo envia sua soma parcial atual e recebe a soma parcial do seu parceiro.
        // MPI_Sendrecv é usado para evitar deadlocks.
        MPI_Sendrecv(&sum, 1, MPI_INT, partner, 0,
                     &received_val, 1, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Atualiza a soma local com o valor recebido do parceiro.
        sum += received_val;
    }

    // Ao final do loop, cada processo tem a soma total final.
    printf("Processo %d > Valor inicial: %d | Soma final (Allreduce Borboleta): %d\n", my_rank, my_val, sum);

    MPI_Finalize();
    return 0;
}