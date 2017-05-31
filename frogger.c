#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "console.h"
#include "frogger.h"
#include "player.h"
#include "log.h"
#include "logrow.h"
#include "gameglobals.h"

#define GAME_ROWS 24
#define GAME_COLS 80

/* function prototypes */
void *draw();
void *keyboard();

/**** DIMENSIONS MUST MATCH the ROWS/COLS */

char *GAME_BOARD[] = {
"                                   Lives:",
"/------\\          /------\\          /------\\          /------\\          /------\\",
"|      |          |      |          |      |          |      |          |      |",
"+      +----------+      +----------+      +----------+      +----------+      +",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",
"",
"",
"" };

LogRow *logRow0;
LogRow *logRow1;
LogRow *logRow2;
LogRow *logRow3;

// game setup function
void runFrogger()
{
	initializeGameGlobals();
	
	pthread_t drawThread;
	if (pthread_create(&drawThread, NULL, draw, NULL) == -1) { perror("draw"); exit(EXIT_FAILURE); }
		
	pthread_t keyboardThread;
	if (pthread_create(&keyboardThread, NULL, keyboard, NULL) == -1) { perror("keyboard"); exit(EXIT_FAILURE); }
		
	pthread_t playerUpdateThread;
	if (pthread_create(&playerUpdateThread, NULL, playerUpdate, NULL) == -1) { perror("playerUpdate"); exit(EXIT_FAILURE); }
	
	logRow0 = createLogRow(0);
	pthread_t logRow0Thread;
	if (pthread_create(&logRow0Thread, NULL, logRowUpdate, logRow0) == -1) { perror("logRow0"); exit(EXIT_FAILURE); }
	
	logRow1 = createLogRow(1);
	pthread_t logRow1Thread;
	if (pthread_create(&logRow1Thread, NULL, logRowUpdate, logRow1) == -1) { perror("logRow1"); exit(EXIT_FAILURE); }
	
	logRow2 = createLogRow(2);
	pthread_t logRow2Thread;
	if (pthread_create(&logRow2Thread, NULL, logRowUpdate, logRow2) == -1) { perror("logRow2"); exit(EXIT_FAILURE); }
	
	logRow3 = createLogRow(3);
	pthread_t logRow3Thread;
	if (pthread_create(&logRow3Thread, NULL, logRowUpdate, logRow3) == -1) { perror("logRow3"); exit(EXIT_FAILURE); }
	
	pthread_join(drawThread, NULL);
	pthread_join(keyboardThread, NULL);
	pthread_join(playerUpdateThread, NULL);
	pthread_join(logRow0Thread, NULL);
	pthread_join(logRow1Thread, NULL);
	pthread_join(logRow2Thread, NULL);
	pthread_join(logRow3Thread, NULL);
}

void *draw()
{
	consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD);
	drawLives();
	
	pthread_mutex_lock(&drawMutex);
	consoleDrawImage(0, 0, GAME_BOARD, GAME_ROWS);
	pthread_mutex_unlock(&drawMutex);
	while (1)
	{
		sleepTicks(1);
				
		drawLogRow(logRow0);
		drawLogRow(logRow1);
		drawLogRow(logRow2);
		drawLogRow(logRow3);
		drawPlayer();
		
		pthread_mutex_lock(&drawMutex);
		consoleRefresh();
		pthread_mutex_unlock(&drawMutex);
	}
	pthread_exit(NULL);
}

void *keyboard()
{
	fd_set set; /* what to check for our select call */
	int key;
	while (1)
	{
		sleepTicks(1);
        /* setup select to listen to stdin. necessary as getchar
           is blocking and cannot be easily unblocked, e.g.,
           to end the game*/
        /* re-set each time as it can get overwritten */
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        struct timespec timeout = getTimeout(1); /* duration of one tick */
        int ret = pselect(FD_SETSIZE, &set, NULL, NULL, &timeout, NULL);
        /* ret tells you why select returned. Maybe a key is available? maybe it timed out? */
		
		key = getchar();
		
		if (key == 'w')
			movePlayerUp();
		else if (key == 'a')
			movePlayerLeft();
		else if (key == 's')
			movePlayerDown();
		else if (key == 'd')
			movePlayerRight();
	}
	pthread_exit(NULL);
}

void drawLives()
{
	pthread_mutex_lock(&drawMutex);
	char life;
	sprintf(&life, "%d", lives);
	putString(&life, 0, 41, 1);
	pthread_mutex_unlock(&drawMutex);
}
