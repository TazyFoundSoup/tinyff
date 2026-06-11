#ifdef USE_THREAD

#ifndef TINYFF_THREAD_H
#define TINYFF_THREAD_H

#include <tinyff/common.h>
#include <tinyff/result.h>

typedef void (*ff_job_cb)(void *arg);

typedef struct {
    ff_job_cb callback;
    void *arg;
    ff_ctx *ctx;
} ff_thread_job;

void *ff_thread_wrapper(void *ptr);

// High level helper: allocate a job using ctx->allocator and schedule it.
// Returns FF_RESULT_OK on success, or an error ff_result on failure.
ff_result ff_thread_submit(ff_ctx *ctx, ff_job_cb callback, void *arg);

#endif
#endif