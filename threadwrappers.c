#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "threadwrappers.h"

void createThread(pthread_t *thread, void *func, void *in)
{
	if (pthread_create(thread, NULL, func, in) == -1) 
	{ 
		perror("Thread Create Error"); 
		exit(EXIT_FAILURE); 
	}
}