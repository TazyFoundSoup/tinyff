#ifndef DBG_H
#define DBG_H

// Debug writes extra messages to a stream of the user's choice
// To enable debug, the user needs to include this header
// Then they can use the ff_set_debug_stream function to set the stream
// And then use ff_dprintf to print debug messages
// By default, debug is disabled


#define FF_DEBUG_ENABLED 1

#include <tinyff/result.h>
#include <tinyff/common.h>
#include <tinyff/stream.h>

#include <stdarg.h>


// Sets the debug stream. If stream is NULL, debug is disabled.
ff_result ff_set_debug_stream(ff_stream* stream, ff_ctx* ctx);

// Prints a string to the debug stream if debug is enabled.
ff_result ff_dprintf(ff_ctx* ctx, const char *msg, ...);

#endif