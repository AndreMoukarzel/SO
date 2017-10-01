#include "ciclista.h"

/* nao seria melhor deixar essa funçao na corrida.c? */
void *threadCiclista(void * arg) {
	int *id;
	id = (int *) arg;

	printf("dentro da thread: %d\n", *id);
	return NULL;
}