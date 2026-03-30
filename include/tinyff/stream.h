#ifndef STREAM_H
#define STREAM_H

#include <stddef.h>


#define FF_NULL_STREAM {0}


// WARNING: This is just for reading streams for now
// Until I finally add encoding to PNG, it will remain read-only
// Note: I don't care, I'll add writing now. I was so corny back then.
// Now I listen to squee at 100% volume. his 2025 album is fire.
// another good album is apiary by gingerbee.
// oh yeah, and wetdream by willyrodriguezwastaken. that one is FIRE

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