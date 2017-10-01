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
ciclista* c;
/*********************************************************************/

/*************************** DECLARAÇÕES *****************************/
void *threadCiclista(void *arg);
void preparaLargada(int d, int n);
void liberaMemoria(int d, int n);
void corrida(int d, int n, int v);
/*********************************************************************/


void *threadCiclista(void * arg) {
	/* Atribui o argumento ao id do ciclista */
	ciclista *temp, c;
	temp = (int *) arg;
	c = *temp;

	pthread_barrier_wait(&barreira); /* Barreira para inicializaçao */
	printf("dentro da thread: %d\n", c.id);
	return NULL;
}


void preparaLargada(int d, int n) {
	pista = criaPista(d);
	c = criaCiclistas(n);
	posicionaCiclistas(d, n, c, pista);
}


void liberaMemoria(int d, int n) {
	int i, j;

	for (i = 0; i < n; i++)
		free(c[i]);
	free(c);

	for (i = 0; i < d; i++) {
		for (j = 0; j < 10; j++) {
			free(pista[i].faixa[j]);
			free(pista[i].m[j]);
		}
		free(pista[i].faixa);
		free(pista[i].m);
		free(pista[i]);
	}
	free(pista);
}


void corrida(int d, int n, int v){
	int th, i;
	pthread_t *thread = malloc(n * sizeof(pthread_t));

	preparaLargada(d, n);
	pthread_barrier_init(&barreira, NULL, n);

	/* Dispara as threads */
	for (i = 0; i < n; i++) {
		printf("%d\n", *id);
		if ((th = pthread_create(&thread[i], NULL, threadCiclista, (void *) c[i])))
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
	int d, n, v;
	d = atoi(argv[1]);
	n = atoi(argv[2]);
	v = atoi(argv[3]);

	corrida(d, n, v);

	destroiPista(pista, d);

	return 0;
}