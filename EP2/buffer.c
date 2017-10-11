/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#include "buffer.h"

buffer *criaBuffer(int n, int v) {
	int i;
	buffer *b;

	b = malloc(sizeof(buffer));
	/* Matriz vxn */
	b->buf = malloc(v * sizeof(c_buff*));
	b->topo = malloc(v * sizeof(int));
	b->cheio = malloc(v * sizeof(int));
	for (i = 0; i < v; i++) {
		b->buf[i] = malloc(n * sizeof(c_buff));
		b->topo[i] = 0;
		b->cheio[i] = 0;
	}
	return b;
}

void insereBuffer(buffer *b, ciclista c, int cic_ativos) {
	int v;
	c_buff cb;

	v = c.volta;
	cb.id = c.id;
	cb.volta = c.volta;
	cb.p = c.p;
	cb.clas = c.clas;

	/* Insere o ciclista na primeira posiçao livre e incrementa o topo */
	b->buf[v][b->topo[v]++] = cb;

	if (b->topo[v] == cic_ativos)
		imprimeVolta(b, v);
}

void imprimeVolta(buffer *b, int v) {
	c_buff cb;
	int i, j;
	/* Ordena por clas */
	for (i = 1; i < b->topo[v]; i++){
		cb = b->buf[v][i];

		j = i - 1;
		while (j >= 0 && b->buf[v][j].clas > cb.clas) {
			b->buf[v][j + 1] = b->buf[v][j];
			j--;
		}
		b->buf[v][j + 1] = cb;
	}

	printf("Volta %d:\n", v);
	for (i = 0; i < b->topo[v]; i++) {
		cb = b->buf[v][i];
		printf("%4da posição: Ciclista %d\n", cb.clas, cb.id);
	}
	printf("\n");

	/* Se volta for multipla de 10, imprime os pontos */
	if (!(v % 10)) {
		printf("Pontuação na volta %d:\n", v);
		/* Ordena a linha da matriz por pontos */
		for (i = 1; i < b->topo[v]; i++){
			cb = b->buf[v][i];

	        j = i - 1;
			while (j >= 0 && b->buf[v][j].p < cb.p) {
				b->buf[v][j + 1] = b->buf[v][j];
	            j--;
	        }
			b->buf[v][j + 1] = cb;
		}
		/* E imprime */
		for (i = 0; i < b->topo[v]; i++) {
			cb = b->buf[v][i];
			printf("%3d pontos - Ciclista %d\n", cb.p, cb.id);
		}
		printf("\n");
	}
}

void destroiBuffer(buffer *b, int v) {
	int i;
	for (i = 0; i < v; i++)
		free(b->buf[i]);
	free(b->buf);
	free(b->topo);
	free(b->cheio);
	free(b);
}