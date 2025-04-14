#include "cache_utils.h"
#include <iostream>
#include <vector> 

#ifdef _WIN32
    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
    #endif
    #include <windows.h>
#elif __APPLE__
#include <sys/sysctl.h>
bool get_sysctl_value(const char* name, size_t &valueOut) {
    size_t len = sizeof(valueOut);
    if (sysctlbyname(name, &valueOut, &len, NULL, 0) == 0) {
        return true;
    }
    return false;
}
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
#elif __APPLE__
    // Attempt to read the macOS “hw.cachelinesize” sysctl
    size_t lineSize = 0;
    if (get_sysctl_value("hw.cachelinesize", lineSize)) {
        return lineSize;
    }
    // fallback
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
#elif __APPLE__
    // Attempt to read the macOS “hw.l1dcachesize” or “machdep.cpu.cache.size” 
    // but the latter is CPU-dependent. For Apple silicon M1/M2, some sysctl names differ.
    size_t l1Size = 0;
    // Some macs might not have “hw.l1dcachesize” – you can try “hw.l1icachesize”, “hw.l2cachesize”, etc.
    // Or you may not get a value at all, depending on the architecture.
    if (get_sysctl_value("hw.l1dcachesize", l1Size)) {
        return l1Size;
    }
    // fallback
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
