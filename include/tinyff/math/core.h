#ifndef FF_MATH_H
#define FF_MATH_H

// NOTE: I'm not going to use macros until I feel like my trash code
// might actually be used in prod. For now im using my beloved
// static inline functions. Oh god dennis richie, i love you so bad <3



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