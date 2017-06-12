/*
 * FILE     : log.h
 * REMARKS  : header file of log.c
 */

#ifndef LOG_H
#define LOG_H

#include <pthread.h>

#define LOG_HEIGHT 4
#define LOG_LENGTH 24

typedef struct log
{
    int streamRow;      // Stream Row (0..3)
    int row;            // draw row
    int col;            // draw column
    int speed;          // speed
    int direction;      // direction
    int frame;          // frame (animation)
    int alive;          // true if still in bounds
    pthread_t thread;   // for joining purposes
} Log;

/*** public functions ***/

// log constructor
Log* createLog(int streamRow);

// log logic thread
void *logUpdate(void *arg);

// draw the log
void drawLog(Log *log);

// log cleanup
void cleanLog(Log *log);

#endif