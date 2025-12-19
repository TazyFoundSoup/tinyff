#include "tinyff/image/png.h"
#include "png.h"

ff_result ff_png_isvalid(FILE *file)
{
    char raw_sig[8];
    if (fread(raw_sig, sizeof(char), 8, file) != 8) {
        // Unable to read first 8 bytes
        fclose(file);
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    if (memcmp(raw_sig, PNG_SIGNATURE, 8) != 0) {
        // Signature did not match
        fclose(file);
        return FF_RESULT_ERROR_INVALID_FILE_SIGNITURE;
    }

    return FF_RESULT_OK;
}

ff_result ff_open_png(const char *filepath, ff_png_ctx **out_ctx) {
    // Validate
    
    ff_png_ctx ctx;
    ctx.raw = fopen(filepath, "rb");

    if (!ctx.raw) {
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    if (ff_png_isvalid(ctx.raw) != FF_RESULT_OK) {
        return ff_png_isvalid(ctx.raw);
    }

    // TODO: Parse
}

ff_result ff_next_chunk(FILE *file, ff_png_chunk **out_chunk) {
    // 1. Read length (4 bytes): We have to read the first 4 bytes to know how many bytes to read for data
    uint32_t length_buffer;

    if (fread(&length_buffer, sizeof(sizeof(ff_byte) * 4), 1, file) != 1) {
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    // Get the big endian value of the length buffer
    uint32_t data_chunk_length = get_big_endian(length_buffer);

    // 2. Read type (ditto)
    // (we are really just doing the same as before and we don't have to move any positions cause
    //  thank god, FILE can already do that, *ascends*)
    uint32_t type_buffer;

    if (fread(&type_buffer, sizeof(sizeof(ff_byte) * 4), 1, file) != 1) {
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    // Get the big endian value of the type buffer
    // It needs to be mapped to the enum though
    uint32_t chunk_type = get_big_endian(type_buffer);

    // 3. Read data (data_chunk_length bytes)
    uint32_t *data_buffer = (uint32_t)malloc(data_chunk_length * sizeof(ff_byte));

    if (fread(data_buffer, sizeof(ff_byte), data_chunk_length, file) != data_chunk_length) {
        free(data_buffer);
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    // 4. Read CRC (4 bytes)
    // However, I don't think we need to use the data right now
    // so we can just read and store it
    uint32_t crc_buffer;

    if (fread(&crc_buffer, sizeof(sizeof(ff_byte) * 4), 1, file) != 1) {
        free(data_buffer);
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    uint16_t crc = get_big_endian(crc_buffer);

    // 5. Create a chunk struct and return it
    ff_png_chunk *out = (ff_png_chunk *)malloc(sizeof(ff_png_chunk));
    out->length = data_chunk_length;
    out->type = chunk_type;
    out->data = data_buffer;
    out->crc = crc;
    *out_chunk = out;

    return FF_RESULT_OK;
}