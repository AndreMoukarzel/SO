/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#include "pista.h"

metro *criaPista(int d) {
	int i, j, d2 = (int) d;
	metro* pista;

	pista = malloc(d2 * sizeof(metro));


	for (i = 0; i < d2; i++) {
		pista[i].faixa = malloc(10 * sizeof(int));
		pista[i].m = malloc(10 * sizeof(pthread_mutex_t));

		for (j = 0; j < 10; j++) {
			pista[i].faixa[j] = -1;
			pthread_mutex_init(&pista[i].m[j], NULL);
		}
	}
	return pista;
}

void printPista(metro *pista, int d){
	int i, j;
	for (i = 0; i < d; i++){
		printf("%2dm>", i);
		for (j = 0; j < 10; j++){
			printf("%3d", pista[i].faixa[j]);
		}
		printf("\n");
	}
}

void destroiPista(metro* pista, int d) {
	int i, j;

	for (i = 0; i < d; i++){
		free(pista[i].faixa);
		for (j = 0; j < 10; j++)
			pthread_mutex_destroy(&pista[i].m[j]);
		free(pista[i].m);
	}
	free(pista);
}