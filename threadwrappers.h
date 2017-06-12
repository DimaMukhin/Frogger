/*
 * FILE     : threadwrappers.h
 * REMARKS  : header file of threadwrappers.c
 */

#ifndef THREADWRAPPERS_H
#define THREADWRAPPERS_H

#include <pthread.h>

// initialize a new thread (and error check)
void createThread(pthread_t *thread, void *func, void *in);

#endif