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

void drawPlayer()
{
	pthread_mutex_lock(&drawMutex);
	consoleDrawImage(playerRow, playerCol, playerTile, PLAYER_HEIGHT);
	pthread_mutex_unlock(&drawMutex);
}

void *playerUpdate()
{
	int i = 0;
	int animTile = 0;
	while (1)
	{
		sleepTicks(playerSpeed);

		pthread_mutex_lock(&playerMutex);
		
		playerCol -= playerDirection;
		if (playerCol <= 0)
			playerCol = 0;
		else if (playerCol >= 80 - PLAYER_WIDTH)
			playerCol = 80 - PLAYER_WIDTH;	
		
		pthread_mutex_unlock(&playerMutex);
		
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

void clearPlayer()
{
	pthread_mutex_lock(&drawMutex);
	consoleClearImage(playerRow, playerCol, PLAYER_HEIGHT, strlen(PLAYER_GRAPHIC[0][0]));
	pthread_mutex_unlock(&drawMutex);
}

void movePlayerUp()
{
	clearPlayer();
	if (playerBoardRow < 4)
	{
		playerRow -= 4;
		playerBoardRow++;
		landPlayer();
	}
	else if (playerBoardRow == 4)
	{
		playerBoardRow++;
		landPlayer();
	}		
}

void movePlayerDown()
{
	clearPlayer();
	if (playerBoardRow != 0)
	{
		playerRow += 4;
		playerBoardRow--;
		landPlayer();
	}
}

void movePlayerLeft()
{
	clearPlayer();
	playerCol -= 1;
}

void movePlayerRight()
{
	clearPlayer();
	playerCol += 1;
}

void landPlayer()
{
	// if player is above raging crocodile infested water
	if (playerBoardRow > 0 && playerBoardRow < 5)
	{
		Log *log = getLog(playerBoardRow - 1, logList);
		if (!isPlayerOverWater())
		{
			playerSpeed = log->speed;
			playerDirection = log->direction;
		}
		else
			killFrog();
	}
	else if (playerBoardRow == 0) // if player landed on land
	{
		playerSpeed = 1;
		playerDirection = 0;
	}
	else if (playerBoardRow == 5)
	{
		Home *home = getHome(playerCol);
		if (home != NULL && home->open != 0)
		{
			home->open = 0;
			playerRow -= 3;
			playerTile = PLAYER_GRAPHIC[0];
			drawPlayer();
			
			playerRow = DEFAULT_PLAYER_ROW;
			playerCol = DEFAULT_PLAYER_COL;
			playerSpeed = 1;
			playerDirection = 0;
			playerBoardRow = 0;
		}
		else
		{
			playerBoardRow = 4;
		}
	}
}

int isPlayerOverWater()
{
	if (playerBoardRow > 0 && playerBoardRow < 5)
	{
		Log *log = getLog(playerBoardRow - 1, logList);
		if (log == NULL || playerCol < log->col || playerCol > (log->col + LOG_LENGTH))
			return 1;
	}
	
	return 0;
}

void killFrog()
{
	lives--;
	drawLives();
	
	pthread_mutex_lock(&playerMutex);
			
	playerRow = DEFAULT_PLAYER_ROW;
	playerCol = DEFAULT_PLAYER_COL;
	playerSpeed = 1;
	playerDirection = 0;
	playerBoardRow = 0;
	
	pthread_mutex_unlock(&playerMutex);
}