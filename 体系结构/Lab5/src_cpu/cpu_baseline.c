#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int N = (1 << 8);

void gemm_baseline(float *A, float *B, float *C); // you can use inline function

int main(int argc, char *argv[]) {
    if (argc == 2)
        N = atoi(argv[1]);

    // malloc memory for A, B, C
    float *A = (float *)malloc(N * N * sizeof(float));
    float *B = (float *)malloc(N * N * sizeof(float));
    float *C = (float *)malloc(N * N * sizeof(float));

    // random initialize A, B
    srand(time(NULL));
    for (int i = 0; i < N * N; i++) {
        A[i] = (float)rand() / RAND_MAX;
        B[i] = (float)rand() / RAND_MAX;
        C[i] = 0;
    }

    // measure time
    clock_t start = clock();
    gemm_baseline(A, B, C);
    clock_t end = clock();
    printf("clocks: %ld, CLOCKS_PER_SEC: %ld\n", end - start, CLOCKS_PER_SEC);
    printf("seconds: %f\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}

void gemm_baseline(float *A, float *B, float *C) {
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            for (int j = 0; j < N; j++) {
                C[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}