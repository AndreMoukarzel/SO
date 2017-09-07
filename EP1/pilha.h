#ifndef PILHA_H
#define PILHA_H

#include <stdio.h>
#include <stdlib.h>

typedef struct{
	float dt; /* Tempo real necessário */
	float et; /* Tempo restante de execução necessário */
	float quantum;
	char *name;
	int i;
} process;

/* Pilha de process */
typedef struct{
	process **v;
	int topo, max;
} pilha;


int pilhaVazia(pilha *p);

pilha *criaPilha(int MAX);

/* Insere uma line x na pilha de forma a ficar ordenada pelo
// tempo restante de execução (dt - et)  */
void insereOrdenado(pilha *p, process *x);

/* insere x no topo da pilha */
void empilha(pilha *p, process *x);

/* remove o elemento no indice i da pilha e reajusta os elementos seguintes */
process *removePros(pilha *p, int i);

process *desempilha(pilha *p);

process *topoPilha(pilha *p);

void printPilha(pilha *p);

void destroiPilha(pilha *p);

#endif
