#ifndef PILHA_H
#define PILHA_H

#include <stdio.h>
#include <stdlib.h>
#include "fileReader.h"

/* pilha de lines */
typedef struct{
	line **v;
	int topo, max;
} pilha;

pilha *criaPilha(int MAX);

void insereOrdenado(pilha *p, line *x);

line *desempilha(pilha *p);

void printfPilha(pilha *p);

void destroiPilha(pilha *p);

#endif
