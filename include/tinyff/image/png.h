#ifndef PNG_H
#define PNG_H

#include <stdint.h>
#include <stdbool.h>

#include <tinyff/tinyff.h>
#include <tinyff/common.h>
#include <tinyff/stream.h>
#include <tinyff/image/generic.h>
#include <tinyff/dbg.h>
#include <tinyff/math/core.h>



static const unsigned char PNG_SIGNATURE[8] = {
    0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A
};

typedef enum {
    FF_PNG_MODE_NONE = 0,
    FF_PNG_MODE_DIRECT_COLOR = 1, // pixels
    FF_PNG_MODE_PALETTE = -1       // palette
} ff_png_mode;

typedef struct {
    FF_BASE
    
    // Raw stream handle
    ff_stream *raw;

    // Image dimensions
    uint32_t width;
    uint32_t height;

    // Color information
    uint8_t bit_depth;
    uint8_t color_type;

    // Interlace method
    uint8_t interlace_method;

    // Image data
    
    ff_png_mode image_mode;

    union {
        uint8_t* pixels;
        uint8_t* imap; // Map of all the indices defines in PLTE
    } data;

    // Palette (if needed; can be left NULL if it's direct color)
    uint8_t* palette;
    uint32_t palette_size;

} ff_png_ctx;


typedef ff_result (*ff_png_chunk_handler_ptr)(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx* png_ctx);

typedef struct {
    const char *type;
    ff_png_chunk_handler_ptr handler;
} ff_png_chunk_handler;

// Handler declarations
// Massive W.I.P

// Required by definition
ff_result ff_png_header_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx* png_ctx); // IHDR
ff_result ff_png_palette_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx* png_ctx); // PLTE
ff_result ff_png_data_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx* png_ctx); // IDAT
ff_result ff_png_end_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx* png_ctx); // IEND

// Ancillary chunks
ff_result ff_png_trans_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx* png_ctx); // tRNS


extern const ff_png_chunk_handler ff_png_chunk_handlers[];




ff_result ff_png_isvalid(ff_ctx* ctx, ff_stream *stream);
ff_result ff_open_png(ff_ctx* ctx, ff_stream *stream, ff_png_ctx **out_ctx, ff_flag require_valid);
ff_result ff_close_png(ff_ctx* ctx, ff_png_ctx *png_ctx);

ff_result ff_png_normalize(ff_ctx* ctx, ff_png_ctx *png_ctx, ff_image_ctx **out_data, ff_flag consume);

#endif
