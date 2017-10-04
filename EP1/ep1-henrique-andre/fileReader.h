#ifndef FILEREADER_H
#define FILEREADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 1024

typedef struct{
	float t0;
	float dt;
	float deadline;
	char *name;
} line;


line *criaLine(int n);


/* Lê o arquivo e devolve uma lista lines, e o número de lines em
// line_count */
line **readFile(char *filename, int *line_count);


#endif
