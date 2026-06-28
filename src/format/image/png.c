#include "tinyff/result.h"
#include <tinyff/common.h>
#include <tinyff/image/png.h>

// External
#include <ext/tinf.h>

static ff_result ff_png_dispatch(ff_ctx* ctx, ff_stream* stream, ff_png_ctx *png_ctx);

const ff_png_chunk_handler ff_png_chunk_handlers[] = {
    {"IHDR", ff_png_header_handler},
    {"IDAT", ff_png_data_handler},
    {"IEND", ff_png_end_handler},
    {"PLTE", ff_png_palette_handler},   

    // From now on, the handlers will be for ancillary chunks
    {"tRNS", ff_png_trans_handler},

    {NULL, NULL} // Terminator
}; 

static ff_result ff_png_dispatch(ff_ctx* ctx, ff_stream* stream, ff_png_ctx *png_ctx)
{
    while (1)
    {   
        uint8_t chunk_length[4];
        if (!stream->read(chunk_length, 4, stream->user)) {
            png_ctx->last_error = FF_RESULT_ERROR_READ_FILE_FAILURE;
            return FF_RESULT_ERROR_READ_FILE_FAILURE;
        }
        uint32_t length = ff_be32(chunk_length);
        
        uint8_t chunk_type[4];
        if (!stream->read(chunk_type, 4, stream->user)) {
            png_ctx->last_error = FF_RESULT_ERROR_READ_FILE_FAILURE;
            return FF_RESULT_ERROR_READ_FILE_FAILURE;
        }
        
        if (ff_memcmp(chunk_type, "IEND", 4) == 0) return FF_RESULT_OK;
        
        uint8_t* chunk_data = ctx->allocator.ff_alloc(length);
        if (!chunk_data) return FF_RESULT_ERROR_MEMORY_ALLOCATION;
        if (!stream->read(chunk_data, length, stream->user)) {
            ctx->allocator.ff_free(chunk_data);
            png_ctx->last_error = FF_RESULT_ERROR_READ_FILE_FAILURE;
            return FF_RESULT_ERROR_READ_FILE_FAILURE;
        }
        
        for (int i = 0; ff_png_chunk_handlers[i].type != NULL; i++) {
            if (ff_memcmp(chunk_type, ff_png_chunk_handlers[i].type, 4) == 0) {
                if (ff_png_chunk_handlers[i].handler != NULL) {
                    FF_BENCH_MARK(ctx, ff_png_chunk_handlers[i].type);
                    ff_png_chunk_handlers[i].handler(ctx, chunk_data, length, png_ctx);
                }
                break;
            }
        }

        uint8_t* crc_buf = ctx->allocator.ff_alloc(4 + length);
        if (!crc_buf) {
            ctx->allocator.ff_free(chunk_data);
            return FF_RESULT_ERROR_MEMORY_ALLOCATION;
        }
        ff_memcpy(crc_buf, chunk_type, 4);
        ff_memcpy(crc_buf + 4, chunk_data, length);
        ctx->allocator.ff_free(chunk_data);

        uint8_t crc_bytes[4];
        if (!stream->read(crc_bytes, 4, stream->user)) {
            ctx->allocator.ff_free(crc_buf);
            png_ctx->last_error = FF_RESULT_ERROR_READ_FILE_FAILURE;
            return FF_RESULT_ERROR_READ_FILE_FAILURE;
        }

        uint32_t expected = ff_be32(crc_bytes);
        uint32_t actual = tinf_crc32(crc_buf, 4 + length);
        ctx->allocator.ff_free(crc_buf);

        if (actual != expected) {
            png_ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
            return FF_RESULT_ERROR_INVALID_FILE;
        }
    }
}

ff_result ff_png_isvalid(ff_ctx* ctx, ff_stream *stream)
{   
    FF_BENCH_MARK(ctx, "ff_png_isvalid");
    
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

    if (ff_memcmp(raw_sig, PNG_SIGNATURE, 8) != 0) {
        ff_dprintf(ctx, "png: signature mismatch\n");
        return FF_RESULT_ERROR_INVALID_PNG_SIGNATURE;
    }

    ff_dprintf(ctx, "png: signature valid\n");

    return FF_RESULT_OK;
}

ff_result ff_open_png(ff_ctx* ctx, ff_stream *stream, ff_png_ctx **out_ctx, ff_flag require_valid)
{   
    FF_BENCH_START(ctx, "png");
    FF_BENCH_MARK(ctx, "ff_open_png");
    
    ff_png_ctx *png_ctx = ctx->allocator.ff_alloc(sizeof(ff_png_ctx));
    if (!png_ctx) {
        // lol, i actually set the last_error field here. now its just this comment. im so frickin dumb bruh
        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    }

    // Init
    ff_memset(png_ctx, 0, sizeof(*png_ctx));
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

    ff_dprintf(ctx, "png: calling dispatcher\n");
    
    ff_result res = ff_png_dispatch(ctx, stream, png_ctx);
    if (res != FF_RESULT_OK && res != FF_RESULT_WARN_NO_IMPL) {
        ctx->allocator.ff_free(png_ctx);
        return res;
    }
    
    *out_ctx = png_ctx;
    ff_dprintf(ctx, "png: open_png reached end\n");

    png_ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_OK;
}

// Handlers

static inline uint16_t ff_png_bpp(ff_png_ctx *png_ctx)
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
    ff_dprintf(ctx, "png: IHDR chunk received (len=%u)\n", (unsigned int)len);

    if (len != 13) {
        ff_dprintf(ctx, "png: invalid IHDR length\n");

        png_ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    uint32_t w = ff_be32(buf);
    uint32_t h = ff_be32(buf + 4);

    ff_dprintf(ctx, "Width:              %u\n", w);
    ff_dprintf(ctx, "Height:             %u\n", h);
    ff_dprintf(ctx, "Bit depth:          %u\n", buf[8]);
    ff_dprintf(ctx, "Color type:         %u\n", buf[9]);
    ff_dprintf(ctx, "Compression method: %u\n", buf[10]);
    ff_dprintf(ctx, "Filter method:      %u\n", buf[11]);
    ff_dprintf(ctx, "Interlace method:   %u\n", buf[12]);

    png_ctx->width = w;
    png_ctx->height = h;
    png_ctx->bit_depth = buf[8];
    png_ctx->color_type = buf[9];
    png_ctx->filter_method = buf[11];
    png_ctx->interlace_method = buf[12];

    ff_dprintf(ctx, "png: IHDR stored in context\n");

    png_ctx->last_error = FF_RESULT_OK;
    return FF_RESULT_OK;
}

ff_result ff_png_palette_handler(ff_ctx* ctx, uint8_t *buf, size_t len, ff_png_ctx *png_ctx)
{
    ff_dprintf(ctx, "png: PLTE chunk received (len = %u)\n", (unsigned int)len);
    
    if (len % 3 != 0) {
        ff_dprintf(ctx, "png: invalid PLTE length\n");

        png_ctx->last_error = FF_RESULT_ERROR_INVALID_FILE;
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    uint16_t num_entries = len / 3;
    ff_dprintf(ctx, "png: PLTE contains %u palette entries\n", num_entries);
    
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
    ff_dprintf(ctx, "png: IDAT chunk received (len = %u)\n", (unsigned int)len);

    uint8_t *uncompressed_data = NULL;
    
    // Getting the size is weird cause it could the sample count
    // could be different based on color type and bit depth
    size_t uncompressed_size = png_ctx->height * (1 + png_ctx->width * ff_png_bpp(png_ctx));

    uncompressed_data = ctx->allocator.ff_alloc(uncompressed_size);

    if (!uncompressed_data) {
        ff_dprintf(ctx, "png: failed to allocate memory for uncompressed data\n");

        png_ctx->last_error = FF_RESULT_ERROR_MEMORY_ALLOCATION;
        return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    }
    
    unsigned int out_size = (unsigned int)uncompressed_size;
    if (tinf_uncompress(uncompressed_data, &out_size, buf + 2, len - 2) != TINF_OK) {
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
    
    png_ctx->image_mode = (png_ctx->color_type == 3) ? FF_PNG_MODE_PALETTE : FF_PNG_MODE_DIRECT_COLOR;

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
                    ff_dprintf(ctx, "png: unsupported scanline filter method %u\n", (unsigned int)filter_type);
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
                ff_memcpy(&png_ctx->data.pixels[iline * png_ctx->width * bpp], reconstructed_buf, png_ctx->width * bpp);

                // Copy current reconstructed to previous_row for next iteration (you thought i was going to yap more about png huh, you fool, you are so predictable)
                ff_memcpy(previous_row, reconstructed_buf, png_ctx->width * bpp);

        } else {

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
            ff_memcpy(&png_ctx->data.imap[iline * png_ctx->width], reconstructed_buf, png_ctx->width);

            // Copy current reconstructed to previous_row for next iteration
            ff_memcpy(previous_row, reconstructed_buf, png_ctx->width);
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
    return FF_RESULT_OK;
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
    ff_dprintf(ctx, "png: tRNS chunk received\n");

    if (png_ctx->color_type == 3) { // indexed
        size_t count = len;
        if (count > png_ctx->palette_size)
            count = png_ctx->palette_size;

        for (size_t i = 0; i < count; i++) {
            png_ctx->palette[i * 4 + 3] = buf[i];
        }
    }

    return FF_RESULT_OK;
}


ff_result ff_close_png(ff_ctx* ctx, ff_png_ctx *png_ctx)
{
    if (!png_ctx) return FF_RESULT_OK;
    
    if (png_ctx->data.pixels) {
        ctx->allocator.ff_free(png_ctx->data.pixels);
        png_ctx->data.pixels = NULL;
        ff_dprintf(ctx, "png: successfully freed pixels\n");
    }
    
    if (png_ctx->data.imap) {
        ctx->allocator.ff_free(png_ctx->data.imap);
        png_ctx->data.imap = NULL;
        ff_dprintf(ctx, "png: successfully freed index map\n");
    }
    
    ctx->allocator.ff_free(png_ctx);
    ff_dprintf(ctx, "png: successfully freed png context\n");
    
    FF_BENCH_END(ctx);
    
    return FF_RESULT_OK;
}

ff_result ff_write_chunk(ff_stream *stream, const char *type, uint8_t *buf, size_t len)
{
    uint8_t be_len[4];
    ff_write_be32(be_len, (uint32_t)len);

    stream->write(be_len, 4, stream->user);
    stream->write(type, 4, stream->user);
    stream->write(buf, len, stream->user);

    uint32_t zero_crc = 0;
    stream->write(&zero_crc, 4, stream->user);

    return FF_RESULT_OK;
}

ff_result ff_encode_png(ff_ctx *ctx, ff_png_ctx *png_ctx, ff_stream *stream)
{   
    FF_BENCH_START(ctx, "png");
    FF_BENCH_MARK(ctx, "ff_encode_png");

    stream->write(stream, 8, (void *)PNG_SIGNATURE);
    
    // TODO: Actually write the PNG data
    // For now I'll void the parameters
    (void)ctx;
    (void)png_ctx;
    
    FF_BENCH_END(ctx);
    
    return FF_RESULT_WARN_NO_IMPL;
}

ff_result ff_png_normalize(ff_ctx *ctx, ff_png_ctx *png_ctx, ff_image_ctx **out_data, ff_flag consume)
{   
    if (png_ctx->bit_depth != 8) return FF_RESULT_WARN_NO_IMPL;
    
    ff_image_ctx* out_ctx = ctx->allocator.ff_alloc(sizeof(ff_image_ctx));
    if (!out_ctx) return FF_RESULT_ERROR_MEMORY_ALLOCATION;
    
    size_t pixel_count = png_ctx->width * png_ctx->height;
    out_ctx->data = ctx->allocator.ff_alloc(sizeof(uint32_t) * pixel_count);
    
    if (png_ctx->image_mode == FF_PNG_MODE_DIRECT_COLOR) {
        if (out_ctx->data == NULL) {
            return FF_RESULT_ERROR_MEMORY_ALLOCATION;
        }
        
        uint8_t *src = png_ctx->data.pixels;
        uint8_t *dst = (uint8_t *)out_ctx->data;
        size_t bpp = ff_png_bpp(png_ctx);
        
        for (size_t i = 0; i < pixel_count; i++) {
            uint8_t* s = src + i * bpp; // source pixel
            uint8_t* d = dst + i * 4; // destination pixel
            
            uint8_t *R = &d[0];
            uint8_t *G = &d[1];
            uint8_t *B = &d[2];
            uint8_t *A = &d[3];
            
            switch (png_ctx->color_type) {
                case 0: // Grayscale
                    *R = *G = *B = s[0];
                    *A = 255;
                    break;
                case 2: // RGB
                    *R = s[0];
                    *G = s[1];
                    *B = s[2];
                    *A = 255;
                    break;
                case 4: // Grayscale w/ alpha
                    *R = *G = *B = s[0];
                    *A = s[1];
                    break;
                case 6: // RGBA
                    *R = s[0];
                    *G = s[1];
                    *B = s[2];
                    *A = s[3];
                    break;
                default:
                    ctx->allocator.ff_free(out_ctx->data);
                    ctx->allocator.ff_free(out_ctx);
                    return FF_RESULT_ERROR_INVALID_FILE;
            }
        }
        
        
        
    } else if (png_ctx->image_mode == FF_PNG_MODE_PALETTE) {
        //out_ctx->data = png_ctx->data.imap;
        // NOTE: Since I need to resolve the imap and I don't wanna do that now
        // and just wanna get v0.1.0 out, then I'm just gonna use the pixels.
        ctx->allocator.ff_free(out_ctx->data);
        ctx->allocator.ff_free(out_ctx);
        return FF_RESULT_WARN_NO_IMPL;
    } else {
        ctx->allocator.ff_free(out_ctx->data);
        ctx->allocator.ff_free(out_ctx);
        return FF_RESULT_ERROR_INVALID_FILE;
    }
    
    out_ctx->width = png_ctx->width;
    out_ctx->height = png_ctx->height;
    out_ctx->origin = FF_IMAGE_ORIGIN_PNG;
    
    *out_data = out_ctx;
    
    if (consume) {
        ff_close_png(ctx, png_ctx);
        (void)png_ctx;
    }
    return FF_RESULT_OK;
}
