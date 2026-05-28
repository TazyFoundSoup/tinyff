#include <tinyff/thread/thread.h>

void *thread_wrapper(void *ptr) {
    ff_thread_job *job = (ff_thread_job *)ptr;
    
    if (job->callback) {
        job->callback(job->arg);
    }

    free(job);
    return NULL;
}
