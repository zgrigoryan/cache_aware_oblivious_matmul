cmake_minimum_required(VERSION 3.10)
project(cache_aware_oblivious_matmul)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(matmul
    src/main.cpp
    src/cache_utils.cpp
    src/naive_matmul.cpp
    src/cache_aware_matmul.cpp
    src/cache_oblivious_matmul.cpp
)
