#include <tinyff/stream.h>
#include <tinyff/common.h>

// Initializes a new tinyff context.
ff_ctx *ff_init(ff_allocator *allocator) {
    if (!allocator) return NULL;

    ff_ctx *ctx = allocator->ff_alloc(sizeof(ff_ctx));

    if (!ctx) return NULL;

    ctx->allocator = *allocator;

    return ctx;
}

void ff_cleanup(ff_ctx *ctx) {
    if (!ctx) return;
    ctx->allocator.ff_free(ctx);
}
