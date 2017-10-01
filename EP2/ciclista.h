#ifndef CICLISTA_H
#define CICLISTA_H

#include "pista.h"

typedef struct {
	int id; /* Identificador do ciclista */
	int volta; /* Volta atual do ciclista */
	int p; /* Pontuação acumulada do ciclista */
	int v, vMax; /* Velocidade desejada e máxima do ciclista */
	int clas; /* Classificação atual do ciclista na corrida */
	int pos; /* Posição em metros na pista */
	int quebrado; /* Guarda a volta em que ciclista quebrou. Se não, -1 */
} ciclista;


ciclista *criaCiclistas(int n);

void posicionaCiclistas(int d, int n, ciclista* cics, metro* pista);

#endif