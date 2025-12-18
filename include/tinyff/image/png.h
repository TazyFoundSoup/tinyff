#ifndef PNG_H
#define PNG_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "tinyff/result.h"

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

ff_result ff_png_isvalid(FILE *file);
ff_png_ctx *ff_open_png(const char *filepath);

#endif
