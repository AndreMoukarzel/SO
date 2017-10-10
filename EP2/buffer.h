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
	int clas;
} c_buff;

typedef struct {
	c_buff **buf;
	int *size; /* Tamanho dos buffers de cada volta no buffer */
	int *ativ; /* Número de ciclistas ativos em cada volta no buffer */
	int max; /* Tamanho de buf */
} buffer;


buffer criaBuffer();

void insereBuffer(ciclista c, buffer b, int n);

void defineAtivos(buffer b, int volta, int ativos);

int imprimeVolta(int volta, int n, buffer b);

void destroiBuffer(buffer b);

#endif