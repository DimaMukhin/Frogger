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

LogRow* createLogRow(int row)
{	
	LogRow *logRowObj = (LogRow*) malloc(sizeof(LogRow));
	logRowObj->row = row;
	logRowObj->logs = (LogList*) malloc(sizeof(LogList));
	
	return logRowObj;
}

void *logRowUpdate(void *arg)
{
	srand(time(NULL));
	LogRow *logRow = (LogRow*) arg;
	pthread_t logThread[5];
	
	int i = 0;
	int randomWait = rand() % 200 + 300;
	while (!gameOver)
	{
		sleepTicks(1);
		
		if (i == 0)
		{
			Log *log = createLog(logRow->row);
			addLog(log, logRow->logs);
			
			// TODO: this one is also very dangerous, what if top log is not the new one?
			createThread(&logThread[0], logUpdate, logRow->logs->top->log);
		}
		
		i = (i + 1) % randomWait;
	}
	
	// TODO: HUGE problem with this. when you dont join a thread, you dont release it from memory
	// pthread_join(logThread[0], NULL);
	// pthread_join(logThread[1], NULL);
	// pthread_join(logThread[2], NULL);
	// pthread_join(logThread[3], NULL);
	// pthread_join(logThread[4], NULL);
	
	pthread_exit(NULL);
}

void drawLogRow(LogRow *logRow)
{
	LogNode *curr = logRow->logs->top;
	while (curr != NULL)
	{
		drawLog(curr->log);
		curr = curr->next;
	}
}