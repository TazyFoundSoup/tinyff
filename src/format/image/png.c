#include "tinyff/image/png.h"
#include "tinyff/dbg.h"
#include "png.h"

ff_result ff_png_isvalid(FILE *file)
{
    ff_dprintf("png: validating signature\n");

    char raw_sig[8];
    if (fread(raw_sig, 1, 8, file) != 8) {
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
        return FF_RESULT_ERROR_INVALID_FILE_SIGNITURE;
    }

    ff_dprintf("png: signature valid\n");
    return FF_RESULT_OK;
}

ff_result ff_open_png(const char *filepath, ff_png_ctx **out_ctx)
{
    ff_dprintf("png: opening file '%s'\n", filepath);

    ff_png_ctx ctx;
    ctx.raw = fopen(filepath, "rb");

    if (!ctx.raw) {
        ff_dprintf("png: fopen failed\n");
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    ff_dprintf("png: file opened successfully\n");

    ff_result res = ff_png_isvalid(ctx.raw);
    if (res != FF_RESULT_OK) {
        ff_dprintf("png: validation failed (%d)\n", res);
        fclose(ctx.raw);
        return res;
    }

    ff_dprintf("png: validation passed\n");

    *out_ctx = NULL;
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

ff_result ff_png_chunk_palette_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: PLTE chunk received (len=%zu)\n", len);
    
    uint16_t num_entries = len / 3;
    ff_dprintf("png: PLTE contains %u palette entries\n", num_entries);

    

    return FF_RESULT_WARN_NO_IMPL;
}   

ff_result ff_png_data_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: IDAT chunk received (len=%zu)\n", len);
    
    // For now, compression handling is not implemented
    // and instead will assume data is uncompressed

    ff_dprintf("png: IDAT handler WIP - data is assumed uncompressed\n");
    ff_dprintf("png: IDAT handler WIP - expect undefined behavior\n");

    // We will need to loop over all the scanlines and apply filters
    // But im too lazy to do that right now
    // TODO implement
    

    return FF_RESULT_WARN_NO_IMPL;
}

ff_result ff_png_end_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: IEND chunk received (len=%zu)\n", len);
    ff_dprintf("png: all chunks received\n");

    return FF_RESULT_OK;
}