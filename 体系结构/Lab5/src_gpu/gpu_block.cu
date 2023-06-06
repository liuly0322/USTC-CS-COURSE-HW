#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// #define VERIFY
#define BLOCK 32 // 宏定义分块大小

__global__ void gemm_block(const float *A, const float *B, float *C, int N);
void gemm_verify(const float *A, const float *B, float *C, int N);

constexpr float rand_max = static_cast<float>(RAND_MAX);

int main(int argc, char *argv[]) {
    int N = 1 << 10;
    if (argc > 1)
        N = std::atoi(argv[1]);

    float *A;
    float *B;
    float *C;
    // Allocate memory for A, B, C
    auto size = N * N * sizeof(float);
    cudaMallocManaged(&A, size);
    cudaMallocManaged(&B, size);
    cudaMallocManaged(&C, size);

    // Random initialize A, B
    for (int i = 0; i < N * N; i++) {
        A[i] = static_cast<float>(rand()) / rand_max;
        B[i] = static_cast<float>(rand()) / rand_max;
    }

    // Define grid size and block size
    const dim3 blockSize(BLOCK, BLOCK);
    const dim3 gridSize((N + BLOCK - 1) / BLOCK, (N + BLOCK - 1) / BLOCK);

    // Create CUDA events for timing
    cudaEvent_t start;
    cudaEvent_t stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Record start event
    cudaEventRecord(start);

    // Compute
    gemm_block<<<gridSize, blockSize>>>(A, B, C, N);

    // Record stop event
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    // Compute elapsed time
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    printf("Elapsed time: %f ms\n", elapsedTime);

    // Destroy CUDA events
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

#ifdef VERIFY
    // Verify the result
    gemm_verify(A, B, C, N);
#endif

    // Free memory
    cudaFree(A);
    cudaFree(B);
    cudaFree(C);

    return 0;
}

/**
 * @brief gemm_block
 * one thread block share one block of A and B.
 * one thread compute one element of C.
 * @param A pointer to the first matrix
 * @param B pointer to the second matrix
 * @param C pointer to the result matrix
 * @param N the size of matrix
 * @return void
 */
__global__ void gemm_block(const float *A, const float *B, float *C, int N) {
    __shared__ float sA[BLOCK][BLOCK];
    __shared__ float sB[BLOCK][BLOCK];

    auto tx = threadIdx.x;
    auto ty = threadIdx.y;

    auto row = blockIdx.y * BLOCK + ty;
    auto col = blockIdx.x * BLOCK + tx;

    if (row >= N || col >= N)
        return;

    auto sum = 0.0F;

    for (auto m = 0; m < (N + BLOCK - 1) / BLOCK; m++) {
        sA[ty][tx] = A[row * N + m * BLOCK + tx];
        sB[ty][tx] = B[col + (m * BLOCK + ty) * N];
        __syncthreads();

        for (auto k = 0; k < BLOCK; k++)
            sum += sA[ty][k] * sB[k][tx];
        __syncthreads();
    }

    C[row * N + col] = sum;
}

void gemm_verify(const float *A, const float *B, float *C, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            auto sum = 0.0F;
            for (int k = 0; k < N; k++)
                sum += A[i * N + k] * B[k * N + j];
            if (std::fabs(C[i * N + j] - sum) > 1e-4) {
                printf("C[%d][%d] = %f, sum = %f\n", i, j, C[i * N + j], sum);
                printf("Verification failed.\n");
                return;
            }
        }
    }
    printf("Verification succeeded.\n");
}
