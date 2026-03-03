/* File:     mpi_vector_prefixes.c
 *
 *
 * Compile:  mpicc -g -Wall -o b_vector_pararel b_vector_pararel.c
 * Run:      mpiexec -n <n> ./b_vector_pararel
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Read_n(int* n_p, int my_rank);
void Allocate_vector(double** x_pp, int n);
void Read_vector(double a[], int n, char vec_name[]);
void Print_vector(double b[], int n, char title[]);

int main(int argc, char* argv[]) {
   int n;
   int comm_sz, my_rank;
   double *vector_x = NULL, *vector_result = NULL;
   double local_x;
   double soma_prefixo;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   if (my_rank == 0) {
      Read_n(&n, my_rank);
      if (n != comm_sz) {
         fprintf(stderr, "Erro: O numero de processos (%d) deve ser igual à total de elementos (%d).\n", comm_sz, n);
         MPI_Abort(MPI_COMM_WORLD, 1);
      }
      Allocate_vector(&vector_x, n);
      Read_vector(vector_x, n, "X");
      printf("\n");
      Print_vector(vector_x, n, "VETOR X:");
   }

   // O processo 0 envia um elemento de "X" para cada processo.
   // Cada processo recebe seu elemento em 'local_x'.
   MPI_Scatter(vector_x, 1, MPI_DOUBLE, &local_x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   
   soma_prefixo = local_x;

   // Se eu não sou o 0, preciso receber a soma do processo anterior.
   if (my_rank > 0) {
      double receber_soma;
      MPI_Recv(&receber_soma, 1, MPI_DOUBLE, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      soma_prefixo += receber_soma;
   }

   // Se eu não sou o último, preciso enviar minha soma de prefixo para o próximo processo.
   if (my_rank < comm_sz - 1) {
      MPI_Send(&soma_prefixo, 1, MPI_DOUBLE, my_rank + 1, 0, MPI_COMM_WORLD);
   }

   // Junta todas as somas de prefixos de cada processo no 'vector_result' de 0.
   if (my_rank == 0) {
      Allocate_vector(&vector_result, n);
   }
   MPI_Gather(&soma_prefixo, 1, MPI_DOUBLE, vector_result, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

   if (my_rank == 0) {
      Print_vector(vector_result, n, "RESULTADO:");
      printf("\n");
      free(vector_x);
      free(vector_result);
   }

   MPI_Finalize();
   return 0;
}  /* main */

void Read_n(int* n_p /* out */, int my_rank /* in */) {
   if (my_rank == 0) {
      printf("Quantos elementos no vetor?\n");
      scanf("%d", n_p);
      if (*n_p <= 0) {
         fprintf(stderr, "Order should be positive\n");
         MPI_Abort(MPI_COMM_WORLD, 1);
      }
   }
}  /* Read_n */

void Allocate_vector(
      double** x_pp  /* out */, 
      int       n     /* in  */) {
   *x_pp = malloc(n*sizeof(double));
   if (*x_pp == NULL) {
      fprintf(stderr, "Can't allocate vectors\n");
      exit(-1);
   }
}  /* Allocate_vectors */

void Read_vector(
      double  a[]         /* out */, 
      int     n           /* in  */, 
      char    vec_name[]  /* in  */) {
   int i;
   printf("Informe os valores de %s\n", vec_name);
   for (i = 0; i < n; i++)
      scanf("%lf", &a[i]);
}  /* Read_vector */  

void Print_vector(
      double  b[]     /* in */, 
      int     n       /* in */, 
      char    title[] /* in */) {
   int i;
   printf("%s\n", title);
   for (i = 0; i < n; i++)
      printf("%.2f ", b[i]);
   printf("\n");
}  /* Print_vector */