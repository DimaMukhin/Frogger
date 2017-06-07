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
#include "threadwrappers.h"

#define NUM_ROWS 4
#define LIFE_COL 41
#define LIFE_ROW 1
#define MSG_LOST "looser"
#define MSG_WON "you won!"
#define KEY_W 'w'
#define KEY_A 'a'
#define KEY_S 's'
#define KEY_D 'd'

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

/* function prototypes */
void *draw();
void *keyboard();
void initializeThreads();
void gameOverWait();
void joinThreads();
void drawGameBoard();
int gameWon();

/* private variables */
char *gameOverMessage;

/* pthreads */
pthread_t drawThread;
pthread_t keyboardThread;
pthread_t playerUpdateThread;
pthread_t logRowThread[NUM_ROWS];

void *runFrogger()
{
	// initialize all game globals
	initializeGameGlobals();
		
	// initialize all threads
	initializeThreads();
	
	// wait for game over signal
	gameOverWait();
	
	// clean up threads
	joinThreads();

	// print final message and exit
	consoleFinish();
	putBanner(gameOverMessage);
	finalKeypress();
	pthread_exit(NULL);
}

void *draw()
{
	consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD);
	drawLives();
	
	drawGameBoard();
	while (!gameOver)
	{
		sleepTicks(DEFAULT_SLEEP_TICKS);
		
		for (int i = 0; i < NUM_ROWS; i++)
			drawLogRow(logRows[i]);

		drawPlayer();
		
		pthread_mutex_lock(&drawMutex);
		consoleRefresh();
		pthread_mutex_unlock(&drawMutex);
	}
	pthread_exit(NULL);
}

void *keyboard()
{
	fd_set set;
	int key;
	while (!gameOver)
	{
		sleepTicks(DEFAULT_SLEEP_TICKS);
		
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        struct timespec timeout = getTimeout(1); /* duration of one tick */
        int ret = pselect(FD_SETSIZE, &set, NULL, NULL, &timeout, NULL);
		if (ret == -1) 
		{ 
			perror("pselect error"); 
			exit(EXIT_FAILURE);
		}
		
		key = getchar();	
		if (key == KEY_W)
			movePlayerUp();
		else if (key == KEY_A)
			movePlayerLeft();
		else if (key == KEY_S)
			movePlayerDown();
		else if (key == KEY_D)
			movePlayerRight();
	}
	pthread_exit(NULL);
}

void initializeThreads()
{
	createThread(&drawThread, draw, NULL);
	createThread(&keyboardThread, keyboard, NULL);
	createThread(&playerUpdateThread, playerUpdate, NULL);
	for (int i = 0; i < NUM_ROWS; i++)
	{
		logRows[i] = createLogRow(i);
		createThread(&logRowThread[i], logRowUpdate, logRows[i]);
	}
}

void joinThreads()
{
	pthread_join(drawThread, NULL);
	pthread_join(keyboardThread, NULL);
	pthread_join(playerUpdateThread, NULL);
	for (int i = 0; i < NUM_ROWS; i++)
		pthread_join(logRowThread[i], NULL);
}

void gameOverWait()
{
	pthread_mutex_lock(&gameOverMutex);
	while (!gameOver)
		pthread_cond_wait(&gameOverCV, &gameOverMutex);
	pthread_mutex_unlock(&gameOverMutex);
}

void drawGameBoard()
{
	pthread_mutex_lock(&drawMutex);
	consoleDrawImage(0, 0, GAME_BOARD, GAME_ROWS);
	pthread_mutex_unlock(&drawMutex);
}

void drawLives()
{
	char life;
	sprintf(&life, "%d", getLives());
	
	pthread_mutex_lock(&drawMutex);
	putString(&life, 0, LIFE_COL, LIFE_ROW);
	pthread_mutex_unlock(&drawMutex);
}

void checkGameOver()
{
	if (getLives() == 0)
	{
		gameOverMessage = MSG_LOST;
		gameOver = 1;
	}	
	else if (gameWon())
	{
		gameOverMessage = MSG_WON;
		gameOver = 1;
	}
		
	if (gameOver)
	{
		pthread_mutex_lock(&gameOverMutex);
		pthread_cond_signal(&gameOverCV);
		pthread_mutex_unlock(&gameOverMutex);
	}
}

int gameWon()
{
	Home *home;
	for (int i = 0; i < NUM_HOMES; i++)
	{
		home = getHomeIndex(i);
		if (home->open)
			return 0;
	}
	
	return 1;
}
