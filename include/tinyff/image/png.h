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
    
    // Raw file handle
    FILE *raw;

    // Image dimensions
    uint32_t width;
    uint32_t height;

    // Color information
    uint8_t bit_depth;
    uint8_t color_type;

    // Image data
    //
    // It's weird because theres actually multiple ways to store it
    // we need to be able to have the same fields to hold the data
    // or have two seperate fields and a flag pointing to which one is actually
    // being used sorta because of palette vs direct color
    // so until i find a better way than holding this crappy project together with duct tape
    // im just gonna use two fields and a flag
    // but again with bit depths and color types this can get really messy really fast
    // and i have NO idea what what type direct color data should be stored as.
    // god... i hate png
    bool valid;
} ff_png_ctx;


typedef struct {
    uint32_t length;
    const char *type;
    uint32_t *data; // length bytes (maximum is uint32_t)
    uint32_t crc;
} ff_png_chunk;


// Chunk handling

typedef (*ff_png_chunk_handler_ptr)(uint8_t *buf, size_t len, ff_png_ctx* ctx);

typedef struct {
    const char *type;
    ff_png_chunk_handler_ptr handler;
} ff_png_chunk_handler;

const ff_png_chunk_handler ff_png_chunk_handlers[] = {
    {"IHDR", ff_png_header_handler},
    {"IDAT", ff_png_data_handler},
    {"IEND", ff_png_end_handler},
    {"PLTE", ff_png_palette_handler},   

    // From now on, the handlers will be for ancillary chunks


    {NULL, NULL} // Terminator
}; 

// Handler declarations
// Massive W.I.P

// Required by definition
ff_result ff_png_header_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // IHDR
ff_result ff_png_palette_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // PLTE
ff_result ff_png_data_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // IDAT
ff_result ff_png_end_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // IEND

// PLTE is a funny one
// Its only required for indexed color types


ff_result ff_png_isvalid(FILE *file);
ff_result ff_open_png(const char *filepath, ff_png_ctx **out_ctx);

#endif
