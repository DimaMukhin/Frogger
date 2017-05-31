#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "logrow.h"
#include "log.h"
#include "console.h"
#include "gameglobals.h"
#include "loglist.h"

LogRow* createLogRow(int row)
{	
	LogRow *logRowObj = (LogRow*) malloc(sizeof(LogRow));
	logRowObj->row = row;
	logRowObj->logs = (LogList*) malloc(sizeof(LogList));
	
	return logRowObj;
}

void *logRowUpdate(void *arg)
{
	LogRow *logRow = (LogRow*) arg;
	pthread_t logThread[5];
	
	int i = 0;
	while (1)
	{
		sleepTicks(rand() % 500 + 500);
		
		Log *log = createLog(logRow->row);
		addLog(log, logRow->logs);
		
		// TODO: this one is also very dangerous, what if top log is not the new one?
		if (pthread_create(&logThread[0], NULL, logUpdate, logRow->logs->top->log) == -1) { perror("log"); exit(EXIT_FAILURE); }
		
		i = (i + 1) % 5;
		srand(time(NULL));
	}
	
	// TODO: HUGE problem with this. when you dont join a thread, you dont release it from memory
	pthread_join(logThread[0], NULL);
	pthread_join(logThread[1], NULL);
	pthread_join(logThread[2], NULL);
	pthread_join(logThread[3], NULL);
	pthread_join(logThread[4], NULL);
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