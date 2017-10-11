/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#ifndef BUFFER_H
#define BUFFER_H

#include "ciclista.h"

typedef struct {
	int id; /* Identificador do ciclista */
	int volta; /* Volta atual do ciclista */
	int p; /* Pontuação acumulada do ciclista */
	int clas; /* Classificação do ciclista na corrida */
} c_buff;

typedef struct {
	c_buff **buf; /* Matriz de ciclistas na ordem que terminaram cada volta */
	int *topo; /* Vetor com a primeira posiçao livre de cada linha da matriz */
	int *cheio; /* 1 se todos os ciclistas ja terminaram aquela volta */
} buffer;


buffer *criaBuffer(int n, int v);

void insereBuffer(buffer *b, ciclista c, int cic_ativos);

void imprimeVolta(buffer *b, int v);

void destroiBuffer(buffer *b, int v);

#endif