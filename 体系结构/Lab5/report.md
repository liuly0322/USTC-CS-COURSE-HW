# CA Lab5 数据级并行

PB20000180 刘良宇

## 实验环境

- g++ (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0
- AMD Ryzen 7 4800H with 3200 MHz 16GB DDR4 RAM
- Nvidia GeForce RTX 2060 Max-Q (6GB)

## 实验说明

本实验所有可执行文件通过 `make` 编译。

对于 CPU 部分，性能测试时统一采用 `-O2` 编译优化参数，以减少编译器优化对性能的影响。

## CPU 部分

### 基础矩阵乘法

基础矩阵乘法的实现如下：

```c
void gemm_baseline(float *A, float *B, float *C) {
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            for (int j = 0; j < N; j++) {
                C[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}
```

$C_{ij} = \sum_{k=0}^{N-1} A_{ik}B_{kj}$，时间复杂度为 $O(N^3)$。

值得一提的是下标顺序是 ikj 而不是 ijk，这是因为这样的顺序可以使得内存访问更加连续，从而减少缓存的缺失。

通过 `./cpubaseline 1024` 指定 n 的规模。

```
$ ./cpu_baseline 1024
clocks: 170811, CLOCKS_PER_SEC: 1000000
```

### AVX 矩阵乘法

需要我们熟悉 AVX 指令集。

```c
void gemm_avx(float *A, float *B, float *C) {
    if (N < 8) {
        return gemm_baseline(A, B, C);
    }
    __m256 a8, b8, c8;
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            a8 = _mm256_set1_ps(A[i * N + k]);
            for (int j = 0; j <= N - 8; j += 8) {
                b8 = _mm256_load_ps(B + k * N + j);
                c8 = _mm256_load_ps(C + i * N + j);
                c8 = _mm256_fmadd_ps(a8, b8, c8);
                _mm256_store_ps(C + i * N + j, c8);
            }
        }
    }
}
```

值得一提的是，`__mm256_load_ps` 等指令要求内存地址为 32 字节对齐，因此我们需要使用 `posix_memalign` 来分配内存。

```c
float *A, *B, *C;
// 为 A, B, C 分配对齐的内存，返回值为 0 表示成功
int alloc1 = posix_memalign((void **)&A, 256, N * N * sizeof(float));
int alloc2 = posix_memalign((void **)&B, 256, N * N * sizeof(float));
int alloc3 = posix_memalign((void **)&C, 256, N * N * sizeof(float));

if (alloc1 != 0 || alloc2 != 0 || alloc3 != 0) {
    printf("alloc memory failed!\n");
    return 0;
}
```

需要调整编译参数：

```makefile
CFLAGS := -O2 -mavx -mavx2 -mfma -msse -msse2 -msse3
```

正确性测试通过：

```
❯ ./cpu_avx 1024
clocks: 184698, CLOCKS_PER_SEC: 1000000
gemm_avx is correct!
```

### AVX 矩阵分块乘法

分别对 A, B, C 进行分块，每个块的大小也作为参数可调。

```c
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
```

正确性测试通过。

```
❯ ./cpu_avx_block 1024 16
clocks: 187453, CLOCKS_PER_SEC: 1000000
gemm_avx_block is correct!
```

### 不同输入规模及分块参数的性能对比

| n/算法（毫秒） | baseline | avx   | block4 | block8 | block16 | block32 | block64 |
| -------------- | -------- | ----- | ------ | ------ | ------- | ------- | ------- |
| 128            | 0.902    | 0.221 | 0.355  | 0.303  | 0.271   | 0.279   | 0.270   |
| 256            | 6.359    | 1.820 | 2.970  | 2.599  | 2.325   | 2.190   | 2.931   |
| 512            | 50.28    | 17.51 | 25.51  | 23.78  | 23.07   | 22.09   | 18.02   |
| 1024           | 434.4    | 188.2 | 235.2  | 197.2  | 194.2   | 196.8   | 174.9   |
| 2048           | 3490     | 1807  | 2037   | 1628   | 1829    | 1570    | 1544    |

- 使用了 AVX 实现后性能会有可观的提升。
- 分块的具体性能取决于分块大小。n 越大，对应适宜的分块大小就越大（缓存带来的收益越能摊平分支带来的代价）。

### CPU 平台上其它矩阵乘法的优化手段

1. Cache 优化：使用局部性原理，进一步优化矩阵的访问顺序。
2. 算法优化：其他的算法可以减少计算量（如乘法次数）。例如 Strassen 算法和 Coppersmith-Winograd 算法等。
3. 并行化：如使用线程级并行或任务并行来同时计算多个矩阵乘法的子任务。

## GPU 部分

### 基础矩阵乘法

需要理解 cuda 的基本概念。

```c++
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
```

每个线程计算一个 C 矩阵的元素即可。

`blockSize` 和 `girdSize` 的确定：

```cpp
const dim3 blockSize(block_size, block_size);
const dim3 gridSize((N + block_size - 1) / block_size,
                    (N + block_size - 1) / block_size);
```

下面调节 `block_size`，测试不同规模矩阵的时间（单位：毫秒）：

| N\block_size | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- |
| 256 | 5.316608 | 2.328064 | 1.961312 | 0.812416 |
| 512 | 56.181278 | 24.79744 | 10.05568 | 4.841472 |
| 1024 | 487.871765 | 161.318909 | 86.039902 | 40.840801 |
| 2048 | 4621.831543 | 1391.416748 | 766.842285 | 311.623016 |

这里一个线程块最多 1024 个线程，所以 `block_size` 上限 32。

可以看出这里 `block_size` 越大性能越好，可能是因为一个线程块内多个线程可以更好利用局部性。

### 分块矩阵乘法

这里的 BLOCK 参数需要宏定义，因为共享内存的大小不能在运行时才确定。

```cpp
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
```

这里的基本思路是，每个线程负责计算一个元素，即 $c_{ij} = \sum_{k=0}^{n-1} a_{ik}b_{kj}$，根据分块大小 `BLOCK`，可以拆分为 $\lceil \mathrm{N} / \mathrm{BLOCK} \rceil$ 次循环计算，每次循环计算的元素都来自对应的矩阵分块（所以可以读入线程块的共享内存加速），最后结果累加起来即可。

这里固定 `block_size` 和矩阵分块大小 `BLOCK` 相同是一个合理的设计，否则无法充分利用共享内存。

下面时间测试（单位：毫秒）：

| N\BLOCK | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- |
| 256 | 0.879392 | 0.968704 | 0.868352 | 1.049216 |
| 512 | 4.11648 | 3.712416 | 3.939488 | 4.03456 |
| 1024 | 32.213631 | 31.544544 | 38.23859 | 32.523422 |
| 2048 | 351.410675 | 305.884491 | 260.249084 | 285.640717 |

可以看出此时分块大小不是越大越好了，而是需要权衡以充分利用共享内存的局部性。
