#include <tinyff/dbg.h>
#include <tinyff/common.h>

#ifdef USE_HOSTED
#include <stdlib.h>
#endif


// Initializes a new tinyff context.
ff_ctx* ff_init(ff_allocator* allocator)
{
    
#ifdef USE_HOSTED
    ff_allocator default_alloc = { malloc, free, calloc };
    if (!allocator) allocator = &default_alloc;
#else
    if (!allocator) return NULL;
#endif

    ff_ctx* ctx = allocator->ff_alloc(sizeof(ff_ctx));

    if (!ctx) return NULL;

    ctx->ff_debug_enabled = FF_DISABLE;
    ctx->ff_debug_stream = NULL;

    ctx->allocator = *allocator;

    return ctx;
}

void ff_cleanup(ff_ctx *ctx) {
    if (!ctx) return;
    ff_dprintf(ctx, "goodbye from tinyff ;]");
    ctx->allocator.ff_free(ctx);    
    
}
