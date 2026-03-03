/* File:    omp_trap2b.c
 
 * Compile: gcc -g -Wall -fopenmp -o q23 q23.c
 * Usage:   ./q23 <number of threads> <number of interactions>\
 *
 
 */

#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void Usage(char* prog_name);

int main(int argc, char* argv[]){
    int number_of_interactions, number_of_threads;
    int i, my_rank;

    if (argc != 3) Usage(argv[0]);
    number_of_threads = strtol(argv[1], NULL, 10);
    number_of_interactions = strtol(argv[2], NULL, 10);

#   pragma omp parallel num_threads(number_of_threads) \
    default(none) private(i, my_rank) shared(number_of_interactions)
    {
#   pragma omp for
    for(i = 0; i < number_of_interactions; i++){
        my_rank = omp_get_thread_num();
        printf("Thread: %d, interaction: %d\n", my_rank, i);
    }
    }
    return 0;
}

void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <number of interactions>\n", prog_name);
   fprintf(stderr, "   number of threads must be set\n");
   fprintf(stderr, "   number of interactions must be set\n");
   exit(0);
}  /* Usage */
