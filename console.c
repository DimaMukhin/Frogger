/**********************************************************************
  Module: console.c
  Author: Jim Young
  Date:   2017 May 3

  Purpose: see console.h

  Changes: 
    2017 May 3 [ Jim Young ]

  NOTES: none
**********************************************************************/

#include "console.h"
#include <curses.h>
#include <string.h>
#include <time.h>        /*for nano sleep */
#include <stdbool.h>


static int CON_WIDTH, CON_HEIGHT;
static int consoleLock = false;
static int MAX_STR_LEN = 256; /* for strlen checking */

/* Local functions */

static bool checkConsoleSize(int reqHeight, int reqWidth) 
{

	if ( (reqWidth > COLS) || (reqHeight > LINES) ) 
 	{
    		fprintf(stderr, "\n\n\rSorry, your window is only %ix%i. \n\r%ix%i is required. Sorry.\n\r", COLS, LINES, reqWidth, reqHeight);
    		return (false);
  	}

  return(true);
}

bool consoleInit(int height, int width, char *image[])  /* assumes image height/width is same as height param */
{
	bool status;

	initscr();
	crmode();
	noecho();
	clear();

	CON_HEIGHT = height;  CON_WIDTH = width;
	status = checkConsoleSize(CON_HEIGHT, CON_WIDTH);

	if (status) 
	{
		consoleDrawImage(0, 0, image, CON_HEIGHT);
		consoleRefresh();
	}

	return(status);
}

void consoleDrawImage(int row, int col, char *image[], int height) 
{
	int i, length;
	int newLeft, newRight, newOffset, newLength;

	if (consoleLock) return;

	newLeft  = col < 0 ? 0 : col;
	newOffset = col < 0 ? -col : 0;

	for (i = 0; i < height; i++) 
	{
		if (row+i < 0 || row+i >= CON_HEIGHT)
			continue;
		length = strnlen(image[i], MAX_STR_LEN);
		newRight = col+length >= CON_WIDTH ? CON_WIDTH-1 : col+length;
		newLength = newRight - newLeft + 1;
		if (newOffset >= length || newLength <= 0)
		  continue;

		if (mvaddnstr(row+i, newLeft, image[i]+newOffset, newLength) == ERR)
			fprintf(stderr, "ERROR drawing to screen"); /* smarter handling is needed */
	}
}

void consoleClearImage(int row, int col, int height, int width) 
{
	int i, j;
	if (consoleLock) return;

	if (col+width > CON_WIDTH)
		width = CON_WIDTH-col;
	if (col < 0) 
	{
		width += col; /* -= -col */
		col = 0;
	}

	if (width < 1 || col >= CON_WIDTH) /* nothing to clear */
		return;

	for (i = 0; i < height; i++) 
	{
		if (row+i < 0 || row+i >= CON_HEIGHT)
			continue;
		move(row+i, col);
		for (j = 0; j < width; j++)
	    		addch(' ');
	}
}

void consoleRefresh(void)
{
	if (!consoleLock) 
	{
	    move(LINES-1, COLS-1);
	    refresh();
	}
}

void consoleFinish(void) 
{
    endwin();
}

void putBanner(const char *str) 
{
  if (consoleLock) return;
  int len;

  len = strnlen(str,MAX_STR_LEN);
  
  move (CON_HEIGHT/2, (CON_WIDTH-len)/2);
  addnstr(str, len);

  consoleRefresh();
}

void putString(char *str, int row, int col, int maxlen) 
{
  if (consoleLock) return;
  move(row, col);
  addnstr(str, maxlen);
}


/* setup to work in USECS, reduces risk of overflow */
/* 10000 usec = 10 ms, or 100fps */
#define TIMESLICE_USEC 10000
#define TIME_USECS_SIZE 1000000
#define USEC_TO_NSEC 1000  
struct timespec getTimeout(int ticks) 
{
  struct timespec rqtp;

  /* work in usecs at first */
  rqtp.tv_nsec = TIMESLICE_USEC * ticks;

  /* handle usec overflow */
  rqtp.tv_sec = rqtp.tv_nsec / TIME_USECS_SIZE;
  rqtp.tv_nsec %= TIME_USECS_SIZE;

  rqtp.tv_nsec *= USEC_TO_NSEC;  /*convert to nsecs */
  return rqtp;
}

void sleepTicks(int ticks) 
{

  if (ticks <= 0)
    return;

  struct timespec rqtp = getTimeout(ticks);
  nanosleep(&rqtp, NULL);
}

#define FINAL_PAUSE 2 
void finalKeypress() 
{
	flushinp();
	sleepTicks(FINAL_PAUSE);
    	move(LINES-1, COLS-1);
	getch(); /* wait for user to press a character, blocking. */
}

void disableConsole(int disabled) 
{
	consoleLock = disabled;
}
