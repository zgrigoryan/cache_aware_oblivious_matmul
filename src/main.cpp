#include <iostream>
#include <vector>
#include <chrono>
#include "kaizen.h"
#include "naive_matmul.h"
#include "cache_aware_matmul.h"
#include "cache_oblivious_matmul.h"
#include "cache_utils.h"

// meaningless comment 

constexpr int DEFAULT_SIZE = 512;

int main(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);
    
    int size = DEFAULT_SIZE;
    if (args.is_present("--size")) {
        size = std::stoi(args.get_options("--size")[0]);
    }
    
    // Determine cache parameters
    int cacheLine = get_cache_line_size();
    int l1Cache = get_l1_cache_size();
    std::cout << "Detected Cache Line Size: " << cacheLine << " bytes\n";
    std::cout << "Detected L1 Cache Size: " << l1Cache << " bytes\n";

    // Initialize matrices (using vector of vectors for simplicity)
    std::vector<std::vector<int>> A(size, std::vector<int>(size, 1));
    std::vector<std::vector<int>> B(size, std::vector<int>(size, 1));
    std::vector<std::vector<int>> C(size, std::vector<int>(size, 0));

    // Benchmark naive multiplication
    auto start = std::chrono::high_resolution_clock::now();
    naive_matmul(A, B, C);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Naive matmul time: " 
              << std::chrono::duration<double, std::milli>(end-start).count() 
              << " ms\n";

    // Reset result matrix, then run cache-aware multiplication
    for(auto &row : C) std::fill(row.begin(), row.end(), 0);
    start = std::chrono::high_resolution_clock::now();
    cache_aware_matmul(A, B, C, cacheLine, l1Cache);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Cache-aware matmul time: " 
              << std::chrono::duration<double, std::milli>(end-start).count() 
              << " ms\n";

    // Reset result matrix, then run cache-oblivious multiplication
    for(auto &row : C) std::fill(row.begin(), row.end(), 0);
    start = std::chrono::high_resolution_clock::now();
    cache_oblivious_matmul(A, B, C);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Cache-oblivious matmul time: " 
              << std::chrono::duration<double, std::milli>(end-start).count() 
              << " ms\n";

    return 0;
}
