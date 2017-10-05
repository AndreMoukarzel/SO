/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#ifndef PISTA_H
#define PISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	int *faixa; /* As 10 faixas da pista, -1 se livre */
	int vazia; /* 0 se não há ninguém nesse metro, 1 caso constrário */
	pthread_mutex_t *m;	/* Um semáforo para cada faixa deste metro */
} metro; /* matriz da pista, de d linhas e 10 colunas */

metro *criaPista(int d);

void printPista(metro *pista, int d);

void destroiPista(metro* pista, int d);

#endif
