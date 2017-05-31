#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "logrow.h"
#include "log.h"
#include "console.h"
#include "gameglobals.h"

LogRow* createLogRow(int row)
{	
	LogRow *logRowObj = (LogRow*) malloc(sizeof(LogRow));
	logRowObj->row = row;
	logRowObj->log = NULL;
	
	return logRowObj;
}

void *logRowUpdate(void *arg)
{
	LogRow *logRow = (LogRow*) arg;
	Log *log = createLog(logRow->row);
	logRow->log = log;
	addLog(log, logList);
	
	while (1)
	{
		sleepTicks(rand() % 250);
		pthread_t logThread;
		if (pthread_create(&logThread, NULL, logUpdate, logRow->log) == -1) { perror("log"); exit(EXIT_FAILURE); }
		
		pthread_join(logThread, NULL);
		Log *log = createLog(logRow->row);
		logRow->log = log;
		addLog(log, logList);
		srand(time(NULL));
	}
	
	
}

void drawLogRow(LogRow *logRow)
{
	if (logRow->log != NULL)
		drawLog(logRow->log);
}