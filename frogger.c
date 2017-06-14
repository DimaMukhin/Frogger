/*
 * FILE     : frogger.c
 * REMARKS  : Contains main game logic, thread initialization, etc.
 */

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
#define MSG_WON "winner!"
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

/* private function prototypes */

void *draw();               // draw and refresh thread
void *keyboard();           // keyboard thread
void *upkeep();             // upkeep thread
void initializeThreads();   // initialize all threads
void gameOverWait();        // wait for game to be over (CV)
void joinThreads();         // join all threads
void cleanDeadLogs();       // cleanup of dead logs
void cleanLogRows();        // general cleanup of all log rows
void drawGameBoard();       // draw the game board
void printEndGameMessage(); // print the end game message
int gameWon();              // was game won?

/* private variables */

char *gameOverMessage;      // message to display once the game is over (win/lose)

/* private pthreads */

pthread_t drawThread;               // draw/refresh thread
pthread_t keyboardThread;           // keyboard input thread
pthread_t playerUpdateThread;       // player logic thread
pthread_t upkeepThread;             // upkeep (cleanup) thread
pthread_t logRowThread[NUM_ROWS];   // log row thread

/*** public functions ***/

/*-----------------------------------------------------------------------------runFrogger
 * main game thread,
 * controls initialization of threads, and end game cleanup
 */
void *runFrogger()
{
    initializeGameGlobals();
        
    initializeThreads();
    
    gameOverWait();
    
    // Wait is done... (game is over)
    
    joinThreads();
    cleanLogRows();
    consoleFinish();
    
    finalKeypress();
    pthread_exit(NULL);
}

/*-------------------------------------------------------------------------drawLives
 * Draws remaining life on screen
 */
void drawLives()
{
    char life;
    sprintf(&life, "%d", getLives());
    
    pthread_mutex_lock(&drawMutex);
    putString(&life, 0, LIFE_COL, LIFE_ROW);
    pthread_mutex_unlock(&drawMutex);
}

/*---------------------------------------------------------------------checkGameOver
 * Check if the game is over and handle end of game
 */
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

/*** private functions ***/

/*------------------------------------------------------------------------------draw
 * Refresh and draw of different components
 */
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
    
    printEndGameMessage();
    pthread_exit(NULL);
}

/*--------------------------------------------------------------------------keyboard
 * Handle keyboard input
 */
void *keyboard()
{
    fd_set set;
    int key;
    while (!gameOver)
    {
        sleepTicks(DEFAULT_SLEEP_TICKS);
        
        /* timeout logic for joining the thread at end of game */
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        struct timespec timeout = getTimeout(1); /* duration of one tick */
        int ret = pselect(FD_SETSIZE, &set, NULL, NULL, &timeout, NULL);
        if (ret == -1) 
        { 
            perror("pselect error"); 
            exit(EXIT_FAILURE);
        }
        
        /* keyboard input logic */
        if (ret != 0)
        {
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
        
    }
    pthread_exit(NULL);
}

/*----------------------------------------------------------------------------upkeep
 * Upkeep (log cleanup)
 */
void *upkeep()
{
    while (!gameOver)
    {
        sleepTicks(DEFAULT_SLEEP_TICKS);
        cleanDeadLogs();
    }
    
    pthread_exit(NULL);
}

/*-----------------------------------------------------------------initializeThreads
 * initialize all threads (except for log threads)
 */
void initializeThreads()
{
    createThread(&drawThread, draw, NULL);
    createThread(&keyboardThread, keyboard, NULL);
    createThread(&upkeepThread, upkeep, NULL);
    createThread(&playerUpdateThread, playerUpdate, NULL);
    for (int i = 0; i < NUM_ROWS; i++)
    {
        logRows[i] = createLogRow(i);
        createThread(&logRowThread[i], logRowUpdate, logRows[i]);
    }
}

/*-----------------------------------------------------------------------joinThreads
 * join all threads (except for log threads)
 */
void joinThreads()
{
    pthread_join(drawThread, NULL);
    pthread_join(keyboardThread, NULL);
    pthread_join(upkeepThread, NULL);
    pthread_join(playerUpdateThread, NULL);
    for (int i = 0; i < NUM_ROWS; i++)
        pthread_join(logRowThread[i], NULL);
}

/*---------------------------------------------------------------------cleanDeadLogs
 * free dead logs and remove them from the list
 */
void cleanDeadLogs()
{
    for (int i = 0; i < NUM_LOG_ROWS; i++)
    {
        LogRow *logRow = logRows[i];
        LogList *logList = logRow->logs;
        LogNode *curr = logList->top;
        while (curr != NULL) 
        {
            Log *log = curr->log;
            if (log != NULL && !log->alive)
            {
                cleanLog(log);
            }
            curr = curr->next;
        }
    }
}

/*----------------------------------------------------------------------cleanLogRows
 * clean of all data stractures: log list, logs, log rows etc.
 * cleanup for end of game.
 */
void cleanLogRows()
{
    for (int i = 0; i < NUM_LOG_ROWS; i++)
    {
        cleanLogRow(logRows[i]);
    }
}

/*----------------------------------------------------------------------gameOverWait
 * setup a condition variable and wait for an end of game signal
 */
void gameOverWait()
{
    pthread_mutex_lock(&gameOverMutex);
    while (!gameOver)
        pthread_cond_wait(&gameOverCV, &gameOverMutex);
    pthread_mutex_unlock(&gameOverMutex);
}

/*---------------------------------------------------------------------drawGameBoard
 * draw the board!
 */
void drawGameBoard()
{
    pthread_mutex_lock(&drawMutex);
    consoleDrawImage(0, 0, GAME_BOARD, GAME_ROWS);
    pthread_mutex_unlock(&drawMutex);
}

/*---------------------------------------------------------------printEndGameMessage
 * print end game message and disable the console
 * REMARKS: disabling the console is important for not having any
 *          artifacts on the screen or missing content
 */
void printEndGameMessage()
{
    pthread_mutex_lock(&drawMutex);
    putBanner(gameOverMessage);
    consoleRefresh();
    pthread_mutex_unlock(&drawMutex);
    disableConsole(1);
}

/*---------------------------------------------------------------------------gameWon
 * check if game was won
 */
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
