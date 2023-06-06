#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int N = (1 << 8);
int BLOCK_SIZE = 8;

void gemm_baseline(float *A, float *B, float *C);
void gemm_verify(float *A, float *B, float *C);
void gemm_avx_block(float *A, float *B, float *C);

#define min(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char *argv[]) {
    if (argc == 3) {
        N = atoi(argv[1]);
        BLOCK_SIZE = atoi(argv[2]);
    }

    // malloc memory for A, B, C
    float *A, *B, *C;
    int alloc1 = posix_memalign((void **)&A, 256, N * N * sizeof(float));
    int alloc2 = posix_memalign((void **)&B, 256, N * N * sizeof(float));
    int alloc3 = posix_memalign((void **)&C, 256, N * N * sizeof(float));

    if (alloc1 != 0 || alloc2 != 0 || alloc3 != 0) {
        printf("alloc memory failed!\n");
        return 0;
    }

    // random initialize A, B
    srand(time(NULL));
    for (int i = 0; i < N * N; i++) {
        A[i] = (float)rand() / RAND_MAX;
        B[i] = (float)rand() / RAND_MAX;
        C[i] = 0;
    }

    // measure time
    clock_t start = clock();
    gemm_avx_block(A, B, C);
    clock_t end = clock();
    printf("clocks: %ld, CLOCKS_PER_SEC: %ld\n", end - start, CLOCKS_PER_SEC);
    printf("seconds: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    // use gemm_baseline verify gemm_avx_block
    gemm_verify(A, B, C);
    return 0;
}
void gemm_verify(float *A, float *B, float *C) {
    float *C_baseline = (float *)malloc(N * N * sizeof(float));
    for (int i = 0; i < N * N; i++)
        C_baseline[i] = 0;
    gemm_baseline(A, B, C_baseline);
    for (int i = 0; i < N * N; i++) {
        if (C[i] != C_baseline[i]) {
            printf("gemm_avx_block is wrong!\n");
            printf("C[%d] = %f, C_baseline[%d] = %f\n", i, C[i], i,
                   C_baseline[i]);
            return;
        }
    }
    printf("gemm_avx_block is correct!\n");
}

void gemm_avx_block(float *A, float *B, float *C) {
    if (N < 8) {
        return gemm_baseline(A, B, C);
    }
    int step_size = BLOCK_SIZE * 8;
    for (int bi = 0; bi < N; bi += step_size) {
        for (int bj = 0; bj < N; bj += step_size) {
            for (int bk = 0; bk < N; bk += step_size) {
                for (int i = bi; i < min(N, bi + step_size); i++) {
                    for (int k = bk; k < min(N, bk + step_size); k++) {
                        __m256 a8 = _mm256_set1_ps(A[i * N + k]);
                        for (int j = bj; j < min(N, bj + step_size); j += 8) {
                            __m256 b8 = _mm256_load_ps(B + k * N + j);
                            __m256 c8 = _mm256_load_ps(C + i * N + j);
                            c8 = _mm256_fmadd_ps(a8, b8, c8);
                            _mm256_store_ps(C + i * N + j, c8);
                        }
                    }
                }
            }
        }
    }
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