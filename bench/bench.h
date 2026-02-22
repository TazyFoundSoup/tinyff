#ifndef TINYFF_BENCH_H_
#define TINYFF_BENCH_H_

#include <time.h>
#include <tinyff/result.h>

#define FF_BENCH_MAX_MARKERS 32

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

static inline ff_bench ff_bench_start(const char* label) {
    ff_bench b;
    b.label = label;
    b._start = clock();
    b._end = 0;
    
    // Markers
    b._mcount = 0;
    
    return b;
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
}

static inline double ff_bench_seconds(const ff_bench *b) {
    return (double)(b->_end - b->_start) / CLOCKS_PER_SEC;
}

#endif // TINYFF_BENCH_H