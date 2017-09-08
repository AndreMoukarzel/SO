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
#include "fila.h"

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

int LINE_COUNT, context_changes = 0, finished_thread = 0;
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
	float t0, t1;

	t0 = t1 = get_time();
	p = (process *) arg;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	printf("Inicio da thread %s em: %f\n", p->name, get_time());

	while (p->et > 0) {
		t1 = get_time();
		p->et -= (double)(t1 - t0);
		t0 = t1;
	}

	printf("Finalização da thread %s em: %f\n", p->name, get_time());

	return NULL;
}


void *newQuantumThread(void* arg) {
	process *p;
	float t0, t1, elapsed = 0.0;

	t0 = t1 = get_time();
	p = (process *) arg;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	/* Passa a execuçao para outra thread se o quantum for cumprido ou
	// se o processo acabar */
	while (elapsed < p->quantum && p->et > 0){
		t1 = get_time();
		elapsed += t1 - t0;
		p->et -= t1 - t0;
		t0 = t1;
	}
	/* Saiu por causa do tempo */
	if (elapsed >= p->quantum) {
		pthread_mutex_lock(&mutex1);
		context_changes++;
		pthread_mutex_unlock(&mutex1);
		printf("acabou o tempo de %s\n", p->name);

		pthread_exit(NULL); /* é correto ter isso aqui??? */
	}
	/* Saiu porque terminou */
	finished_thread = 1;
	printf("Finalização da thread %s em: %f\n", p->name, get_time());

	return NULL;
}


/* Cria novo process baseado na line l */
process *lineToProcess(line *l, int index, float quantum) {
	process *p = malloc(sizeof(process));;

	p->t0 = l->t0;
	p->dt = l->dt;
	p->et = l->dt;
	p->quantum = quantum;
	p->name = l->name;
	p->i = index;

	return p;
}


void shortestJobFirst(line **dados){
	int i = 0, new_job = 0;
	float cur_time;
	pthread_t *threads = malloc(LINE_COUNT * sizeof(pthread_t));
	process *top_pros, **pros = malloc(LINE_COUNT * sizeof(process*));
	pilha *job_order;

	job_order = criaPilha(LINE_COUNT); /* processo mais curto estará sempre no topo da pilha */

	while (i < LINE_COUNT) {
		cur_time = get_time();
		top_pros = topoPilha(job_order);

		if (!new_job) {
			while (!pilhaVazia(job_order)) {
				top_pros = desempilha(job_order);
				pthread_create(&threads[top_pros->i], NULL, newThread, (void *) top_pros);
				pthread_join(threads[top_pros->i], NULL);
			}
		}

		/* Novo processo recebido */
		if (cur_time >= dados[i]->t0) {
			new_job = 1;
			top_pros = topoPilha(job_order);
			pros[i] = lineToProcess(dados[i], i, dados[i]->dt + 1); /* Quantum absurdo nunca será atingido */
			insereOrdenado(job_order, pros[i]);
			printf("Novo processo recebido | Processo = %s\n", pros[i]->name);

			i++;
		}
		else
			new_job = 0;
	}

	printf("Começa a espera\n");
	/* Termina todos os processos restantes na pilha */
	while (!pilhaVazia(job_order)) {
		top_pros = desempilha(job_order);
		pthread_create(&threads[top_pros->i], NULL, newThread, (void *) top_pros);
		pthread_join(threads[top_pros->i], NULL);
	}
	printf("Fim da espera\n");

	free(threads);
	free(pros);
	destroiPilha(job_order);
}


void roundRobin(line **dados) {
	int i = 0, j, th;
	float cur_time, quantum = 0.5;
	pthread_t *threads = malloc(LINE_COUNT * sizeof(pthread_t));
	process *top_pros;
	fila *jobs;

	jobs = criaFila(LINE_COUNT);

	while (i < LINE_COUNT) {
		cur_time = get_time();

		/* Recebeu um novo processo */
		if (cur_time >= dados[i]->t0){
			insere(jobs, lineToProcess(dados[i], i, quantum));
			printf("Inseriu %s\n", dados[i]->name);
			i++;
		}

		if (!filaVazia(jobs)){
			/* Executa todas as threads na fila 1 vez */
			for (j = jobs->tam; j > 0; j--) {
				top_pros = removeFila(jobs);
				if ((th = pthread_create(&threads[top_pros->i], NULL, newQuantumThread, (void *) top_pros)))
					printf("Failed to create thread %d\n", th);
				pthread_join(threads[top_pros->i], NULL);

				/* Passa para o processo para o fim da fila, caso não tenha terminado */
				if (!finished_thread) {
					insere(jobs, top_pros);
					/* Se um processo é o único da fila, seu quantum acabar n conta como mudança de contexto */
					if (jobs->tam == 0)
						context_changes--;
				}
				/* Caso contrario, o processo é permanentemente removido e a execução continua */
				finished_thread = 0;
			}
		}
	}

	while (!filaVazia(jobs)) {
		top_pros = removeFila(jobs);
		if ((th = pthread_create(&threads[top_pros->i], NULL, newQuantumThread, (void *) top_pros)))
			printf("Failed to create thread %d\n", th);
		pthread_join(threads[top_pros->i], NULL);

		/* Passa para o processo para o fim da fila, caso não tenha terminado */
		if (!finished_thread) {
			insere(jobs, top_pros);
		}
		/* Caso contrario, o processo é permanentemente removido e a execução continua */
		finished_thread = 0;
	}
}


void priorityEscalonator(line **dados) {

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
	int i, tipo = 1;
	line **dados;

	gettimeofday(&tv, NULL);
	starting_time = tv;
 	tipo = atoi(argv[2]);

	dados = readFile(argv[1], &LINE_COUNT);

	simulador(dados, tipo);

	gettimeofday(&tv, NULL);
	printf("context changes: %d\n", context_changes);

	for (i = 0; i < LINE_COUNT; i++)
		free(dados[i]);
	free(dados);

	return 0;
}
