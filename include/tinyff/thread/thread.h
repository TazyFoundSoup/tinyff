#ifdef USE_THREAD

#ifndef TINYFF_THREAD_H
#define TINYFF_THREAD_H

typedef void (*ff_job_cb)(void *arg);

typedef struct {
    ff_job_cb callback;
    void *arg;
} ff_thread_job;

void *ff_thread_wrapper(void *ptr);

// Each platform must have their own bridge for running a thread job
// Like pthreads for POSIX systems
#endif
