/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
////////////////////////// COMO RODAR /////////////////////////////*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include "ciclista.h"

#define MAX_LENGTH 1024

/*pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;*/

int **pista; /* matriz da pista, de d linhas e 10 colunas */


void simulador(int d, int n, int v){
	int th, i, *id;
	pthread_t *ciclistas = malloc(n * sizeof(pthread_t));

	id = &i;

	for (i = 0; i < n; i++){
		printf("%d\n", *id);
		if ((th = pthread_create(&ciclistas[i], NULL, threadCiclista, (void *) id)))
			printf("Failed to create thread %d\n", th);
	}
	for (i = 0; i < n; i++)
		pthread_join(ciclistas[i], NULL);

	free(ciclistas);
}


int main(int argc, char **argv) {
	int d, n, v, i;
	d = atoi(argv[1]);
	n = atoi(argv[2]);
	v = atoi(argv[3]);

	/* essa alocagem ta certa?>?? */
	pista = malloc(d*n*sizeof(int));
	for (i = 0; i < d; i++)
		pista[i] = malloc(n*sizeof(int));

	simulador(d, n, v);

	for (i = 0; i < d; i++)
		free(pista[i]);
	free(pista);
	return 0;
}