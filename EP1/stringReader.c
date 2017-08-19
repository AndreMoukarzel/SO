#include "stringReader.h"




/* Lê linha e devolve um vetor de strings contendo
// as strings de linha. Devolve o número de linhas
// da matriz em lin
*/
char **leLinha(char *line, int *lin) {
	int size = strlen(line);
	int pos = 0, str_num = 0;
	char **strings = malloc(sizeof(char *));

	while (pos < size) {
		while (pos < size && isspace((int) line[pos])) /* scaneia pela proxima string */
			pos++;

		if (pos < size) {
			str_num++;
			strings = realloc(strings, str_num * sizeof(char *));
			strings[str_num - 1] = leString(line, pos);
			pos += strlen(strings[str_num - 1]);
		}
	}

	*lin = str_num;
	return strings;
}


/* Lê próxima string da linha, começando na posição
// init, e devolve tal string */
char *leString(char *line, int init) {
	int size = strlen(line);
	int buff_size = 2, len = 0;
	char *buffer = malloc(buff_size * sizeof(char));

	while ((init + len) < size && !isspace((int) line[init + len])) { /* lê string */
		if (len > buff_size - 1) {
			buff_size *= 2;
			buffer = realloc(buffer, buff_size * sizeof(char));
		}

		buffer[len] = line[init + len];

		len++;
	}
	buffer[len] = '\0';

	return buffer;
}


/* Libera uma matriz de lin linhas */
void freeMatrix(char **matrix, int lin) {
	int i;
	for (i = 0; i < lin; i++)
		free(matrix[i]);
	free(matrix);
}