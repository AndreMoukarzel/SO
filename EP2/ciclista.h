#ifndef CICLISTA_H
#define CICLISTA_H

#include "pista.h"

typedef struct {
	int id; /* Identificador do ciclista */
	int volta; /* Volta atual do ciclista */
	int p; /* Pontuação acumulada do ciclista */
	int v, vMax; /* Velocidade desejada e máxima do ciclista */
	float pos; /* Posição em metros na pista */
	int quebrado; /* Guarda a volta em que ciclista quebrou. Se não, -1 */
} ciclista;


ciclista *criaCiclistas(int n);

void posicionaLargada(int n, int d, ciclista* cics, metro* pista);

#endif