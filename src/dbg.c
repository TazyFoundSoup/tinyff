#include <tinyff/dbg.h>

ff_result ff_set_debug_stream(ff_stream* stream, ff_ctx* ctx)
{
    ctx->ff_debug_stream = stream;
    ctx->ff_debug_enabled = (stream != NULL);

    ff_dprintf(ctx, "tinyff 0.1.0 debug stream\n");
    ff_dprintf(ctx, "For more information, visit\n");
    ff_dprintf(ctx, "https://github.com/TazyFoundSoup/tinyff\n");
    ff_dprintf(ctx, "------------------------------\n");

    return FF_RESULT_OK;
}

ff_result ff_dprintf(ff_ctx* ctx, const char *msg)
{
    if (!ctx->ff_debug_enabled || !ctx->ff_debug_stream) return FF_RESULT_WARN_DEBUG_DISABLED;

    ctx->ff_debug_stream->write(msg, ff_strlen(msg), ctx->ff_debug_stream->user);

    return FF_RESULT_OK;
}
