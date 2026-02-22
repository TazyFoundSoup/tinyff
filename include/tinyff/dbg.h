#ifndef DBG_H
#define DBG_H

// Debug writes extra messages to a stream of the user's choice
// To enable debug, the user needs to include this header
// Then they can use the ff_set_debug_stream function to set the stream
// And then use ff_dprintf to print debug messages
// By default, debug is disabled


#define FF_DEBUG_ENABLED 1

#include <stdio.h>
#include <stdarg.h>
#include <tinyff/result.h>
#include <tinyff/common.h>

extern FILE *intff_debug_stream;

// This flag is the same name as FF_DEBUG_ENABLED but is local to this file
// Thats why it has the int prefix
extern ff_flag intff_debug_enabled;

// Sets the debug stream. If stream is NULL, debug is disabled.
ff_result ff_set_debug_stream(FILE *stream);

// Prints a string to the debug stream if debug is enabled.
// The call formatting is the same as printf
ff_result ff_dprintf(const char *format, ...);

#endif