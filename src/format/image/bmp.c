#include "tinyff/image/bmp.h"
#include "tinyff/dbg.h"
#include "bmp.h"

ff_result ff_bmp_isvalid(ff_stream* stream) 
{
    ff_dprintf("png: validating signature\n");

    char raw_sig[2];
    if (stream->read(raw_sig, 2, stream->user) != 2) {
        ff_dprintf("bmp: failed to read signature bytes\n");
        return FF_RESULT_ERROR_INVALID_FILE;
    }

    ff_dprintf(
        "bmp: signature read: %02X %02X \n",
        raw_sig[0], raw_sig[1]
    );

    if (memcmp(raw_sig, BMP_SIGNATURE, 2) != 0) {
        ff_dprintf("bmp: signature mismatch\n");
        return FF_RESULT_ERROR_INVALID_PNG_SIGNITURE;
    }

    ff_dprintf("bmp: signature valid\n");
    return FF_RESULT_OK;
}