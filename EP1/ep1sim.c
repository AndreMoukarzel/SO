/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "fileReader.h"
#include "pilha.h"
#include "fila.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

int LINE_COUNT, DEBUG = 0, context_changes = 0, finished_thread = 0, F_LINE = 0;
float STDQUANTUM = 0.5;
struct timeval starting_time;
FILE *f;


/* Retorna o valor em float com 2 casas decimais */
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
	float t0, t1;

	t0 = t1 = get_time();
	p = (process *) arg;

	if (DEBUG)
		fprintf(stderr, "Processo %s passou a usar a CPU %d\n", p->name, sched_getcpu());

	while (p->et > 0) {
		t1 = get_time();
		p->et -= (double)(t1 - t0);
		t0 = t1;
	}

	if (DEBUG) {
		fprintf(stderr, "Processo %s liberando a CPU %d\n", p->name, sched_getcpu());
		fprintf(stderr, "Processo %s finalizado. Escrevendo linha %d no arquivo de saída\n", p->name, F_LINE);
	}
	t1 = get_time();
	fprintf(f, "%s %f %f\n", p->name, t1, t1 - p->t0);
	pthread_mutex_lock(&mutex);
	F_LINE++;
	pthread_mutex_unlock(&mutex);

	return NULL;
}


void *newQuantumThread(void* arg) {
	process *p;
	float t0, t1, elapsed = 0.0;

	t0 = t1 = get_time();
	p = (process *) arg;

	if (DEBUG)
		fprintf(stderr, "Processo %s passou a usar a CPU %d\n", p->name, sched_getcpu());
	/* Passa a execuçao para outra thread se o quantum for cumprido ou
	// se o processo acabar */
	while (elapsed < p->quantum && p->et > 0){
		t1 = get_time();
		elapsed += t1 - t0;
		p->et -= t1 - t0;
		t0 = t1;
	}
	if (DEBUG)
		fprintf(stderr, "Processo %s liberando a CPU %d\n", p->name, sched_getcpu());
	/* Saiu por causa do tempo */
	if (elapsed >= p->quantum) {
		pthread_mutex_lock(&mutex);
		context_changes++;
		pthread_mutex_unlock(&mutex);

		pthread_exit(NULL);
	}
	/* Saiu porque terminou */
	if (DEBUG)
		fprintf(stderr, "Processo %s finalizado. Escrevendo linha %d no arquivo de saída\n", p->name, F_LINE);
	t1 = get_time();
	fprintf(f, "%s %f %f\n", p->name, t1, t1 - p->t0);
	pthread_mutex_lock(&mutex);
	finished_thread = 1;
	F_LINE++;
	pthread_mutex_unlock(&mutex);

	return NULL;
}


/* Cria novo process baseado na line l */
process *lineToProcess(line *l, int index, float quantum) {
	process *p = malloc(sizeof(process));;

	p->t0 = l->t0;
	p->dt = l->dt;
	p->et = l->dt;
	p->deadline = l->deadline;
	p->quantum = quantum;
	p->name = l->name;
	p->i = index;

	return p;
}


void shortestJobFirst(line **dados){
	int i = 0, th, pros_done = 0;
	float cur_time;
	pthread_t *threads = malloc(LINE_COUNT * sizeof(pthread_t));
	process *top_pros, **pros = malloc(LINE_COUNT * sizeof(process*));
	pilha *job_order;

	job_order = criaPilha(LINE_COUNT); /* processo mais curto estará sempre no topo da pilha */

	/* Executa enquanto nao acabarem os processos */
	while (pros_done < LINE_COUNT) {
		cur_time = get_time();
		/* Pega o processo com maior prioridade (topo da pilha) */
		top_pros = topoPilha(job_order);
		/* Executa ele até o fim  */
		if (!pilhaVazia(job_order)) {
			top_pros = desempilha(job_order);
			if ((th = pthread_create(&threads[top_pros->i], NULL, newThread, (void *) top_pros)))
				printf("Failed to create thread %d\n", th);
			pthread_join(threads[top_pros->i], NULL);
			pros_done++;
		}

		/* Recebe todos os processos disponíveis no t atual */
		while (i < LINE_COUNT && cur_time >= dados[i]->t0) {
			top_pros = topoPilha(job_order);
			pros[i] = lineToProcess(dados[i], i, dados[i]->dt + 1); /* Quantum absurdo nunca será atingido */
			insereOrdenado(job_order, pros[i]);
			if (DEBUG)
				fprintf(stderr, "Processo %s da linha %d recebido\n", pros[i]->name, i);

			i++;
		}
	}

	free(threads);
	for (i = 0; i < LINE_COUNT; i++)
		free(pros[i]);
	free(pros);
	destroiPilha(job_order);
}


void roundRobin(line **dados) {
	int i = 0, j, th, pros_done = 0;
	float cur_time;
	pthread_t *threads = malloc(LINE_COUNT * sizeof(pthread_t));
	process *top_pros;
	fila *jobs;

	jobs = criaFila(LINE_COUNT);

	while (pros_done < LINE_COUNT) {
		cur_time = get_time();

		/* Recebe todos os processos disponíveis no t atual */
		while (i < LINE_COUNT && cur_time >= dados[i]->t0){
			insere(jobs, lineToProcess(dados[i], i, STDQUANTUM));
			i++;
		}

		if (!filaVazia(jobs)){
			/* Executa todas as threads na fila 1 vez */
			for (j = jobs->tam; j > 0; j--) {
				/* Tira o processo da fila e executa */
				top_pros = removeFila(jobs);
				if ((th = pthread_create(&threads[top_pros->i], NULL, newQuantumThread, (void *) top_pros)))
					printf("Failed to create thread %d\n", th);
				pthread_join(threads[top_pros->i], NULL);

				/* Passa para o processo para o fim da fila, caso não tenha terminado */
				if (!finished_thread)
					insere(jobs, top_pros);
				else /* Caso contrario, o processo é permanentemente removido e a execução continua */
					pros_done++;
				finished_thread = 0;
			}
		}
	}
	destroiFila(jobs);
}


float decideQuantum(process *p) {
	float time = get_time();
	/* Deadlines impossíveis ou muito justas obrigam o processo a ser executado imediatamente por inteiro */
	if (p->t0 + p->dt >= p->deadline)
		return p->dt;
	/* Processo está estourando sua deadline */
	if (p->et + time >= p->deadline)
		return p->et + 0.1;
	/* P está próximo da conclusão, vamos poupá-lo do escalonamento */
	if (p->et < 2 * STDQUANTUM)
		return p->et + 0.1;
	/* P tem muito tempo para finalizar, vamos dar menos prioridade a ele */
	if (p->et + time < p->deadline - 5)
		return STDQUANTUM;
	return STDQUANTUM;
}


void priorityEscalonator(line **dados) {
	int i = 0, j, th, pros_done = 0;
	float cur_time;
	pthread_t *threads = malloc(LINE_COUNT * sizeof(pthread_t));
	process *top_pros, *new_pros;
	fila *jobs;

	jobs = criaFila(LINE_COUNT);

	while (pros_done < LINE_COUNT) {
		cur_time = get_time();

		/* Recebe todos os processos disponíveis no t atual */
		while (i < LINE_COUNT && cur_time >= dados[i]->t0) {
			new_pros = lineToProcess(dados[i], i, STDQUANTUM/2);
			new_pros->quantum = decideQuantum(new_pros);
			insere(jobs, new_pros);

			i++;
		}

		if (!filaVazia(jobs)){
			/* Executa todas as threads na fila 1 vez */
			for (j = jobs->tam; j > 0; j--) {
				top_pros = removeFila(jobs);
				/* Adaptação dinâmica do quantum */
				top_pros->quantum = decideQuantum(top_pros);
				if ((th = pthread_create(&threads[top_pros->i], NULL, newQuantumThread, (void *) top_pros)))
					printf("Failed to create thread %d\n", th);
				pthread_join(threads[top_pros->i], NULL);

				/* Passa para o processo para o fim da fila, caso não tenha terminado */
				if (!finished_thread)
					insere(jobs, top_pros);
				else /* Caso contrario, o processo é permanentemente removido e a execução continua */
					pros_done++;
				finished_thread = 0;
			}
		}
	}
	destroiFila(jobs);
}


void simulador(line **dados, int tipo){
	/* Shortest Job First */
	if (tipo == 1)
		shortestJobFirst(dados);
	/* Round Robin */
	else if (tipo == 2)
		roundRobin(dados);
	/* Escalonamento com Prioridade */
	else
		priorityEscalonator(dados);
}


int main(int argc, char **argv){
	struct timeval tv;
	int i;
	line **dados;

	gettimeofday(&tv, NULL);
	starting_time = tv;
	dados = readFile(argv[2], &LINE_COUNT);
	f = fopen(argv[3], "w");

	if (argc >= 5 && !strcmp(argv[4], "d"))
		DEBUG = 1;

	simulador(dados, atoi(argv[1]));

	for (i = 0; i < LINE_COUNT; i++)
		free(dados[i]);
	free(dados);

	if (DEBUG)
		fprintf(stderr, "Ocorreram %d mudanças de contexto\n", context_changes);
	fprintf(f, "%d", context_changes);
	fclose(f);

	return 0;
}
