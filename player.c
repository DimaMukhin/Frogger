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
void clearPlayer();
void landPlayer();
void killFrog();
void checkBounds();
void animatePlayer();
void restartPlayer();
int isPlayerOverWater();

/* private setters (for mutex) */
void setPlayerBoardRow(int new);
void setPlayerSpeed(int new);
void setPlayerDirection(int new);
void setPlayerRow(int new);
void setPlayerCol(int new);

/* private variables */
int playerBoardRow = DEFAULT_BOARD_ROW;
int playerSpeed = DEFAULT_SPEED;
int playerDirection = DIRECTION_STAY;
int playerRow = DEFAULT_PLAYER_ROW;
int playerCol = DEFAULT_PLAYER_COL;

static char* PLAYER_GRAPHIC[ANIM_TILES][PLAYER_HEIGHT+1] = {
  {"@@",
   "<>"},
  {"--",
   "<>"}
};
char** playerTile = PLAYER_GRAPHIC[1];
int animationCounter = 0;
int animTile = 0;

// player thread
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

/* public functions */
/* NOTE: can be called by different threads (use mutex) */

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

void movePlayerLeft()
{
	clearPlayer();
	setPlayerCol(playerCol - 1);
}

void movePlayerRight()
{
	clearPlayer();
	setPlayerCol(playerCol + 1);
}

/* private functions */

void checkBounds()
{
	if (playerCol <= LEFT_BOARD_BOUND)
		setPlayerCol(LEFT_BOARD_BOUND);
	else if (playerCol >= GAME_COLS - PLAYER_WIDTH)
		setPlayerCol(GAME_COLS - PLAYER_WIDTH);	
}

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

void drawPlayer()
{
	pthread_mutex_lock(&drawMutex);
	consoleDrawImage(playerRow, playerCol, playerTile, PLAYER_HEIGHT);
	pthread_mutex_unlock(&drawMutex);
}

void clearPlayer()
{
	pthread_mutex_lock(&drawMutex);
	consoleClearImage(playerRow, playerCol, PLAYER_HEIGHT, strlen(PLAYER_GRAPHIC[0][0]));
	pthread_mutex_unlock(&drawMutex);
}

void landPlayer()
{
	// if player is above raging crocodile infested water
	if (playerBoardRow >= WATTER_LOWER_ROW && playerBoardRow <= WATTER_UPPER_ROW)
	{
		if (!isPlayerOverWater())
		{
			Log *log = logRows[playerBoardRow - 1]->logs->top->log; // TODO: this is not safe!! what if log row is empty
			setPlayerSpeed(log->speed);
			setPlayerDirection(log->direction);
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

void killFrog()
{
	setLives(getLives() - 1);
	drawLives();
	checkGameOver();		
	restartPlayer();
}

void restartPlayer()
{
	setPlayerRow(DEFAULT_PLAYER_ROW);
	setPlayerCol(DEFAULT_PLAYER_COL);
	setPlayerSpeed(DEFAULT_SPEED);
	setPlayerDirection(DIRECTION_STAY);
	setPlayerBoardRow(DEFAULT_BOARD_ROW);
}

/* private setters */

void setPlayerBoardRow(int boardRow)
{
	pthread_mutex_lock(&playerMutex);
	playerBoardRow = boardRow;
	pthread_mutex_unlock(&playerMutex);
}

void setPlayerSpeed(int speed)
{
	pthread_mutex_lock(&playerMutex);
	playerSpeed = speed;
	pthread_mutex_unlock(&playerMutex);
}

void setPlayerDirection(int direction)
{
	pthread_mutex_lock(&playerMutex);
	playerDirection = direction;
	pthread_mutex_unlock(&playerMutex);
}

void setPlayerRow(int row)
{
	pthread_mutex_lock(&playerMutex);
	playerRow = row;
	pthread_mutex_unlock(&playerMutex);
}

void setPlayerCol(int col)
{
	pthread_mutex_lock(&playerMutex);
	playerCol = col;
	pthread_mutex_unlock(&playerMutex);
}
