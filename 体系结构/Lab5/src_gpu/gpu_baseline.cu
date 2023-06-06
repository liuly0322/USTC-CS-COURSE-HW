#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
// #define VERIFY

__global__ void gemm_baseline(const float *A, const float *B, float *C, int N);
void gemm_verify(const float *A, const float *B, float *C, int N);

constexpr float rand_max = static_cast<float>(RAND_MAX);

int main(int argc, char *argv[]) {
    int N = 1 << 10;
    int block_size = 16;
    if (argc > 1)
        N = std::atoi(argv[1]);
    if (argc > 2)
        block_size = std::atoi(argv[2]);

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
    const dim3 blockSize(block_size, block_size);
    const dim3 gridSize((N + block_size - 1) / block_size,
                        (N + block_size - 1) / block_size);

    // Create CUDA events for timing
    cudaEvent_t start;
    cudaEvent_t stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Record start event
    cudaEventRecord(start);

    // Compute
    gemm_baseline<<<gridSize, blockSize>>>(A, B, C, N);

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

__global__ void gemm_baseline(const float *A, const float *B, float *C, int N) {
    auto row = blockIdx.y * blockDim.y + threadIdx.y;
    auto col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row >= N || col >= N)
        return;

    auto sum = 0.0F;
    for (int k = 0; k < N; k++)
        sum += A[row * N + k] * B[k * N + col];

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
