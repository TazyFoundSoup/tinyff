#ifndef STREAM_H
#define STREAM_H

#include <stddef.h>


#define FF_NULL_STREAM (ff_stream){0}



typedef size_t (*ff_read_cb)(void *ptr, size_t size, void *user);
typedef size_t (*ff_write_cb)(const void *ptr, size_t size, const void *user);
//                                                            ^
//                                                            |
//                                                  there clang, happy now?

typedef struct {
    ff_read_cb read;
    ff_write_cb write;
    void *user;
} ff_stream;

#endif 