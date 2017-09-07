#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct{
	float t0; /* Tempo em que processo iniciou */
	float dt; /* Tempo real necessário */
	float et; /* Tempo restante de execução necessário */
	float quantum;
	char *name;
	int i;
} process;

#endif