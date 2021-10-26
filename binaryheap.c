#include <stdio.h>
#include <stdlib.h>
#include "binaryheap.h"
#include "utils.h"

void swap(BinaryHeap* heap, int i, int j) {
	swap_double(heap->heap, i, j);
	swap_int(heap->nodes, i, j);
	swap_int(heap->loc, heap->nodes[i], heap->nodes[j]);
}

void min_heapify(BinaryHeap* heap, int i) {
	int left = 2 * i + 1;
	int right = 2 * i + 2;
	int smallest = i;

	if (left < heap->n && heap->heap[left] < heap->heap[smallest]) {
		smallest = left;
	}

	if (right < heap->n && heap->heap[right] < heap->heap[smallest]) {
		smallest = right;
	}

	if (smallest != i) {
		swap(heap, i, smallest);
		min_heapify(heap, smallest);
	}
}

BinaryHeap* binary_heap_new(int* xs, double* vals, int n, int m) {
	BinaryHeap* heap;
	if ((heap = malloc(sizeof * heap)) != NULL) {
		if ((heap->heap = malloc(m * sizeof(double))) != NULL && (heap->nodes = malloc(m * sizeof(int))) != NULL && (heap->loc = malloc(n * sizeof(int))) != NULL) {
			heap->n = m;
			for (int i = 0; i < m; i++) {
				heap->heap[i] = vals[i]; // Set value of node at index i
				heap->nodes[i] = xs[i]; // Node at index i
				heap->loc[xs[i]] = i; // Positition/location of node at index i in the heap
			}
			for (int i = m / 2 - 1; i >= 0; i--) {
				min_heapify(heap, i);
			}
		}
	}
	return heap;
}

BinaryHeap* binary_heap_new_test(int* xs, double* vals, int n) {
	BinaryHeap* heap;
	if ((heap = malloc(sizeof * heap)) != NULL) {
		if ((heap->heap = malloc(n * sizeof * heap->heap)) != NULL && (heap->nodes = malloc(n * sizeof * heap->nodes)) != NULL && (heap->loc = malloc(n * sizeof * heap->loc)) != NULL) {
			heap->n = n;
			for (int i = 0; i < n; i++) {
				heap->heap[i] = vals[i];
				heap->nodes[i] = xs[i];
				heap->loc[xs[i]] = i;
			}
			for (int i = n / 2 - 1; i >= 0; i--) {
				min_heapify(heap, i);
			}
		}
		else {
			printf("ERROR - Ran out of memory: binary_heap_new_test - inner\n");
		}
	}
	else {
		printf("ERROR - Ran out of memory: binary_heap_new_test - heap\n");
	}
	return heap;
}

BinaryHeap* binary_heap_new_range(double* vals, int n) {
	BinaryHeap* heap;
	if ((heap = malloc(sizeof * heap)) != NULL) {
		if ((heap->heap = malloc(n * sizeof * heap->heap)) != NULL && (heap->nodes = malloc(n * sizeof * heap->nodes)) != NULL && (heap->loc = malloc(n * sizeof * heap->loc)) != NULL) {
			heap->n = n;
			for (int i = 0; i < n; i++) {
				heap->heap[i] = vals[i];
				heap->nodes[i] = i;
				heap->loc[i] = i;
			}
			for (int i = n / 2 - 1; i >= 0; i--) {
				min_heapify(heap, i);
			}
		}
		else {
			printf("ERROR - Ran out of memory: binary_heap_new_test - inner\n");
		}
	}
	else {
		printf("ERROR - Ran out of memory: binary_heap_new_test - heap\n");
	}
	return heap;
}

void binary_heap_free(BinaryHeap* heap) {
	free(heap->heap);
	free(heap->nodes);
	free(heap);
}

PairIntDouble binary_heap_extract_min(BinaryHeap* heap) {
	// Get node with min value
	int node = heap->nodes[0];
	double value = heap->heap[0];

	// Move last node to the first spot
	swap(heap, 0, heap->n - 1);
	heap->n--;

	// Heapify
	min_heapify(heap, 0);

	// Return result
	PairIntDouble result = { node, value };
	return result;
}

void min_heapify_up(BinaryHeap* heap, int i) {
	if (i > 0) {
		int parent = (i - 1) / 2;
		if (heap->heap[parent] > heap->heap[i]) {
			swap(heap, parent, i);
			min_heapify_up(heap, parent);
		}
	}
}

void binary_heap_insert(BinaryHeap* heap) {
	// Heapify up and then increase number of elements in heap (re-add last removed)
	min_heapify_up(heap, heap->n++);
}

int binary_heap_search_old(BinaryHeap* heap, int node) {
	// Find index of node
	for (int i = 0; i < heap->n; i++) {
		if (heap->nodes[i] = node) return i;
	}
	return -1;
}

int binary_heap_search(BinaryHeap* heap, int node) {
	return heap->loc[node];
}

void binary_heap_decrease_key(BinaryHeap* heap, int node, double value) {
	// Find node
	int i = binary_heap_search(heap, node);
	if (i >= heap->n) return;

	// Decrease key
	heap->heap[i] = value;

	// Heapify up
	min_heapify_up(heap, i);
}

void binary_heap_increase_key(BinaryHeap* heap, int node, double value) {
	// Find node
	int i = binary_heap_search(heap, node);
	if (i >= heap->n) return;
	
	// Increase key
	heap->heap[i] = value;

	// Heapify down
	min_heapify(heap, i);
}

void binary_heap_update_key(BinaryHeap* heap, int node, double value) {
	// Find node
	int i = binary_heap_search(heap, node);
	if (i >= heap->n) return;

	if (value > heap->heap[i]) {
		// Increase key
		heap->heap[i] = value;

		// Heapify down
		min_heapify(heap, i);
	}
	else if (value < heap->heap[i]) {
		// Decrease key
		heap->heap[i] = value;

		// Heapify up
		min_heapify_up(heap, i);
	}
}

int binary_heap_is_empty(BinaryHeap* heap) {
	return heap->n == 0;
}