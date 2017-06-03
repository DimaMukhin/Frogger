#ifndef THREADWRAPPERS_H
#define THREADWRAPPERS_H

#include <pthread.h>

void createThread(pthread_t *thread, void *func, void *in);

#endif