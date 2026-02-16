// Common header for tinyff library
// Not recommended to be included directly
#ifndef TINYFF_COMMON_H
#define TINYFF_COMMON_H

#include <stdint.h>
#include <stdbool.h>

// Flags
typedef bool ff_flag;

#define FF_ENABLE   true
#define FF_DISABLE  false

// Functions

inline uint32_t get_big_endian(const uint8_t *buffer);
inline uint32_t get_little_endian(const uint8_t *buffer);

#endif