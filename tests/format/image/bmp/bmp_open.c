#include <stdlib.h>
#include <tinyff/tinyff.h>
#include <tinyff/image/bmp.h>
#include <bridges/stdio/stream_bridge.h>
#include <stdio.h>
#include <tinyff/dbg.h>

ff_result fftest_rgb2x2(ff_ctx* ctx) {
    FILE* file = fopen("tests/format/image/bmp/assets/rgb2x2.bmp", "rb");
    if (!file) {
        printf("file failed to open\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_stream stream = ff_create_file_stream(file);

    ff_bmp_ctx* png_ctx = NULL;
    ff_result res = ff_open_bmp(ctx, &stream, &png_ctx, FF_ENABLE);

    fclose(file);

    return res;
}

int main() {
    ff_allocator alloc = { malloc, free, calloc };
    ff_ctx* ctx = ff_init(&alloc);

    ff_stream out = ff_create_file_stream(stdout);
    ff_set_debug_stream(&out, ctx);

    ff_result res = fftest_rgb2x2(ctx);

    if (res == FF_RESULT_WARN_NO_IMPL) {
        printf("Works!\n");
    } else {
        printf("Doesn't work :(\n");
    }

    ff_cleanup(ctx);

    return 0;
}
