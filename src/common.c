#include "tinyff/result.h"
#include <tinyff/stream.h>
#include <tinyff/common.h>

ff_result ff_init(ff_ctx *ctx, void *buffer, size_t capacity) {
    if (!ctx || !buffer) return FF_RESULT_BAD;
    
    ctx->arena.buffer = (uint8_t *)buffer;
    ctx->arena.capacity = capacity;
    ctx->arena.offset = 0;

    return FF_RESULT_OK;
}

ff_result ff_cleanup(ff_ctx *ctx) {
    if (!ctx) return FF_RESULT_BAD;
    (void)ctx;
    return FF_RESULT_OK;
}
