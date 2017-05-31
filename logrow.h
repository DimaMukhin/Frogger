#ifndef LOGROW_H
#define LOGROW_H

#include "log.h"

typedef struct logRow
{
	int row;
	Log *log;
} LogRow;

LogRow* createLogRow(int row);

void *logRowUpdate(void *arg);

void drawLogRow(LogRow *logRow);

#endif