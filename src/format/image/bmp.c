#include <tinyff/image/bmp.h>
#include <tinyff/dbg.h>
#include <tinyff/result.h>
#include <tinyff/dbg.h>

ff_result ff_bmp_isvalid(ff_ctx* ctx, ff_stream* stream) 
{
    ff_dprintf(ctx, "png: validating signature\n");

    char raw_sig[2];
    if (stream->read(raw_sig, 2, stream->user) != 2) {
        ff_dprintf(ctx, "bmp: failed to read signature bytes\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_dprintf(ctx, // TODO: fix formatting
        "bmp: signature read \n"
    );

    if (memcmp(raw_sig, BMP_SIGNATURE, 2) != 0) {
        ff_dprintf(ctx, "bmp: signature mismatch\n");
        return FF_RESULT_ERROR_INVALID_PNG_SIGNATURE;
    }

    ff_dprintf(ctx, "bmp: signature valid\n");
    return FF_RESULT_OK;
}

ff_result ff_open_bmp(ff_ctx* ctx, ff_stream *stream, ff_bmp_ctx **out_ctx, ff_flag require_valid)
{

    ff_bmp_ctx *bmp_ctx = ctx->allocator.ff_alloc(sizeof(ff_bmp_ctx));
    if (!ctx) return FF_RESULT_ERROR_MEMORY_ALLOCATION;

    // Init
    memset(ctx, 0, sizeof(*ctx));
    bmp_ctx->raw = stream;
    bmp_ctx->last_error = FF_RESULT_OK;


    if (bmp_ctx->raw->read == NULL) {
        ff_dprintf(ctx, "bmp: stream failed to read\n");
        ctx->allocator.ff_free(bmp_ctx);
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    ff_dprintf(ctx, "bmp: stream read successfully\n");

    if (require_valid) {
        ff_result res = ff_bmp_isvalid(ctx, bmp_ctx->raw);
        if (res != FF_RESULT_OK) {
            ff_dprintf(ctx, "bmp: validation failed\n"); // TODO: Formatting
            ctx->allocator.ff_free(bmp_ctx);
            return res;
        }
    }

    ff_dprintf(ctx, "bmp: validation passed\n");
    bmp_ctx->valid = true;

    *out_ctx = bmp_ctx;
    ff_dprintf(ctx, "bmp: open_bmp reached WIP end\n");

    return FF_RESULT_WARN_NO_IMPL;
}

ff_result ff_bmp_header_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_bmp_ctx *bmp_ctx) {
    (void) buf;
    (void) bmp_ctx;

    if (len != FF_BMP_HEADER_SIZE) {
        ff_dprintf(ctx, "bmp: bitmap header is not correct size (expected 14, got [n length])\n"); // TODO: formatting
        bmp_ctx->last_error = FF_RESULT_ERROR_INVALID_BMP_HEADER;
        return FF_RESULT_ERROR_INVALID_BMP_HEADER;
    }
    
    // We've already read the signiture so we just jump straight to 
    // the fileSize part
    
    bmp_ctx->file_size = buf[4];
    
    return FF_RESULT_WARN_NO_IMPL;
}