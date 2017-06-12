/*
 * FILE     : gameglobals.h
 * REMARKS  : header file of gameglobals.h
 */

#ifndef GAMEGLOBALS_H
#define GAMEGLOBALS_H

#include <pthread.h>

#include "loglist.h"
#include "logrow.h"

#define NUM_HOMES 5             // number of homes
#define NUM_LOG_ROWS 4          // number of log rows
#define DEFAULT_SLEEP_TICKS 1   // default sleep ticks (tick = 10ms)
#define GAME_ROWS 24            // amount of game rows
#define GAME_COLS 80            // amount of game columns
#define LEFT_BOARD_BOUND 0      // bound of left side of game board
#define DIRECTION_LEFT -1       // movement of left direction
#define DIRECTION_RIGHT 1       // movement of right direction
#define DIRECTION_STAY 0        // stay direction
#define DEFAULT_LOG_SPEED 10    // default speed of the logs
#define ROW_SPEED_INC 2         // speed increment amount of each log row
#define START_LIFE 4            // starting amount of life

typedef struct home
{
    int open;
    int leftCol;
    int rightCol;
} Home;

/*** public global variables ***/

int gameOver;                   // is the game over?
LogRow* logRows[NUM_LOG_ROWS];  // array of all log rows (0..3)

/*** public global mutexes and condition variables ***/

pthread_cond_t gameOverCV;      // game over condition variable
pthread_mutex_t gameOverMutex;  // game over mutex
pthread_mutex_t drawMutex;      // draw (and all console functions) mutex
pthread_mutex_t playerMutex;    // player mutex
pthread_mutex_t logListMutex;   // log list mutex

/*** public global variable getters/setters ***/

// get remaining lives
int getLives();

// set remaining lives
void setLives(int lifeNum);

// get all log rows
LogRow** getLogRows();

/*** public global functions ***/

// initialize all game globals
void initializeGameGlobals();

// get home by column
Home* getHome(int col);

// get home by index
Home* getHomeIndex(int index);

#endif