#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_ELEMENT_SIZE 1024
#define NUMERO_DE_LINHAS 3

typedef struct{
	int t0;
	int dt;
	int deadline;
	char *name;
} line;

line *criaLine(int n){
	line *l;
	l = malloc(sizeof(line));
	/* n = tamanho maximo dos nomes */
	l->name = malloc(n*sizeof(char));
	return l;
}

/* CONVERTE AS STRINGS DE NUMERO PRA INT (precisa ser pr float?) */

line **readFile(char *name){
	/* elemento da linha que esta sendo lido
	(1 = t0, 2 = dt, 3 = deadline, 4 = nome) */
	int line_element;
	int i, k;
	char c;
	char buf[MAX_ELEMENT_SIZE];
	line **dados;

	/* inicia a lista de dados */
	dados = malloc(NUMERO_DE_LINHAS*sizeof(line));
	for (i = 0; i < NUMERO_DE_LINHAS; i++)
		dados[i] = criaLine(MAX_ELEMENT_SIZE);

	/* abre o arquivo */
	FILE *file;
	file = fopen(name, "r");

	if (file)
		/* i = current char, k = current line */
		for (i = 0, line_element = 1, k = 0; (c = getc(file)) != EOF;){
			if (c == ' '){
				buf[i] = '\0';
				/* converte a string pra int */
				if (line_element == 1)
					dados[k]->t0 = atof(buf);
				else if (line_element == 2)
					dados[k]->dt = atof(buf);
				else /* deadline */
					dados[k]->deadline = atof(buf);
				/* limpa o buffer e passa para o proximo elemento da linha */
				line_element++;
				i = 0;
			}
			else if (c == '\n'){
				buf[i] = '\0';
				/* chegou no final da linha, passa o nome pro dado atual,
				reseta o buffer, i e line_element*/
				strcpy(dados[k]->name, buf);
				i = 0;
				line_element = 1;
				k++;
			}
			else{
				buf[i] = c;
				i++;
			}
		}
		/* copia o nome da ultima linha, pois nao tem \n no final do atquivo*/
		buf[i] = '\0';
		strcpy(dados[k]->name, buf);

	return dados;
}

int main(int argc, char **argv){
	/* le o arquivo dado como primeiro argumento e printa os elementos
	de cada linha dele *dabs* */
	line **dados = readFile(argv[1]);
	for (int i = 0; i<NUMERO_DE_LINHAS; i++){
		printf("%d\n", dados[i]->t0);
		printf("%d\n", dados[i]->dt);
		printf("%d\n", dados[i]->deadline);
		printf("%s\n", dados[i]->name);
	}
	for (int i = 0; i < NUMERO_DE_LINHAS; i++)
		free(dados[i]);
	return 0;
}