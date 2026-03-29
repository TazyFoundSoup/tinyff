// Common header for tinyff library
// Not recommended to be included directly
#ifndef TINYFF_COMMON_H
#define TINYFF_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <tinyff/stream.h>

// Flags
typedef bool ff_flag;

#define FF_ENABLE   true
#define FF_DISABLE  false

// Functions

static inline uint32_t get_big_endian(const uint8_t *buffer) {
    return (uint32_t)(buffer[0] << 24) |
           (uint32_t)(buffer[1] << 16) |
           (uint32_t)(buffer[2] << 8)  |
           (uint32_t)(buffer[3]);
}

static inline uint32_t get_little_endian(const uint8_t *buffer)
{
    return (uint32_t)(buffer[0])       |
           (uint32_t)(buffer[1] << 8)  |
           (uint32_t)(buffer[2] << 16) |
           (uint32_t)(buffer[3] << 24);
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


// Context bases
// Default

typedef struct {
    void* (*ff_alloc)(size_t size);
    void  (*ff_free)(void* ptr);
    void* (*ff_calloc)(size_t count, size_t size);
} ff_allocator;

typedef struct {
    // Debug settings
    ff_stream* ff_debug_stream;
    ff_flag ff_debug_enabled;
    
    // Allocation
    ff_allocator allocator;
} ff_ctx;

ff_ctx* ff_init(ff_allocator* allocator);
void ff_cleanup(ff_ctx* ctx);


// Small math functions
// TODO: Implement a full math library for tinyff

#define FF_ABS(a) (((a) < 0) ? -(a) : (a))

#endif