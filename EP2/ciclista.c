#include "ciclista.h"

/* nao seria melhor deixar essa funçao na corrida.c? */
void *threadCiclista(void * arg) {
	int *id;
	ciclista c;
	id = (int *) arg;

	c.n = 1;
	c.v = 30; /* todos começam a 30km/h */
	c.id = id;
	pista[0][id] = id; /* ciclista esta em 0 metros e na pista de numero id */

	printf("dentro da thread: %d\n", *id);
	return NULL;
}