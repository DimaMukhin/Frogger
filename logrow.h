/*
 * FILE     : logrow.h
 * REMARKS  : header file for logrow.c
 */

#ifndef LOGROW_H
#define LOGROW_H

#include "log.h"
#include "loglist.h"

typedef struct logRow
{
    int row;        // row (0..3)
    int speed;      // the speed of the logs in the row
    int direction;  // the direction of the logs in the row
    LogList *logs;  // the logs in the row
} LogRow;

/*** public functions ***/

// LogRow constructor
LogRow* createLogRow(int row);

// log row logic thread (spawn new logs)
void *logRowUpdate(void *arg);

// draw the whole row
void drawLogRow(LogRow *logRow);

// clean the whole row
void cleanLogRow(LogRow *logRow);

#endif