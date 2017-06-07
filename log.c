#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"
#include "log.h"
#include "frogger.h"
#include "gameglobals.h"

#define LOG_ANIM_TILES 2
#define BOTTOM_WATTER_ROW 16
#define DEFAULT_LOG_SPEED 10
#define ROW_SPEED_INC 2

/* private function prototypes */
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

Log* createLog(int streamRow)
{
	int row = BOTTOM_WATTER_ROW - streamRow * LOG_HEIGHT;
	int speed = DEFAULT_LOG_SPEED - streamRow * ROW_SPEED_INC;
	
	int direction;
	int col;
	if (streamRow % 2 == 0)
	{
		col = LEFT_BOARD_BOUND - LOG_LENGTH;
		direction = DIRECTION_RIGHT;
	}
	else
	{
		col = GAME_COLS;
		direction = DIRECTION_LEFT;
	}
	
	Log *log = (Log*) malloc(sizeof(Log));
	log->streamRow = streamRow;
	log->row = row;
	log->col = col;
	log->speed = speed;
	log->direction = direction;
	log->frame = 0;
	return log;
}

void *logUpdate(void *arg)
{
	Log *log = (Log*) arg;
	int frameCounter = 0;
	while (!gameOver)
	{
		sleepTicks(log->speed);
		clearLog(log);
		
		log->col += log->direction;
		
		if ((log->col + LOG_LENGTH < LEFT_BOARD_BOUND && log->direction == DIRECTION_LEFT) ||
			(log->col > GAME_COLS && log->direction == DIRECTION_RIGHT))
		{
			removeLog(log, logRows[log->streamRow]->logs);
			free(log);
			pthread_exit(NULL);
		}
				
		if (frameCounter == 0)
			log->frame = (log->frame + 1) % 2;
		frameCounter = (frameCounter + 1) % 4;
	}
	
	pthread_exit(NULL);
}

void drawLog(Log *log)
{
	pthread_mutex_lock(&drawMutex);
	consoleDrawImage(log->row, log->col, LOG_GRAPHIC[log->frame], LOG_HEIGHT);
	pthread_mutex_unlock(&drawMutex);
}

void clearLog(Log *log)
{
	pthread_mutex_lock(&drawMutex);
	consoleClearImage(log->row, log->col, LOG_HEIGHT, strlen(LOG_GRAPHIC[0][0]));
	pthread_mutex_unlock(&drawMutex);
}