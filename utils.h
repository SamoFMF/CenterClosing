#ifndef UTILS_H
#define UTILS_H

typedef struct PairIntDouble {
	int node;
	double value;
} PairIntDouble;

PairIntDouble* pair_new(int node, double value);

typedef struct LinkedListT {
	int delta;
	double val;
	struct LinkedListT* prev;
	struct LinkedListT* next;
} LinkedList;

void swap_int(int* A, int i, int j);

void swap_double(double* A, int i, int j);

int rand_lim(int n);

LinkedList* linkedlist_new();

void linkedlist_free(LinkedList* linkedlist);

LinkedList* linkedlist_next(LinkedList* linkedlist_prev, int delta);

LinkedList* linkedlist_remove_last(LinkedList* linkedlist);

void floyd_warshall_algorithm(double** D, int n);

double** distance_matrix_new(int n);

void free_matrix_double(double** M, int n);

void free_matrix_int(int** M, int n);

// PRINT FUNCTIONS
void print_array_double(double* xs, int n);

void print_matrix_double(double** M, int n);

void print_array_int(int* xs, int n);

void print_matrix_int(int** M, int n);

#endif