/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#ifndef DEFINECLAS_H
#define DEFINECLAS_H

#include "pista.h"
#include "ciclista.h"

/* Ordena o vetor de ciclistas pelo numero de voltas de cada ciclista e pela
// sua respectiva posição, determinando assim a classificação de cada
// ciclsita */
void defineClas(ciclista *clas, int n, int num_voltas);

#endif