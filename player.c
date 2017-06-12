/*
 * FILE     : player.c
 * REMARKS  : player logic
 */

#include <string.h>
#include <pthread.h>

#include "console.h"
#include "player.h"
#include "frogger.h"
#include "log.h"
#include "loglist.h"
#include "gameglobals.h"

#define ANIMATION_SPEED 200
#define ANIM_TILES 2
#define DEFAULT_BOARD_ROW 0
#define DEFAULT_PLAYER_COL 40
#define DEFAULT_PLAYER_ROW 21
#define DEFAULT_SPEED 1
#define FROG_JUMP_LENGTH 4
#define HOME_ROW 5
#define LAND_ROW 0
#define PLAYER_HEIGHT 2
#define PLAYER_WIDTH 2
#define GAME_COLS 80
#define WATTER_LOWER_ROW 1
#define WATTER_UPPER_ROW 4

/* private functions */

void clearPlayer();         // clear the player before drawing
void landPlayer();          // land the player after jump (landing logic)
void killFrog();            // kill the frog
void checkBounds();         // check right/left game board bounds
void animatePlayer();       // player animation logic
void restartPlayer();       // restart the player
int isPlayerOverWater();    // is the player over/above water?

/* private setters (for mutex) */

void setPlayerBoardRow(int new);    // playerBoardRow setter
void setPlayerSpeed(int new);       // playerSpeed setter
void setPlayerDirection(int new);   // playerDirection setter
void setPlayerRow(int new);         // playerRow setter
void setPlayerCol(int new);         // playerCol setter

/* private variables */

int playerBoardRow = DEFAULT_BOARD_ROW; // players row relative to board (0..5)
int playerSpeed = DEFAULT_SPEED;        // the speed of the player
int playerDirection = DIRECTION_STAY;   // movement direction of the player
int playerRow = DEFAULT_PLAYER_ROW;     // player draw row
int playerCol = DEFAULT_PLAYER_COL;     // player draw col

static char* PLAYER_GRAPHIC[ANIM_TILES][PLAYER_HEIGHT+1] = {
  {"@@",
   "<>"},
  {"--",
   "<>"}
};
char** playerTile = PLAYER_GRAPHIC[1];
int animationCounter = 0;
int animTile = 0;

/*** public functions ***/

/*----------------------------------------------------------------------playerUpdate
 * player logic thread
 */
void *playerUpdate()
{
    while (!gameOver)
    {
        sleepTicks(playerSpeed);
        
        // move player
        if (playerDirection != DIRECTION_STAY)
        {
            clearPlayer();
            setPlayerCol(playerCol + playerDirection);
        }
        
        checkBounds();
        
        if (isPlayerOverWater())
                killFrog();
            
        animatePlayer();
    }
    pthread_exit(NULL);
}

/*------------------------------------------------------------------------drawPlayer
 * Draw the player
 */
void drawPlayer()
{
    pthread_mutex_lock(&drawMutex);
    consoleDrawImage(playerRow, playerCol, playerTile, PLAYER_HEIGHT);
    pthread_mutex_unlock(&drawMutex);
}

/*----------------------------------------------------------------------movePlayerUp
 * Move plater UP
 * NOTE: can be called by different threads (use mutex)
 */
void movePlayerUp()
{
    clearPlayer();
    if (playerBoardRow < WATTER_UPPER_ROW)
    {
        setPlayerRow(playerRow - FROG_JUMP_LENGTH);
        setPlayerBoardRow(playerBoardRow + 1);
        landPlayer();
    }
    else if (playerBoardRow == WATTER_UPPER_ROW)
    {
        setPlayerBoardRow(playerBoardRow + 1);
        landPlayer();
    }       
}

/*--------------------------------------------------------------------movePlayerDown
 * Move plater DOWN
 * NOTE: can be called by different threads (use mutex)
 */
void movePlayerDown()
{
    clearPlayer();
    if (playerBoardRow != 0)
    {
        setPlayerRow(playerRow + FROG_JUMP_LENGTH);
        setPlayerBoardRow(playerBoardRow - 1);
        landPlayer();
    }
}

/*--------------------------------------------------------------------movePlayerLeft
 * Move plater LEFT
 * NOTE: can be called by different threads (use mutex)
 */
void movePlayerLeft()
{
    clearPlayer();
    setPlayerCol(playerCol - 1);
}

/*-------------------------------------------------------------------movePlayerRight
 * Move plater RIGHT
 * NOTE: can be called by different threads (use mutex)
 */
void movePlayerRight()
{
    clearPlayer();
    setPlayerCol(playerCol + 1);
}

/*** private functions ***/

/*-----------------------------------------------------------------------checkBounds
 * check if player hit any of the bounds and push him back
 */
void checkBounds()
{
    if (playerCol <= LEFT_BOARD_BOUND)
        setPlayerCol(LEFT_BOARD_BOUND);
    else if (playerCol >= GAME_COLS - PLAYER_WIDTH)
        setPlayerCol(GAME_COLS - PLAYER_WIDTH); 
}

/*---------------------------------------------------------------------animatePlayer
 * Animate the player (blinking)
 */
void animatePlayer()
{
    if (animationCounter == 0)
    {
        playerTile = PLAYER_GRAPHIC[animTile];
        animTile = (animTile + 1) % 2;
    }
    
    animationCounter = (animationCounter + playerSpeed);
    if (animationCounter > ANIMATION_SPEED)
        animationCounter = 0;
}

/*-----------------------------------------------------------------------clearPlayer
 * clear the player before drawing again
 */
void clearPlayer()
{
    pthread_mutex_lock(&drawMutex);
    consoleClearImage(playerRow, playerCol, PLAYER_HEIGHT, strlen(PLAYER_GRAPHIC[0][0]));
    pthread_mutex_unlock(&drawMutex);
}

/*------------------------------------------------------------------------landPlayer
 * landing player logic. 3 different cases:
 *  1. player is above water (above log or above empty water)
 *  2. player is above land
 *  3. player is above home
 */
void landPlayer()
{
    // if player is above raging crocodile infested water
    if (playerBoardRow >= WATTER_LOWER_ROW && playerBoardRow <= WATTER_UPPER_ROW)
    {
        if (!isPlayerOverWater()) // means he is on a log
        {
            if (logRows[playerBoardRow - 1] != NULL)
            {
                setPlayerSpeed(logRows[playerBoardRow - 1]->speed);
                setPlayerDirection(logRows[playerBoardRow - 1]->direction);
            }
        }
        else
            killFrog();
    }
    else if (playerBoardRow == LAND_ROW) // if player landed on land
    {
        setPlayerSpeed(DEFAULT_SPEED);
        setPlayerDirection(DIRECTION_STAY);
    }
    else if (playerBoardRow == HOME_ROW)
    {
        Home *home = getHome(playerCol);
        if (home != NULL && home->open != 0)
        {
            home->open = 0;
            setPlayerRow(playerRow - FROG_JUMP_LENGTH + 1);
            playerTile = PLAYER_GRAPHIC[0];
            drawPlayer();
            
            restartPlayer();
            
            checkGameOver();
        }
        else
        {
            setPlayerBoardRow(HOME_ROW - 1);
        }
    }
}

/*-----------------------------------------------------------------isPlayerOverWater
 * is the player over water? 
 * NOTE: if the player is above a log, it is not over water.
 */
int isPlayerOverWater()
{
    int flag = 1;
    if (playerBoardRow == LAND_ROW)
        flag = 0;
    else if (playerBoardRow >= WATTER_LOWER_ROW && playerBoardRow <= WATTER_UPPER_ROW)
    {
        LogRow *logRow = logRows[playerBoardRow - 1];
        LogNode *curr = logRow->logs->top;
        while (curr != NULL)
        {
            Log *log = curr->log;
            
            if (log != NULL && playerCol > log->col && playerCol < (log->col + LOG_LENGTH))
                flag = 0;
            
            curr = curr->next;
        }   
    }
    
    return flag;
}

/*--------------------------------------------------------------------------killFrog
 * Kill frog, update lives, is game over?
 */
void killFrog()
{
    setLives(getLives() - 1);
    drawLives();
    checkGameOver();        
    restartPlayer();
}

/*---------------------------------------------------------------------restartPlayer
 * restart player's variables to default
 */
void restartPlayer()
{
    setPlayerRow(DEFAULT_PLAYER_ROW);
    setPlayerCol(DEFAULT_PLAYER_COL);
    setPlayerSpeed(DEFAULT_SPEED);
    setPlayerDirection(DIRECTION_STAY);
    setPlayerBoardRow(DEFAULT_BOARD_ROW);
}

/* private setters (to ensure MUTEX) */

/*-----------------------------------------------------------------setPlayerBoardRow
 * set player board row
 */
void setPlayerBoardRow(int boardRow)
{
    pthread_mutex_lock(&playerMutex);
    playerBoardRow = boardRow;
    pthread_mutex_unlock(&playerMutex);
}

/*--------------------------------------------------------------------setPlayerSpeed
 * set player speed
 */
void setPlayerSpeed(int speed)
{
    pthread_mutex_lock(&playerMutex);
    playerSpeed = speed;
    pthread_mutex_unlock(&playerMutex);
}

/*----------------------------------------------------------------setPlayerDirection
 * set player direction
 */
void setPlayerDirection(int direction)
{
    pthread_mutex_lock(&playerMutex);
    playerDirection = direction;
    pthread_mutex_unlock(&playerMutex);
}

/*----------------------------------------------------------------------setPlayerRow
 * set player row
 */
void setPlayerRow(int row)
{
    pthread_mutex_lock(&playerMutex);
    playerRow = row;
    pthread_mutex_unlock(&playerMutex);
}

/*----------------------------------------------------------------------setPlayerCol
 * set player column
 */
void setPlayerCol(int col)
{
    pthread_mutex_lock(&playerMutex);
    playerCol = col;
    pthread_mutex_unlock(&playerMutex);
}
