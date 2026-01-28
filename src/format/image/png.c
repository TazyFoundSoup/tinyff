#include "tinyff/image/png.h"
#include "tinyff/dbg.h"
#include "png.h"

ff_result ff_png_isvalid(ff_stream *stream)
{
    ff_dprintf("png: validating signature\n");

    char raw_sig[8];
    if (stream->read(raw_sig, 8, stream->user) != 8) {
        ff_dprintf("png: failed to read signature bytes\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_dprintf(
        "png: signature read: %02X %02X %02X %02X %02X %02X %02X %02X\n",
        raw_sig[0], raw_sig[1], raw_sig[2], raw_sig[3],
        raw_sig[4], raw_sig[5], raw_sig[6], raw_sig[7]
    );

    if (memcmp(raw_sig, PNG_SIGNATURE, 8) != 0) {
        ff_dprintf("png: signature mismatch\n");
        return FF_RESULT_ERROR_INVALID_PNG_SIGNITURE;
    }

    ff_dprintf("png: signature valid\n");
    return FF_RESULT_OK;
}

ff_result ff_open_png(ff_stream *stream, ff_png_ctx **out_ctx)
{

    ff_png_ctx *ctx = malloc(sizeof(ff_png_ctx));
    if (!ctx) return FF_RESULT_ERROR_MEMORY_ALLOCATION;

    // Init
    memset(ctx, 0, sizeof(*ctx));
    ctx->image_mode = FF_PNG_MODE_NONE;
    ctx->raw = stream;
    ctx->last_error = FF_RESULT_OK;


    if (ctx->raw->read == NULL) {
        ff_dprintf("png: stream failed to read\n");
        free(ctx);
        ctx->last_error = FF_RESULT_ERROR_READ_FILE_FAILURE;
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    ff_dprintf("png: stream read successfully\n");

    ff_result res = ff_png_isvalid(ctx->raw);
    if (res != FF_RESULT_OK) {
        ff_dprintf("png: validation failed (%d)\n", res);
        free(ctx);
        ctx->last_error = res;
        return res;
    }

    ff_dprintf("png: validation passed\n");
    ctx->valid = true;

    *out_ctx = ctx;
    ff_dprintf("png: open_png reached WIP end\n");

    return FF_RESULT_OK;
}

// Handlers

ff_result ff_png_header_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: IHDR chunk received (len=%zu)\n", len);

    if (len != 13) {
        ff_dprintf("png: invalid IHDR length\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    uint32_t w = get_big_endian(buf);
    uint32_t h = get_big_endian(buf + 4);

    ff_dprintf("Width:              %u\n", w);
    ff_dprintf("Height:             %u\n", h);
    ff_dprintf("Bit depth:          %u\n", buf[8]);
    ff_dprintf("Color type:         %u\n", buf[9]);
    ff_dprintf("Compression method: %u\n", buf[10]);
    ff_dprintf("Filter method:      %u\n", buf[11]);
    ff_dprintf("Interlace method:   %u\n", buf[12]);

    ctx->width = w;
    ctx->height = h;
    ctx->bit_depth = buf[8];
    ctx->color_type = buf[9];

    ff_dprintf("png: IHDR stored in context\n");

    return FF_RESULT_OK;
}

ff_result ff_png_palette_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: PLTE chunk received (len=%zu)\n", len);
    
    if (len % 3 != 0) {
        ff_dprintf("png: invalid PLTE length\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    uint16_t num_entries = len / 3;
    ff_dprintf("png: PLTE contains %u palette entries\n", num_entries);
    
    ctx->palette_size = num_entries;
    ctx->palette = malloc(len);
    if (!ctx->palette) return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    
    for (uint16_t i = 0; i < num_entries; i++) {
        // There's 4 bytes per entry just because if there's
        // a tRNS chunk later, we can just fill in the alpha values
        ctx->palette[i * 4 + 0] = buf[i * 3 + 0]; // R
        ctx->palette[i * 4 + 1] = buf[i * 3 + 1]; // R
        ctx->palette[i * 4 + 2] = buf[i * 3 + 2]; // R
    }
    
    return FF_RESULT_OK;
}   

ff_result ff_png_data_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: IDAT chunk received (len=%zu)\n", len);
    
    // Update: Now I got tinf integrated, I can actually implement this

    uint8_t *uncompressed_data = NULL;
    
    // Getting the size is weird cause it could the sample count
    // could be different based on color type and bit depth
    // Update: Now we have a bpp helper function
    size_t uncompressed_size = ctx->width * ctx->height * ff_png_bpp(ctx);

    uncompressed_data = malloc(uncompressed_size);

    if (!uncompressed_data) {
        ff_dprintf("png: failed to allocate memory for uncompressed data\n");
        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    }
    
    if (tinf_uncompress(uncompressed_data, uncompressed_size, buf, len) != TINF_OK) {
        ff_dprintf("png: failed to uncompress IDAT data\n");
        free(uncompressed_data);
        return FF_RESULT_ERROR_DECOMPRESSION_FAILURE;
    }

    return FF_RESULT_WARN_NO_IMPL;
}

ff_result ff_png_end_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: IEND chunk received (len=%zu)\n", len);
    ff_dprintf("png: all chunks received\n");

    return FF_RESULT_OK;
}