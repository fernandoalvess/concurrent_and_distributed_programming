/* File:    omp_trap1.c
 * Purpose: Estimate definite integral (or area under curve) using trapezoidal 
 *          rule.
 *
 * Input:   a, b, n
 * Output:  estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: gcc -g -Wall -fopenmp -o q26 q26.c -lm
 * Usage:   ./q26 <number of threads>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void Usage(char* prog_name);

int main(int argc, char* argv[]) {
   int n;
   int thread_count;
   double time_result;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   printf("Enter number interations\n");
   scanf("%d", &n);
   if (n % thread_count != 0) Usage(argv[0]);


#  pragma omp parallel num_threads(thread_count) default (none) shared (time_result, n)
   {
      double start_time, finish_time;
    int i;
    double minha_soma = 0.0;

    start_time = omp_get_wtime();
    for (i = 1; i <= n; i ++) {
#       pragma omp atomic
        minha_soma += sin(i);
    }
    finish_time = omp_get_wtime();

    int thead_number = omp_get_thread_num();
    printf("Thread %d - Soma: %.2f\n", thead_number, minha_soma);
    printf("Thread %d Time: %f\n", thead_number, (finish_time - start_time));

   if (time_result < (finish_time - start_time)) {
         time_result = (finish_time - start_time);
   }
   }

   printf ("Tempo: %f\n", time_result);
   return 0;
}  /* main */

/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   fprintf(stderr, "   number of interations must be evenly divisible by\n");
   fprintf(stderr, "   number of threads\n");
   exit(0);
}  /* Usage */
