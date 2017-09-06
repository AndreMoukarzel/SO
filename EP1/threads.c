#include "threads.h"

void *newThread(void* arg) {
	process *p;
	clock_t t0, t;
	double real_time = 0.0;

	p = (process *) arg;
	t0 = t = clock();

	while (real_time < p->dt) {
		t = clock();
		real_time = (double)(t - t0) / (double)CLOCKS_PER_SEC;
	}

	return NULL;
}

process *lineToProcess(line *l) {
	process *p = malloc(sizeof(process));;

	p->dt = l->dt;
	p->name = l->name;

	return p;
}