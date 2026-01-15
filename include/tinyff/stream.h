#ifndef STREAM_H
#define STREAM_H

#include <stddef.h>
#include <stdio.h>


#define FF_NULL_STREAM {0}


// WARNING: This is just for reading streams for now
// Until I finally add encoding to PNG, it will remain read-only

typedef size_t (*ff_read_cb)(void *ptr, size_t size, void *user);

typedef struct {
    ff_read_cb read;
    void *user;
} ff_stream;

// Default reads for common types (just FILE for now, memory later)
size_t ff_file_read(void *ptr, size_t size, void *user);

// Stream creation helpers
ff_stream ff_create_file_stream(FILE *f);

#endif 