#ifndef BMP_H_
#define BMP_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "tinyff/result.h"
#include "tinyff/common.h"
#include "tinyff/stream.h"
#include "tinyff/image/generic.h"

static const unsigned char BMP_SIGNATURE[2] = {
    'B', 'M'
};

typedef struct {
    uint32_t file_size; // tbh, i actually have no idea why this is needed
    uint32_t data_offset;
    
    uint32_t width;
    uint32_t height;
    uint16_t bpp;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_ppm; // X pixels per meter
    uint32_t y_ppm; // Y pixels per meter
    uint32_t colors_used;
    uint32_t important_colors;

    
} ff_bmp_ctx;

#endif BMP_H_