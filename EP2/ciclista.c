/* //////////////////////////////////////////////////////////////////
// Nome: André Ferrari Moukarzel						NUSP: 9298166
// Nome: Henrique Cerquinho								NUSP: 9793700
///////////////////////////////////////////////////////////////////*/

#include "ciclista.h"


ciclista *criaCiclistas(int n) {
	ciclista *cics = malloc(n * sizeof(ciclista));
	int i;

	for (i = 0; i < n; i++) {
		cics[i].id = i;
		cics[i].volta = 0;
		cics[i].p = 0;
		cics[i].v = 30;
		cics[i].vMax = 30;
		cics[i].quebrado = -1;
		cics[i].clas = i + 1;
	}

	return cics;
}


void posicionaCiclistas(int d, int n, ciclista* cics, metro* pista) {
	int pos = d - 1, num, i;

	for(num = 0; num < n; pos--) {
		/* Bota até 10 ciclistas lado a lado */
		for (i = 0; i < 10; i++) {
			if (num >= n)
				break;
			cics[num].pos = (float)pos;
			cics[num].faixa = i;
			cics[num].clas = d - pos;
			pista[pos].faixa[i] = num++;
		}
	}
}


int quebraCiclista(ciclista c) {
	int r = rand() % 100;

	if (r < 1) {
		c.quebrado = c.volta;
		printf("Ciclista %d quebrou! Ele estava na %da volta em %do lugar.\n", c.id, c.volta, c.clas);
		return 1;
	}
	return 0;
}


ciclista defineVel(ciclista arg) {
	ciclista c = arg;
	int r = rand() % 100;

	if (c.v == 30) {
		if (r < 30)
			c.v = 30;
		else
			c.v = 60;
	}
	else { /* v = 60 */
		if (r < 50)
			c.v = 30;
		else
			c.v = 60;
	}
	c.vMax = c.v;

	return c;
}


/* Ordena os ciclistas no vetor clas baseado em sua classificação na corrida */
void defineClas(ciclista *clas, int n, int num_voltas) {
    int i, j, ini, fim, v, k;
    ciclista atual;

    /* Ordena por volta */
	for (i = 1; i < n; i++){
		atual = clas[i];

        j = i - 1;
		while (j >= 0 && clas[j].volta < atual.volta) {
			clas[j + 1] = clas[j];
            j--;
        }
		clas[j+1] = atual;
	}

    /* Ordena por posição
    // Aqui ordenaremos o vetor por "blocos" de numero de voltas iguais, pois
    // se apenas ordenassemos novamente por posição, desordenariamos
    // por volta */
    k = 0;
    v = clas[k].volta;
    while(k < n) {
        /* k será a primeira posiçao do vetor que o v é diferente.
        // Assim, usaremos ele para determinar os intervalos de ordenação */
        ini = k;
        while (v == clas[k].volta && k < n)
            k++;
        fim = k - 1;

        for (i = ini + 1; i <= fim && i < n; i++) {
            atual = clas[i];

            j = i - 1;
            while (j >= ini && clas[j].pos < atual.pos) {
                clas[j + 1] = clas[j];
                j--;
            }
            clas[j+1] = atual;
        }
        v = clas[k].volta;
    }

    /* Atualiza as classificações */
    j = 1;
    clas[0].clas = 1;
    for (i = 1; i < n; i++) {
        if ((clas[i].volta != clas[i-1].volta) || (clas[i].pos != clas[i-1].pos)) /* Não estão empatados */
            j++;
        clas[i].clas = j;
    }
}