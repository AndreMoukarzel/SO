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
ciclista* ciclistas;
metro* pista;
int num_voltas, num_ciclistas, tam_pista, cic_finalizados = 0;
/*********************************************************************/


void *threadDummy() {
	while (cic_finalizados < num_ciclistas)
		pthread_barrier_wait(&barreira);

	return NULL;
}


void *threadCiclista(void * arg) {
	/* Atribui o argumento ao id do ciclista */
	ciclista *temp, c;
	pthread_t dummy;
	int a, b;

	temp = (ciclista *) arg;
	c = *temp;

	while (c.volta < num_voltas) {
		a = (int) c.pos;
		b = c.faixa;
		/* Ciclista tenta ir para a faixa mais interna possível, já que
		// nessa velocidade ele não vai ultrapassar ninguém. */
		if (c.v == 30) {
			while (b > 0) {
				pthread_mutex_lock(&(pista[a].m[b]));
				pthread_mutex_lock(&(pista[a].m[b-1]));
				if (pista[a].faixa[b - 1] == -1) {
					pista[a].faixa[b] = -1;
					pista[a].faixa[b - 1] = c.id;
					pthread_mutex_unlock(&(pista[a].m[b]));
					pthread_mutex_unlock(&(pista[a].m[b-1]));

					b--;
				}
				else {
					pthread_mutex_unlock(&(pista[a].m[b]));
					pthread_mutex_unlock(&(pista[a].m[b-1]));
					break;
				}
			}
			c.pos += (float)c.vMax/60;
			/* printf("%f\n", c.pos); */
		}

		if (c.pos > tam_pista - 1) {
			c.pos -= tam_pista - 1;
			c.volta += 1;

			/* Ciclista tem 1% de chance de quebrar a cada 15 voltas */
			if ((c.volta % 15) == 0) {
				if (quebraCiclista(c)) {
					pthread_create(&dummy, NULL, &threadDummy, NULL);
					/* Botar semaforo aqui e TIRAR DA PISTAAA */
					cic_finalizados++;
					/***********************/
					return NULL;
				}
			}
		}
		/* pthread_barrier_wait(&barreira); */
	}
	/* Botar semaforo aqui */
	cic_finalizados++;
	/***********************/

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

	/* Dispara as threads */
	for (i = 0; i < n; i++) {
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
	tam_pista = atoi(argv[1]);
	num_ciclistas = atoi(argv[2]);
	num_voltas = atoi(argv[3]);

	corrida(tam_pista, num_ciclistas);

	return 0;
}