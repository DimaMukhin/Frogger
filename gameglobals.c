/*
 * FILE     : gameglobals.c
 * REMARKS  : Game globals initialization, getters and setters
 */

#include <stdlib.h>
#include <pthread.h>

#include "gameglobals.h"
#include "loglist.h"

/* private function prototypes */

void initializeHomes();     // initialize the homes
void initializeMutexes();   // initialize all MUTEX

/* private global variables */

int lives;      // remaining lives
Home *homes;    // all homes

/*** public functions ***/

/*-------------------------------------------------------------initializeGameGlobals
 * initialize all game globals (called at start of game)
 */
void initializeGameGlobals()
{
    gameOver = 0;
    lives = START_LIFE;
    initializeMutexes();
    initializeHomes();
}

/*---------------------------------------------------------------------------getHome
 * get home based on a column on the board
 * example: if there is a home between cols 10 and 20, anything in between
 *          will return the home.
 */
Home* getHome(int col)
{
    for (int i = 0; i < NUM_HOMES; i++)
    {
        if (homes[i].leftCol <= col && homes[i].rightCol >= col)
            return &homes[i];
    }
    
    return NULL;
}

/*----------------------------------------------------------------------getHomeIndex
 * get home by its index
 */
Home* getHomeIndex(int index)
{
    if (index < NUM_HOMES)
        return &homes[index];
    return NULL;
}

/*** public global variable getters/setters ***/

/*--------------------------------------------------------------------------getLives
 * get remaining lives
 */
int getLives()
{
    return lives;
}

/*--------------------------------------------------------------------------setLives
 * set remaining lives
 */
void setLives(lifeNum)
{
    lives = lifeNum;
}

/*** private functions ***/

/*-------------------------------------------------------------------initializeHomes
 * initialize all homes
 */
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

/*-----------------------------------------------------------------initializeMutexes
 * initialize all MUTEX with default attributes
 */
void initializeMutexes()
{
    pthread_mutex_init(&gameOverMutex, NULL);
    pthread_mutex_init(&drawMutex, NULL);
    pthread_mutex_init(&playerMutex, NULL);
    pthread_mutex_init(&logListMutex, NULL);
}