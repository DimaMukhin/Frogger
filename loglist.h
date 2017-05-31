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

LogList* createLogList();

void addLog(Log *log, LogList *logList);

void removeLog(Log *log, LogList *logList);

Log* getLog(int streamRow, LogList *logList);

#endif