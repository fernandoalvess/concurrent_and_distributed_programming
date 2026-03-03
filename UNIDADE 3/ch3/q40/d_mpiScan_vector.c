/**
 * Arquivo:  d_mpiScan_vector.c
 *
 * Propósito: Demonstrar o uso de MPI_Scan para calcular somas de prefixos
 * em vetores de números aleatórios gerados por cada processo.
 *
 * Compilar:  mpicc -g -Wall -o d_mpiScan_vector d_mpiScan_vector.c
 * Executar:  mpiexec -n <numero_de_processos> ./d_mpiScan_vector
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void print_vector(const char* titulo, int* vetor, int tamanho) {
    printf("%s: [ ", titulo);
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", vetor[i]);
    }
    printf("]\n");
}

int main(int argc, char* argv[]) {
    const int COUNT = 5; //elementos em cada vetor
    int meu_rank, comm_sz;
    int *vetor_local = NULL;
    int *somas_de_prefixo = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_rank);

    vetor_local = malloc(COUNT * sizeof(int));
    somas_de_prefixo = malloc(COUNT * sizeof(int));

    if (vetor_local == NULL || somas_de_prefixo == NULL) {
        fprintf(stderr, "Processo %d: Nao alocou memoria!\n", meu_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    srand(time(NULL) + meu_rank);
    for (int i = 0; i < COUNT; i++) {
        vetor_local[i] = rand() % 10; 
    }
    
    MPI_Scan(vetor_local, somas_de_prefixo, COUNT, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    for (int i = 0; i < comm_sz; i++) {
        if (meu_rank == i) {
            printf("--- Processo %d ---\n", meu_rank);
            print_vector("Vetor gerado", vetor_local, COUNT);
            print_vector("Somas de Prefixo", somas_de_prefixo, COUNT);
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    free(vetor_local);
    free(somas_de_prefixo);

    MPI_Finalize();
    return 0;
}