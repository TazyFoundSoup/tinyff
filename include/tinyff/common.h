// Common header for tinyff library
// Not recommended to be included directly
#ifndef TINYFF_COMMON_H
#define TINYFF_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <tinyff/stream.h>

#ifdef USE_BENCH

#include <tinyff/bench.h>

#endif

// Flags
typedef bool ff_flag;

#define FF_ENABLE   true
#define FF_DISABLE  false

// Functions

static inline uint32_t ff_be32(const uint8_t *b) {
    return ((uint32_t)b[0] << 24) |
           ((uint32_t)b[1] << 16) |
           ((uint32_t)b[2] << 8)  |
           ((uint32_t)b[3]);
}

static inline uint32_t ff_le32(const uint8_t *b) {
    return ((uint32_t)b[0])       |
           ((uint32_t)b[1] << 8)  |
           ((uint32_t)b[2] << 16) |
           ((uint32_t)b[3] << 24);
}


// Small stdlib helper functions
static inline size_t ff_strlen(const char *str)
{
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

static inline int ff_memcmp(const void *a, const void *b, size_t n)
{
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    for (size_t i = 0; i < n; i++) {
        if (pa[i] != pb[i]) return pa[i] - pb[i];
    }
    return 0;
}

static inline void ff_memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *pd = (uint8_t *)dest;
    const uint8_t *ps = (const uint8_t *)src;
    for (size_t i = 0; i < n; i++) {
        pd[i] = ps[i];
    }
}


// Context bases
// Default

typedef struct {
    void* (*ff_alloc)(size_t size);
    void  (*ff_free)(void* ptr);
    void* (*ff_calloc)(size_t count, size_t size);
} ff_allocator;

typedef struct {
    // Debug settings
    ff_stream ff_debug_stream;
    ff_flag ff_debug_enabled;
    
    // Allocation
    ff_allocator allocator;
    
    #ifdef USE_BENCH
    ff_bench bench;
    #endif
} ff_ctx;

ff_ctx* ff_init(ff_allocator* allocator);
void ff_cleanup(ff_ctx* ctx);

#endif