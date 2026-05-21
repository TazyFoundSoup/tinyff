#include <stdlib.h>
#include <tinyff/tinyff.h>
#include <tinyff/image/png.h>
#include <bridges/stdio/stream_bridge.h>
#include <tinyff/bench/bench.h>
#include <stdio.h>

ff_result fftest_rgb2x2(ff_ctx* ctx) {
    FILE* file = fopen("tests/format/image/png/assets/rgb2x2.png", "rb");
    if (!file) {
        printf("file failed to open\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_stream stream = ff_create_file_stream(file);

    ff_png_ctx* png_ctx = NULL;
    ff_result res = ff_open_png(ctx, &stream, &png_ctx, FF_ENABLE);

    fclose(file);
    ff_close_png(ctx, png_ctx);
    return res;
}

ff_result fftest_lenna256(ff_ctx* ctx) {
    FILE* file = fopen("tests/format/image/png/assets/lenna256.png", "rb");
    if (!file) {
        printf("file failed to open\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_stream stream = ff_create_file_stream(file);

    ff_png_ctx* png_ctx = NULL;
    ff_result res = ff_open_png(ctx, &stream, &png_ctx, FF_ENABLE);

    fclose(file);
    ff_close_png(ctx, png_ctx);
    ff_bench_print(ctx, &(ctx)->bench);
    return res;
}

int main() {
    ff_allocator alloc = { malloc, free, calloc };
    ff_ctx* ctx = ff_init(&alloc);

    ff_stream out = ff_create_file_stream(stdout);
    ff_set_debug_stream(&out, ctx);

    
    // rgb2x2
    ff_result res = fftest_rgb2x2(ctx);

    if (res == FF_RESULT_OK) {
        printf("rgb2x2: Works!\n");
    } else {
        printf("rgb2x2: Doesn't work :(\n");
    }
    
    
    // lenna256
    res = fftest_lenna256(ctx);
    if (res == FF_RESULT_OK) {
        printf("lenna256: Works!\n");
    } else {
        printf("lenna256: Doesn't work :(\n");
    }
    
    ff_cleanup(ctx);

    return 0;
}
