#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include "kaizen.h"
#include "naive_matmul.h"
#include "cache_aware_matmul.h"
#include "cache_oblivious_matmul.h"
#include "cache_utils.h"
#include "cache_aware_matmul_1D.h" 

using Clock = std::chrono::high_resolution_clock;

constexpr int DEFAULT_SIZE = 1024;

int main(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);
    
    int size = DEFAULT_SIZE;
    if (args.is_present("--size")) {
        size = std::stoi(args.get_options("--size")[0]);
    }
    
    // Determine cache parameters
    int cacheLine = static_cast<int>(get_cache_line_size());
    int l1Cache   = static_cast<int>(get_l1_cache_size());
    std::cout << "Detected Cache Line Size: " << cacheLine << " bytes\n";
    std::cout << "Detected L1 Cache Size:   " << l1Cache  << " bytes\n";

    //--------------------------------------------------------------------------
    // 1) Benchmark (vector-of-vectors) for naive, cache-aware, cache-oblivious
    //--------------------------------------------------------------------------
    std::vector<std::vector<int>> A(size, std::vector<int>(size, 1));
    std::vector<std::vector<int>> B(size, std::vector<int>(size, 1));
    std::vector<std::vector<int>> C(size, std::vector<int>(size, 0));

    // ---------------- Naive ----------------
    auto startNaive = Clock::now();
    naive_matmul(A, B, C);
    auto endNaive   = Clock::now();
    std::cout << "Naive matmul time: "
              << std::chrono::duration<double, std::milli>(endNaive - startNaive).count()
              << " ms\n";

    // ---------------- Cache-Aware (vec-of-vec) ----------------
    std::fill(C.begin(), C.end(), std::vector<int>(size, 0));
    auto startAware = Clock::now();
    cache_aware_matmul(A, B, C, cacheLine, l1Cache);
    auto endAware   = Clock::now();
    std::cout << "Cache-aware matmul time: "
              << std::chrono::duration<double, std::milli>(endAware - startAware).count()
              << " ms\n";

    // ---------------- Cache-Oblivious (vec-of-vec) ----------------
    std::fill(C.begin(), C.end(), std::vector<int>(size, 0));
    auto startObliv = Clock::now();
    cache_oblivious_matmul(A, B, C);
    auto endObliv   = Clock::now();
    std::cout << "Cache-oblivious matmul time: "
              << std::chrono::duration<double, std::milli>(endObliv - startObliv).count()
              << " ms\n";

    //--------------------------------------------------------------------------
    // 2) Benchmark the 1D-Aligned approach with std::thread
    //--------------------------------------------------------------------------
    {
        int n = size; 
        int threadCount = 8; 

        int* A_ = allocate_aligned_matrix(n);
        int* B_ = allocate_aligned_matrix(n);
        int* C_ = allocate_aligned_matrix(n);

        // Initialize data
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                mat_elem(A_, n, i, j) = 1;
                mat_elem(B_, n, i, j) = 1;
                mat_elem(C_, n, i, j) = 0;
            }
        }

        auto start_1D = Clock::now();
        cache_aware_matmul_1D(A_, B_, C_, n, threadCount);
        auto end_1D = Clock::now();

        double elapsed_ms = std::chrono::duration<double,std::milli>(end_1D - start_1D).count();
        std::cout << "1D matmul (std::thread, " << threadCount 
                  << " threads) took " << elapsed_ms << " ms.\n";

        free_aligned_matrix(A_);
        free_aligned_matrix(B_);
        free_aligned_matrix(C_);
    }

    //--------------------------------------------------------------------------
    // 3) Benchmark all approaches for a range of sizes [1012..1036], write CSV
    //--------------------------------------------------------------------------
    std::ofstream csv("results.csv");
    csv << "Size,Naive,CacheAware,CacheOblivious,CacheAware1D\n";

    for (int test_size = 1012; test_size <= 1036; ++test_size) {
        // --- (A) Prepare data for the 3 existing (vector-of-vector) approaches ---
        std::vector<std::vector<int>> A2(test_size, std::vector<int>(test_size, 1));
        std::vector<std::vector<int>> B2(test_size, std::vector<int>(test_size, 1));
        std::vector<std::vector<int>> C2(test_size, std::vector<int>(test_size, 0));

        int cacheLineLocal = get_cache_line_size();
        int l1CacheLocal   = get_l1_cache_size();

        // (A) Naive
        auto startA = Clock::now();
        naive_matmul(A2, B2, C2);
        auto endA   = Clock::now();
        double naive_time = std::chrono::duration<double,std::milli>(endA - startA).count();

        // (B) Cache-Aware (vec-of-vec)
        for (auto &row : C2) std::fill(row.begin(), row.end(), 0);
        startA = Clock::now();
        cache_aware_matmul(A2, B2, C2, cacheLineLocal, l1CacheLocal);
        endA   = Clock::now();
        double cache_aware_time = std::chrono::duration<double,std::milli>(endA - startA).count();

        // (C) Cache-Oblivious (vec-of-vec)
        for (auto &row : C2) std::fill(row.begin(), row.end(), 0);
        startA = Clock::now();
        cache_oblivious_matmul(A2, B2, C2);
        endA   = Clock::now();
        double cache_oblivious_time = std::chrono::duration<double,std::milli>(endA - startA).count();

        // --- (D) 1D approach ---
        int* A1 = allocate_aligned_matrix(test_size);
        int* B1 = allocate_aligned_matrix(test_size);
        int* C1 = allocate_aligned_matrix(test_size);

        for (int i = 0; i < test_size; i++){
            for (int j = 0; j < test_size; j++){
                mat_elem(A1,test_size,i,j) = 1;
                mat_elem(B1,test_size,i,j) = 1;
                mat_elem(C1,test_size,i,j) = 0;
            }
        }

        startA = Clock::now();
        cache_aware_matmul_1D(A1, B1, C1, test_size, 8);
        endA   = Clock::now();
        double cache_aware_1D_time = std::chrono::duration<double,std::milli>(endA - startA).count();

        free_aligned_matrix(A1);
        free_aligned_matrix(B1);
        free_aligned_matrix(C1);

        // Write one CSV row
        csv << test_size << ","
            << naive_time << ","
            << cache_aware_time << ","
            << cache_oblivious_time << ","
            << cache_aware_1D_time << "\n";
    }

    csv.close();
    std::cout << "Results CSV written to results.csv\n";

    return 0;
}
