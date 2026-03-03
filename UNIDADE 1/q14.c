#include <stdio.h>
#include <math.h>

double t_sequencial(int n) {
    return n * n;
}

double t_paralelo(int n, int p) {
    return (n * n) / (double)p + log2(p);
}

int main() {
    int n_valores[6] = {10, 20, 40, 80, 160, 320};
    int p_valores[8] = {1, 2, 4, 8, 16, 32, 64, 128};
    double sequencial_T, paralelo_T, speedups, eficiencia;

    printf("%-10s %-10s %-15s %-15s\n", "N", "P", "Speedups", "Eficiência");
    for (int i = 0; i < 6; i++) {
        int n = n_valores[i];
        sequencial_T = t_sequencial(n);

        for (int j = 0; j < 8; j++) {
            int p = p_valores[j];
            paralelo_T = t_paralelo(n, p);

            // Calcula speedups e eficiência
            speedups = sequencial_T / paralelo_T;
            eficiencia = sequencial_T / (p * paralelo_T);

            printf("%-10d %-10d %-15.2f %-15.2f\n", n, p, speedups, eficiencia);
        }

        printf("------------------------------------------------\n");
    }

    return 0;
}