#ifndef RESULT_H
#define RESULT_H

typedef enum {
    // -- SUCCESS -- 
    FF_RESULT_OK = 0,

    // -- WARNINGS --
    FF_RESULT_WARN_DEBUG_DISABLED,
    FF_RESULT_WARN_NO_IMPL,

    // -- ERRORS --

    // File
    FF_RESULT_ERROR_INVALID_FILE_SIGNITURE,
    FF_RESULT_ERROR_INVALID_FILE,
    FF_RESULT_ERROR_READ_FILE_FAILURE,

    // Memory
    FF_RESULT_ERROR_MEMORY_ALLOCATION,

    // More will be created when more errors occur
} ff_result;

#endif