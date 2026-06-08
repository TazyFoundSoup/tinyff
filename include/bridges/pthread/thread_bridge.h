#ifdef USE_THREAD

#ifndef BRIDGES_PTHREAD_THREAD_BRIDGE_H
#define BRIDGES_PTHREAD_THREAD_BRIDGE_H

#include <pthread.h>
#include <tinyff/thread/thread.h>

pthread_t run_job_in_thread(ff_thread_job *job);

#endif

#endif