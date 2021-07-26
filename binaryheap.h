#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include "utils.h"

typedef struct BinaryHeap {
	double* heap; // Values of nodes
	int* nodes; // Nodes
	int* loc; // Locations of nodes in heap
	int n;
} BinaryHeap;

BinaryHeap* binary_heap_new(int* xs, double* vals, int n, int m);

void binary_heap_free(BinaryHeap* heap);

PairIntDouble binary_heap_extract_min(BinaryHeap* heap);

void binary_heap_insert(BinaryHeap* heap);

void binary_heap_decrease_key(BinaryHeap* heap, int node, double value);

void binary_heap_increase_key(BinaryHeap* heap, int node, double value);

void binary_heap_update_key(BinaryHeap* heap, int node, double value);

int binary_heap_is_empty(BinaryHeap* heap);

#endif