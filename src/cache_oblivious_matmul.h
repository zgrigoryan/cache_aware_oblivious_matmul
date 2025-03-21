#ifndef CACHE_OBLIVIOUS_MATMUL_H
#define CACHE_OBLIVIOUS_MATMUL_H

#include <vector>

void cache_oblivious_matmul(const std::vector<std::vector<int>>& A,
                            const std::vector<std::vector<int>>& B,
                            std::vector<std::vector<int>>& C);

#endif // CACHE_OBLIVIOUS_MATMUL_H
