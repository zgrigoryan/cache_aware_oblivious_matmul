#ifndef CACHE_UTILS_H
#define CACHE_UTILS_H

#include <cstddef> // size_t

// Attempt to retrieve a best-guess for L1 data cache size and cache line size.
size_t get_cache_line_size();
size_t get_l1_cache_size();

#endif // CACHE_UTILS_H