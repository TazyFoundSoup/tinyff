#include <tinyff/bench/bench.h>
#include <tinyff/common.h>

#ifdef USE_BENCH

inline void ff_bench_print(ff_ctx* ctx, const ff_bench *b)
{
    double total_ms = (double)(b->_end - b->_start) * 1000.0 / CLOCKS_PER_SEC;

    ff_dprintf(ctx, "== bench: %s ==\n", b->label);

    for (int i = 0; i < b->_mcount; i++) {
        double ms = (double)(b->markers[i].tick - b->_start) * 1000.0 / CLOCKS_PER_SEC;
        ff_dprintf(ctx, "  [%s] +%f ms\n", b->markers[i].label, ms);
    }

    ff_dprintf(ctx, "total: %f ms\n", total_ms);
}

#endif