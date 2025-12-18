#include "tinyff/image/png.h"
#include "png.h"

ff_result ff_png_isvalid(FILE *file)
{
    char raw_sig[8];
    if (fread(raw_sig, sizeof(char), 8, file) != 8) {
        // Inable to read first 8 bytes
        fclose(file);
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    if (memcmp(raw_sig, PNG_SIGNATURE, 8) != 0) {
        // Signiture did not match
        fclose(file);
        return FF_RESULT_ERROR_INVALID_FILE_SIGNITURE;
    }

    return FF_RESULT_OK;
}

ff_png_ctx *ff_open_png(const char *filepath) {
    // Validate
    
    ff_png_ctx ctx;
    ctx.raw = fopen(filepath, "rb");

    if (!ctx.raw) {
        return NULL;
    }

    if (ff_png_isvalid(ctx.raw) != FF_RESULT_OK) {
        return NULL;
    }

    // TODO: Parse
}