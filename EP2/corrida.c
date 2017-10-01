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
pthread_mutex_t volta_mutex, init_mutex = PTHREAD_MUTEX_INITIALIZER;
ciclista* ciclistas;
metro* pista;
int num_voltas;
int cic_ativos; /* Número de ciclistas que não quebraram */
int fim = 0;
/*********************************************************************/


void *threadDummy() {
	while (!fim)
		pthread_barrier_wait(&barreira);

	return NULL;
}


void *threadCiclista(void * arg) {
	/* Atribui o argumento ao id do ciclista */
	ciclista *temp, c;
	pthread_t dummy;

	temp = (ciclista *) arg;
	c = *temp;
	pthread_mutex_unlock(&init_mutex);

	while (c.volta < num_voltas) {
		if ((c.volta % 15) == 0)
			if (quebraCiclista(c)) {
				pthread_create(&dummy, NULL, &threadDummy, NULL);
				return NULL;
			}

		c.volta += 1;
		pthread_barrier_wait(&barreira); /* Barreira para inicializaçao */
	}
	return NULL;
}


void preparaLargada(int d, int n) {
	pista = criaPista(d);
	ciclistas = criaCiclistas(n);
	posicionaCiclistas(d, n, ciclistas, pista);
}


void corrida(int d, int n){
	int th, i;
	pthread_t *thread = malloc(n * sizeof(pthread_t));

	preparaLargada(d, n);
	pthread_barrier_init(&barreira, NULL, n);
	cic_ativos = n;

	/* Dispara as threads */
	for (i = 0; i < n; i++) {
		pthread_mutex_lock(&init_mutex);
		if ((th = pthread_create(&thread[i], NULL, threadCiclista, (void *) &ciclistas[i])))
			printf("Failed to create thread %d\n", th);
	}

	/* Espera a corrida acabar */
	for (i = 0; i < n; i++) {
		pthread_join(thread[i], NULL);
	}


	/* Imprimir resultados aqui */

	destroiPista(pista, d);
	free(ciclistas);
	free(thread);
}


int main(int argc, char **argv) {
	int d, n;

	d = atoi(argv[1]);
	n = atoi(argv[2]);
	num_voltas = atoi(argv[3]);

	corrida(d, n);

	return 0;
}