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
int d, n, v;
/*********************************************************************/

/*************************** DECLARAÇÕES *****************************/
void *threadCiclista(void *arg);
void simulador();
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


void simulador(){
	int th, i, *id;
	pthread_t *ciclistas = malloc(n * sizeof(pthread_t));
	pthread_barrier_init(&barreira, NULL, (unsigned) n);

	id = &i;

	for (i = 0; i < n; i++){
		/* Fecha o mutex para passar o argumento e ele nao ser mudado
		// enquanto o ciclista o grava no id */
		pthread_mutex_lock(&init_mutex);
		if ((th = pthread_create(&ciclistas[i], NULL, threadCiclista, (void *) id)))
			printf("Failed to create thread %d\n", th);
	}
	for (i = 0; i < n; i++)
		pthread_join(ciclistas[i], NULL);

	free(ciclistas);
}


int main(int argc, char **argv) {
	d = atoi(argv[1]);
	n = atoi(argv[2]);
	v = atoi(argv[3]);

	simulador();
	pista = criaPista(d);


	destroiPista(pista, d);

	return 0;
}