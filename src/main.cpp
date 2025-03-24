#include <iostream>
#include <vector>
#include <chrono>
#include "kaizen.h"
#include "naive_matmul.h"
#include "cache_aware_matmul.h"
#include "cache_oblivious_matmul.h"
#include "cache_utils.h"
#include <fstream>

constexpr int DEFAULT_SIZE = 512;

int main(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);
    
    int size = DEFAULT_SIZE;
    if (args.is_present("--size")) {
        size = std::stoi(args.get_options("--size")[0]);
    }
    
    // Determine cache parameters
    int cacheLine = static_cast<int>(get_cache_line_size());
    int l1Cache = static_cast<int>(get_l1_cache_size());
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

    // Benchmark all three algorithms for a range of sizes
    std::ofstream csv("results.csv");
    csv << "Size,Naive,CacheAware,CacheOblivious\n";

    for (int test_size = 500; test_size <= 524; ++test_size) {
        std::vector<std::vector<int>> A(test_size, std::vector<int>(test_size, 1));
        std::vector<std::vector<int>> B(test_size, std::vector<int>(test_size, 1));
        std::vector<std::vector<int>> C(test_size, std::vector<int>(test_size, 0));

        int cacheLine = get_cache_line_size();
        int l1Cache = get_l1_cache_size();

        auto start = std::chrono::high_resolution_clock::now();
        naive_matmul(A, B, C);
        auto end = std::chrono::high_resolution_clock::now();
        double naive_time = std::chrono::duration<double, std::milli>(end - start).count();

        for (auto &row : C) std::fill(row.begin(), row.end(), 0);
        start = std::chrono::high_resolution_clock::now();
        cache_aware_matmul(A, B, C, cacheLine, l1Cache);
        end = std::chrono::high_resolution_clock::now();
        double cache_aware_time = std::chrono::duration<double, std::milli>(end - start).count();

        for (auto &row : C) std::fill(row.begin(), row.end(), 0);
        start = std::chrono::high_resolution_clock::now();
        cache_oblivious_matmul(A, B, C);
        end = std::chrono::high_resolution_clock::now();
        double cache_oblivious_time = std::chrono::duration<double, std::milli>(end - start).count();

        csv << test_size << "," << naive_time << "," << cache_aware_time << "," << cache_oblivious_time << "\n";
    }

    csv.close();

    return 0;
}
