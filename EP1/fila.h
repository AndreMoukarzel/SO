#ifndef FILA_H
#define FILA_H

#include "process.h"

/* Fila de process */
typedef struct{
	process **v;
	int frente, tam, max;
} fila;

int filaVazia(fila *f);

fila *criaFila(int MAX);

/* Insere x no topo da fila */
void insere(fila *f, process *x);

/* Remove o elemento no índice i da fila e reajusta os elementos seguintes
process *removeIndex(fila *f, int i);
*/

process *removeFila(fila *f);

/* Devolve o próximo elemento da fila, sem removê-lo */
process *topoFila(fila *f);

void printFila(fila *f);

void destroiFila(fila *f);

#endif