#include <stdio.h>
#include <math.h>

int main() {
    int n[6] = {10, 20, 40, 80, 160, 320};
    int p[8] = {1, 2, 4, 8, 16, 32, 64, 128};
    double t_seqencial, t_paralelo, speedup, efficiency;
    

    for(int x = 0 ; x < 6; x++){
        printf("------------- TAMANHO DO PROBLEMA: %d -------------\n", n[x]);
        t_seqencial = pow(n[x], 2);
        printf("-- TEMPO SEQUENCIAL: %.2f \n", t_seqencial);
        printf("-- PARALELO: ");
        for(int y = 0; y < 8; y++){
            //t_seqencial = pow(n[x], 2);
            t_paralelo = (t_seqencial / p[y]) + log2(p[y]);
            printf("- PARA %d THREADS: \n", p[y]);
            printf("TEMPO: %.2f \n", t_paralelo);
            speedup = t_seqencial / t_paralelo;
            printf("SPEEDUP: %.2f \n", speedup);
            efficiency = t_seqencial / (p[y]*t_paralelo);
            printf("EFICIENCIA: %.2f \n", efficiency);
        }

    }

    return 0;
}