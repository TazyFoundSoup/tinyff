#include "../bridges/stdio/stream.h"

size_t ff_file_read(void *ptr, size_t size, void *user)
{
    FILE *f = (FILE *)user;
    return fread(ptr, size, 1, f);
}

ff_stream ff_create_file_stream(FILE *f)
{
    ff_stream stream;
    stream.read = ff_file_read;
    stream.user = (void *)f;
    return stream;
}

// Memory stream coming soon