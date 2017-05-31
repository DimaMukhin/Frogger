#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"
#include "log.h"
#include "frogger.h"
#include "gameglobals.h"

#define LOG_ANIM_TILES 2

/* private functions */
void clearLog();

static char* LOG_GRAPHIC[LOG_ANIM_TILES][LOG_HEIGHT+1] = {
  {"/======================\\",
   "|  x               x   |",
   "|   x             x    |",
   "\\======================/"},
  {"/======================\\",
   "|   x             x    |",
   "|  x               x   |",
   "\\======================/"}
};
char** logTile = LOG_GRAPHIC[0];

Log* createLog(int streamRow)
{
	int row = 16 - streamRow * LOG_HEIGHT;
	int speed = 10 - streamRow * 2;
	
	int direction;
	int col;
	if (streamRow % 2 == 0)
	{
		col = 0 - LOG_LENGTH;
		direction = -1;
	}
	else
	{
		col = 80;
		direction = 1;
	}
	
	Log *log = (Log*) malloc(sizeof(Log));
	log->streamRow = streamRow;
	log->row = row;
	log->col = col;
	log->speed = speed;
	log->direction = direction;
	return log;
}

void *logUpdate(void *arg)
{
	Log *log = (Log*) arg;
	int i = 0;
	while (1)
	{
		sleepTicks(log->speed);
		clearLog(log);
		
		log->col -= log->direction;
		
		// merge if and else if
		if (log->col + LOG_LENGTH < -10 && log->direction == 1)
		{
			removeLog(log, logList);
			free(log);
			pthread_exit(NULL);
		}
		else if (log->col > 80 && log->direction == -1)
		{
			removeLog(log, logList);
			free(log);
			pthread_exit(NULL);
		}
		
		logTile = LOG_GRAPHIC[i];
		i = (i + 1) % 2;
	}
}

void drawLog(Log *log)
{
	pthread_mutex_lock(&drawMutex);
	consoleDrawImage(log->row, log->col, logTile, LOG_HEIGHT);
	pthread_mutex_unlock(&drawMutex);
}

void clearLog(Log *log)
{
	pthread_mutex_lock(&drawMutex);
	consoleClearImage(log->row, log->col, LOG_HEIGHT, strlen(LOG_GRAPHIC[0][0]));
	pthread_mutex_unlock(&drawMutex);
}