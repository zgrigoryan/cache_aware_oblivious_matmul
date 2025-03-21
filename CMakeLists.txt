cmake_minimum_required(VERSION 3.10)
project(cache_aware_oblivious_matmul)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(matmul
    main.cpp
    matmul.cpp
    cache_utils.cpp
)