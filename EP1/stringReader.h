#ifndef STRINGREADER_H
#define STRINGREADER_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Lê linha e devolve um vetor de strings contendo
// as strings de linha.
*/
char **leLinha(char *line, int *lin);


/* Lê próxima string da linha, começando na posição
// pos, e devolve tal string */
char *leString(char *line, int init);


#endif