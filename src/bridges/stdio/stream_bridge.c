#include <tinyff/stream.h>
#ifdef USE_HOSTED
#include <bridges/stdio/stream_bridge.h>


size_t ff_file_read(void *ptr, size_t size, void *user)
{
    FILE *f = (FILE *)user;
    return fread(ptr, 1, size, f);
}

size_t ff_file_write(const void *ptr, size_t size, const void *user)
{
    FILE *f = (FILE *)user;
    return fwrite(ptr, 1, size, f);
}

ff_stream ff_create_file_stream(FILE *f)
{
    ff_stream stream;
    stream.read = ff_file_read;
    stream.write = ff_file_write;
    stream.user = (void *)f;
    return stream;
}

// Memory stream coming soon


#endif