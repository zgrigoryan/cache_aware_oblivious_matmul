#ifndef CACHE_AWARE_MATMUL_H
#define CACHE_AWARE_MATMUL_H

#include <vector>

void cache_aware_matmul(const std::vector<std::vector<int>>& A,
                        const std::vector<std::vector<int>>& B,
                        std::vector<std::vector<int>>& C,
                        int cacheLineSize, int l1CacheSize);

#endif // CACHE_AWARE_MATMUL_H
