#include "cache_aware_matmul.h"
#include <algorithm>

void cache_aware_matmul(const std::vector<std::vector<int>>& A,
                        const std::vector<std::vector<int>>& B,
                        std::vector<std::vector<int>>& C,
                        int cacheLineSize, int l1CacheSize) {
    int n = A.size();
    // Compute block size: assume an int is sizeof(int) bytes.
    int blockSize = l1CacheSize / (3 * sizeof(int)); // factor 3: A, B, C blocks

    blockSize = std::max(1, std::min(blockSize, n));

    for (int ii = 0; ii < n; ii += blockSize)
        for (int jj = 0; jj < n; jj += blockSize)
            for (int kk = 0; kk < n; kk += blockSize)
                for (int i = ii; i < std::min(ii+blockSize, n); ++i)
                    for (int k = kk; k < std::min(kk+blockSize, n); ++k)
                        for (int j = jj; j < std::min(jj+blockSize, n); ++j)
                            C[i][j] += A[i][k] * B[k][j];
}
