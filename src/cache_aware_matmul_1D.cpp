#include "cache_aware_matmul_1D.h"

#include <cstdlib>   // for posix_memalign, aligned_alloc, _aligned_malloc
#include <cstring>   // for memset
#include <iostream>
#include <algorithm> 
#include <thread>
#include <vector>

/**
 * Allocates a 1D array of (n*n) ints, 64-byte aligned.
 */
int* allocate_aligned_matrix(std::size_t n)
{
    std::size_t bytes = n * n * sizeof(int);
    void* ptr = nullptr;

#if defined(_MSC_VER)
    // Windows + MSVC
    ptr = _aligned_malloc(bytes, 64);
    if (!ptr) {
        std::cerr << "_aligned_malloc failed!\n";
        std::abort();
    }
#elif defined(__APPLE__) || defined(__linux__)
    // Unix-like systems
    const std::size_t alignment = 64;
    if (posix_memalign(&ptr, alignment, bytes) != 0) {
        std::cerr << "posix_memalign failed!\n";
        std::abort();
    }
#else
    // Fallback: C++17 aligned_alloc (non-MSVC)
    ptr = std::aligned_alloc(64, bytes);
    if (!ptr) {
        std::cerr << "aligned_alloc failed!\n";
        std::abort();
    }
#endif

    std::memset(ptr, 0, bytes);
    return static_cast<int*>(ptr);
}

void free_aligned_matrix(int* ptr)
{
#if defined(_MSC_VER)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

/**
 * Parallel blocked matmul with std::thread.
 */
void cache_aware_matmul_1D(const int* A, const int* B, int* C, int n, int threadCount)
{
    int blockSize = 64;

    auto worker = [&](int threadId)
    {
        for (int ii = threadId * blockSize; ii < n; ii += blockSize * threadCount) {
            for (int jj = 0; jj < n; jj += blockSize) {
                for (int kk = 0; kk < n; kk += blockSize) {

                    int iMax = std::min(ii + blockSize, n);
                    int jMax = std::min(jj + blockSize, n);
                    int kMax = std::min(kk + blockSize, n);

                    for (int i = ii; i < iMax; ++i) {
                        for (int k = kk; k < kMax; ++k) {
                            int aVal = A[i*n + k];
                            for (int j = jj; j < jMax; ++j) {
                                C[i*n + j] += aVal * B[k*n + j];
                            }
                        }
                    }
                }
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    for (int t = 0; t < threadCount; t++) {
        threads.emplace_back(worker, t);
    }

    for (auto &th : threads) {
        th.join();
    }
}
