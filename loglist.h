/*
 * FILE     : loglist.h
 * REMARKS  : header file of loglist.c
 */

#ifndef LOGLIST_H
#define LOGLIST_H

#include "log.h"

typedef struct logNode LogNode;
struct logNode
{
    Log *log;
    LogNode *next; 
};

typedef struct logList
{
    LogNode *top;
} LogList;

/*** public functions ***/

// constructor
LogList* createLogList();

// add a new log to the list
void addLog(Log *log, LogList *logList);

// remove a log from the list
void removeLog(Log *log, LogList *logList);

// clean the log list
void cleanLogList(LogList *logList);

// get the first log of a stream row from the list
Log* getLog(int streamRow, LogList *logList);

#endif