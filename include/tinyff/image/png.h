#ifndef PNG_H
#define PNG_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "tinyff/result.h"
#include "tinyff/common.h"


static const unsigned char PNG_SIGNATURE[8] = {
    0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A
};


typedef struct {
    FILE *raw;
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t *pixels;
    size_t count_pixels;
    bool valid;
} ff_png_ctx;



typedef enum {
    FF_PNG_CHUNK_TYPE_IHDR,
    FF_PNG_CHUNK_TYPE_PLTE,
    FF_PNG_CHUNK_TYPE_IDAT,
    FF_PNG_CHUNK_TYPE_IEND,

    // There are more,
    // but these ones will be
    // enough for now
} ff_png_chunk_type;


typedef struct {
    uint32_t length;
    ff_png_chunk_type type;
    uint32_t *data; // length bytes (maximum is uint32_t)
    uint32_t crc;
} ff_png_chunk;


ff_result ff_png_isvalid(FILE *file);
ff_result ff_open_png(const char *filepath, ff_png_ctx **out_ctx);
ff_result ff_next_chunk(FILE *file, ff_png_chunk **out_chunk);

#endif
