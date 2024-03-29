/*
 * FILE     : log.c
 * REMARKS  : Log object implementation
 */

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"
#include "log.h"
#include "frogger.h"
#include "gameglobals.h"
#include "threadwrappers.h"

#define LOG_ANIM_TILES 2
#define BOTTOM_WATTER_ROW 16

/* private function prototypes */

void clearLog(); // clear log before drawing

static char* LOG_GRAPHIC[LOG_ANIM_TILES][LOG_HEIGHT+1] = {
  {"/======================\\",
   "|  x               x   |",
   "|   x             x    |",
   "\\======================/"},
  {"/======================\\",
   "|   x             x    |",
   "|  x               x   |",
   "\\======================/"}
};

/*** public functions ***/

/*-------------------------------------------------------------------------createLog
 * create a new log and return it (constructor)
 * also creates a new thread and starts it
 * streamRow: the stream row of the new log
 */
Log* createLog(int streamRow)
{
    int row = BOTTOM_WATTER_ROW - streamRow * LOG_HEIGHT;
    int speed = DEFAULT_LOG_SPEED - streamRow * ROW_SPEED_INC;
    
    int direction;
    int col;
    if (streamRow % 2 == 0)
    {
        col = LEFT_BOARD_BOUND - LOG_LENGTH;
        direction = DIRECTION_RIGHT;
    }
    else
    {
        col = GAME_COLS;
        direction = DIRECTION_LEFT;
    }
    
    /* properties assignment */
    Log *log = (Log*) malloc(sizeof(Log));
    log->streamRow = streamRow;
    log->row = row;
    log->col = col;
    log->speed = speed;
    log->direction = direction;
    log->frame = 0;
    log->alive = 1;
    
    createThread(&log->thread, logUpdate, log);
    
    return log;
}

/*-------------------------------------------------------------------------logUpdate
 * Log logic thread
 */
void *logUpdate(void *arg)
{
    Log *log = (Log*) arg;
    int frameCounter = 0;
    while (!gameOver && log->alive)
    {
        sleepTicks(log->speed);
        clearLog(log);
        
        log->col += log->direction;
        
        // if log has exited the game board, kill it
        if ((log->col + LOG_LENGTH < LEFT_BOARD_BOUND 
             && log->direction == DIRECTION_LEFT) ||
            (log->col > GAME_COLS && log->direction == DIRECTION_RIGHT))
        {
            log->alive = 0;
            pthread_exit(NULL);
        }
            
        // log animation
        if (frameCounter == 0)
            log->frame = (log->frame + 1) % 2;
        frameCounter = (frameCounter + 1) % 4;
    }
    
    pthread_exit(NULL);
}

/*--------------------------------------------------------------------------cleanLog
 * clean (free from mem) the log, remove from list, and join its thread
 * REMARKS: this public function is called by the upkeep thread
 */
void cleanLog(Log *log)
{
    if (log != NULL)
    {
        pthread_join(log->thread, NULL);
        removeLog(log, logRows[log->streamRow]->logs);
        free(log);
    }
}

/*---------------------------------------------------------------------------drawLog
 * draw the log
 */
void drawLog(Log *log)
{
    if (log != NULL)
    {
        pthread_mutex_lock(&drawMutex);
        consoleDrawImage(log->row, log->col, LOG_GRAPHIC[log->frame], LOG_HEIGHT);
        pthread_mutex_unlock(&drawMutex);
    }
}

/*** private functions ***/

/*--------------------------------------------------------------------------clearLog
 * clear the log (before drawing it)
 */
void clearLog(Log *log)
{
    if (log != NULL)
    {
        pthread_mutex_lock(&drawMutex);
        consoleClearImage(log->row, log->col, LOG_HEIGHT, strlen(LOG_GRAPHIC[0][0]));
        pthread_mutex_unlock(&drawMutex);
    }
}