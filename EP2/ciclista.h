#ifndef CICLISTA_H
#define CICLISTA_H

#include <stdlib.h>

typedef struct {
	int v; /* Volta atual do ciclista */
} ciclista;

void *threadCiclista(void* arg);

#endif