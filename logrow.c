/*
 * FILE     : logrow.c
 * REMARKS  : LogRow object implementation
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "logrow.h"
#include "log.h"
#include "console.h"
#include "gameglobals.h"
#include "loglist.h"
#include "threadwrappers.h"

#define MIN_RAND_WAIT 200   // minimum random wait before log spawn
#define MAX_RAND_WAIT 500   // maximum random wait before log spawn

/*** public functions ***/

/*----------------------------------------------------------------------createLogRow
 * Log row constructor
 */
LogRow* createLogRow(int row)
{   
    LogRow *logRowObj = (LogRow*) malloc(sizeof(LogRow));
    logRowObj->row = row;
    logRowObj->speed = DEFAULT_LOG_SPEED - row * ROW_SPEED_INC;
    
    if (row % 2 == 0)
        logRowObj->direction = DIRECTION_RIGHT;
    else
        logRowObj->direction = DIRECTION_LEFT;
    
    logRowObj->logs = (LogList*) malloc(sizeof(LogList));
    
    return logRowObj;
}

/*----------------------------------------------------------------------logRowUpdate
 * Log row logic thread (spawning of logs)
 */
void *logRowUpdate(void *arg)
{
    srand(time(NULL));
    LogRow *logRow = (LogRow*) arg;
    
    int randomCounter = 0; 
    int randomWait = rand() % MIN_RAND_WAIT + (MAX_RAND_WAIT - MIN_RAND_WAIT);
    while (!gameOver)
    {
        sleepTicks(DEFAULT_SLEEP_TICKS);
        
        if (randomCounter == 0)
        {
            Log *log = createLog(logRow->row); 
            addLog(log, logRow->logs);
        }
        
        randomCounter = (randomCounter + 1) % randomWait;
    }
    
    pthread_exit(NULL);
}

/*-----------------------------------------------------------------------cleanLogRow
 * log row cleanup (free and clean all logs associated with it)
 */
void cleanLogRow(LogRow *logRow)
{
    if (logRow != NULL)
    {
        cleanLogList(logRow->logs);
        free(logRow);
    }
}

/*------------------------------------------------------------------------drawLogRow
 * draw all the logs associated with this row
 */
void drawLogRow(LogRow *logRow)
{
    LogNode *curr = logRow->logs->top;
    while (curr != NULL)
    {
        drawLog(curr->log);
        curr = curr->next;
    }
}