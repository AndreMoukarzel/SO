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
#include "pilha.h"

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

int LINE_COUNT, interupt = 0, context_changes = 0;
struct timeval starting_time;


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
	float cur_time;
	pthread_t *threads = malloc(LINE_COUNT * sizeof(pthread_t));
	process **pros = malloc(LINE_COUNT * sizeof(process*));
	pilha *job_order;

	job_order = criaPilha(LINE_COUNT); /* processo com a prioridade estara sempre no topo da pilha */

	while (i < LINE_COUNT) {
		cur_time = get_time();
		/* Novo processo recebido */
		if (cur_time >= dados[i]->t0) { 
			pros[i] = lineToProcess(dados[i]);

			/* Novo processo é mais curto que o sendo executado //
			if (pros[i]->dt < (pros[?]->dt - pros[?]->et)) { // obviamente substituir ? pelo elemento no topo da pilha //
				interupt = 1;
				// modifica a fila e cria thread com processo do topo bla bla bla //
				pthread_create(&threads[i], NULL, newThread, (void *) pros[i])
			}
			*/
			if ((th = pthread_create(&threads[i], NULL, newThread, (void *) pros[i])))
				printf("Failed to create thread %d\n", th);
			else /* Nova thread criada */
				i++;
		}
	}

	/* Espera as threads terminarem de processar */
	for (i = 0; i < LINE_COUNT; i++) {
		pthread_join(threads[i], NULL);
	}

	destroiPilha(job_order);
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
	/*
	pilha *jobs;
	jobs = criaPilha(LINE_COUNT);
	for (i = 0; i < LINE_COUNT - 1; i++)
		insereOrdenado(jobs, dados[i]);
	printPilha(jobs);
	printf("%f\n", dados[0]->dt);
	*/
	simulador(dados, 1);

	gettimeofday(&tv, NULL);
	printf("%d\n", LINE_COUNT);

	for (i = 0; i < LINE_COUNT; i++)
		free(dados[i]);
	free(dados);

	return 0;
}
