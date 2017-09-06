#ifndef THREADS_H
#define THREADS_H

#include "fileReader.h"
#include <time.h>

typedef struct{
	float dt; /* Tempo real necessário */
	char *name;
} process;


void *newThread(void *arg);

process *lineToProcess(line *l);

#endif