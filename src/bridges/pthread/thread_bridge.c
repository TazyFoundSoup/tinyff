#ifdef USE_THREAD

#include <bridges/pthread/thread_bridge.h>

pthread_t run_job_in_thread(ff_thread_job *job) {
    pthread_t thread;
    pthread_create(&thread, NULL, ff_thread_wrapper, job);
    return thread;
}

#endif