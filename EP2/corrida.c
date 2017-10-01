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
pthread_barrier_t barreira, barr_tempo;
pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
ciclista* ciclistas;
metro* pista;
int d;
int num_voltas;
int cic_ativos; /* Número de ciclistas que não quebraram */
int fim = 0;
int elapsed_time = 0;
/*********************************************************************/


void *threadDummy() {
	while (!fim)
		pthread_barrier_wait(&barreira);

	return NULL;
}


/* Thread que atualiza o tempo de simulaçao para ser usado nas velocidades */
void *threadCoordenador(){
	int ciclo_atual = 0;
	int volta_atual = 1; /* Seria a volta atual do ciclista em 1o lugar (?) */

	while(!fim){
		pthread_barrier_wait(&barr_tempo);
		/* Atualiza o tempo (em ms) caso nao seja a primeira iteração */
		if (ciclo_atual != 0){
			if (volta_atual < num_voltas - 2 && ciclo_atual != 0)
				elapsed_time += 60;
			else
				elapsed_time += 20;
		}
		ciclo_atual++;
		pthread_barrier_wait(&barr_tempo);
	}
	return NULL;
}


void *threadCiclista(void * arg) {
	/* Atribui o argumento ao id do ciclista */
	ciclista *temp, c;
	pthread_t dummy;

	temp = (ciclista *) arg;
	c = *temp;
	pthread_mutex_unlock(&init_mutex);
	printf("oi eu sou o %d\n", c.id);

	/* Espera o coordenador mudar o tempo */
	pthread_barrier_wait(&barr_tempo);
	pthread_barrier_wait(&barr_tempo);

	while (c.volta < num_voltas) {
		if ((c.volta % 15) == 0)
			if (quebraCiclista(c)) {
				pthread_create(&dummy, NULL, &threadDummy, NULL);
				cic_ativos--;
				return NULL;
			}

		/* Trava a posição atual do ciclista antes dele se mover */
		pthread_mutex_lock(&pista[c.pos].m[c.faixa]);

		if ((elapsed_time % 120 == 0 && c.vMax == 30) ||
			(elapsed_time % 60 == 0 && c.vMax == 60) ||
			(c.vMax == 90)){
			if (!c.impedido){
				/* Espera o ciclista da frente andar, caso tenha algum */
				while(pista[(c.pos + 1) % d].faixa[c.faixa] != -1) continue; /*ou tenta ultrapassar*/
				pthread_mutex_lock(&pista[(c.pos + 1) % d].m[c.faixa]);
				pista[c.pos].faixa[c.faixa] = -1;
				pista[(c.pos + 1) % d].faixa[c.faixa] = c.id;
				pthread_mutex_unlock(&pista[(c.pos + 1) % d].m[c.faixa]);
			}
		}
		pthread_mutex_unlock(&pista[c.pos].m[c.faixa]);

		c.volta += 1;
		printf("%d\n", c.id);
		pthread_barrier_wait(&barreira); /* Barreira para cada iteração */
	}
	return NULL;
}


void preparaLargada(int d, int n) {
	pista = criaPista(d);
	ciclistas = criaCiclistas(n);
	posicionaCiclistas(d, n, ciclistas, pista);
}


void corrida(int n){
	int th, i;
	pthread_t *thread = malloc(n * sizeof(pthread_t));
	pthread_t coord;

	preparaLargada(d, n);
	pthread_barrier_init(&barreira, NULL, n);
	pthread_barrier_init(&barr_tempo, NULL, n + 1);
	cic_ativos = n;

	/* Dispara as threads */
	if ((th = pthread_create(&coord, NULL, threadCoordenador, NULL)))
		printf("Failed to create thread %d\n", th);

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
	int n;

	d = atoi(argv[1]);
	n = atoi(argv[2]);
	num_voltas = atoi(argv[3]);

	corrida(n);

	return 0;
}