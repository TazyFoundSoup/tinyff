#ifndef RESULT_H
#define RESULT_H

typedef enum {
    FF_RESULT_OK = 0,
    FF_RESULT_ERROR_INVALID_FILE_SIGNITURE,
    FF_RESULT_ERROR_INVALID_FILE,

    // More will be created when more errors occur
} ff_result;

#endif