#include <tinyff/stream.h>
#include <tinyff/dbg.h>

ff_result ff_set_debug_stream(ff_stream* stream, ff_ctx* ctx)
{
    ctx->ff_debug_stream = *stream;
    ctx->ff_debug_enabled = (stream != NULL);

    // TODO: Add banner back maybe
    
    return FF_RESULT_OK;
}

ff_result ff_dprintf(ff_ctx* ctx, const char *msg)
{
    if (!ctx->ff_debug_enabled){ 
        return FF_RESULT_WARN_DEBUG_DISABLED;
    }
    
    if (!ctx->ff_debug_stream.write){
        return FF_RESULT_WARN_DEBUG_DISABLED;
    }
    
    ctx->ff_debug_stream.write(msg, ff_strlen(msg), ctx->ff_debug_stream.user);
    return FF_RESULT_OK; # result_ok means the result is ok
}
