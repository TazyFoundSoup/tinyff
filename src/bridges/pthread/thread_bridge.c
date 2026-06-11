#ifdef USE_THREAD

#include <bridges/pthread/thread_bridge.h>
#include <pthread.h>
#include <errno.h>

pthread_t run_job_in_thread(ff_thread_job *job) {
    pthread_t thread;
    int r = pthread_create(&thread, NULL, ff_thread_wrapper, job);
    if (r != 0) {
        // pthread_create failed; return an invalid thread value (0) to signal error
        return (pthread_t)0;
    }

    // Detach the thread to avoid requiring callers to join it. If you want join
    // semantics, consider changing the API to return an error and an out pthread_t.
    pthread_detach(thread);
    return thread;
}

#endif