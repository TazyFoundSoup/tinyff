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
    // Raw stream handle
    ff_stream *raw;

    
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

    uint8_t* palette; 
    
    uint8_t* pixels;
    

    bool valid;
    ff_result last_error;
} ff_bmp_ctx;

ff_result ff_bmp_isvalid(ff_stream *stream);
ff_result ff_open_bmp(ff_stream *stream, ff_bmp_ctx **out_ctx);


#endif BMP_H_