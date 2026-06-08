#ifndef BMP_H_
#define BMP_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


#include <tinyff/result.h>
#include <tinyff/stream.h>
#include <tinyff/image/generic.h>
#include <tinyff/tinyff.h>

#define FF_BMP_HEADER_SIZE 14
#define FF_BMP_INFO_SIZE 40
#define FF_BMP_COLOR_TABLE_SIZE(bpp, clrused) \
    ((bpp) <= 8 ? (4 * ((clrused) ? (clrused) : (1u << (bpp)))) : 0)

static const unsigned char BMP_SIGNATURE[2] = {
    'B', 'M'
};

typedef struct {
    FF_BASE
    
    // Raw stream handle
    ff_stream *raw;

    
    uint32_t file_size;
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
    
} ff_bmp_ctx;

typedef ff_result (*ff_bmp_section_handler_ptr)(ff_ctx* ctx, uint8_t *buf, size_t len, ff_bmp_ctx* bmp_ctx);

typedef struct {
    const char* type;
    ff_bmp_section_handler_ptr handler;
} ff_bmp_section_handler;

ff_result ff_bmp_header_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_bmp_ctx* bmp_ctx);
ff_result ff_bmp_info_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_bmp_ctx* bmp_ctx);
ff_result ff_bmp_color_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_bmp_ctx* bmp_ctx);
ff_result ff_bmp_data_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_bmp_ctx* bmp_ctx);

const ff_bmp_section_handler ff_bmp_section_handlers[] = {
    
    {"Header", ff_bmp_header_handler},
    {"InfoHeader", ff_bmp_info_handler},
    {"ColorTable", ff_bmp_color_handler},
    {"RasterData", ff_bmp_data_handler},
    
    {NULL, NULL} // Terminator
}; 

ff_result ff_bmp_isvalid(ff_ctx* ctx, ff_stream *stream);
ff_result ff_open_bmp(ff_ctx* ctx, ff_stream *stream, ff_bmp_ctx **out_ctx, ff_flag require_valid);


#endif // BMP_H_