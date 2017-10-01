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

/* Mutex que sera usado na inicializaçao dos ciclistas */
pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barreira;
metro* pista;


void *threadCiclista(void * arg) {
	/* Atribui o argumento ao id do ciclista */
	int *temp, id;
	temp = (int *) arg;
	id = *temp;

	pthread_mutex_unlock(&init_mutex); /* Pode iniciar outro ciclista */

	pthread_barrier_wait(&barreira); /* Barreira para inicializaçao */
	printf("dentro da thread: %d\n", id);
	return NULL;
}


void simulador(int d, int n, int v){
	int th, i, *id;
	pthread_t *ciclistas = malloc(n * sizeof(pthread_t));
	pthread_barrier_init(&barreira, NULL, (unsigned) d);

	id = &i;

	for (i = 0; i < n; i++){
		/* Fecha o mutex para passar o argumento e ele nao ser mudado
		// enquanto o ciclista o grava no id */
		pthread_mutex_lock(&init_mutex);
		printf("%d\n", *id);
		if ((th = pthread_create(&ciclistas[i], NULL, threadCiclista, (void *) id)))
			printf("Failed to create thread %d\n", th);
	}
	for (i = 0; i < n; i++)
		pthread_join(ciclistas[i], NULL);

	free(ciclistas);
}


int main(int argc, char **argv) {
	int d, n, v;
	d = atoi(argv[1]);
	n = atoi(argv[2]);
	v = atoi(argv[3]);

	simulador(d, n, v);
	pista = criaPista(d);


	destroiPista(pista, d);

	return 0;
}