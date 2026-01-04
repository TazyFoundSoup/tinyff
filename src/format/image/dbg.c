#include "dbg.h"

ff_result ff_set_debug_stream(FILE *stream)
{
    intff_debug_stream = stream;
    intff_debug_enabled = (stream != NULL);
    return FF_RESULT_OK;
}

ff_result ff_dprintf(const char *format, ...)
{
    if (!intff_debug_enabled || !intff_debug_stream) return FF_RESULT_WARN_DEBUG_DISABLED;

    va_list args;
    va_start(args, format);
    vfprintf(intff_debug_stream, format, args);
    va_end(args);

    return FF_RESULT_OK;
}
