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
metro *pista;
ciclista *c;
int d, n, v;
/*********************************************************************/

/*************************** DECLARAÇÕES *****************************/
void *threadCiclista(void *arg);
<<<<<<< HEAD
void simulador();
=======
void preparaLargada(int d, int n);
void liberaMemoria(int d, int n);
void corrida(int d, int n, int v);
>>>>>>> 535f0511c581dc92562cf0f27f3f2737cdb03875
/*********************************************************************/


void *threadCiclista(void * arg) {
	/* Atribui o argumento ao id do ciclista */
	int *temp;
	ciclista c;
	temp = (int *) arg;
	c.id = *temp;

	c.pos = d - c.id/10;

	/*pthread_barrier_wait(&barreira); Barreira para inicializaçao/largada */
	printf("oi eu sou o ciclista %d, minha pos %d\n", c.id, c.pos);
	pthread_mutex_unlock(&init_mutex);
	return NULL;
}


<<<<<<< HEAD
void simulador(){
	int th, i, *id;
	pthread_t *ciclistas = malloc(n * sizeof(pthread_t));
	pthread_barrier_init(&barreira, NULL, (unsigned) n);
=======
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
>>>>>>> 535f0511c581dc92562cf0f27f3f2737cdb03875


<<<<<<< HEAD
	for (i = 0; i < n; i++){
		/* Fecha o mutex para passar o argumento e ele nao ser mudado
		// enquanto o ciclista o grava no id */
		pthread_mutex_lock(&init_mutex);
		if ((th = pthread_create(&ciclistas[i], NULL, threadCiclista, (void *) id)))
=======
void corrida(int d, int n, int v){
	int th, i;
	pthread_t *thread = malloc(n * sizeof(pthread_t));

	preparaLargada(d, n);
	pthread_barrier_init(&barreira, NULL, n);

	/* Dispara as threads */
	for (i = 0; i < n; i++) {
		printf("%d\n", *id);
		if ((th = pthread_create(&thread[i], NULL, threadCiclista, (void *) c[i])))
>>>>>>> 535f0511c581dc92562cf0f27f3f2737cdb03875
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
	d = atoi(argv[1]);
	n = atoi(argv[2]);
	v = atoi(argv[3]);

<<<<<<< HEAD
	simulador();
	pista = criaPista(d);

	corrida(d, n, v);

	destroiPista(pista, d);

	return 0;
}