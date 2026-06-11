#ifdef USE_THREAD

#include <tinyff/thread/thread.h>
#include <bridges/pthread/thread_bridge.h>
#include <tinyff/result.h>

ff_result ff_thread_submit(ff_ctx *ctx, ff_job_cb callback, void *arg) {
    if (!ctx || !callback) return FF_RESULT_ERROR_NULL_PTR;

    ff_thread_job *job = ctx->allocator.ff_alloc(sizeof(ff_thread_job));
    if (!job) return FF_RESULT_ERROR_MEMORY_ALLOCATION;

    job->callback = callback;
    job->arg = arg;
    job->ctx = ctx;

    pthread_t t = run_job_in_thread(job);
    if ((pthread_t)0 == t) {
        // failed to create thread
        ctx->allocator.ff_free(job);
        return FF_RESULT_WARN_NO_IMPL; // thread creation failed
    }

    return FF_RESULT_OK;
}

#endif