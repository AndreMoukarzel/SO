/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
////////////////////////// COMO RODAR /////////////////////////////*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "ciclista.h"
#include "pista.h"

#define MAX_LENGTH 1024

/************************ VARIAVEIS GLOBAIS **************************/
pthread_barrier_t barreira;
pthread_mutex_t volta_mutex;
metro* pista;
ciclista* ciclistas;
int num_voltas;
/*********************************************************************/


void *threadCiclista(void * arg) {
	/* Atribui o argumento ao id do ciclista */
	ciclista *temp, c;
	int newPos = 0;

	temp = (ciclista *) arg;
	c = *temp;

	printf("dentro da thread: %d\n", c.id);
	while (c.volta < num_voltas) {
		if ((c.volta % 15) == 0)
			if (quebraCiclista(c))
				return NULL;

		c.volta =+ 1;
		pthread_barrier_wait(&barreira); /* Barreira para inicializaçao */
	}

	return NULL;
}


void preparaLargada(int d, int n) {
	pista = criaPista(d);
	ciclistas = criaCiclistas(n);
	posicionaCiclistas(d, n, ciclistas, pista);
}


void liberaMemoria(int d, int n) {
	int i;

	free(ciclistas);

	for (i = 0; i < d; i++) {
		free(pista[i].faixa);
		free(pista[i].m);
	}
	free(pista);
}


void corrida(int d, int n){
	int th, i;
	pthread_t *thread = malloc(n * sizeof(pthread_t));

	preparaLargada(d, n);
	pthread_barrier_init(&barreira, NULL, n);

	/* Dispara as threads */
	for (i = 0; i < n; i++) {
		printf("%d\n", ciclistas[i].id);
		if ((th = pthread_create(&thread[i], NULL, threadCiclista, (void *) &ciclistas[i])))
			printf("Failed to create thread %d\n", th);
	}

	/* Espera a corrida acabar */
	for (i = 0; i < n; i++)
		pthread_join(thread[i], NULL);


	/* Imprimir resultados aqui */

	liberaMemoria(d, n);
	free(thread);
}


int main(int argc, char **argv) {
	int d, n;
	d = atoi(argv[1]);
	n = atoi(argv[2]);
	num_voltas = atoi(argv[3]);

	corrida(d, n);

	destroiPista(pista, d);

	return 0;
}