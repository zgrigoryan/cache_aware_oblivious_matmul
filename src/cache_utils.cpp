#include "cache_utils.h"
#include <unistd.h>
#include <iostream>

// Try sysconf calls, then fallback
size_t get_cache_line_size() {
#ifdef _SC_LEVEL1_DCACHE_LINESIZE
    long lineSize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    if (lineSize > 0) {
        return static_cast<size_t>(lineSize);
    }
#endif
    // Fallback guess
    return 64; // typical line size on many modern x86 systems
}

size_t get_l1_cache_size() {
#ifdef _SC_LEVEL1_DCACHE_SIZE
    long cacheSize = sysconf(_SC_LEVEL1_DCACHE_SIZE);
    if (cacheSize > 0) {
        return static_cast<size_t>(cacheSize);
    }
#endif
    // Fallback guess
    return 32 * 1024; // 32 KB fallback
}
