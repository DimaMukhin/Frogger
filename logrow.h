#ifndef LOGROW_H
#define LOGROW_H

#include "log.h"
#include "loglist.h"

typedef struct logRow
{
	int row;
	int speed;
	int direction;
	LogList *logs;
} LogRow;

LogRow* createLogRow(int row);

void *logRowUpdate(void *arg);

void drawLogRow(LogRow *logRow);

void cleanLogRow(LogRow *logRow);

#endif