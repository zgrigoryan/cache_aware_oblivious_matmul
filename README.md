# Cache-Aware & Cache-Oblivious Matrix Multiplication

This repository demonstrates three variants of matrix multiplication:

1. **Naive (Standard) Matrix Multiplication**  
2. **Cache-Aware (Blocked) Matrix Multiplication**  
3. **Cache-Oblivious (Recursive) Matrix Multiplication**  

The code attempts to detect certain cache parameters (like line size and L1 cache size) to set a block size for the "cache-aware" approach. The "cache-oblivious" approach recursively divides matrices into quadrants without explicitly using any cache-related constants.

## Build & Run

```bash
make
./cache_matmul --size 512 --iterations 10       

## Example output 

Detected Cache Line Size: 64 bytes
Detected L1 Cache Size: 32768 bytes
Naive matmul time: 24.5793 ms
Cache-aware matmul time: 16.9571 ms
Cache-oblivious matmul time: 18.2666 ms