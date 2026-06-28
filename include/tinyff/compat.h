#ifndef TINYFF_COMPAT_H_
#define TINYFF_COMPAT_H_

#if defined(__STDC_HOSTED__) && !(__STDC_HOSTED__ == 0)
#   define COMPAT_HOSTED 1
#else
#   define COMPAT_HOSTED 0
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   error "compat: ISO C99 or newer compiler required."
#endif

#include <limits.h>
#include <stdint.h>

#if INT_MAX < 32767
#   error "compat: 8-bit 'int' architectures are not supported."
#endif

#if INTPTR_MAX == INT32_MAX
#   define COMPAT_SZ_32 1
#elif INTPTR_MAX == INT64_MAX
#   define COMPAT_SZ_64 1
#endif

// Static assertion fallback for c99
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    // static_assert already in version
#   include <assert.h>
#else
    // c99 dummy
#   define COMPAT_STATIC_ASSERT(expr, msg) \
        typedef char compat_assertion_failed##__LINE__[(expr) ? 1 : -1]

#   undef static_assert
#   define static_assert(expr, msg) COMPAT_STATIC_ASSERT(expr, msg)
#endif

static_assert(sizeof(uint8_t)  == 1, "compat: broken uint8_t width");
static_assert(sizeof(uint32_t) == 4, "compat: broken uint32_t width");

#endif
