/**
 * @file png.h
 * @brief PNG image format handling for tinyff.
 * @author @TazyFoundSoup
 * 
 * This header defines structure and functions for working with PNG images,
 * including validation and opening PNG files.
 * 
 * @see src/format/image/png.c
 */

#ifndef PNG_H
#define PNG_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "tinyff/result.h"


/**
 * @brief PNG file signature (first 8 bytes)
 * 
 * Used to validate PNG files.
 * Will not be used for users directly.
 * 
 * @see ff_png_isvalid
 */
static const unsigned char PNG_SIGNATURE[8] = {
    0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A
};

/**
 * @brief PNG image context structure
 * 
 * Holds information about a PNG image, including its raw file pointer,
 * dimensions, bit depth, color type, pixel data, and validity status.
 */
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


/**
 * @brief PNG chunk types
 * 
 * Enumerates the different types of PNG chunks.
 * 
 * @see ff_png_chunk
 */
typedef enum {
    FF_PNG_CHUNK_TYPE_IHDR,
    FF_PNG_CHUNK_TYPE_PLTE,
    FF_PNG_CHUNK_TYPE_IDAT,
    FF_PNG_CHUNK_TYPE_IEND,

    // There are more,
    // but these ones will be
    // enough for now
} ff_png_chunk_type;

/**
 * @brief PNG chunk structure
 * 
 * Represents a PNG chunk, including its length, type, data, and CRC.
 * 
 * @see ff_png_chunk_type
 */
typedef struct {
    uint32_t length;
    ff_png_chunk_type type;
    uint32_t *data; // length bytes (maximum is uint32_t)
    uint32_t crc;
} ff_png_chunk;

/**
 * @brief Validate PNG from FILE pointer
 * 
 * Checks necessary validations for PNG files.
 * 
 * @param file An already opened FILE pointer located at start
 * @return A result indicating the success/failure of the validation
 * 
 * @warning FILE pointer must be at the start of the file, or unexpected results may occur.
 */
ff_result ff_png_isvalid(FILE *file);

/**
 * @brief Open a PNG file and create its context
 * 
 * Opens a PNG file, validates it, and prepares its context for further processing.
 * 
 * @param filepath The path to the PNG file
 * @param out_ctx Pointer to store the created PNG context
 * @return A result indicating the success/failure of the operation
 * 
 * @todo Implement parsing
 * @warning The caller is responsible for freeing the allocated ff_png_ctx and its resources.
 * 
 * 
 * @see ff_png_ctx
 */
ff_result ff_open_png(const char *filepath, ff_png_ctx **out_ctx);



#endif
