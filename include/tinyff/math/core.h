#ifndef FF_MATH_H
#define FF_MATH_H

// Static inline functions for type safety over macros



static inline int ff_maxi(int a, int b)
{
    return a > b ? a : b;
}

static inline float ff_maxf(float a, float b)
{
    return a > b ? a : b;
}

static inline int ff_mini(int a, int b)
{
    return a < b ? a : b;
}

static inline float ff_minf(float a, float b)
{
    return a < b ? a : b;
}

static inline int ff_clampi(int x, int low, int high)
{
    return ff_maxi(low, ff_mini(x, high));
}

static inline float ff_clampf(float x, float low, float high)
{
    return ff_maxf(low, ff_minf(x, high));
}

static inline int ff_absi(int x)
{
    return x < 0 ? -x : x;
}

static inline float ff_absf(float x)
{
    return x < 0 ? -x : x;
}

#endif