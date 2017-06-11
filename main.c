/*
 * NAME                     : Dima Mukhin
 * STUDENT NUMBER           : 7773184
 * COURSE                   : COMP 3430
 * INSTRUCTOR               : Dr. Jim Young
 * ASSIGNMENT               : 2
 * Question                 : 1
 * 
 */

/*
 * FILE     : main.c
 * REMARKS  : Contains main function for running the game
 */

#include <stdio.h>
#include <stdlib.h>

#include "gameglobals.h"
#include "frogger.h" 
#include "threadwrappers.h"

// Start Frogger thread and wait for the game to finish
int main(int argc, char**argv) {
    runFrogger();
    
    pthread_t froggerThread;
    createThread(&froggerThread, runFrogger, NULL);
    
    pthread_join(froggerThread, NULL);
}
