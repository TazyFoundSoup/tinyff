#ifndef PNG_H
#define PNG_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "tinyff/result.h"
#include "tinyff/common.h"
#include "tinyff/stream.h"

// External
#include "ext/tinf.h"


static const unsigned char PNG_SIGNATURE[8] = {
    0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A
};

typedef enum {
    FF_PNG_MODE_NONE = 0,
    FF_PNG_MODE_DIRECT_COLOR, // pixels
    FF_PNG_MODE_PALETTE       // palette
} ff_png_mode;

typedef struct {
    // Raw stream handle
    ff_stream *raw;

    // Image dimensions
    uint32_t width;
    uint32_t height;

    // Color information
    uint8_t bit_depth;
    uint8_t color_type;

    // Image data
    
    ff_png_mode image_mode;

    union {
        uint8_t* pixels;
        uint8_t* imap; // Map of all the indices defines in PLTE
    } data;

    // Palette (if needed; can be left NULL if it's direct color)
    uint8_t* palette;
    uint32_t palette_size;

    bool valid;
    ff_result last_error;

} ff_png_ctx;

// Small BpP helper function

static inline uint16_t ff_png_bpp(ff_png_ctx *ctx)
{
    int16_t byte_depth = ctx->bit_depth / 8;
    switch (ctx->color_type) {
        case 0: // Grayscale
            return byte_depth * 1; // Gray
        case 2: // Truecolor
            return byte_depth * 3; // R, G, B
        case 3: // Indexed-color
            return byte_depth * 1; // Index
        case 4: // Grayscale with alpha
            return byte_depth * 2; // Gray w/ alpha
        case 6: // Truecolor with alpha
            return byte_depth * 4; // R, G, B w/ alpha
        default:
            return 0; // damn, you corrupt
    }
}


// Chunk handling

typedef (*ff_png_chunk_handler_ptr)(uint8_t *buf, size_t len, ff_png_ctx* ctx);

typedef struct {
    const char *type;
    ff_png_chunk_handler_ptr handler;
} ff_png_chunk_handler;

// Handler declarations
// Massive W.I.P

// Required by definition
ff_result ff_png_header_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // IHDR
ff_result ff_png_palette_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // PLTE
ff_result ff_png_data_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // IDAT
ff_result ff_png_end_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // IEND


const ff_png_chunk_handler ff_png_chunk_handlers[] = {
    {"IHDR", ff_png_header_handler},
    {"IDAT", ff_png_data_handler},
    {"IEND", ff_png_end_handler},
    {"PLTE", ff_png_palette_handler},   

    // From now on, the handlers will be for ancillary chunks


    {NULL, NULL} // Terminator
}; 




ff_result ff_png_isvalid(ff_stream *stream);
ff_result ff_open_png(ff_stream *stream, ff_png_ctx **out_ctx);

#endif
