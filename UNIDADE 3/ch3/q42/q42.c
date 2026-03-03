/* File:     q42.c
 *
 * Compile:  mpicc -g -Wall -o q42 q42.c -lm
 * Usage:    mpiexec -n<number of processes> ./q42
 *
 */

#include <stdio.h>
#include <mpi.h> 
#include <stdlib.h>
#include <math.h>

void Check_for_error(int local_ok, char fname[], char message[], MPI_Comm comm);
void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, MPI_Comm comm);
void Allocate_vector(double** local_x_pp, int local_n, MPI_Comm comm);
void Read_vector(double local_a[], int sendcounts[], int display[], int local_n, int n, char vec_name[], int my_rank, MPI_Comm comm);
void Print_vector(double local_b[], int sendcounts[], int display[], int local_n, int n, char title[], int my_rank, MPI_Comm comm);
void Set_scale(int* scale, int my_rank, MPI_Comm  comm);

int main(void) {
   int my_rank, comm_sz, n_vector, local_n_vector, scale, *vector_send, *vector_display;
   double *local_vector_1, *local_vector_2;
   double sum = 0, normalization=0;

   MPI_Init(NULL, NULL); 
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

   Read_n(&n_vector, &local_n_vector, my_rank, comm_sz, MPI_COMM_WORLD);

   Allocate_vector(&local_vector_1, local_n_vector, MPI_COMM_WORLD);

   if (my_rank == 0) {
       vector_send = malloc(comm_sz * sizeof(int));
       vector_display = malloc(comm_sz * sizeof(int));
   }

   MPI_Gather(&local_n_vector, 1, MPI_INT, vector_send, 1, MPI_INT, 0, MPI_COMM_WORLD);

   if (my_rank == 0){
      vector_display[0] = 0;
      for(int i = 0; i < comm_sz-1; i++){
         vector_display[i+1] = vector_display[i] + vector_send[i];
      }
   }

   Read_vector(local_vector_1, vector_send, vector_display, local_n_vector, n_vector, "X", my_rank, MPI_COMM_WORLD);

   Print_vector(local_vector_1, vector_send, vector_display, local_n_vector, n_vector, "X", my_rank, MPI_COMM_WORLD);

   Set_scale(&scale, my_rank, MPI_COMM_WORLD);

   for(int i = 0; i < local_n_vector; i++){
      local_vector_1[i] = local_vector_1[i]*scale;
   }

   Print_vector(local_vector_1, vector_send, vector_display, local_n_vector, n_vector, "Vetor X multiplicado pelo escalar", my_rank, MPI_COMM_WORLD);

   Allocate_vector(&local_vector_2, local_n_vector, MPI_COMM_WORLD);

   Read_vector(local_vector_2, vector_send, vector_display, local_n_vector, n_vector, "Y", my_rank, MPI_COMM_WORLD);

   Print_vector(local_vector_2, vector_send, vector_display, local_n_vector, n_vector, "Y", my_rank, MPI_COMM_WORLD);

   for(int i = 0; i < local_n_vector; i++){
      local_vector_2[i] = local_vector_2[i]*local_vector_2[i];
      sum += local_vector_2[i];
   }

   MPI_Reduce(&sum, &normalization, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

   if(my_rank == 0){
      normalization = sqrt(normalization);
      printf("Norma do vetor Y: %.2lf\n", normalization); 
   }
   
   free(local_vector_1);
   free(local_vector_2);
   if(my_rank==0){
      free(vector_send);
   }
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
      int*      n_p        /* out */, 
      int*      local_n_p  /* out */, 
      int       my_rank    /* in  */, 
      int       comm_sz    /* in  */,
      MPI_Comm  comm       /* in  */) {
   int local_ok = 1, resto;
   char *fname = "Read_n";
   
   if (my_rank == 0) {
      printf("Quantos elementos no vetor?\n");
      scanf("%d", n_p);
   }
   MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
   if (*n_p <= 0) local_ok = 0;
   Check_for_error(local_ok, fname, "n should be > 0 and evenly divisible by comm_sz", comm);
   resto = *n_p % comm_sz;
   if(resto == 0){
      *local_n_p = *n_p/comm_sz;
   }else{
      if(my_rank<resto){
         *local_n_p = (*n_p/comm_sz) +1;
      }else{
         *local_n_p = *n_p/comm_sz;
      }
   }
}  /* Read_n */

void Allocate_vector(
      double**   local_x_pp  /* out */, 
      int        local_n     /* in  */,
      MPI_Comm   comm        /* in  */) {
   int local_ok = 1;
   char* fname = "Allocate_vector";

   *local_x_pp = malloc(local_n*sizeof(double));

   if (*local_x_pp == NULL) local_ok = 0;
   Check_for_error(local_ok, fname, "Can't allocate local vector(s)", comm);
}  /* Allocate_vectors */

void Read_vector(
      double    local_a[]   /* out */, 
      int       sendcounts[] ,
      int       display[],
      int       local_n     /* in  */, 
      int       n           /* in  */,
      char      vec_name[]  /* in  */,
      int       my_rank     /* in  */, 
      MPI_Comm  comm        /* in  */) {

   double* a = NULL;
   int i;
   int local_ok = 1;
   char* fname = "Read_vector";

   if (my_rank == 0) {
      a = malloc(n*sizeof(double));
      if (a == NULL) local_ok = 0;
      Check_for_error(local_ok, fname, "Can't allocate temporary vector", comm);
      printf("Informe o vetor %s\n", vec_name);
      for (i = 0; i < n; i++){
         scanf("%lf", &a[i]);
      }
      MPI_Scatterv(a, sendcounts, display, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0, comm);
      free(a);
   } else {
      Check_for_error(local_ok, fname, "Can't allocate temporary vector", 
            comm);
      MPI_Scatterv(a, sendcounts, display, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0, comm);
   }
}  /* Read_vector */ 

void Print_vector(
      double    local_b[]  /* in */,
      int       recvcounts[]/* in */,
      int       displs[]   /* in */, 
      int       local_n    /* in */, 
      int       n          /* in */, 
      char      title[]    /* in */, 
      int       my_rank    /* in */,
      MPI_Comm comm) {

   double* b = NULL;
   int i;
   int local_ok = 1;
   char* fname = "Print_vector";

   if (my_rank == 0) {
      b = malloc(n*sizeof(double));
      if (b == NULL) local_ok = 0;
      Check_for_error(local_ok, fname, "Can't allocate temporary vector", 
            comm);
      MPI_Gatherv(local_b, local_n, MPI_DOUBLE, b, recvcounts, displs, MPI_DOUBLE, 0, comm);
      printf("%s: ", title);
      for (i = 0; i < n; i++)
         printf("%.2f ", b[i]);
      printf("\n");
      free(b);
   } else {
      Check_for_error(local_ok, fname, "Can't allocate temporary vector", 
            comm);
      MPI_Gatherv(local_b, local_n, MPI_DOUBLE, b, recvcounts, displs, MPI_DOUBLE, 0, comm);
   }
}  /* Print_vector */

void Set_scale(
   int* scale            /* out */,
   int my_rank          /* in */,
   MPI_Comm  comm       /* in */
){
   if (my_rank == 0) {
      printf("Informe o escalar:\n");
      scanf("%d", scale);
   }
   MPI_Bcast(scale, 1, MPI_INT, 0, comm);
}