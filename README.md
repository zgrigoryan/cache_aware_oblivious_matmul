# Cache-Aware & Cache-Oblivious Matrix Multiplication

This repository demonstrates three variants of matrix multiplication:

1. **Naive (Standard) Matrix Multiplication**  
2. **Cache-Aware (Blocked) Matrix Multiplication**  
3. **Cache-Oblivious (Recursive) Matrix Multiplication**  

The code attempts to detect certain cache parameters (like line size and L1 cache size) to set a block size for the "cache-aware" approach. The "cache-oblivious" approach recursively divides matrices into quadrants without explicitly using any cache-related constants.

## Building

```bash
mkdir build
cd build
cmake ..
make