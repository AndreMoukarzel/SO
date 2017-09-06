#ifndef PILHA_H
#define PILHA_H

#include <stdio.h>
#include <stdlib.h>

typedef struct{
	float dt; /* Tempo real necessário */
	float et; /* Tempo que o processo foi executado */
	char *name;
} process;

/* Pilha de process */
typedef struct{
	process **v;
	int topo, max;
} pilha;

pilha *criaPilha(int MAX);

/* Insere uma line x na pilha de forma a ficar ordenada pelo
// tempo restante de execução (dt - et)  */
void insereOrdenado(pilha *p, process *x);

process *desempilha(pilha *p);

void printfPilha(pilha *p);

void destroiPilha(pilha *p);

#endif
