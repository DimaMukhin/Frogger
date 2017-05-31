#ifndef GAMEGLOBALS_H
#define GAMEGLOBALS_H

#include <pthread.h>

#include "loglist.h"

#define NUM_HOMES 5

typedef struct home
{
	int open;
	int leftCol;
	int rightCol;
} Home;

pthread_mutex_t	drawMutex;

pthread_mutex_t	playerMutex;

LogList *logList;

int lives;

Home *homes;

void initializeGameGlobals();

Home* getHome(int col);

#endif