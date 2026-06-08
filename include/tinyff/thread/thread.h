#ifdef USE_THREAD

#ifndef TINYFF_THREAD_H
#define TINYFF_THREAD_H

#include <tinyff/common.h>

typedef void (*ff_job_cb)(void *arg);

typedef struct {
    ff_job_cb callback;
    void *arg;
    ff_ctx *ctx;
} ff_thread_job;

void *ff_thread_wrapper(void *ptr);

#endif
#endif