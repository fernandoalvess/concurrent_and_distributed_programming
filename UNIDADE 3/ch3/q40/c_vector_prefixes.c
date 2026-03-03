/* File:     vector_prefixes.c
 *
 * Purpose:  Implement vector prefixes
 *
 * Compile:  mpicc -g -Wall -o c_vector_prefixes c_vector_prefixes.c -lm
 * Usage:    mpiexec -n<number of processes> ./c_vector_prefixes
 */
#include <stdio.h>
#include <mpi.h> 
#include <stdlib.h>
#include <math.h>

void Check_for_error(int local_ok, char fname[], char message[], MPI_Comm comm);
void Read_n(int* k_p, int* n_p, int my_rank, int comm_sz, MPI_Comm comm);
void Allocate_vector(double** x_pp, int n, MPI_Comm comm);
void Read_vector(double a[], int n, char vec_name[], int my_rank, MPI_Comm comm);
void Element_prefixes(double  d[], double  c[], int n);

int main(void) {
   int k, n, comm_sz, my_rank, i, start, duo_i, factor_one = 1 , factor_two = 2, location, displacement;
   double *vector_x, *vector_result;

   MPI_Init(NULL, NULL); 
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

   Read_n(&k, &n, my_rank, comm_sz, MPI_COMM_WORLD);
   Allocate_vector(&vector_x, n, MPI_COMM_WORLD);
   if(my_rank==0){
      printf("%d ELEMENTOS\n", n);
   }
   Read_vector(vector_x, n, "VETOR X", my_rank, MPI_COMM_WORLD);
   displacement = n / comm_sz;
   Allocate_vector(&vector_result, n, MPI_COMM_WORLD);
   
   start = my_rank*displacement;
   for(i = my_rank*displacement; i<(start+displacement); i++){
      Element_prefixes(vector_result, vector_x, i);
   }
   
   while (factor_one < comm_sz)
   {
      if((my_rank % factor_one) == 0){
         if((my_rank % factor_two) == 0){
            duo_i = my_rank + factor_one;
            location = start+displacement;
            MPI_Recv(&vector_result[location], displacement, MPI_DOUBLE, duo_i, 0, MPI_COMM_WORLD, NULL);
         }else{
            duo_i = my_rank - factor_one;
            MPI_Send(&vector_result[start], displacement, MPI_DOUBLE, duo_i, 0, MPI_COMM_WORLD);
         }
      }
      factor_one *= 2;
      factor_two *= 2;
      displacement *= 2;
   }
   if(my_rank == 0){
      printf("RESULTADO: ");
      for(i = 0; i < n ; i++){
         printf("%.2f ",vector_result[i]);
      }
      printf("\n");
   }

   free(vector_result);
   free(vector_x);
   MPI_Finalize();
   return 0;
}  /* main */

void Check_for_error(
      int       local_ok   /* in */, 
      char      fname[]    /* in */,
      char      message[]  /* in */, 
      MPI_Comm  comm       /* in */) {
   int ok;

   MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
   if (ok == 0) {
      int my_rank;
      MPI_Comm_rank(comm, &my_rank);
      if (my_rank == 0) {
         fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, 
               message);
         fflush(stderr);
      }
      MPI_Finalize();
      exit(-1);
   }
}  /* Check_for_error */

void Read_n(
      int*      k_p        /* out */,
      int*      n_p        /* out */,
      int       my_rank    /* in  */, 
      int       comm_sz    /* in  */,
      MPI_Comm  comm       /* in  */) {
   int local_ok = 1;
   char *fname = "Read_n";
   
   if (my_rank == 0) {
      printf("Informe o valor de k?\n");
      scanf("%d", k_p);
      *n_p = pow(2, *k_p);
   }
   MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
   if (*n_p <= 0 || *n_p % comm_sz != 0) local_ok = 0;
   Check_for_error(local_ok, fname, "n should be > 0 and evenly divisible by comm_sz", comm);
}  /* Read_n */


void Allocate_vector(
      double**   x_pp  /* out */, 
      int        n     /* in  */,
      MPI_Comm   comm  /* in  */) {
   int local_ok = 1;
   char* fname = "Allocate_vector";

   *x_pp = malloc(n*sizeof(double));

   if (*x_pp == NULL) local_ok = 0;
   Check_for_error(local_ok, fname, "Can't allocate local vector(s)", comm);
}  /* Allocate_vectors */


void Read_vector(
      double    a[]         /* out */, 
      int       n           /* in  */,
      char      vec_name[]  /* in  */,
      int       my_rank     /* in  */, 
      MPI_Comm  comm        /* in  */) {

   int i;
   int local_ok = 1;
   char* fname = "Read_vector";
   
   if (a == NULL) local_ok = 0;
   Check_for_error(local_ok, fname, "Vector not allocated", comm);

   if (my_rank == 0) {
      printf("Informe os valores do %s\n", vec_name);
      for (i = 0; i < n; i++){
         scanf("%lf", &a[i]);
      }
   } 
   MPI_Bcast(a, n, MPI_DOUBLE, 0, comm);
}  /* Read_vector */ 

void Element_prefixes(
      double  d[]     /* out */,
      double  c[]     /* in */,
      int     n       /* in */) {
   d[n] = 0;
   for(int i = 0; i <= n; i++){
      d[n] += c[i];
   }
}