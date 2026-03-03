/* File:     a_vector_prefixes.c
 *
 * Purpose:  Implement vector prefixes
 *
 * Compile:  gcc -g -Wall -o a_vector_prefixes a_vector_prefixes.c
 * Run:      ./vector_prefixes
 */
#include <stdio.h>
#include <stdlib.h>

void Read_n(int* n_p);
void Allocate_vector(double** x_pp, int n);
void Read_vector(double a[], int n, char vec_name[]);
void Print_vector(double b[], int n, char title[]);

int main(void) {
   int n;
   double *vector_x, *vector_result;

   Read_n(&n);
   Allocate_vector(&vector_x, n);
   Read_vector(vector_x, n, "X");
   Print_vector(vector_x, n, "VETOR X:");

   Allocate_vector(&vector_result, n);

   int aux = 0;
    for (int i = 0; i < n; i++) {
        aux += vector_x[i];
        vector_result[i] = aux;
    }

   Print_vector(vector_result, n, "RESULTADO:");

   free(vector_x);
   free(vector_result);

   return 0;
}  /* main */

void Read_n(int* n_p /* out */) {
   printf("Quantos elementos no vetor?\n");
   scanf("%d", n_p);
   if (*n_p <= 0) {
      fprintf(stderr, "Order should be positive\n");
      exit(-1);
   }
}  /* Read_n */

void Allocate_vector(
      double**  x_pp  /* out */, 
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
   printf("Informe os valores %s\n", vec_name);
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