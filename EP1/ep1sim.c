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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

int LINE_COUNT, context_changes = 0;
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
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	printf("Inicio da thread %s em: %f\n", p->name, get_time());

	while (p->et > 0) {
		t1 = clock();
		p->et -= (double)(t1 - t0) / (double)CLOCKS_PER_SEC;
		t0 = t1;
	}

	printf("Finalização da thread %s em: %f\n", p->name, get_time());

	return NULL;
}


/* Cria novo process baseado na line l */
process *lineToProcess(line *l, int index) {
	process *p = malloc(sizeof(process));;

	p->dt = l->dt;
	p->et = l->dt;
	p->name = l->name;
	p->i = index;

	return p;
}


void shortestJobFirst(line **dados){
	int i = 0, th;
	float cur_time;
	pthread_t *threads = malloc(LINE_COUNT * sizeof(pthread_t));
	process *top_pros, **pros = malloc(LINE_COUNT * sizeof(process*));
	pilha *job_order;

	job_order = criaPilha(LINE_COUNT); /* processo mais curto estará sempre no topo da pilha */

	while (i < LINE_COUNT) {
		cur_time = get_time();
		top_pros = topoPilha(job_order);

		/* Processo mais curto acabou */
		if (!pilhaVazia(job_order) && top_pros->et <= 0) {
			printf("Topo finalizou. Topo = %s\n", top_pros->name);
			desempilha(job_order);

			if (!pilhaVazia(job_order)) {
				top_pros = topoPilha(job_order);
				if ((th = pthread_create(&threads[top_pros->i], NULL, newThread, (void *) top_pros)))
					printf("Failed to create thread %d\n", th);
				else /* Nova thread criada */
					i++;
			}
		}

		/* Novo processo recebido */
		if (cur_time >= dados[i]->t0) {
			top_pros = topoPilha(job_order);
			pros[i] = lineToProcess(dados[i], i);
			insereOrdenado(job_order, pros[i]);
			printf("Novo processo recebido | Processo = %s\n", pros[i]->name);

			/* Novo processo é mais curto que o sendo executado (topo da pilha mudou) */
			if (top_pros != topoPilha(job_order)) {
				if (top_pros != NULL) { /* Cancela thread que saiu do topo */
					printf("Thread %s a ser cancelada\n", top_pros->name);
					pthread_cancel(threads[top_pros->i]);
					context_changes++;
				}
				top_pros = topoPilha(job_order);
				printf("Mudou o topo! | Novo topo = %s\n", top_pros->name);
				if ((th = pthread_create(&threads[i], NULL, newThread, (void *) top_pros)))
					printf("Failed to create thread %d\n", th);
			}
			i++;
		}
	}

	printf("Começa a espera\n");
	/* Termina todos os processos restantes na pilha */
	while (!pilhaVazia(job_order)) {
		top_pros = desempilha(job_order);
		pthread_join(threads[top_pros->i], NULL);
		if (!pilhaVazia(job_order)) {
			top_pros = topoPilha(job_order);
			pthread_create(&threads[top_pros->i], NULL, newThread, (void *) top_pros);
		}
	}
	printf("Fim da espera\n");

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
	printf("context changes: %d\n", context_changes);

	for (i = 0; i < LINE_COUNT; i++)
		free(dados[i]);
	free(dados);

	return 0;
}
