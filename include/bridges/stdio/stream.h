#include <stdio.h>
#include <tinyff/stream.h>

// Default reads for common types (just FILE for now, memory later)
size_t ff_file_read(void *ptr, size_t size, void *user);

// Stream creation helpers
ff_stream ff_create_file_stream(FILE *f);