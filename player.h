/*
 * FILE     : player.h
 * REMARKS  : header file of player.c
 */

#ifndef PLAYER_H
#define PLAYER_H

/*** public functions ***/

// draw the player on screen
void drawPlayer();

// player update thread
void *playerUpdate();

// move player UP
void movePlayerUp();

// move player DOWN
void movePlayerDown();

// move player LEFT
void movePlayerLeft();

// move player RIGHT
void movePlayerRight();

#endif