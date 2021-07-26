#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void swap_int(int* A, int i, int j) {
	int tmp = A[i];
	A[i] = A[j];
	A[j] = tmp;
}

void swap_double(double* A, int i, int j) {
	double tmp = A[i];
	A[i] = A[j];
	A[j] = tmp;
}

int rand_lim(int n) {
	int limit = RAND_MAX - (RAND_MAX % n);
	int random_int = rand();
	while (random_int >= limit) {
		random_int = rand();
	}
	return random_int % n;
}

PairIntDouble* pair_new(int node, double value) {
	PairIntDouble* pair;
	if ((pair = malloc(sizeof * pair)) == NULL) {
		printf("ERROR - Ran out of memory: pair_new - pair");
	}
	else {
		pair->node = node;
		pair->value = value;
	}
	return pair;
}

LinkedList* linkedlist_new() {
	LinkedList* linkedlist;
	if ((linkedlist = malloc(sizeof * linkedlist)) != NULL) {
		linkedlist->val = 0.0;
		linkedlist->delta = 0;
		if ((linkedlist->prev = malloc(sizeof * linkedlist->prev)) == NULL || (linkedlist->next = malloc(sizeof * linkedlist->prev)) == NULL) {
			printf("ERROR: Ran out of memory - linkedlist_new - prev/next malloc");
		}
	}
	else {
		printf("ERROR: Ran out of memory - linkedlist_new");
	}
	return linkedlist;
}

void linkedlist_free(LinkedList* linkedlist) {
	free(linkedlist); // TODO - premisli: prev in next hocemo se obdrzati v spominu?
}

LinkedList* linkedlist_next(LinkedList* linkedlist_prev, int delta) {
	LinkedList* linkedlist;
	if ((linkedlist = malloc(sizeof * linkedlist)) != NULL) {
		linkedlist->val = linkedlist_prev->val;
		linkedlist->delta = delta;
		linkedlist->prev = linkedlist_prev;
		linkedlist->next = NULL;

		linkedlist_prev->next = linkedlist;
	}
	else {
		printf("ERROR: Ran out of memory - linkedlist_next");
	}
	return linkedlist;
}

LinkedList* linkedlist_remove_last(LinkedList* linkedlist) {
	LinkedList* linkedlist_prev = linkedlist->prev; // TODO - what if this is NULL??
	linkedlist_prev->next = NULL;
	free(linkedlist);
	return linkedlist_prev;
}

/// <summary>
/// Runs Floyd-Warshall algorithm on given matrix of distances.
/// All weights of links in the graph should already be added.
/// Distance between node and itself should be set to 0.
/// Other distances should be set to INT_MAX.
/// </summary>
/// <param name="D">Matrix of distances.</param>
/// <param name="n">Dimension of matrix is n x n.</param>
void floyd_warshall_algorithm(double** D, int n) {
	double dist;
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (D[i][k] < INT_MAX && D[k][j] < INT_MAX) {
					dist = D[i][k] + D[k][j];
					if (dist < D[i][j]) D[i][j] = dist;
				}
			}
		}
	}
}

/// <summary>
/// Creates an empty distance (square) matrix of dimensions n x n.
/// </summary>
/// <param name="n">Matrix dimension.</param>
/// <returns>Empty n x n matrix of type double.</returns>
double** distance_matrix_new(int n) {
	double** D;
	if ((D = malloc(n * sizeof * D)) == NULL) {
		printf("ERROR - Ran out of memory: distance_matrix_new - D");
	}
	else {
		for (int i = 0; i < n; i++) {
			if ((D[i] = malloc(n * sizeof * D[i])) == NULL) {
				printf("ERROR - Ran out of memory: distance_matrix_new - D[i]");
				// Delete D and D[j] for j < i from memory
				for (int j = 0; j < i; j++) free(D[j]);
				free(D);
				return NULL;
			}
		}
	}
	return D;
}