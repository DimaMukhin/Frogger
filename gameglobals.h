#ifndef GAMEGLOBALS_H
#define GAMEGLOBALS_H

#include <pthread.h>

#include "loglist.h"
#include "logrow.h"

#define NUM_HOMES 5
#define NUM_LOG_ROWS 4
#define DEFAULT_SLEEP_TICKS 1
#define GAME_ROWS 24
#define GAME_COLS 80
#define LEFT_BOARD_BOUND 0
#define DIRECTION_LEFT -1
#define DIRECTION_RIGHT 1
#define DIRECTION_STAY 0
#define DEFAULT_LOG_SPEED 10
#define ROW_SPEED_INC 2
#define START_LIFE 4

typedef struct home
{
	int open;
	int leftCol;
	int rightCol;
} Home;

/* public global variables */

int gameOver;

LogRow* logRows[NUM_LOG_ROWS];

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