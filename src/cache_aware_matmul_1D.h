#pragma once

#include <cstddef>

int* allocate_aligned_matrix(std::size_t n);
void free_aligned_matrix(int* ptr);

inline int& mat_elem(int* M, int n, int i, int j) {
    return M[i*n + j];
}

void cache_aware_matmul_1D(const int* A, const int* B, int* C, int n, int threadCount);
