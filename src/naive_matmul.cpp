#include "naive_matmul.h"

void naive_matmul(const std::vector<std::vector<int>>& A,
                  const std::vector<std::vector<int>>& B,
                  std::vector<std::vector<int>>& C) {
    int n = A.size();
    for (int i = 0; i < n; ++i)
        for (int k = 0; k < n; ++k)
            for (int j = 0; j < n; ++j)
                C[i][j] += A[i][k] * B[k][j];
}
