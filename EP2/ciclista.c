#include "ciclista.h"


ciclista *criaCiclistas(int n) {
	ciclista *cics = malloc(n * sizeof(ciclista));
	int i;

	for (i = 0; i < n; i++) {
		cics[i].id = i;
		cics[i].volta = -1;
		cics[i].p = 0;
		cics[i].impedido = 0;
		cics[i].v = 30;
		cics[i].vMax = 30;
		cics[i].quebrado = -1;
	}

	return cics;
}


void posicionaCiclistas(int d, int n, ciclista* cics, metro* pista) {
	int pos = d -1, num, i;

	for(num = 0; num < n; pos--) {
		/* Bota até 10 ciclistas lado a lado */
		for (i = 0; i < 10; i++) { 
			if (num >= n)
				break;
			pista[pos].faixa[i] = num++;
			cics[num].pos = pos;
			cics[num].faixa = i;
			cics[num].clas = d - pos;
			cics[num].faixa = i;
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


void defineVel(ciclista c, metro* pista) {
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

	if (!c.impedido)
		c.vMax = c.v;

	/* impede todos os ciclistas atrás dele */
}