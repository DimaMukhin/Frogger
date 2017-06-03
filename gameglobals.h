#ifndef GAMEGLOBALS_H
#define GAMEGLOBALS_H

#include <pthread.h>

#include "loglist.h"
#include "logrow.h"

#define NUM_HOMES 5
#define DEFAULT_SLEEP_TICKS 1

typedef struct home
{
	int open;
	int leftCol;
	int rightCol;
} Home;

/* public global variables */

int gameOver;

LogRow* logRows[4];

/* public global mutexes and condition variables */

pthread_cond_t gameOverCV;

pthread_mutex_t gameOverMutex;

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

Home* getHomeIndex(int index);

#endif