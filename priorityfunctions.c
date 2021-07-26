#include <stdio.h>
#include <stdlib.h>
#include "algutils.h"
#include "graph.h"
#include "priorityfunctions.h"

PriorityFunction* priority_functions_new(double (*single)(Center* center), double* (*overall)(Graph* G, Center** centers)) {
	PriorityFunction* priority_function;
	if ((priority_function = malloc(sizeof * priority_function)) == NULL) {
		printf("ERROR - Ran out of memory: priority_function_new - priority_function");
	}
	else {
		priority_function->single = single;
		priority_function->overall = overall;
	}
	return priority_function;
}

void priority_function_free(PriorityFunction* priority_function) {
	free(priority_function);
}

double priority_eval_center(Center* center) {
	return center->history->val;
}

double* priority_eval(Graph* G, Center** centers) {
	double* vals;
	if ((vals = malloc(G->m * sizeof(double))) == NULL) {
		printf("ERROR - Ran out of memory: priority_val - vals");
	}
	else {
		for (int i = 0; i < G->m; i++) {
			vals[i] = priority_eval_center(centers[i]);
		}
	}
	return vals;
}

double priority_num_closest_center(Center* center) {
	return center->numOfNodes;
}

double* priority_num_closest(Graph* G, Center** centers) {
	double* vals;
	if ((vals = malloc(G->m * sizeof(double))) == NULL) {
		printf("ERROR - Ran out of memory: priority_val - vals");
	}
	else {
		for (int i = 0; i < G->m; i++) {
			vals[i] = priority_num_closest_center(centers[i]);
		}
	}
	return vals;
}