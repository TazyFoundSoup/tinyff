#ifdef USE_THREAD

#include <tinyff/thread/thread.h>

void *ff_thread_wrapper(void *ptr) {
    ff_thread_job *job = (ff_thread_job *)ptr;

    if (job->callback) {
        job->callback(job->arg);
    }

    job->ctx->allocator.ff_free(job);
    return NULL;
}

#endif