#include <stdio.h>
#include <stdlib.h>
#include "gameglobals.h"
#include "frogger.h" 

int main(int argc, char**argv) {
	runFrogger();
	
	pthread_t froggerThread;
	if (pthread_create(&froggerThread, NULL, runFrogger, NULL) == -1) 
	{ 
		perror("froggerThread"); 
		exit(EXIT_FAILURE);
	}
	
	pthread_join(froggerThread, NULL);
	printf("done!\n");
}
