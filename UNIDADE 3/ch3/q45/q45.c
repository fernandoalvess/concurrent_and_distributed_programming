/* File:     q45.c
 *
 * Compile:  mpicc -g -Wall -o q45 q45.c
 * Usage:    mpiexec -n<number of processes> ./q45
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Check_for_error(int local_ok, char fname[], char message[],MPI_Comm comm);
void Read_n(int* n_p, int my_rank, int comm_sz, MPI_Comm comm);
void Print_matrix( double *matrix, int n);
void Print_matrix_up( double *matrix, int n);

int main(void) {
      int n, comm_sz, my_rank, *blocklengths, *displacements;
      double *matrix, *matrix_tri_up;
      MPI_Comm comm;

      MPI_Init(NULL, NULL);
      comm = MPI_COMM_WORLD;
      MPI_Comm_size(comm, &comm_sz);
      MPI_Comm_rank(comm, &my_rank);

      Read_n(&n, my_rank, comm_sz, comm);
      blocklengths = malloc(n * sizeof(double));
      displacements = malloc(n * sizeof(double));
      for (int i = 0; i < n; i++) {
            blocklengths[i] = n - i;
            displacements[i] = i * n + i;
      }

      MPI_Datatype upper_triangle_type;
      MPI_Type_indexed(n, blocklengths, displacements, MPI_DOUBLE, &upper_triangle_type);
      MPI_Type_commit(&upper_triangle_type);


      if (my_rank == 0) {
            matrix = malloc(n * n * sizeof(double));
            
            for (int i = 0; i < n; i++) {
                  for (int j = 0; j < n; j++) {
                  matrix[i * n + j] = rand()%100;
                  }
            }

            printf("MATRIZ:\n");
            Print_matrix(matrix, n);

            MPI_Send(matrix, 1, upper_triangle_type, 1, 0, MPI_COMM_WORLD);

      } else if (my_rank == 1) {
            int num_elements = n * (n + 1) / 2;
            matrix_tri_up = malloc(num_elements * sizeof(double));
            
            MPI_Recv(matrix_tri_up, num_elements, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, NULL);

            printf("\nMATRIZ TRIANGULAR:\n");
            
            Print_matrix_up(matrix_tri_up, n);
            
            free(matrix_tri_up);
      }

      if (matrix){ 
            free(matrix);
      }
      free(blocklengths);
      free(displacements);
      MPI_Type_free(&upper_triangle_type);
      MPI_Finalize();
      return 0;
}

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
      int       my_rank    /* in  */, 
      int       comm_sz    /* in  */,
      MPI_Comm  comm       /* in  */) {
      int local_ok = 1;
      char *fname = "Read_n";
      
      if (my_rank == 0) {
            printf("Qual a ordem da Matriz?\n");
            scanf("%d", n_p);
      }
      MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
      if (*n_p <= 0 || *n_p % comm_sz != 0) local_ok = 0;
      Check_for_error(local_ok, fname,
            "n should be > 0 and evenly divisible by comm_sz", comm);
}  /* Read_n */

void Print_matrix(
      double *matrix   /*  in  */, 
      int n            /*  in  */) {
      for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++){
                  printf("%.2f\t", matrix[i * n + j]);
            }
            printf("\n");
      }
} /* Print_matrix */

void Print_matrix_up(
      double *matrix_up   /*  in  */, 
      int n            /*  in  */) {
      
      int k = 0;
      for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                  if (j < i){
                        printf("-\t");
                  } else {
                        printf("%.2f\t", matrix_up[k++]);
                        }
                  }
            printf("\n");
            }
} /* Print_matrix_up */