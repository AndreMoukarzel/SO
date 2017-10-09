/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#include "defineClas.h"


ciclista *defineClas(ciclista *clas, metro *pista, int n) {
    int i, j;
    ciclista atual;
    printf("antes:");
    for (i = 0; i < n; i++){
        clas[i].clas = i + 1;
        printf("clas:%2d, pos:%.2f, volta:%2d\n", clas[i].clas,clas[i].pos, clas[i].volta);
    }

    /* Ordena por volta */
	for (i = 1; i < n - 1; i++){
		atual = clas[i];

		for (j = i; j >= 0 && clas[j].volta > atual.volta; j++)
            /* troca as duas posiçoes */
			clas[j + 1] = clas[j];
		clas[j+1] = atual;
	}

    /* Ordena por posição
    for (i = 0; i < n; i++) {
        atual = clas[i + 1];

        for (j = i; (j >= 0) && (clas[i].pos < atual.pos); j--) {
            clas[j + 1] = clas[j];
        }
        clas[j + 1] = atual;
    }*/

    /* Atualiza as classificações */
    printf("depois:");
    for (i = 0; i < n; i++){
        clas[i].clas = i + 1;
        printf("clas:%2d, pos:%.2f, volta:%2d\n", clas[i].clas,clas[i].pos, clas[i].volta);
    }

    return clas;
}