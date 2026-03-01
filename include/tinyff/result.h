#ifndef RESULT_H
#define RESULT_H

typedef enum {
    FF_TRUE = 0,
    FF_FALSE = 1
} FF_FLAG;

typedef enum {
    // -- SUCCESS -- 
    FF_RESULT_OK = 0,

    // -- WARNINGS --
    FF_RESULT_WARN_DEBUG_DISABLED,
    FF_RESULT_WARN_NO_IMPL,

    // -- ERRORS --

    // File
    FF_RESULT_ERROR_INVALID_FILE,
    FF_RESULT_ERROR_READ_FILE_FAILURE,
    
    // Generic
    FF_RESULT_ERROR_OUT_OF_BOUNDS,

    // Media specific
    
    // PNG
    FF_RESULT_ERROR_INVALID_PNG_SIGNATURE,

    // Archive (more general archives terms)
    FF_RESULT_ERROR_DECOMPRESSION_FAILURE,

    // Memory
    FF_RESULT_ERROR_MEMORY_ALLOCATION,

    // More will be created when more errors occur
} ff_result;

#endif