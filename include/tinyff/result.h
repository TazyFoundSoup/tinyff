#ifndef RESULT_H
#define RESULT_H

typedef enum {
    FF_RESULT_OK = 0,

    // File
    FF_RESULT_ERROR_INVALID_FILE_SIGNITURE,
    FF_RESULT_ERROR_INVALID_FILE,
    FF_RESULT_ERROR_READ_FILE_FAILURE,

    // More will be created when more errors occur
} ff_result;

#endif