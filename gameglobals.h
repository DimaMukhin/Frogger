#ifndef GAMEGLOBALS_H
#define GAMEGLOBALS_H

#include <pthread.h>

#include "loglist.h"
#include "logrow.h"

#define NUM_HOMES 5

typedef struct home
{
	int open;
	int leftCol;
	int rightCol;
} Home;

/* public global variables */

LogRow* logRows[4];

/* public global mutexes */

pthread_mutex_t	drawMutex;

pthread_mutex_t	playerMutex;

pthread_mutex_t logListMutex;

/* public global variable getters/setters */

int getLives();

void setLives(int lifeNum);

LogRow** getLogRows();

/* public global functions */

void initializeGameGlobals();

Home* getHome(int col);

#endif