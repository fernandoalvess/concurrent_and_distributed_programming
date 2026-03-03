#include <stdio.h>
#include <math.h>

// Função para calcular o tempo sequencial
double tempo_sequencial(int n) {
    return n * n;
}

// Função para calcular o tempo paralelo
double tempo_paralelo(int n, int p) {
    return (n * n) / (double)p + log2(p);
}

int main() {
    int n_values[] = {10, 20, 40, 80, 160, 320};
    int p_values[] = {1, 2, 4, 8, 16, 32, 64, 128};
    int num_n = sizeof(n_values) / sizeof(n_values[0]);
    int num_p = sizeof(p_values) / sizeof(p_values[0]);

    printf("Resultados:\n");
    printf("%-10s %-10s %-15s %-15s %-15s\n", "n", "p", "Tempo Seq.", "Aceleração", "Eficiência");

    // Itera sobre todos os valores de n e p
    for (int i = 0; i < num_n; i++) {
        int n = n_values[i];
        double Tseq = tempo_sequencial(n);

        for (int j = 0; j < num_p; j++) {
            int p = p_values[j];
            double Tpar = tempo_paralelo(n, p);

            // Calcula aceleração e eficiência
            double S = Tseq / Tpar;
            double E = S / p;

            printf("%-10d %-10d %-15.2f %-15.2f %-15.2f\n", n, p, Tseq, S, E);
        }

        printf("------------------------------------------------------------\n");
    }

    return 0;
}