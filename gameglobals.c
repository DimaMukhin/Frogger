#include <stdlib.h>
#include <pthread.h>

#include "gameglobals.h"
#include "loglist.h"

/* private function prototypes */

void initializeHomes();

void initializeMutexes();

/* private global variables */

int lives;
Home *homes;

/*** public functions ***/

void initializeGameGlobals()
{
	gameOver = 0;
	lives = START_LIFE;
	initializeMutexes();
	initializeHomes();
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

Home* getHomeIndex(int index)
{
	return &homes[index];
}

/* public global variable getters/setters */

int getLives()
{
	return lives;
}

void setLives(lifeNum)
{
	lives = lifeNum;
}

/*** private functions ***/

void initializeHomes()
{
	homes = (Home*) malloc(sizeof(Home) * NUM_HOMES);
	
	homes[0].open = 1;
	homes[1].open = 1;
	homes[2].open = 1;
	homes[3].open = 1;
	homes[4].open = 1;
	
	// NOTE: could not find a mathematical order so hard coded :(
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
}

void initializeMutexes()
{
	pthread_mutex_init(&gameOverMutex, NULL);
	pthread_mutex_init(&drawMutex, NULL);
	pthread_mutex_init(&playerMutex, NULL);
	pthread_mutex_init(&logListMutex, NULL);
}