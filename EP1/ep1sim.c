/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
////////////////////////// COMO RODAR /////////////////////////////*/

#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "fileReader.h"

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

int LINE_COUNT, interupt = 0, context_changes = 0;
struct timeval starting_time;

typedef struct{
	float dt; /* Tempo real necessário */
	float et; /* Tempo que o processo foi executado */
	char *name;
} process;


/* Retorna o valor em float com 2 casas decimais (praying hand emoji) */
float get_time(){
	struct timeval tv, temp;

	gettimeofday(&tv, NULL);

	temp = starting_time;
	temp.tv_usec /= 10000;
	tv.tv_usec /= 10000;

	tv.tv_sec -= temp.tv_sec;
	if (tv.tv_usec > temp.tv_usec)
		tv.tv_usec -= temp.tv_usec;
	else
		tv.tv_usec += 1 - temp.tv_usec;

	return (float)(tv.tv_sec + (tv.tv_usec/100.0) - 0.01);
}


void *newThread(void* arg) {
	process *p;
	clock_t t0, t1;

	p = (process *) arg;
	t0 = t1 = clock();

	printf("Inicio da thread %s\n", p->name);

	while (p->et < p->dt) {
		if (interupt) {
			/* Seção crítica */
			context_changes++;
			/*****************/
			return NULL;
		}
		t1 = clock();
		p->et += (double)(t1 - t0) / (double)CLOCKS_PER_SEC;
		t0 = t1;
	}

	printf("Finalização da thread %s\n", p->name);

	return NULL;
}


/* Cria novo process baseado na line l */
process *lineToProcess(line *l) {
	process *p = malloc(sizeof(process));;

	p->dt = l->dt;
	p->et = 0.0;
	p->name = l->name;

	return p;
}


void shortestJobFirst(line **dados){
	int i = 0, th;
	/* o dt do ultimo processo recebido */
	float last_dt = 0.0, cur_time;
	pthread_t *threads = malloc(LINE_COUNT * sizeof(pthread_t));

	while (i < LINE_COUNT) {
		/* atualiza o tempo */
		cur_time = get_time();

		/* cria a thread no t0 do processo */
		if (cur_time >= dados[i]->t0) {
			if ((th = pthread_create(&threads[i], NULL, newThread, (void *) lineToProcess(dados[i]))))
				printf("Failed to create thread %d\n", th);
			else {
				last_dt = dados[i]->dt;
				i++;
			}
		}
	}

	/* espera as threads terminarem de processar */
	for (i = 0; i < LINE_COUNT; i++) {
		pthread_join(threads[i], NULL);
	}
}


void roundRobin(line **dados) {

}


void priorityEscalonator(line **dados) {

}


void simulador(line **dados, int tipo){

	/* Shortest Job First */
	if (tipo == 1){
		shortestJobFirst(dados);
	}

	/* Round Robin */
	else if (tipo == 2){
		roundRobin(dados);
	}

	/* Escalonamento com Prioridade */
	else{
		priorityEscalonator(dados);
	}
}


int main(int argc, char **argv){
	struct timeval tv;
	int i;
	line **dados;

	gettimeofday(&tv, NULL);
	starting_time = tv;

	/*
	while (1)
		printf("%f\n", get_time());
	*/

	dados = readFile(argv[1], &LINE_COUNT);
	simulador(dados, 1);

	gettimeofday(&tv, NULL);
	printf("%d\n", LINE_COUNT);

	for (i = 0; i < LINE_COUNT; i++)
		free(dados[i]);
	free(dados);

	return 0;
}
