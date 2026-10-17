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

ff_result ff_alloc(ff_ctx *ctx, size_t size, void **buffer) {
    if (!ctx || size <= 0 || !buffer) return FF_RESULT_BAD;

    // TODO: implement
    
    return FF_RESULT_WARN_NO_IMPL;
}

ff_result ff_reset(ff_ctx *ctx) {
    ctx->arena.offset = 0;
    return FF_RESULT_OK;
}
