/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "ciclista.h"
#include "pista.h"

#define MAX_LENGTH 1024

/****************************** MACROS *******************************/
#define LOCK(mutex) pthread_mutex_lock(mutex)
#define UNLOCK(mutex) pthread_mutex_unlock(mutex)
#define WAIT(barrier) pthread_barrier_wait(barrier)
/************************ VARIAVEIS GLOBAIS **************************/
pthread_barrier_t barreira;
pthread_mutex_t mutex_finaliza = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_print = PTHREAD_MUTEX_INITIALIZER;
ciclista* ciclistas;
metro* pista;
int tam_pista, num_ciclistas, num_voltas, cic_finalizados = 0;
/*********************************************************************/


void megaBarreira() {
	int b = WAIT(&barreira);

	/* Só ocorre 1 vez por sincronização */
	if (b == PTHREAD_BARRIER_SERIAL_THREAD) {
		printf("\n");
		printPista(pista, tam_pista);
		/* Adicionar manejamento da corrida aqui */
	}
}


/* Retorna 1 se conseguir mudar de faixa, 0 c.c.
// Se "dentro" = 1, tenta mudar para uma faixa mais interna, e tenta
// ir para uma mais externa c. c. */
int mudaFaixa(ciclista c, int dentro) {
	int pos = (int) c.pos, f = c.faixa;
	int alvo = f + 1 - (2 * dentro);

	if (alvo < 0 || alvo >= 10)
		return 0;

	LOCK(&(pista[pos].m[alvo]));
	if (pista[pos].faixa[alvo] == -1) {
		LOCK(&(pista[pos].m[f]));
		pista[pos].faixa[f] = -1;
		pista[pos].faixa[alvo] = c.id;
		c.faixa = alvo;
		UNLOCK(&(pista[pos].m[alvo]));
		UNLOCK(&(pista[pos].m[f]));

		return 1;
	} 
	UNLOCK(&(pista[pos].m[alvo]));

	return 0;
}


/* Retorna 1 se conseguir andar, 0 c.c. */
int andaFrente(ciclista c) {
	int pos = (int)c.pos, f = c.faixa;

	LOCK(&(pista[(pos + 1) % tam_pista].m[f]));
	if (pista[(pos + 1) % tam_pista].faixa[f] == -1){
		LOCK(&(pista[pos].m[f]));
		pista[pos].faixa[f] = -1;
		pista[(pos + 1) % tam_pista].faixa[f] = c.id;
		UNLOCK(&(pista[(pos + 1) % tam_pista].m[f]));
		UNLOCK(&(pista[pos].m[f]));

		return 1;
	}
	UNLOCK(&(pista[(pos + 1) % tam_pista].m[f]));

	return 0;
}


void *threadDummy() {
	while (cic_finalizados < num_ciclistas)
		megaBarreira();

	return NULL;
}


void *threadCiclista(void * arg) {
	/* Atribui o argumento ao id do ciclista */
	ciclista *temp, c;
	pthread_t dummy;
	int pos, next_pos;

	temp = (ciclista *) arg;
	c = *temp;

	WAIT(&barreira); /* Ciclistas dão a largada sincronizados */

	while (c.volta < num_voltas) {
		pos = (int) c.pos;
		next_pos = (int)(c.pos + (float)c.vMax/60);

		/* Ciclista tenta ir para a faixa mais interna possível, já que
		// nessa velocidade ele não vai ultrapassar ninguém. */
		if (c.v == 30) {
			while (mudaFaixa(c, 1));

			/* Anda pra frente/espera o da frente andar, caso o acrescimo
			// de velocidade adicione um metro inteiro à sua posição */
			if (next_pos > pos)
				andaFrente(c);
		}
		else if (c.v == 60) {
			/* corre bem rapido e tenta ultrapassar */
		}

		/* Atualiza a posição */
		c.pos += (float)c.vMax/60;

		/* Final da volta */
		if (c.pos > tam_pista - 1) {
			c.pos -= tam_pista - 1;
			c.volta += 1;
			/*c = defineVel(c, pista);*/

			/* Ciclista tem 1% de chance de quebrar a cada 15 voltas */
			if ((c.volta % 15) == 0) {
				if (quebraCiclista(c)) {
					pista[(int)c.pos].faixa[c.faixa] = -1;
					pthread_create(&dummy, NULL, &threadDummy, NULL);
					LOCK(&mutex_finaliza);
					cic_finalizados++;
					UNLOCK(&mutex_finaliza);
					return NULL;
				}
			}
		}
		megaBarreira();
	}
	pthread_create(&dummy, NULL, &threadDummy, NULL);
	LOCK(&mutex_finaliza);
	cic_finalizados++;
	UNLOCK(&mutex_finaliza);

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