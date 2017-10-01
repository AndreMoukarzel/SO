#ifndef CICLISTA_H
#define CICLISTA_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int id; /* Identificador do ciclista */
	int n; /* Volta atual do ciclista */
	int p; /* Pontuação acumulada do ciclista */
	int v, vMax; /* Velocidade desejada e máxima do ciclista */
	int pos; /* Posição relativa à corrida do ciclista */
	int p_pos[2]; /* Posição [linha, coluna] da pista */ 
	int quebrado; /* Guarda a volta em que ciclista quebrou. Se não, -1 */
} ciclista;

void *threadCiclista(void* arg);

#endif