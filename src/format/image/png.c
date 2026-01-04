#include "tinyff/image/png.h"
#include "png.h"



ff_result ff_png_isvalid(FILE *file)
{
    char raw_sig[8];
    if (fread(raw_sig, sizeof(char), 8, file) != 8) {
        // Unable to read first 8 bytes
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    if (memcmp(raw_sig, PNG_SIGNATURE, 8) != 0) {
        // Signature did not match
        return FF_RESULT_ERROR_INVALID_FILE_SIGNITURE;
    }

    return FF_RESULT_OK;
}

ff_result ff_open_png(const char *filepath, ff_png_ctx **out_ctx) {
    // Validate
    
    ff_png_ctx ctx;
    ctx.raw = fopen(filepath, "rb");

    if (!ctx.raw) {
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    if (ff_png_isvalid(ctx.raw) != FF_RESULT_OK) {
        return ff_png_isvalid(ctx.raw);
    }

    // This is a heavy W.I.P
    
}

void ff_png_header_handler(uint8_t *data, size_t len, ff_png_ctx *ctx)
{
    if (len != 13) return; // Invalid length
    printf("Width:              %d\n", get_big_endian(data));
    printf("Height:             %d\n", get_big_endian(data + 4));
    printf("Bit depth:          %d\n", (unsigned char)data[8]);
    printf("Color type:         %d\n", (unsigned char)data[9]);
    printf("Compression method: %d\n", (unsigned char)data[10]);
    printf("Filter method:      %d\n", (unsigned char)data[11]);
    printf("Interlace method:   %d\n", (unsigned char)data[12]);

    // Store in context
    ctx->width = get_big_endian(data);
    ctx->height = get_big_endian(data + 4);
    ctx->bit_depth = (unsigned char)data[8];
    ctx->color_type = (unsigned char)data[9];
}