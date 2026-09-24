#ifndef BMP_H_
#define BMP_H_

#include <tinyff/tinyff.h>

typedef struct {
  FF_BASE
  
  uint32_t bf_size;
  uint32_t bf_offbits; // It is actually supposed to be offset bytes (as in the microsft spec*), so don't worry about 'bits'
  
} bmp_ctx_t;

ff_result 
#endif // !BMP_H_


// * https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
