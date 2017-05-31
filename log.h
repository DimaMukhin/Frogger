#ifndef LOG_H
#define LOG_H

#define LOG_HEIGHT 4
#define LOG_LENGTH 24

typedef struct log
{
	int streamRow;
	int row;
	int col;
	int speed;
	int direction;
} Log;

/* public functions */

void drawLog(Log *log);

void *logUpdate(void *arg);

Log* createLog(int streamRow);

#endif