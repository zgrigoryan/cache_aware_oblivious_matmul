#include "cache_utils.h"
#include <iostream>
#include <vector> 

#ifdef _WIN32
    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
    #endif
    #include <windows.h>
#else
    #include <unistd.h>
    #ifdef _SC_LEVEL1_DCACHE_LINESIZE
        #define HAS_SC_LEVEL1_DCACHE_LINESIZE
    #endif
    #ifdef _SC_LEVEL1_DCACHE_SIZE
        #define HAS_SC_LEVEL1_DCACHE_SIZE
    #endif
#endif

/**
 * Retrieve cache line size in bytes.
 * - On Linux/macOS, uses sysconf, else fallback = 64.
 * - On Windows, uses GetLogicalProcessorInformation, else fallback = 64.
 */
size_t get_cache_line_size() {
#ifdef _WIN32
    DWORD bufferSize = 0;
    // First call to get the size of the buffer.
    GetLogicalProcessorInformation(nullptr, &bufferSize);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(
            bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        if (GetLogicalProcessorInformation(buffer.data(), &bufferSize)) {
            for (auto &info : buffer) {
                // Look for L1 data cache information.
                if (info.Relationship == RelationCache && info.Cache.Level == 1) {
                    return info.Cache.LineSize;
                }
            }
        }
    }
    return 64;
#else
    #ifdef HAS_SC_LEVEL1_DCACHE_LINESIZE
        long lineSize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
        if (lineSize > 0) {
            return static_cast<size_t>(lineSize);
        }
    #endif
    return 64;
#endif
}

/**
 * Retrieve L1 cache size in bytes.
 * - On Linux/macOS, uses sysconf, else fallback = 32 KB.
 * - On Windows, uses GetLogicalProcessorInformation, else fallback = 32 KB.
 */
size_t get_l1_cache_size() {
#ifdef _WIN32
    DWORD bufferSize = 0;
    GetLogicalProcessorInformation(nullptr, &bufferSize);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(
            bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        if (GetLogicalProcessorInformation(buffer.data(), &bufferSize)) {
            for (auto &info : buffer) {
                if (info.Relationship == RelationCache && info.Cache.Level == 1) {
                    return info.Cache.Size;
                }
            }
        }
    }
    return 32 * 1024;
#else
    #ifdef HAS_SC_LEVEL1_DCACHE_SIZE
        long cacheSize = sysconf(_SC_LEVEL1_DCACHE_SIZE);
        if (cacheSize > 0) {
            return static_cast<size_t>(cacheSize);
        }
    #endif
    return 32 * 1024;
#endif
}
