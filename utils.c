#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
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

double get_time(clock_t start, clock_t end) {
	return ((double)(end - start)) / CLOCKS_PER_SEC;
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
		/*if ((linkedlist->prev = malloc(sizeof * linkedlist->prev)) == NULL || (linkedlist->next = malloc(sizeof * linkedlist->prev)) == NULL) {
			printf("ERROR: Ran out of memory - linkedlist_new - prev/next malloc");
		}*/
		linkedlist->prev = NULL;
		linkedlist->next = NULL;
	}
	else {
		printf("ERROR: Ran out of memory - linkedlist_new");
	}
	return linkedlist;
}

void linkedlist_free(LinkedList* linkedlist) {
	free(linkedlist);
}

void linkedlist_free_all(LinkedList* linkedlist) { // Frees linkedlist and all previous linked lists
	if (linkedlist->prev != NULL)
		linkedlist_free_all(linkedlist->prev);
	linkedlist_free(linkedlist);
}

LinkedList* linkedlist_next(LinkedList* linkedlist_prev, int delta) {
	LinkedList* linkedlist = linkedlist_new();
	if (linkedlist != NULL) {
		linkedlist->val = linkedlist_prev->val;
		linkedlist->delta = delta;
		linkedlist->prev = linkedlist_prev;

		linkedlist_prev->next = linkedlist;
	}
	return linkedlist;
}

LinkedList* linkedlist_remove_last(LinkedList* linkedlist) {
	LinkedList* linkedlist_prev = linkedlist->prev;
	if (linkedlist_prev != NULL)
		linkedlist_prev->next = NULL;
	linkedlist_free(linkedlist);
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
		if (k % 100 == 0) printf("%d\n", k);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (D[i][k] < DBL_MAX && D[k][j] < DBL_MAX) {
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

/// <summary>
/// Frees 2D matrix of doubles of size n x n
void free_matrix_double(double** M, int n) {
	if (M != NULL) {
		for (int i = 0; i < n; i++) {
			if (M[i] != NULL) free(M[i]);
		}
		free(M);
	}
}

/// <summary>
/// Frees 2D matrix of ints of size n x n
void free_matrix_int(int** M, int n) {
	if (M != NULL) {
		for (int i = 0; i < n; i++) {
			if (M[i] != NULL) free(M[i]);
		}
		free(M);
	}
}


// PRINT FUNCTIONS
void print_array_double(double* xs, int n) {
	for (int i = 0; i < n; i++)
		printf("%6.2f ", xs[i]);
	printf("\n");
}

void print_matrix_double(double** M, int n) {
	for (int i = 0; i < n; i++) {
		print_array_double(M[i], n);
	}
	printf("\n");
}

void print_array_int(int* xs, int n) {
	for (int i = 0; i < n; i++)
		printf("%6d ", xs[i]);
	printf("\n");
}

void print_matrix_int(int** M, int n) {
	for (int i = 0; i < n; i++) {
		print_array_int(M[i], n);
	}
	printf("\n");
}