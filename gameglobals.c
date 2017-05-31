#include <stdlib.h>
#include <pthread.h>

#include "gameglobals.h"
#include "loglist.h"

void initializeGameGlobals()
{
	lives = 4;
	
	// pthread_mutexattr_t attributes;
	// pthread_mutexattr_init(&attributes);
	// pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
	// pthread_mutex_init(&playerMutex, &attributes);
	
	// homes[0] = (Home*) malloc(sizeof(Home));
	// homes[1] = (Home*) malloc(sizeof(Home));
	// homes[2] = (Home*) malloc(sizeof(Home));
	// homes[3] = (Home*) malloc(sizeof(Home));
	// homes[4] = (Home*) malloc(sizeof(Home));
	
	homes = (Home*) malloc(sizeof(Home) * NUM_HOMES);
	
	homes[0].open = 1;
	homes[1].open = 1;
	homes[2].open = 1;
	homes[3].open = 1;
	homes[4].open = 1;
	
	homes[0].leftCol = 1;
	homes[1].leftCol = 19;
	homes[2].leftCol = 37;
	homes[3].leftCol = 55;
	homes[4].leftCol = 73;
	
	homes[0].rightCol = 6;
	homes[1].rightCol = 24;
	homes[2].rightCol = 42;
	homes[3].rightCol = 60;
	homes[4].rightCol = 78;
	
	logList = (LogList*) malloc(sizeof(LogList));
}

Home* getHome(int col)
{
	for (int i = 0; i < NUM_HOMES; i++)
	{
		if (homes[i].leftCol <= col && homes[i].rightCol >= col)
			return &homes[i];
	}
	
	return NULL;
}