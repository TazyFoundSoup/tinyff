#include "result.h"
#include <tinyff/image/bmp.h>
#include <tinyff/dbg.h>

ff_result ff_bmp_isvalid(ff_stream* stream) 
{
    ff_dprintf("png: validating signature\n");

    char raw_sig[2];
    if (stream->read(raw_sig, 2, stream->user) != 2) {
        ff_dprintf("bmp: failed to read signature bytes\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_dprintf(
        "bmp: signature read: %02X %02X \n",
        raw_sig[0], raw_sig[1]
    );

    if (memcmp(raw_sig, BMP_SIGNATURE, 2) != 0) {
        ff_dprintf("bmp: signature mismatch\n");
        return FF_RESULT_ERROR_INVALID_PNG_SIGNATURE;
    }

    ff_dprintf("bmp: signature valid\n");
    return FF_RESULT_OK;
}

ff_result ff_open_bmp(ff_stream *stream, ff_bmp_ctx **out_ctx, ff_flag require_valid)
{

    ff_bmp_ctx *ctx = malloc(sizeof(ff_bmp_ctx));
    if (!ctx) return FF_RESULT_ERROR_MEMORY_ALLOCATION;

    // Init
    memset(ctx, 0, sizeof(*ctx));
    ctx->raw = stream;
    ctx->last_error = FF_RESULT_OK;


    if (ctx->raw->read == NULL) {
        ff_dprintf("bmp: stream failed to read\n");
        free(ctx);
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    ff_dprintf("bmp: stream read successfully\n");

    if (require_valid) {
        ff_result res = ff_bmp_isvalid(ctx->raw);
        if (res != FF_RESULT_OK) {
            ff_dprintf("bmp: validation failed (%d)\n", res);
            free(ctx);
            return res;
        }
    }

    ff_dprintf("bmp: validation passed\n");
    ctx->valid = true;

    *out_ctx = ctx;
    ff_dprintf("bmp: open_bmp reached WIP end\n");

    return FF_RESULT_WARN_NO_IMPL;
}

ff_result ff_bmp_header_handler(uint8_t *buf, size_t len, ff_bmp_ctx *ctx) {
    (void) buf;
    (void) ctx;
    
    if (len != FF_BMP_HEADER_SIZE) {
        ff_dprintf("bmp: bitmap header is not correct size (expected 14, got %d)\n", len);
        ctx->last_error = FF_RESULT_ERROR_INVALID_BMP_HEADER;
        return FF_RESULT_ERROR_INVALID_BMP_HEADER;
    }
    
    return FF_RESULT_WARN_NO_IMPL;
}