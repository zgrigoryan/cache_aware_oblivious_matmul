#include "cache_aware_matmul_1D.h"

#include <cstdlib>   // for posix_memalign / aligned_alloc
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

#if defined(__APPLE__) || defined(__linux__)
    // posix_memalign for Unix-like systems
    const std::size_t alignment = 64;
    if (posix_memalign(&ptr, alignment, bytes) != 0) {
        std::cerr << "posix_memalign failed!\n";
        std::abort();
    }
#else
    // For other platforms, e.g. Windows + MSVC:
    // ptr = _aligned_malloc(bytes, 64);
    // Or if C++17's aligned_alloc is available:
    ptr = std::aligned_alloc(64, bytes);
    if (!ptr) {
        std::cerr << "aligned_alloc failed!\n";
        std::abort();
    }
#endif

    // Initialize to zero
    std::memset(ptr, 0, bytes);
    return static_cast<int*>(ptr);
}

void free_aligned_matrix(int* ptr)
{
#if defined(__APPLE__) || defined(__linux__)
    // posix_memalign -> free
    free(ptr);
#else
    // If using _aligned_malloc:
    // _aligned_free(ptr);
    free(ptr);
#endif
}

/**
 * Parallel blocked matmul with std::thread.
 * - We split the row blocks [ii in 0..n..blockSize] across 'threadCount' threads.
 * - Each thread runs a loop over its subset of blocks.
 * - Within each block, we do the triple nested loop with blocking in (ii, jj, kk).
 */
void cache_aware_matmul_1D(const int* A, const int* B, int* C, int n, int threadCount)
{
    // Choose a tile/block size that fits in L1/L2
    int blockSize = 64;

    // Create a lambda that each thread will run:
    auto worker = [&](int threadId)
    {
        // Each thread processes the outer 'ii' loop in a "round-robin" or "block-cyclic" manner.
        // That is, if threadId=0 handles ii=0, then ii= blockSize*threadCount, etc.
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

    // Launch threads
    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    for (int t = 0; t < threadCount; t++) {
        threads.emplace_back(worker, t);
    }

    // Join 
    for (auto &th : threads) {
        th.join();
    }
}
