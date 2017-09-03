#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#define MAX_LINE_SIZE 1024

int LINE_COUNT;

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

/* Le o arquivo e devolve uma lista de structs de dado com 4 elementos:
// t0, dt, deadline, nome de cada processo */

line **readFile(char *name){
	/* elemento da linha que esta sendo lido
	// (1 = t0, 2 = dt, 3 = deadline, 4 = nome) */
	int line_element;
	int i, k;
	char c;
	char buf[MAX_LINE_SIZE / 4];
	line **dados;

	/* inicia a lista de dados */
	dados = malloc(sizeof(line*));
	dados[0] = criaLine(MAX_LINE_SIZE);

	/* abre o arquivo */
	FILE *file;
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

void runProcess(char *name){
	char process[512];
	strcpy(process, "/usr/bin/");
	strcat(process, name);
	int exec;
	int pid = fork();

	if (pid != 0) /* processo pai */
		wait(NULL); /* espera processo filho acabar */
	else { /* processo filho */
		if (!(exec = execl(process, process, NULL, NULL)))
			/* nao sei se o valor de retorno é de erro ou nao D: */
			printf("deu ruim?\n");
		return;
	}
}

void simulador(line **dados, int tipo){
	int i, th;
	pthread_t threads[LINE_COUNT];
	for (i = 0; i < LINE_COUNT; i++){
		if ((th = pthread_create(&threads[i], NULL, (void *) runProcess, dados[i]->name)))
			printf("failed to create thread: %d\n", th);
	}
	for (i = 0; i < LINE_COUNT; i++)
		pthread_join(threads[i], NULL);
}


int main(int argc, char **argv){
	/* le o arquivo dado como primeiro argumento e
	// printa os elementos de cada linha dele */

	simulador(readFile(argv[1]), 0);

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