#include <tinyff/common.h>
#include <tinyff/result.h>

#ifndef GENERIC_IMAGE_H
#define GENERIC_IMAGE_H


// Tracks which format the image was decoded from

typedef enum {
    FF_IMAGE_ORIGIN_PNG,
    FF_IMAGE_ORIGIN_BMP
} ff_image_origin;

// Warning: Data is RGBA8888, scanlines top to bottom
typedef struct {
    uint32_t width;
    uint32_t height;
    ff_image_origin origin;

    uint8_t* data;

} ff_image_ctx;


#endif