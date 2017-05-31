#include <stdlib.h>

#include "loglist.h"

LogList* createLogList()
{
	LogList *logList = (LogList*) malloc(sizeof(LogList));
	logList->top = NULL;
	return logList;
}

void addLog(Log *log, LogList *logList)
{
	LogNode *newNode = (LogNode*) malloc(sizeof(LogNode));
	newNode->log = log;
	
	LogNode *curr = logList->top;
	LogNode *prev = NULL;
	while (curr != NULL && curr->log->streamRow < log->streamRow)
	{
		prev = curr;
		curr = curr->next;
	}
	
	if (prev == NULL)
		logList->top = newNode;
	else
		prev->next = newNode;
	
	newNode->next = curr;
}

void removeLog(Log *log, LogList *logList)
{
	LogNode *curr = logList->top;
	LogNode *prev = NULL;
	while (curr != NULL && curr->log->streamRow != log->streamRow)
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
}

Log* getLog(int streamRow, LogList *logList)
{
	LogNode *curr = logList->top;
	while (curr != NULL && curr->log->streamRow != streamRow)
		curr = curr->next;
	
	if (curr != NULL)
		return curr->log;
	
	return NULL;
}