#include <string.h>
#include <pthread.h>

#include "console.h"
#include "player.h"
#include "frogger.h"
#include "log.h"
#include "loglist.h"
#include "gameglobals.h"

#define PLAYER_ANIM_TILES 2
#define PLAYER_HEIGHT 2
#define PLAYER_WIDTH 2
#define DEFAULT_PLAYER_ROW 21
#define DEFAULT_PLAYER_COL 40

/* private functions */
void clearPlayer();
void landPlayer();
int isPlayerOverWater();
void killFrog();

/* private setters (for mutex) */
void setPlayerBoardRow(int new);
void setPlayerSpeed(int new);
void setPlayerDirection(int new);
void setPlayerRow(int new);
void setPlayerCol(int new);

/* private variables */
int playerBoardRow = 0;
int playerSpeed = 1;
int playerDirection = 0;
int playerRow = DEFAULT_PLAYER_ROW;
int playerCol = DEFAULT_PLAYER_COL;

static char* PLAYER_GRAPHIC[PLAYER_ANIM_TILES][PLAYER_HEIGHT+1] = {
  {"@@",
   "<>"},
  {"--",
   "<>"}
};
char** playerTile = PLAYER_GRAPHIC[1];

// player thread
void *playerUpdate()
{
	int i = 0;
	int animTile = 0;
	while (1)
	{
		sleepTicks(playerSpeed);
		
		setPlayerCol(playerCol - playerDirection);
		if (playerCol <= 0)
			setPlayerCol(0);
		else if (playerCol >= 80 - PLAYER_WIDTH)
			setPlayerCol(80 - PLAYER_WIDTH);	
		
		if (isPlayerOverWater())
				killFrog();
		
		if (i == 0)
		{
			playerTile = PLAYER_GRAPHIC[animTile];
			animTile = (animTile + 1) % 2;
		}
		
		i = (i + playerSpeed);
		if (i > 200)
			i = 0;
	}
	pthread_exit(NULL);
}

/* public functions */
/* NOTE: can be called by different threads (use mutex) */

void movePlayerUp()
{
	clearPlayer();
	if (playerBoardRow < 4)
	{
		setPlayerRow(playerRow - 4);
		setPlayerBoardRow(playerBoardRow + 1);
		landPlayer();
	}
	else if (playerBoardRow == 4)
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
		setPlayerRow(playerRow + 4);
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
	if (playerBoardRow > 0 && playerBoardRow < 5)
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
	else if (playerBoardRow == 0) // if player landed on land
	{
		setPlayerSpeed(1);
		setPlayerDirection(0);
	}
	else if (playerBoardRow == 5)
	{
		Home *home = getHome(playerCol);
		if (home != NULL && home->open != 0)
		{
			home->open = 0;
			setPlayerRow(playerRow - 3);
			playerTile = PLAYER_GRAPHIC[0];
			drawPlayer();
			
			setPlayerRow(DEFAULT_PLAYER_ROW);
			setPlayerCol(DEFAULT_PLAYER_COL);
			setPlayerSpeed(1);
			setPlayerDirection(0);
			setPlayerBoardRow(0);
		}
		else
		{
			setPlayerBoardRow(4);
		}
	}
}

int isPlayerOverWater()
{
	int flag = 1;
	if (playerBoardRow == 0)
		flag = 0;
	else if (playerBoardRow > 0 && playerBoardRow < 5)
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
			
	setPlayerRow(DEFAULT_PLAYER_ROW);
	setPlayerCol(DEFAULT_PLAYER_COL);
	setPlayerSpeed(1);
	setPlayerDirection(0);
	setPlayerBoardRow(0);
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
