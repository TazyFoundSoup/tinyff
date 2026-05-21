#include <tinyff/stream.h>
#include <tinyff/dbg.h>
#include <stdarg.h>
#include <stdint.h>

static size_t ff_internal_itoa(int value, char* buf)
{
    char temp[12];
    size_t i = 0, j = 0;

    if (value == 0) {
        buf[0] = '0';
        return 1;
    }

    unsigned int uvalue;

    if (value < 0) {
        buf[j++] = '-';
        uvalue = (unsigned int)(-(long)value);
    } else {
        uvalue = (unsigned int)value;
    }

    while (uvalue > 0) {
        temp[i++] = (uvalue % 10) + '0';
        uvalue /= 10;
    }

    while (i > 0) {
        buf[j++] = temp[--i];
    }

    return j;
}

ff_result ff_set_debug_stream(ff_stream* stream, ff_ctx* ctx)
{
    if (!ctx) return FF_RESULT_ERROR_NULL_PTR;

    if (stream) {
        ctx->ff_debug_stream = *stream;
        ctx->ff_debug_enabled = true;
    } else {
        ctx->ff_debug_enabled = false;
    }

    return FF_RESULT_OK;
}

static inline void ff_debug_flush(ff_ctx* ctx, char* buffer, size_t* idx)
{
    if (*idx == 0) return;

    ctx->ff_debug_stream.write(
        &ctx->ff_debug_stream,
        *idx,
        (uint8_t*)buffer
    );

    *idx = 0;
}

ff_result ff_dprintf(ff_ctx* ctx, const char *format, ...)
{
    if (!ctx || !ctx->ff_debug_enabled || !ctx->ff_debug_stream.write) {
        return FF_RESULT_WARN_DEBUG_DISABLED;
    }

    va_list args;
    va_start(args, format);

    char buffer[512];
    size_t buf_idx = 0;
    const char* p = format;

    while (*p) {

        if (buf_idx >= sizeof(buffer) - 64) {
            ff_debug_flush(ctx, buffer, &buf_idx);
        }

        if (*p == '%') {
            p++;

            if (!*p) break;

            if (*p == 's') {
                const char* s = va_arg(args, const char*);
                if (!s) s = "(null)";

                while (*s) {
                    buffer[buf_idx++] = *s++;

                    if (buf_idx >= sizeof(buffer)) {
                        ff_debug_flush(ctx, buffer, &buf_idx);
                    }
                }
            }
            else if (*p == 'd') {
                int d = va_arg(args, int);
                buf_idx += ff_internal_itoa(d, &buffer[buf_idx]);
            }
            else if (*p == 'u') {
                unsigned int u = va_arg(args, unsigned int);
                buf_idx += ff_internal_itoa((int)u, &buffer[buf_idx]);
            }
            else if (*p == 'l' && *(p + 1) == 'd') {
                long d = va_arg(args, long);
                buf_idx += ff_internal_itoa((int)d, &buffer[buf_idx]);
                p++;
            }
            else if (*p == 'f') {
                double f = va_arg(args, double);

                if (f < 0) {
                    buffer[buf_idx++] = '-';
                    f = -f;
                }

                int whole = (int)f;
                double frac = f - whole;

                buf_idx += ff_internal_itoa(whole, &buffer[buf_idx]);

                buffer[buf_idx++] = '.';

                for (int i = 0; i < 3; i++) {
                    frac *= 10.0;
                    int digit = (int)frac;
                    buffer[buf_idx++] = '0' + digit;
                    frac -= digit;
                }
            }
            else if (*p == 'c') {
                buffer[buf_idx++] = (char)va_arg(args, int);
            }
            else if (*p == '%') {
                buffer[buf_idx++] = '%';
            }
            else {
                buffer[buf_idx++] = '%';
                buffer[buf_idx++] = *p;
            }
        }
        else {
            buffer[buf_idx++] = *p;
        }

        p++;
    }

    ff_debug_flush(ctx, buffer, &buf_idx);

    va_end(args);
    return FF_RESULT_OK;
}