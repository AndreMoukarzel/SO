/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#ifndef CICLISTA_H
#define CICLISTA_H

#include "pista.h"

typedef struct {
	int id; /* Identificador do ciclista */
	int volta; /* Volta atual do ciclista */
	int p; /* Pontuação acumulada do ciclista */
	int v, vMax; /* Velocidade desejada e máxima do ciclista */
	int clas; /* Classificação atual do ciclista na corrida */
	float pos; /* Posição em metros na pista */
	int faixa; /* Faixa atual da pista em que o ciclista está */
	int quebrado; /* Guarda a volta em que ciclista quebrou. Se não, -1 */
} ciclista;


ciclista *criaCiclistas(int n);

void posicionaCiclistas(int d, int n, ciclista* cics, metro* pista);

int quebraCiclista(ciclista c);

ciclista defineVel(ciclista c, metro* pista);


#endif