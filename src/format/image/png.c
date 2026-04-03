#include <tinyff/common.h>
#include <tinyff/image/png.h>

// External
#include <ext/tinf.h>

const ff_png_chunk_handler ff_png_chunk_handlers[] = {
    {"IHDR", ff_png_header_handler},
    {"IDAT", ff_png_data_handler},
    {"IEND", ff_png_end_handler},
    {"PLTE", ff_png_palette_handler},   

    // From now on, the handlers will be for ancillary chunks
    {"tRNS", ff_png_trans_handler},

    {NULL, NULL} // Terminator
}; 

ff_result ff_png_isvalid(ff_ctx* ctx, ff_stream *stream)
{
    ff_dprintf(ctx, "png: validating signature\n");
    
    char raw_sig[8];
    if (stream->read(raw_sig, 8, stream->user) != 8) {
        ff_dprintf(ctx, "png: failed to read signature bytes\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_dprintf(ctx,
        // TODO: Write raw sig to the log in hex cause formatting needs reworking
        "png: signature read\n"
    );

    if (memcmp(raw_sig, PNG_SIGNATURE, 8) != 0) {
        ff_dprintf(ctx, "png: signature mismatch\n");
        return FF_RESULT_ERROR_INVALID_PNG_SIGNATURE;
    }

    ff_dprintf(ctx, "png: signature valid\n");

    return FF_RESULT_OK;
}

ff_result ff_open_png(ff_ctx* ctx, ff_stream *stream, ff_png_ctx **out_ctx, ff_flag require_valid)
{
    
    ff_png_ctx *png_ctx = ctx->allocator.ff_alloc(sizeof(ff_png_ctx));
    if (!png_ctx) {
        // lol, i actually set the last_error field here. now its just this comment. im so frickin dumb bruh
        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    }

    // Init
    memset(png_ctx, 0, sizeof(*png_ctx));
    png_ctx->image_mode = FF_PNG_MODE_NONE;
    png_ctx->raw = stream;
    png_ctx->last_error = FF_RESULT_OK;


    if (png_ctx->raw->read == NULL) {
        ff_dprintf(ctx, "png: stream failed to read\n");
        ctx->allocator.ff_free(png_ctx);
        return FF_RESULT_ERROR_READ_FILE_FAILURE;
    }

    ff_dprintf(ctx, "png: stream read successfully\n");

    if (require_valid) {
        ff_result res = ff_png_isvalid(ctx, png_ctx->raw);
        if (res != FF_RESULT_OK) {
            ff_dprintf(ctx, "png: validation failed\n");
            ctx->allocator.ff_free(png_ctx);
            return res;
        }
    }

    ff_dprintf(ctx, "png: validation passed\n");
    png_ctx->valid = true;

    *out_ctx = png_ctx;
    ff_dprintf(ctx, "png: open_png reached WIP end\n");

    png_ctx->last_error = FF_RESULT_WARN_NO_IMPL;
    return FF_RESULT_WARN_NO_IMPL;
}

// Handlers

inline uint16_t ff_png_bpp(ff_png_ctx *png_ctx)
{
    int16_t byte_depth = png_ctx->bit_depth / 8;
    switch (png_ctx->color_type) {
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

ff_result ff_png_header_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx *png_ctx)
{
    ff_dprintf(ctx, "png: IHDR chunk received (len=%zu)\n");

    if (len != 13) {
        ff_dprintf(ctx, "png: invalid IHDR length\n");

        png_ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    uint32_t w = ff_be32(buf);
    uint32_t h = ff_be32(buf + 4);

    // TODO: Do formatting.
    //ff_dprintf("Width:              %u\n", w);
    //ff_dprintf("Height:             %u\n", h);
    //ff_dprintf("Bit depth:          %u\n", buf[8]);
    //ff_dprintf("Color type:         %u\n", buf[9]);
    //ff_dprintf("Compression method: %u\n", buf[10]);
    //ff_dprintf("Filter method:      %u\n", buf[11]);
    //ff_dprintf("Interlace method:   %u\n", buf[12]);

    png_ctx->width = w;
    png_ctx->height = h;
    png_ctx->bit_depth = buf[8];
    png_ctx->color_type = buf[9];
    png_ctx->interlace_method = buf[12];

    ff_dprintf(ctx, "png: IHDR stored in context\n");

    png_ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_OK;
}

ff_result ff_png_palette_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx *png_ctx)
{
    ff_dprintf(ctx, "png: PLTE chunk received\n");
    
    if (len % 3 != 0) {
        ff_dprintf(ctx, "png: invalid PLTE length\n");

        png_ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    uint16_t num_entries = len / 3;
    ff_dprintf(ctx, "png: PLTE contains [number of entries] palette entries\n"); // TODO: Format this with the actual number of entries
    
    png_ctx->palette_size = num_entries;
    png_ctx->palette = ctx->allocator.ff_alloc(len);
    if (!png_ctx->palette) {
        png_ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    }
    
    for (uint16_t i = 0; i < num_entries; i++) {
        // There's 4 bytes per entry just because if there's
        // a tRNS chunk later, we can just fill in the alpha values
        png_ctx->palette[i * 4 + 0] = buf[i * 3 + 0]; // R
        png_ctx->palette[i * 4 + 1] = buf[i * 3 + 1]; // G
        png_ctx->palette[i * 4 + 2] = buf[i * 3 + 2]; // B
    }
    
    png_ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_OK;
}   

ff_result ff_png_data_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx *png_ctx)
{
    ff_dprintf(ctx, "png: IDAT chunk received\n");

    uint8_t *uncompressed_data = NULL;
    
    // Getting the size is weird cause it could the sample count
    // could be different based on color type and bit depth
    size_t uncompressed_size = png_ctx->width * png_ctx->height * ff_png_bpp(png_ctx);

    uncompressed_data = ctx->allocator.ff_alloc(uncompressed_size);

    if (!uncompressed_data) {
        ff_dprintf(ctx, "png: failed to allocate memory for uncompressed data\n");

        png_ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    }
    
    if (tinf_uncompress(uncompressed_data, (unsigned int *)uncompressed_size, buf, len) != TINF_OK) {
        ff_dprintf(ctx, "png: failed to uncompress IDAT data\n");
        ctx->allocator.ff_free(uncompressed_data);

        png_ctx->last_error = FF_RESULT_ERROR_DECOMPRESSION_FAILURE;
        return FF_RESULT_ERROR_DECOMPRESSION_FAILURE;
    }

    ff_dprintf(ctx, "png: IDAT data uncompressed successfully\n");

    // Now we parse the uncompressed data into our pixel buffer
    // However I don't understand Adam7 so interlaced images are not supported yet
    if (png_ctx->interlace_method != 0) {
        ff_dprintf(ctx, "png: interlaced images are not supported yet\n");
        ctx->allocator.ff_free(uncompressed_data);

        png_ctx->last_error = FF_RESULT_WARN_NO_IMPL;
        return FF_RESULT_WARN_NO_IMPL;
    }

    // Now I am very inexperienced so I'm doing what I think is right
    // TODO: Review this code later


    // Filtering

    size_t bpp = ff_png_bpp(png_ctx);
    uint8_t *reconstructed_buf = (uint8_t *) ctx->allocator.ff_alloc(png_ctx->width * bpp); // This is the current reconstructed scanline
    uint8_t *previous_row = (uint8_t *) ctx->allocator.ff_calloc(png_ctx->width * bpp, 1); // This is the prior scanline (reconstructed)



    // Iterate over every scanline
    for (int iline = 0; (uint32_t) iline < png_ctx->height; iline++) {
        // Make some variables so it's not hella unreadable
        size_t scanline_start = iline * (1 + (png_ctx->width * bpp));
        uint8_t filter_type = uncompressed_data[scanline_start];
        uint8_t *raw = &uncompressed_data[scanline_start + 1];

        
        // Now I need to iterate over the entire scanline and apply the filter
        for (size_t x = 0; x < png_ctx->width * bpp; x++) {
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
                        int pa = ff_absi(p - left);
                        int pb = ff_absi(p - above);
                        int pc = ff_absi(p - diagonal);

                        if (pa <= pb && pa <= pc) 
                            reconstructed_buf[x] = raw[x] + left;
                        else if (pb <= pc)
                            reconstructed_buf[x] = raw[x] + above;
                        else
                            reconstructed_buf[x] = raw[x] + diagonal;
                    }
                    break;
                default: // Unknown
                    ff_dprintf(ctx, "png: unsupported scanline filter method"); // TODO: Format
                    ctx->allocator.ff_free(reconstructed_buf);
                    ctx->allocator.ff_free(previous_row);
                    ctx->allocator.ff_free(uncompressed_data);

                    png_ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
                    return FF_RESULT_ERROR_INVALID_FILE;
            }

        }

        if (png_ctx->color_type != 3) {
                // For non-indexed color types, we can directly store the pixels
                // Allocate pixel buffer if not already done
                if (png_ctx->data.pixels == NULL) {
                    png_ctx->data.pixels = ctx->allocator.ff_alloc(png_ctx->width * png_ctx->height * bpp);
                    if (!png_ctx->data.pixels) {
                        ff_dprintf(ctx, "png: failed to allocate memory for pixel data\n");
                        ctx->allocator.ff_free(reconstructed_buf);
                        ctx->allocator.ff_free(previous_row);
                        ctx->allocator.ff_free(uncompressed_data);

                        png_ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
                        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
                    }
                }

                // Now we put the current one into previous and into the pixels buffer
                
                // Copy current reconstructed to pixels in the context of the png graphical method of storing visual data also known as an image which is trademarked by the png development community as a way to store visual data in a compressed format known as the PNG format (ok ill shut up )
                memcpy(&png_ctx->data.pixels[iline * png_ctx->width * bpp], reconstructed_buf, png_ctx->width * bpp);

                // Copy current reconstructed to previous_row for next iteration (you thought i was going to yap more about png huh, you fool, you are so predictable)
                memcpy(previous_row, reconstructed_buf, png_ctx->width * bpp);

        } else {
            // Hissy fit time
            // Just kidding
            // I got all day because of the heatwave going on outside
            // Indexed color handling
            // Allocate index map if not already done

            if (png_ctx->data.imap == NULL) {
                png_ctx->data.imap = ctx->allocator.ff_alloc(png_ctx->width * png_ctx->height);
                if (!png_ctx->data.imap) {
                    ff_dprintf(ctx, "png: failed to allocate memory for index map\n");
                    ctx->allocator.ff_free(reconstructed_buf);
                    ctx->allocator.ff_free(previous_row);
                    ctx->allocator.ff_free(uncompressed_data);

                    png_ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
                    return FF_RESULT_ERROR_MEMORY_ALLOCATION;
                }
            }

            // Copy current reconstructed to index map in the context of the png graphical method of storing visual data also known as an image which is trademarked by the png development community as a way to store visual data in a compressed format known as the PNG format (he he he ha )
            memcpy(&png_ctx->data.imap[iline * png_ctx->width], reconstructed_buf, png_ctx->width);

            // Copy current reconstructed to previous_row for next iteration
            memcpy(previous_row, reconstructed_buf, png_ctx->width);
        }
        
    }

    // Ahh yes, the memory demons
    // We must not fall before them
    // We must study the patterns of their ways
    // The buddha has truly been testing us
    
    ctx->allocator.ff_free(uncompressed_data);
    ctx->allocator.ff_free(reconstructed_buf);
    ctx->allocator.ff_free(previous_row);

    png_ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_WARN_NO_IMPL;
}

ff_result ff_png_end_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx *png_ctx)
{   
    (void)buf; // Unused (for now)
    (void)len;
    
    ff_dprintf(ctx, "png: IEND chunk received\n"); // TODO: Maybe format?
    ff_dprintf(ctx, "png: all chunks received\n");

    png_ctx->valid = true;
    png_ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_OK;
}

ff_result ff_png_trans_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx *png_ctx)
{
    (void)buf; // Unused (for now)
    
    ff_dprintf(ctx, "png: PLTE chunk received\n"); // TODO: Format

    if (png_ctx->color_type == 0) { // Grayscale
        if (len != 2) {
            ff_dprintf(ctx, "png: invalid tRNS length for grayscale image\n");
            png_ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
            return FF_RESULT_ERROR_INVALID_FILE;
        }
    }
    (void)buf; // Unused (for now)

    return FF_RESULT_WARN_NO_IMPL;
}
