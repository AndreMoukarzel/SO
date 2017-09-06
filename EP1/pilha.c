#include "pilha.h"


int pilhaVazia(pilha *p){
	if(p->topo == 0) return 1;
	return 0;
}


pilha *criaPilha(int MAX){
	pilha *p;
	p = malloc (sizeof(pilha));
	p->v = malloc (MAX * sizeof(process));
	p->topo = 0;
	p->max = MAX;
	return p;
}


/* Insere um process x na pilha de forma a ficar ordenada pelo
// tempo restante de execução (dt - et)  */
void insereOrdenado(pilha *p, process *x){
    int i, j, ok = 0;

	if (p->topo < p->max){
        if (p->topo != 0){
    		for (i = 0; i < p->topo && ok == 0; i++) {
                /* se achar um dt maior, move o resto do vetor
                // e insere x ordenado */
                if ( (p->v[i]->dt - p->v[i]->et) > x->dt) {
                    for (j = p->topo; j >= i; j--){
                        p->v[j+1] = p->v[j];
                    }
                    p->v[i] = x;
                    ok = 1;
                }
            }
            /* x eh o maior valor do vetor */
            if (!ok) {
                p->v[p->topo] = x;
            }
        }
        else { /* Pilha estava vazia */
            p->v[0] = x;
        }
        (p->topo)++;
	}
    else
        printf("pilha cheia!\n");
}


process *desempilha(pilha *p){
	if (pilhaVazia(p) == 1){
		return NULL;
	}
	(p->topo)--;
	return (p->v[ p->topo ]);
}


void printPilha(pilha *p){
    int i;
    for (i = 0; i < p->topo; i++)
        printf("%f ", p->v[i]->dt);
    printf("\n");
}


void destroiPilha(pilha *p){
	while ((p->topo) != 0) {
        free(p->v[p->topo]);
        (p->topo)--;
    } 
	free(p);
}
