#ifdef USE_BENCH

#ifndef TINYFF_BENCH_H_
#define TINYFF_BENCH_H_

#include <time.h>
#include <tinyff/result.h>

// Forward declarations
typedef struct ff_ctx ff_ctx;
ff_result ff_dprintf(ff_ctx* ctx, const char *msg, ...);


#define FF_BENCH_MAX_MARKERS 32

typedef struct ff_ctx ff_ctx;

typedef struct {
    const char *label;
    clock_t tick;
} ff_bench_marker;

typedef struct {
    const char *label;
    clock_t _start;
    clock_t _end;
    
    ff_bench_marker markers[FF_BENCH_MAX_MARKERS];
    int _mcount;
} ff_bench;

// These need to be inline so they are fast

static inline void ff_bench_start(ff_bench *b, const char* label) {
    b->label = label;
    b->_start = clock();
    b->_end = 0;
    b->_mcount = 0;

}


static inline void ff_bench_end(ff_bench *b) {
    b->_end = clock();
}

static inline ff_result ff_bench_mark(ff_bench *b, const char *label) {
    if (b->_mcount >= FF_BENCH_MAX_MARKERS) return FF_RESULT_ERROR_OUT_OF_BOUNDS;
    
    ff_bench_marker m;
    m.label = label;
    m.tick = clock();
    
    b->markers[b->_mcount] = m;
    b->_mcount++;
    
    return FF_RESULT_OK;
}

static inline double ff_bench_seconds(const ff_bench *b) {
    return (double)(b->_end - b->_start) / CLOCKS_PER_SEC;
}

static inline void ff_bench_print(ff_ctx* ctx, const ff_bench *b)
{
    double total_ms = (double)(b->_end - b->_start) * 1000.0 / CLOCKS_PER_SEC;

    ff_dprintf(ctx, "== bench: %s ==\n", b->label);

    for (int i = 0; i < b->_mcount; i++) {
        double ms = (double)(b->markers[i].tick - b->_start) * 1000.0 / CLOCKS_PER_SEC;
        ff_dprintf(ctx, "  [%s] +%.3f ms\n", b->markers[i].label, ms);
    }

    ff_dprintf(ctx, "total: %.3f ms\n", total_ms);
}


#endif // TINYFF_BENCH_H
#endif // USE_BENCH