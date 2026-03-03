#include <stdio.h>
#include <math.h>

double t_sequencial(int n) {
    return pow(n, 2);
}

double t_overhead(int n) {
    return pow(n, 2.5);
}

int main() {
    int n_valores[6] = {10, 20, 40, 80, 160, 320};
    int p = 2;
    double sequencial_T, paralelo_T, speedups, eficiencia, overhead_T;

    printf("%-10s %-10s %-15s %-15s %-15s %-15s %-15s\n", "N", "P", "Speedups", "Eficiência", "Tseque", "Tparal", "ToverH");
    for (int i = 0; i < 6; i++) {
        int n = n_valores[i];
        sequencial_T = t_sequencial(n);
        overhead_T = t_overhead(n);

        paralelo_T = (sequencial_T / p) + overhead_T;

        speedups = sequencial_T / paralelo_T;
        eficiencia = sequencial_T / (p * paralelo_T);

        printf("%-10d %-10d %-15.2f %-15.2f %-15.2f %-15.2f %-15.2f\n", n, p, speedups, eficiencia, sequencial_T, paralelo_T, overhead_T);

        printf("------------------------------------------------------------------------------------------------\n");
    }

    return 0;
}