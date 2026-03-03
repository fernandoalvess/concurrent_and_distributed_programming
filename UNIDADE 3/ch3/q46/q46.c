/* File:     q46.c
 * Purpose:  Use MPI to implement a parallel version of the trapezoidal 
 *           rule.  This version accepts input of the endpoints of the
 *           interval and the number of trapezoids.
 *
 * Input:    The endpoints of the interval of integration and the number
 *           of trapezoids
 * Output:   Estimate of the integral from a to b of f(x)
 *           using the trapezoidal rule and n trapezoids.
 *
 * Compile:  mpicc -g -Wall -o q46 q46.c
 * Run:      mpiexec -n <number of processes> ./q46
 *
 * Algorithm:
 *    1.  Each process calculates "its" interval of
 *        integration.
 *    2.  Each process estimates the integral of f(x)
 *        over its interval using the trapezoidal rule.
 *    3a. Each process != 0 sends its integral to 0.
 *    3b. Process 0 sums the calculations received from
 *        the individual processes and prints the result.
 *
 * Note:  f(x) is all hardwired.
 *
 * IPP:   Section 3.3.2  (pp. 100 and ff.)
 */
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

void Get_input(int my_rank, int comm_sz, double* a_p, double* b_p, int* n_p);

double Trap(double left_endpt, double right_endpt, int trap_count, double base_len);    

double f(double x); 

int main(void) {
   int my_rank, comm_sz, n, local_n;   
   double a, b, h, local_a, local_b;
   double local_int, total_int;
   int source; 

   MPI_Init(NULL, NULL);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

   Get_input(my_rank, comm_sz, &a, &b, &n);

   h = (b-a)/n;       
   local_n = n/comm_sz; 

   local_a = a + my_rank*local_n*h;
   local_b = local_a + local_n*h;
   local_int = Trap(local_a, local_b, local_n, h);

   if (my_rank != 0)
      MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, 
            MPI_COMM_WORLD);
   else {
      total_int = local_int;
      for (source = 1; source < comm_sz; source++) {
         MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         total_int += local_int;
      }
   }

   if (my_rank == 0) {
      printf("With n = %d trapezoids, our estimate\n", n);
      printf("of the integral from %f to %f = %.15e\n",
          a, b, total_int);
   }

   MPI_Finalize();

   return 0;
} /*  main  */

//Get_input------------------------------------------------------------------
void Get_input(int my_rank, int comm_sz, double* a_p, double* b_p, int* n_p) {
   int size_a, size_b, size_n, total_size;
   int position = 0;
   char* pack_buf;

   MPI_Pack_size(1, MPI_DOUBLE, MPI_COMM_WORLD, &size_a);
   MPI_Pack_size(1, MPI_DOUBLE, MPI_COMM_WORLD, &size_b);
   MPI_Pack_size(1, MPI_INT,    MPI_COMM_WORLD, &size_n);

   total_size = size_a + size_b + size_n;
   pack_buf = (char*) malloc(total_size); 

   if (my_rank == 0) {
      
      printf("Enter a, b, and n\n");
      scanf("%lf %lf %d", a_p, b_p, n_p);
      
      MPI_Pack(a_p, 1, MPI_DOUBLE, pack_buf, total_size, &position, MPI_COMM_WORLD);
      MPI_Pack(b_p, 1, MPI_DOUBLE, pack_buf, total_size, &position, MPI_COMM_WORLD);
      MPI_Pack(n_p, 1, MPI_INT, pack_buf, total_size, &position, MPI_COMM_WORLD);
   }
   MPI_Bcast(pack_buf, total_size, MPI_PACKED, 0, MPI_COMM_WORLD);

   position = 0;

   MPI_Unpack(pack_buf, total_size, &position, a_p, 1, MPI_DOUBLE, MPI_COMM_WORLD);
   MPI_Unpack(pack_buf, total_size, &position, b_p, 1, MPI_DOUBLE, MPI_COMM_WORLD);
   MPI_Unpack(pack_buf, total_size, &position, n_p, 1, MPI_INT, MPI_COMM_WORLD);

   free(pack_buf);
}  /* Get_input */

//Trap------------------------------------------------------------------
double Trap(
      double left_endpt  /* in */, 
      double right_endpt /* in */, 
      int    trap_count  /* in */, 
      double base_len    /* in */) {
   double estimate, x; 
   int i;

   estimate = (f(left_endpt) + f(right_endpt))/2.0;
   for (i = 1; i <= trap_count-1; i++) {
      x = left_endpt + i*base_len;
      estimate += f(x);
   }
   estimate = estimate*base_len;

   return estimate;
} /*  Trap  */


//f------------------------------------------------------------------
double f(double x) {
   return x*x;
} /* f */
