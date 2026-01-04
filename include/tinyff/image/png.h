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
    {"IHDR", NULL},
    {"IDAT", NULL},
    {"IEND", NULL},


    {NULL, NULL} // Terminator
}; 

// Handler declarations
// Massive W.I.P

// Required by definition
void ff_png_header_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // IHDR
void ff_png_data_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // IDAT
void ff_png_end_handler(uint8_t *buf, size_t len, ff_png_ctx* ctx); // IEND

// PLTE is a funny one
// Its only required for indexed color types


ff_result ff_png_isvalid(FILE *file);
ff_result ff_open_png(const char *filepath, ff_png_ctx **out_ctx);

#endif
