#include "fileReader.h"


line *criaLine(int n) {
	line *l;
	l = malloc(sizeof(line));
	l->name = malloc(n * sizeof(char));
	return l;
}


/* Lê o arquivo e devolve uma lista lines, e o número de lines em
// line_count */
line **readFile(char *filename, int *line_count) {
	int line_element = 1; /* (1 = t0, 2 = dt, 3 = deadline, 4 = nome) */
	int i, k; 
	char c;
	char buf[MAX_LINE_SIZE / 4];
	line **dados;
	FILE *file;

	/* inicia a lista de dados */
	dados = malloc(sizeof(line*));
	dados[0] = criaLine(MAX_LINE_SIZE);

	/* abre o arquivo */
	file = fopen(filename, "r");

	if (!file) {
		printf("ERROR!INVALID FILE!\n");
		return NULL;
	}
	
	i = k = 0; /* i = current char, k = current line */

	while( (c = getc(file)) != EOF ) {
		if (c == ' ') { /* fim de um elemento */
			buf[i] = '\0';
			/* converte a string pra int */
			if (line_element == 1)
				dados[k]->t0 = atof(buf);
			else if (line_element == 2)
				dados[k]->dt = atof(buf);
			else /* deadline */
				dados[k]->deadline = atof(buf);

			/* limpa o buffer e passa para
			// o proximo elemento da linha */
			line_element++;
			i = 0;
		}
		else if (c == '\n') { /* fim de uma linha */
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

	/* copia o nome da ultima linha, pois nao tem \n no final do arquivo*/
	buf[i] = '\0';
	strcpy(dados[k]->name, buf);
	fclose(file);
	k++;
	*line_count = k+1;

	return dados;
}