// Common header for tinyff library
// Not recommended to be included directly
#ifndef TINYFF_COMMON_H
#define TINYFF_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <tinyff/stream.h>
#include <tinyff/compat.h>

#ifdef USE_THREAD
#include <pthread.h>
#endif

#ifdef USE_BENCH

#include <tinyff/bench/bench.h>

#define FF_BENCH_MARK(ctx, label) \
    do { \
        if (ctx) \
            ff_bench_mark(&(ctx)->bench, (label)); \
    } while (0)

#define FF_BENCH_START(ctx, label) \
    do { \
        if (ctx) \
            ff_bench_start(&(ctx)->bench, (label)); \
    } while (0)

#define FF_BENCH_END(ctx) \
    do { \
        if (ctx) \
            ff_bench_end(&(ctx)->bench); \
    } while (0)

#else

#define FF_BENCH_MARK(ctx, label) do {} while (0)
#define FF_BENCH_START(ctx, label) do {} while (0)
#define FF_BENCH_END(ctx) do {} while (0)

#endif

typedef bool ff_flag;

#define FF_ENABLE   true
#define FF_DISABLE  false

// Byte-order conversion

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

static inline void ff_write_be32(uint8_t *out, uint32_t v)
{
    out[0] = (v >> 24) & 0xFF;
    out[1] = (v >> 16) & 0xFF;
    out[2] = (v >> 8)  & 0xFF;
    out[3] = (v)       & 0xFF;
}

static inline void ff_write_le32(uint8_t *out, uint32_t v)
{
    out[0] = (v)        & 0xFF;
    out[1] = (v >> 8)   & 0xFF;
    out[2] = (v >> 16)  & 0xFF;
    out[3] = (v >> 24)  & 0xFF;
}

// Freestanding stdlib replacements
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

static inline void *ff_memset(void *dest, int ch, size_t count) {
    unsigned char *p = (unsigned char *)dest;
    while (count--) {
        *p++ = (unsigned char)ch;
    }
    return dest;
}

// Context and allocator

typedef struct {
    void* (*ff_alloc)(size_t size);
    void  (*ff_free)(void* ptr);
    void* (*ff_calloc)(size_t count, size_t size);
} ff_allocator;

typedef struct ff_ctx {
    ff_stream ff_debug_stream;
    ff_flag ff_debug_enabled;
    ff_allocator allocator;
    
    #ifdef USE_BENCH
    ff_bench bench;
    #endif

    #ifdef USE_THREAD
    /* mutex to protect ctx internal mutable state when threads are enabled */
    pthread_mutex_t ff_lock;
    #endif
} ff_ctx;

ff_ctx* ff_init(ff_allocator* allocator);
void ff_cleanup(ff_ctx* ctx);

#endif
