/*
 * FILE     : loglist.c
 * REMARKS  : a linked list for logs
 */

#include <stdlib.h>

#include "loglist.h"
#include "gameglobals.h"

/*** public functions ***/

/*--------------------------------------------------------------------createLogList
 * constructor
 */
LogList* createLogList()
{
    LogList *logList = (LogList*) malloc(sizeof(LogList));
    logList->top = NULL;
    return logList;
}

/*----------------------------------------------------------------------------addLog
 * add a new log to the list.
 * NOTE: MUTEX is very important here since multiple threads can try to
 *       manipulate the same list at the same time.
 */
void addLog(Log *log, LogList *logList)
{
    pthread_mutex_lock(&logListMutex);
    
    LogNode *newNode = (LogNode*) malloc(sizeof(LogNode));
    newNode->log = log;
    newNode->next = logList->top;
    logList->top = newNode;

    pthread_mutex_unlock(&logListMutex);
}

/*-------------------------------------------------------------------------removeLog
 * remove the specified log from the list.
 * NOTE: MUTEX is very important here since multiple threads can try to
 *       manipulate the same list at the same time.
 */
void removeLog(Log *log, LogList *logList)
{
    if (log == NULL || logList == NULL)
        return;
    
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

/*----------------------------------------------------------------------------getLog
 * get the first log associated with the stream row.
 * NOTE: MUTEX is very important here since multiple threads can try to
 *       manipulate the same list at the same time.
 */
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

/*----------------------------------------------------------------------cleanLogList
 * log list cleanup
 * NOTE: MUTEX is very important here since multiple threads can try to
 *       manipulate the same list at the same time.
 */
void cleanLogList(LogList *logList)
{   
    if (logList == NULL)
        return;
    
    pthread_mutex_lock(&logListMutex);
    
    LogNode *curr = logList->top;
    LogNode *prev = NULL;

    while (curr != NULL)
    {
        if (prev != NULL)
            free(prev);
            
        prev = curr;
        curr = curr->next;
    }

    free(logList);
    
    pthread_mutex_unlock(&logListMutex);
}