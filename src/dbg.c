#include <tinyff/dbg.h>

FILE *intff_debug_stream = NULL;
ff_flag intff_debug_enabled = 0;

ff_result ff_set_debug_stream(FILE *stream)
{
    intff_debug_stream = stream;
    intff_debug_enabled = (stream != NULL);

    ff_dprintf("tinyff 0.1.0 debug stream\n");
    ff_dprintf("For more information, visit\n");
    ff_dprintf("https://github.com/TazyFoundSoup/tinyff\n");
    ff_dprintf("------------------------------\n");

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
