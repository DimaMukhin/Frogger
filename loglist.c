#include <stdlib.h>

#include "loglist.h"
#include "gameglobals.h"

LogList* createLogList()
{
	LogList *logList = (LogList*) malloc(sizeof(LogList));
	logList->top = NULL;
	return logList;
}

void addLog(Log *log, LogList *logList)
{
	pthread_mutex_lock(&logListMutex);
	
	LogNode *newNode = (LogNode*) malloc(sizeof(LogNode));
	newNode->log = log;
	newNode->next = logList->top;
	logList->top = newNode;

	pthread_mutex_unlock(&logListMutex);
}

void removeLog(Log *log, LogList *logList)
{
	pthread_mutex_lock(&logListMutex);
	
	LogNode *curr = logList->top;
	LogNode *prev = NULL;
	while (curr != NULL && curr->log != log)
	{
		prev = curr;
		curr = curr->next;
	}
	
	if (curr != NULL)
	{
		if (prev != NULL)
			prev->next = curr->next;
		else
			logList->top = curr->next;
	}
	
	free(curr);
	
	pthread_mutex_unlock(&logListMutex);
}

Log* getLog(int streamRow, LogList *logList)
{
	pthread_mutex_lock(&logListMutex);
	
	LogNode *curr = logList->top;
	while (curr != NULL && curr->log->streamRow != streamRow)
		curr = curr->next;
	
	if (curr != NULL)
		return curr->log;
	
	return NULL;
	
	pthread_mutex_unlock(&logListMutex);
}