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
#include "buffer.h"

#define MAX_LENGTH 1024

/****************************** MACROS *******************************/
#define LOCK(mutex) pthread_mutex_lock(mutex)
#define UNLOCK(mutex) pthread_mutex_unlock(mutex)
#define WAIT(barrier) pthread_barrier_wait(barrier)
/************************ VARIAVEIS GLOBAIS **************************/
pthread_barrier_t barreira;
pthread_mutex_t mutex_finaliza = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_buf = PTHREAD_MUTEX_INITIALIZER;
ciclista *ciclistas, *classifics;
metro* pista;
buffer *B;
int tam_pista, num_ciclistas, num_voltas, id_frente = 0, DEBUG = 0;
int cic_finalizados = 0, *cic_quebrados, voltas_sobre_outros = 1;
int *pontuado, sorteado_90 = -2, simulacao = 60;
/*********************************************************************/

/* Ordena os ciclistas no vetor classifics em função de sua
// classificação na corrida */
void ordena() {
	int i;
	/* Atualiza os dados dos ciclistas no vetor de classificações */
	for (i = 0; i < num_ciclistas; i++)
		classifics[i] = ciclistas[i];
	/* E o ordena */
	defineClas(classifics, num_ciclistas, num_voltas);
	/* Atualiza o vetor global de ciclistas com as novas classificações */
	for (i = 0; i < num_ciclistas; i++)
        ciclistas[classifics[i].id] = classifics[i];
}


void atribuiPontos() {
	int i;

	if (classifics[0].id != id_frente) {
		/* Ciclista na dianteira mudou */
		id_frente = classifics[0].id;
		voltas_sobre_outros = 1;
	}

	for (i = 0; i < num_ciclistas; i++) {
		/* Volta de sprint: define a pontuação */
		if (ciclistas[i].quebrado == -1) {
			if (pontuado[ciclistas[i].id] == 0) {
				if (ciclistas[i].clas == 1) {
					/* Se o ciclista em primeiro der uma volta sobre todos
					// os outros, ganha 20 pontos */
					if (ciclistas[i].volta - classifics[1].volta > voltas_sobre_outros) {
						ciclistas[i].p += 20;
						/* Para ganhar denovo precisa ter dado uma volta a mais
						// sobre os outros */
						voltas_sobre_outros++;
					}
				}
				if (ciclistas[i].volta != 0 && !(ciclistas[i].volta % 10)) {
					if (ciclistas[i].clas == 1)
						ciclistas[i].p += 5;
					else if (ciclistas[i].clas == 2)
						ciclistas[i].p += 3;
					else if (ciclistas[i].clas == 3)
						ciclistas[i].p += 2;
					else if (ciclistas[i].clas == 4)
						ciclistas[i].p += 1;
				}

				pontuado[ciclistas[i].id] = 1;
			}
		}
	}
}


void megaBarreira() {
	int b = WAIT(&barreira);

	/* Só ocorre 1 vez por sincronização */
	if (b == PTHREAD_BARRIER_SERIAL_THREAD) {
		if (DEBUG)
			printPista(pista, tam_pista);
		ordena();
		atribuiPontos();
		if (sorteado_90 >= 0)
			simulacao = 180;
	}

	WAIT(&barreira);
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
		pista[pos].faixa[alvo] = c.id;
		c.faixa = alvo;
		if (pista[pos].faixa[f] == c.id)
			pista[pos].faixa[f] = -1;

		UNLOCK(&(pista[pos].m[alvo]));
		/* Atualiza o vetor global de ciclistas */
		ciclistas[c.id] = c;
		return 1;
	}

	UNLOCK(&(pista[pos].m[alvo]));
	return 0;
}


/* Retorna -1 se conseguir andar, id do ciclista que o
// impediu c.c. */
int andaFrente(ciclista c) {
	int pos = (int) c.pos, f = c.faixa, id = num_ciclistas;

	while (f <= 9) {
		LOCK(&(pista[(pos + 1) % tam_pista].m[f]));
		if (pista[(pos + 1) % tam_pista].faixa[f] == -1) {
			/* Consegue ir para frente nesta faixa */
			pista[(pos + 1) % tam_pista].faixa[f] = c.id;
			if (pista[pos].faixa[f] == c.id)
				pista[pos].faixa[f] = -1;

			UNLOCK(&(pista[(pos + 1) % tam_pista].m[f]));
			return -1;
		}
		id = pista[(pos + 1) % tam_pista].faixa[f];
		UNLOCK(&(pista[(pos + 1) % tam_pista].m[f]));

		if (!mudaFaixa(c, 0))
			break;
		c = ciclistas[c.id];
		f = c.faixa;
	}

	return id;
}


void *threadDummy() {
	while (cic_finalizados < num_ciclistas)
		megaBarreira();

	return NULL;
}


void *threadCiclista(void * arg) {
	ciclista *temp, c;
	pthread_t dummy;
	int pos, next_pos, impedido, f_id, i, q, r;

	temp = (ciclista *) arg;
	c = *temp;

	WAIT(&barreira); /* Ciclistas dão a largada sincronizados */

	while (c.volta < num_voltas) {
		pos = (int) c.pos;
		next_pos = (int)(c.pos + (float)c.v/simulacao);
		impedido = 0;

		while (mudaFaixa(c, 1))
			c = ciclistas[c.id];
		c = ciclistas[c.id];
		/* Anda pra frente/espera o da frente andar, caso o acrescimo
		// de velocidade adicione um metro inteiro à sua posição */
		if (next_pos > pos) {
			f_id = andaFrente(c);
			if (f_id != -1) { /* Impedido por um ciclista à frente */
				impedido = 1;
			}
			c = ciclistas[c.id];
		}

		/* Atualiza a posição. Simulacao começa com 60 e vai para 180 caso
		// um ciclista seja sorteado para andar a 90km/h */
		if (!impedido)
			c.pos += (float)c.v/simulacao;
		else {
			c.vMax = ciclistas[f_id].vMax;
			next_pos = (int)(c.pos + (float)c.vMax/simulacao);
			if (next_pos == pos)
				c.pos += (float)c.vMax/simulacao;
		}

		/* Completa uma volta */
		if ((int)c.pos > tam_pista - 1) {
			c.pos -= tam_pista;
			c.volta += 1;

			/* Insere seus dados na matriz de impressão e imprime os dados
			// da volta, caso ele tenha sido o ultimo a completá-la,
			// além de possivelmente sortear um ciclistas para andar a 90km/h */
			if (c.volta > 1) {
				LOCK(&mutex_buf);
				if (sorteado_90 == -2 && c.volta == num_voltas - 2) {
					if ((r = rand()%100) < 10) {
						/* Alguem foi sorteado, resorteia se o ciclista
						// estiver quebrado */
						sorteado_90 = rand() % num_ciclistas;
						while (ciclistas[sorteado_90].quebrado != -1)
							sorteado_90 = rand()%num_ciclistas;
					}
					else /* Ninguem foi sorteado */
						sorteado_90 = -1;
				}

				/* q é o numero de ciclistas quebrado até aquela volta */
				for (i = 0, q = 0; i < c.volta; i++)
					q += cic_quebrados[i];
				insereBuffer(B, c, (num_ciclistas - q));
				UNLOCK(&mutex_buf);
			}

			if (c.id == sorteado_90)
				c.v = c.vMax = 90;

			pontuado[c.id] = 0;

			/* Define a velocidade para a proxima volta caso nao esteja a 90 */
			if (c.id != sorteado_90)
				c = defineVel(c);

			/* Ciclista tem 1% de chance de quebrar a cada 15 voltas
			// se tiverem mais de 5 ciclistas */
			if ((c.volta % 15) == 0 && (num_ciclistas - cic_finalizados) > 5) {
				if (quebraCiclista(c)) {
					pista[(int)c.pos].faixa[c.faixa] = -1;
					pthread_create(&dummy, NULL, &threadDummy, NULL);

					LOCK(&mutex_finaliza);
					cic_finalizados++;
					/* Cada posiçao do vetor de quebrados é o numero de
					// ciclsitas que quebraram até aquela volta */
					cic_quebrados[c.volta]++;
					UNLOCK(&mutex_finaliza);

					return NULL;
				}
			}
		}
		ciclistas[c.id] = c;
		megaBarreira();
		c = ciclistas[c.id];
	}
	pista[(int)c.pos].faixa[c.faixa] = -1;
	pthread_create(&dummy, NULL, &threadDummy, NULL);
	LOCK(&mutex_finaliza);
	cic_finalizados++;
	UNLOCK(&mutex_finaliza);

	return NULL;
}


void preparaLargada(int d, int n) {
	int i;

	B = criaBuffer(n, num_voltas);
	pista = criaPista(d);
	ciclistas = criaCiclistas(n);
	classifics = malloc(n * sizeof(ciclista));
	cic_quebrados = malloc(num_voltas * sizeof(int));
	pontuado = malloc(n * sizeof(int));
	for (i = 0; i < n; i++)
		pontuado[i] = 0;
	for (i = 0; i < num_voltas; i++)
		cic_quebrados[i] = 0;
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
	destroiBuffer(B, num_voltas);
	free(thread);
	free(pontuado);
	free(ciclistas);
	free(classifics);
	free(cic_quebrados);
}


int main(int argc, char **argv) {
	tam_pista = atoi(argv[1]);
	num_ciclistas = atoi(argv[2]);
	num_voltas = atoi(argv[3]);
	if (argc > 4)
		DEBUG = 1;

	srand(time(NULL));
	corrida(tam_pista, num_ciclistas);

	return 0;
}