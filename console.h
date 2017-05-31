/**********************************************************************
  Module: console.h
  Author: Jim Young
  Date:   2017 May 3

  Purpose: Draw 2d images on the screen.  Also, draw main game image.

  Changes: 
    2017 May 3 [ Jim Young ]

  NOTES: none of these functions are re-entrant (they are not thread
	 		safe)
**********************************************************************/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>

/**************** DRAWING **************************/

/* directions in terms of deltas in x / y dimension */
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1

#define SCR_LEFT 0
#define SCR_TOP 0

/* Initialize curses, draw initial gamescreen. Refreshes console to terminal. 
 Also stores the requested dimensions of the consoe and tests the terminal for the
 given dimensions.*/
extern bool consoleInit(int reqHeight, int reqWidth, char *image[]);

/* Draws 2d `image' of `height' rows, at curses coordinates `(row, col)'.
   Note: parts of the `image' falling on negative rows are not drawn; each
   row drawn is clipped on the left and right side of the game console (note
   that `col' may be negative, indicating `image' starts to the left of the
   screen and will thus only be partially drawn. Useful for objects that are
   half off the screen  */
extern void consoleDrawImage(int row, int col, char *image[], int height);

/* Clears a 2d `width'x`height' rectangle with spaces.  Upper left hand
   corner is curses coordinate `(row,col)'. */
extern void consoleClearImage(int row, int col, int width, int height);

/* Moves cursor to bottom right corner and refreshes. If this is not done,
   the curses internal buffer (that you have been drawing to) is not dumped
   to screen. */
extern void consoleRefresh(void);

/*  turns off all updates. Can be used to prevent the screen refresh from working, e.g., at game end while threads are all catching up.*/
extern void disableConsole(int disabled);

/* Terminates curses cleanly. */
extern void consoleFinish(void);

/* Puts the given banner in the center of the screen */
void putBanner(const char *);

/* Draws the given string at the given location  */
void putString(char *, int row, int col, int maxlen);

/* Sleeps the given number of 10ms ticks */
void sleepTicks(int ticks);

/* clears the input buffer and then waits for one more key */
void finalKeypress();

/* gets a timespec that represents the time of one tick */
struct timespec getTimeout(int ticks);

#endif /* CONSOLE_H */
