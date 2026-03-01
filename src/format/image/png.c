#include <tinyff/image/png.h>
#include <tinyff/result.h>
#include <tinyff/common.h>
#include <tinyff/dbg.h>

// External
#include <ext/tinf.h>

ff_result ff_png_isvalid(ff_stream *stream)
{
    ff_dprintf("png: validating signature\n");
    
    char raw_sig[8];
    if (stream->read(raw_sig, 8, stream->user) != 8) {
        ff_dprintf("png: failed to read signature bytes\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_dprintf(
        "png: signature read: %02X %02X %02X %02X %02X %02X %02X %02X\n",
        raw_sig[0], raw_sig[1], raw_sig[2], raw_sig[3],
        raw_sig[4], raw_sig[5], raw_sig[6], raw_sig[7]
    );

    if (memcmp(raw_sig, PNG_SIGNATURE, 8) != 0) {
        ff_dprintf("png: signature mismatch\n");
        return FF_RESULT_ERROR_INVALID_PNG_SIGNATURE;
    }

    ff_dprintf("png: signature valid\n");

    return FF_RESULT_OK;
}

ff_result ff_open_png(ff_stream *stream, ff_png_ctx **out_ctx, ff_flag require_valid)
{
    
    (void)require_valid; // Just cast to void for now until I impl
    
    ff_png_ctx *ctx = malloc(sizeof(ff_png_ctx));
    if (!ctx) {
        ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    }

    // Init
    memset(ctx, 0, sizeof(*ctx));
    ctx->image_mode = FF_PNG_MODE_NONE;
    ctx->raw = stream;
    ctx->last_error = FF_RESULT_OK;


    if (ctx->raw->read == NULL) {
        ff_dprintf("png: stream failed to read\n");
        free(ctx);
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    ff_dprintf("png: stream read successfully\n");

    ff_result res = ff_png_isvalid(ctx->raw);
    if (res != FF_RESULT_OK) {
        ff_dprintf("png: validation failed (%d)\n", res);
        free(ctx);
        return res;
    }

    ff_dprintf("png: validation passed\n");
    ctx->valid = true;

    *out_ctx = ctx;
    ff_dprintf("png: open_png reached WIP end\n");

    ctx->last_error = FF_RESULT_WARN_NO_IMPL;
    return FF_RESULT_WARN_NO_IMPL;
}

// Handlers

inline uint16_t ff_png_bpp(ff_png_ctx *ctx)
{
    int16_t byte_depth = ctx->bit_depth / 8;
    switch (ctx->color_type) {
        case 0: // Grayscale
            return byte_depth * 1; // Gray
        case 2: // Truecolor
            return byte_depth * 3; // R, G, B
        case 3: // Indexed-color
            return byte_depth * 1; // Index
        case 4: // Grayscale with alpha
            return byte_depth * 2; // Gray w/ alpha
        case 6: // Truecolor with alpha
            return byte_depth * 4; // R, G, B w/ alpha
        default:
            return 0; // damn, you corrupt
    }
}

ff_result ff_png_header_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: IHDR chunk received (len=%zu)\n", len);

    if (len != 13) {
        ff_dprintf("png: invalid IHDR length\n");

        ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    uint32_t w = get_big_endian(buf);
    uint32_t h = get_big_endian(buf + 4);

    ff_dprintf("Width:              %u\n", w);
    ff_dprintf("Height:             %u\n", h);
    ff_dprintf("Bit depth:          %u\n", buf[8]);
    ff_dprintf("Color type:         %u\n", buf[9]);
    ff_dprintf("Compression method: %u\n", buf[10]);
    ff_dprintf("Filter method:      %u\n", buf[11]);
    ff_dprintf("Interlace method:   %u\n", buf[12]);

    ctx->width = w;
    ctx->height = h;
    ctx->bit_depth = buf[8];
    ctx->color_type = buf[9];
    ctx->interlace_method = buf[12];

    ff_dprintf("png: IHDR stored in context\n");

    ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_OK;
}

ff_result ff_png_palette_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: PLTE chunk received (len=%zu)\n", len);
    
    if (len % 3 != 0) {
        ff_dprintf("png: invalid PLTE length\n");

        ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    uint16_t num_entries = len / 3;
    ff_dprintf("png: PLTE contains %u palette entries\n", num_entries);
    
    ctx->palette_size = num_entries;
    ctx->palette = malloc(len);
    if (!ctx->palette) {
        ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    }
    
    for (uint16_t i = 0; i < num_entries; i++) {
        // There's 4 bytes per entry just because if there's
        // a tRNS chunk later, we can just fill in the alpha values
        ctx->palette[i * 4 + 0] = buf[i * 3 + 0]; // R
        ctx->palette[i * 4 + 1] = buf[i * 3 + 1]; // G
        ctx->palette[i * 4 + 2] = buf[i * 3 + 2]; // B
    }
    
    ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_OK;
}   

ff_result ff_png_data_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    ff_dprintf("png: IDAT chunk received (len=%zu)\n", len);

    uint8_t *uncompressed_data = NULL;
    
    // Getting the size is weird cause it could the sample count
    // could be different based on color type and bit depth
    size_t uncompressed_size = ctx->width * ctx->height * ff_png_bpp(ctx);

    uncompressed_data = malloc(uncompressed_size);

    if (!uncompressed_data) {
        ff_dprintf("png: failed to allocate memory for uncompressed data\n");

        ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    }
    
    if (tinf_uncompress(uncompressed_data, (unsigned int *)uncompressed_size, buf, len) != TINF_OK) {
        ff_dprintf("png: failed to uncompress IDAT data\n");
        free(uncompressed_data);

        ctx->last_error = FF_RESULT_ERROR_DECOMPRESSION_FAILURE;
        return FF_RESULT_ERROR_DECOMPRESSION_FAILURE;
    }

    ff_dprintf("png: IDAT data uncompressed successfully\n");

    // Now we parse the uncompressed data into our pixel buffer
    // However I don't understand Adam7 so interlaced images are not supported yet
    if (ctx->interlace_method != 0) {
        ff_dprintf("png: interlaced images are not supported yet\n");
        free(uncompressed_data);

        ctx->last_error = FF_RESULT_WARN_NO_IMPL;
        return FF_RESULT_WARN_NO_IMPL;
    }

    // Now I am very inexperienced so I'm doing what I think is right
    // TODO: Review this code later


    // Filtering

    size_t bpp = ff_png_bpp(ctx);
    uint8_t *reconstructed_buf = (uint8_t *) malloc(ctx->width * bpp); // This is the current reconstructed scanline
    uint8_t *previous_row = (uint8_t *) calloc(ctx->width * bpp, 1); // This is the prior scanline (reconstructed)



    // Iterate over every scanline
    for (int iline = 0; (uint32_t) iline < ctx->height; iline++) {
        // Make some variables so it's not hella unreadable
        size_t scanline_start = iline * (1 + (ctx->width * bpp));
        uint8_t filter_type = uncompressed_data[scanline_start];
        uint8_t *raw = &uncompressed_data[scanline_start + 1];

        
        // Now I need to iterate over the entire scanline and apply the filter
        for (size_t x = 0; x < ctx->width * bpp; x++) {
            uint8_t left = (x >= bpp) ? reconstructed_buf[x - bpp] : 0;
            uint8_t above = previous_row[x];
            uint8_t diagonal = (x >= bpp) ? previous_row[x - bpp] : 0;

            switch (filter_type) {
                case 0: // No filter
                    reconstructed_buf[x] = raw[x];
                    break;
                case 1: // Sub
                    reconstructed_buf[x] = raw[x] + left;
                    break;
                case 2: // Up
                    reconstructed_buf[x] = raw[x] + above;
                    break;
                case 3: // Average
                    reconstructed_buf[x] = raw[x] + ((left + above) / 2);
                    break;
                case 4: // Paeth Predictor
                    {
                        int p = left + above - diagonal;
                        int pa = abs(p - left);
                        int pb = abs(p - above);
                        int pc = abs(p - diagonal);

                        if (pa <= pb && pa <= pc) 
                            reconstructed_buf[x] = raw[x] + left;
                        else if (pb <= pc)
                            reconstructed_buf[x] = raw[x] + above;
                        else
                            reconstructed_buf[x] = raw[x] + diagonal;
                    }
                    break;
                default: // Unknown
                    ff_dprintf("png: unsupported scanline filter method '%d'", filter_type);
                    free(reconstructed_buf);
                    free(previous_row);
                    free(uncompressed_data);

                    ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
                    return FF_RESULT_ERROR_INVALID_FILE;
            }

        }

        if (ctx->color_type != 3) {
                // For non-indexed color types, we can directly store the pixels
                // Allocate pixel buffer if not already done
                if (ctx->data.pixels == NULL) {
                    ctx->data.pixels = malloc(ctx->width * ctx->height * bpp);
                    if (!ctx->data.pixels) {
                        ff_dprintf("png: failed to allocate memory for pixel data\n");
                        free(reconstructed_buf);
                        free(previous_row);
                        free(uncompressed_data);

                        ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
                        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
                    }
                }

                // Now we put the current one into previous and into the pixels buffer
                
                // Copy current reconstructed to pixels in the context of the png graphical method of storing visual data also known as an image which is trademarked by the png development community as a way to store visual data in a compressed format known as the PNG format (ok ill shut up )
                memcpy(&ctx->data.pixels[iline * ctx->width * bpp], reconstructed_buf, ctx->width * bpp);

                // Copy current reconstructed to previous_row for next iteration (you thought i was going to yap more about png huh, you fool, you are so predictable)
                memcpy(previous_row, reconstructed_buf, ctx->width * bpp);

        } else {
            // Hissy fit time
            // Just kidding
            // I got all day because of the heatwave going on outside
            // Indexed color handling
            // Allocate index map if not already done

            if (ctx->data.imap == NULL) {
                ctx->data.imap = malloc(ctx->width * ctx->height);
                if (!ctx->data.imap) {
                    ff_dprintf("png: failed to allocate memory for index map\n");
                    free(reconstructed_buf);
                    free(previous_row);
                    free(uncompressed_data);

                    ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
                    return FF_RESULT_ERROR_MEMORY_ALLOCATION;
                }
            }

            // Copy current reconstructed to index map in the context of the png graphical method of storing visual data also known as an image which is trademarked by the png development community as a way to store visual data in a compressed format known as the PNG format (he he he ha )
            memcpy(&ctx->data.imap[iline * ctx->width], reconstructed_buf, ctx->width);

            // Copy current reconstructed to previous_row for next iteration
            memcpy(previous_row, reconstructed_buf, ctx->width);
        }
        
    }

    // Ahh yes, the memory demons
    // We must not fall before them
    // We must study the patterns of their ways
    // The buddha has truly been testing us
    
    free(uncompressed_data);
    free(reconstructed_buf);
    free(previous_row);

    ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_WARN_NO_IMPL;
}

ff_result ff_png_end_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{   
    (void)buf; // Unused (for now)
    
    ff_dprintf("png: IEND chunk received (len=%zu)\n", len);
    ff_dprintf("png: all chunks received\n");

    ctx->valid = true;
    ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_OK;
}

ff_result ff_png_trans_handler(uint8_t *buf, size_t len, ff_png_ctx *ctx)
{
    (void)buf; // Unused (for now)
    
    ff_dprintf("png: PLTE chunk received (len=%zu)\n", len);

    if (ctx->color_type == 0) { // Grayscale
        if (len != 2) {
            ff_dprintf("png: invalid tRNS length for grayscale image\n");
            ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
            return FF_RESULT_ERROR_INVALID_FILE;
        }
    }
    (void)buf; // Unused (for now)

    return FF_RESULT_WARN_NO_IMPL;
}
