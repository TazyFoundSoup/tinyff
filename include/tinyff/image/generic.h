// This is the generics image header file for tinyff
// It contains generic image structures and definitions

#include <tinyff/common.h>
#include <tinyff/result.h>

#ifndef GENERIC_IMAGE_H
#define GENERIC_IMAGE_H


// The image origin is **not** the origin point of the image
// But rather from what the image was loaded

typedef enum {
    FF_IMAGE_ORIGIN_PNG,
} ff_image_origin;

// Only the basic information about an image
// Warning: Data is RGBA8888 using scanlines going from top to bottom
typedef struct {
    uint32_t width;
    uint32_t height;
    ff_image_origin origin;

    uint8_t* data;

} ff_image_ctx;


#endif