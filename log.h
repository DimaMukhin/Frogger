#ifndef LOG_H
#define LOG_H

#include <pthread.h>

#define LOG_HEIGHT 4
#define LOG_LENGTH 24

typedef struct log
{
	int streamRow;
	int row;
	int col;
	int speed;
	int direction;
	int frame;
	int alive;
	pthread_t thread; // for joining purposes
} Log;

/* public functions */

void drawLog(Log *log);

void *logUpdate(void *arg);

void cleanLog(Log *log);

Log* createLog(int streamRow);

#endif