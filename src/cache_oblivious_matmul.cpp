#include "cache_oblivious_matmul.h"
#include <algorithm>

// Helper recursive function to multiply sub-matrices.
// (Parameters: starting indices and size for each submatrix.)
static void matmul_recursive(const std::vector<std::vector<int>>& A,
                             const std::vector<std::vector<int>>& B,
                             std::vector<std::vector<int>>& C,
                             int aRow, int aCol, int bRow, int bCol,
                             int cRow, int cCol, int size) {
    // Base case: use naive multiplication for small blocks.
    if (size <= 64) {  // cutoff threshold can be tuned
        for (int i = 0; i < size; ++i)
            for (int k = 0; k < size; ++k)
                for (int j = 0; j < size; ++j)
                    C[cRow + i][cCol + j] += A[aRow + i][aCol + k] * B[bRow + k][bCol + j];
        return;
    }

    int newSize = size / 2;

    // Multiply sub-quadrants recursively.
    // C11 = A11*B11 + A12*B21
    matmul_recursive(A, B, C, aRow, aCol, bRow, bCol, cRow, cCol, newSize);
    matmul_recursive(A, B, C, aRow, aCol + newSize, bRow + newSize, bCol, cRow, cCol, newSize);

    // C12 = A11*B12 + A12*B22
    matmul_recursive(A, B, C, aRow, aCol, bRow, bCol + newSize, cRow, cCol + newSize, newSize);
    matmul_recursive(A, B, C, aRow, aCol + newSize, bRow + newSize, bCol + newSize, cRow, cCol + newSize, newSize);

    // C21 = A21*B11 + A22*B21
    matmul_recursive(A, B, C, aRow + newSize, aCol, bRow, bCol, cRow + newSize, cCol, newSize);
    matmul_recursive(A, B, C, aRow + newSize, aCol + newSize, bRow + newSize, bCol, cRow + newSize, cCol, newSize);

    // C22 = A21*B12 + A22*B22
    matmul_recursive(A, B, C, aRow + newSize, aCol, bRow, bCol + newSize, cRow + newSize, cCol + newSize, newSize);
    matmul_recursive(A, B, C, aRow + newSize, aCol + newSize, bRow + newSize, bCol + newSize, cRow + newSize, cCol + newSize, newSize);
}

void cache_oblivious_matmul(const std::vector<std::vector<int>>& A,
                            const std::vector<std::vector<int>>& B,
                            std::vector<std::vector<int>>& C) {
    int n = A.size();
    matmul_recursive(A, B, C, 0, 0, 0, 0, 0, 0, n);
}
