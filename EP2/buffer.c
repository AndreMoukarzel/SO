/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#include "buffer.h"

int B_MAX = 4;

buffer *criaBuffer() {
	buffer *b;

	b = malloc(sizeof(buffer));
	b->buf = malloc(B_MAX * sizeof(c_buff));
	b->size = malloc(B_MAX * sizeof(int));
	b->ativ = malloc(B_MAX * sizeof(int));
	b->max = B_MAX;

	return b;
}

void insereBuffer(ciclista c, buffer *b, int n) {
	c_buff cb;
	int i = 0, j = 0;

	cb.id = c.id;
	cb.volta = c.volta;
	cb.p = c.p;
	cb.clas = c.clas;

	while (i < b->max && b->buf[i] != NULL && b->buf[i][0].volta != cb.volta)
		i++;

	if (i == b->max) {
		B_MAX *= 2;
		b->max = B_MAX;
		b->buf = (c_buff**) realloc(b->buf, B_MAX * sizeof(c_buff));
		b->size = (int*) realloc(b->size, B_MAX * sizeof(int));
		b->ativ = (int*) realloc(b->ativ, B_MAX * sizeof(int));
	}

	if (b->buf[i] == NULL) {
		b->size[i] = 0;
		b->buf[i] = malloc(n * sizeof(c_buff));
	}
	else if (b->buf[i][0].volta == cb.volta)
		j = b->size[i];

	b->size[1] += 1;
	b->buf[i][j] = cb;
}

void defineAtivos(buffer *b, int volta, int ativos) {
	int i = 0;

	while (i < b->max && b->buf[i] != NULL && volta != b->buf[i][0].volta)
		i++;

	b->ativ[i] = ativos;
}

int imprimeVolta(int volta, int n, buffer *b) {
	c_buff cb;
	int i = 0, j;

	while (i < b->max && b->buf[i] != NULL && volta != b->buf[i][0].volta)
		i++;

	if (i >= b->max || b->buf[i] == NULL) /* Erro */
		return -1;
	if (b->ativ[i] != b->size[1]) /* Ainda n tem ciclistas o suficiente aqui */
		return -2;

	/* Imprime a volta e a libera do buffer */
	printf("\n");
	if (b->buf[i][0].volta % 10) {
		for (j = 0; j < b->size[1]; j++) {
			cb = b->buf[i][j];
			printf("Ciclista de id %d teve classificação %d na volta %d\n", cb.id, cb.clas, cb.volta);
		}
	}
	else {
		for (j = 0; j < b->size[1]; j++) {
			cb = b->buf[i][j];
			/* Imprimir pontuação em ordem decrescente */
		}
	}
	for (j = i; j < b->max - 1; j++) {
		b->buf[j] = b->buf[j + 1];
	}
	b->buf[j + 1] = malloc(n * sizeof(c_buff));

	return 0;
}

void destroiBuffer(buffer *b) {
	free(b->buf);
	free(b->size);
	free(b->ativ);
}