// Common header for tinyff library
// Not recommended to be included directly
#ifndef TINYFF_COMMON_H
#define TINYFF_COMMON_H

#include <stdint.h>

// Human readable number types
// NOTE: The only reason I created these was so
// that debugging will be easier
typedef uint8_t ff_byte;

// Functions

static inline uint32_t get_big_endian(const uint8_t *buffer);

#endif