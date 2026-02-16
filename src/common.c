#include "tinyff/common.h"
#include "common.h"

inline uint32_t get_big_endian(const uint8_t *buffer) {
    return (uint32_t)(buffer[0] << 24) |
           (uint32_t)(buffer[1] << 16) |
           (uint32_t)(buffer[2] << 8)  |
           (uint32_t)(buffer[3]);
}

inline uint32_t get_little_endian(const uint8_t *buffer)
{
    return (uint32_t)(buffer[0])       |
           (uint32_t)(buffer[1] << 8)  |
           (uint32_t)(buffer[2] << 16) |
           (uint32_t)(buffer[3] << 24);
}