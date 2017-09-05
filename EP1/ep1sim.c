/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
////////////////////////// COMO RODAR /////////////////////////////*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#define MAX_LINE_SIZE 1024

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

int LINE_COUNT;
time_t starting_time;
suseconds_t starting_utime;

typedef struct{
	float t0;
	float dt;
	float deadline;
	char *name;
} line;


line *criaLine(int n){
	line *l;
	l = malloc(sizeof(line));
	/* n = tamanho maximo dos nomes */
	l->name = malloc(n*sizeof(char));
	return l;
}


/* Pega o tempo de execucao atual (precisa pegar os milisegundos tmb D:< ) */
long int get_time(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	/* oq acontece se usarmos tv.tv_usec ? */
	return tv.tv_sec - starting_time;
}


/* Lê o arquivo e devolve uma lista de structs de dado com 4 elementos:
// t0, dt, deadline, nome de cada processo */
line **readFile(char *name){
	int line_element; /* (1 = t0, 2 = dt, 3 = deadline, 4 = nome) */
	int i, k;
	char c;
	char buf[MAX_LINE_SIZE / 4];
	line **dados;
	FILE *file;

	/* inicia a lista de dados */
	dados = malloc(sizeof(line*));
	dados[0] = criaLine(MAX_LINE_SIZE);

	/* abre o arquivo */
	file = fopen(name, "r");

	if (file){
		/* i = current char, k = current line */
		for (i = 0, line_element = 1, k = 0; (c = getc(file)) != EOF;){
			if (c == ' '){
				buf[i] = '\0';
				/* converte a string pra int */
				if (line_element == 1)
					dados[k]->t0 = strtof(buf, NULL);
				else if (line_element == 2)
					dados[k]->dt = strtof(buf, NULL);
				else /* deadline */
					dados[k]->deadline = strtof(buf, NULL);

				/* limpa o buffer e passa para
				// o proximo elemento da linha */
				line_element++;
				i = 0;
			}
			else if (c == '\n'){
				/* chegou no final da linha, passa o nome
				// pro dado atual, reseta i e line_element*/
				buf[i] = '\0';
				strcpy(dados[k]->name, buf);
				i = 0;
				line_element = 1;
				k++;
				dados = realloc(dados, (k+1)*sizeof(line));
				dados[k] = criaLine(MAX_LINE_SIZE / 4);
			}
			else{
				buf[i] = c;
				i++;
			}
		}
		/* copia o nome da ultima linha, pois nao
		// tem \n no final do arquivo*/
		buf[i] = '\0';
		strcpy(dados[k]->name, buf);
		fclose(file);
		LINE_COUNT = k+1;
	}

	return dados;
}


void runProcess(line *dado){
	char process[512];
	int exec, pid;
	strcpy(process, "/usr/bin/");
	strcat(process, dado->name);
	pid = fork();

	if (pid != 0){ /* processo pai */
		wait(NULL); /* espera processo filho acabar */
	}
	else { /* processo filho */
		if (!(exec = execl(process, process, NULL, NULL)))
			/* nao sei se o valor de retorno é de erro ou nao D: */
			printf("deu ruim?\n");
		return;
	}
}


void shortestJobFirst(line **dados){
	int i, th;
	/* o dt do ultimo processo recebido */
	float last_dt = 0.0;
	long int start_times[LINE_COUNT];
	pthread_t threads[LINE_COUNT];
	long int cur_time;

	for (i = 0; i < LINE_COUNT - 1;){
		/* atualiza o tempo */
		cur_time = get_time();
		/* cria a thread no t0 do processo */
		if (cur_time >= dados[i]->t0){
			/* caso o novo processo seja mais rapido, executa ele antes */
			if (i > 0 && dados[i]->dt < last_dt){
				/* passa a preferencia pro processo mais curto */
			}
			if ((th = pthread_create(&threads[i], NULL, (void *) runProcess, dados[i])))
				printf("failed to create thread: %d\n", th);
			else{
				start_times[i] = get_time();
				last_dt = dados[i]->dt;
				i++;
			}
		}
	}

	/* espera as threads terminarem de processar */
	for (i = 0; i < LINE_COUNT - 1; i++) {
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
	/* le o arquivo dado como primeiro argumento e
	// printa os elementos de cada linha dele */
	struct timeval tv;
	/* momento que a execucao do programa comecou */
	gettimeofday(&tv, NULL);
	starting_time = tv.tv_sec;
	starting_utime = tv.tv_usec;

	simulador(readFile(argv[1]), 1);

	gettimeofday(&tv, NULL);

	/*line **dados = readFile(argv[1]);
	for (int i = 0; i < LINE_COUNT; i++){
		printf("%f\n", dados[i]->t0);
		printf("%f\n", dados[i]->dt);
		printf("%f\n", dados[i]->deadline);
		printf("%s\n", dados[i]->name);
	}
	for (int i = 0; i < LINE_COUNT; i++)
		free(dados[i]);
	free(dados);*/
	return 0;
}
