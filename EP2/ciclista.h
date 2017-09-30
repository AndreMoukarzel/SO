#ifndef CICLISTA_H
#define CICLISTA_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int n; /* Volta atual do ciclista */
	int v; /* Velocidade do ciclista */
	int id; /* Identificador do ciclista, as ser usado no vetor da pista */
} ciclista;

void *threadCiclista(void* arg);

#endif