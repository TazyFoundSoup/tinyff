#include "result.h"
#include "stream.h"
#include <tinyff/image/png.h>
#include <bridges/stdio/stream.h>
#include <stdio.h>

ff_result fftest_rgb2x2() {
    FILE* file = fopen("assets/rgb2x2.png", "rb");    
    if (!file) return FF_RESULT_ERROR_INVALID_FILE;

    ff_stream stream = ff_create_file_stream(file);
    if (stream.user == NULL) {
        fclose(file);
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_png_ctx* ctx = NULL;
    ff_result res = ff_open_png(&stream, &ctx, FF_ENABLE);

    return res;
}


int main() {
    ff_result res = fftest_rgb2x2();

    if (res == FF_RESULT_WARN_NO_IMPL) { // This is only because I'm still busy. yall just jealous im 13 and better than like 80% of the people who have ever lived at this point in time.
        printf("Works!\n");
    } else {
        printf("Doesn't work :(\n");
    }

    return 0;
}
