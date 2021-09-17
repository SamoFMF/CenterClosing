#include <stdio.h>
#include <stdlib.h>
#include "algutils.h"
#include "utils.h"
#include "binaryheap.h"
#include "backtracking.h"

void backtracking_recursive(Graph* G, int k, BitSet* R, BinaryHeap* queue, Center** centers, Options* options, double* valopt, BitSet* Ropt) {
	if (k == 0) {
		double val = eval_score(G, R, options);
		if (val < *valopt) {
			*valopt = val;
			bitset_soft_copy(Ropt, R);
		}
	}
	else {
		PairIntDouble pair = binary_heap_extract_min(queue);

		// Keep center pair.node
		if (queue->n > k) {
			backtracking_recursive(G, k, R, queue, centers, options, valopt, Ropt);
		}

		// Remove center pair.node
		centers_redistribute(centers, G, R, pair.node, options);
		double priority;
		for (int i = 0; i < G->m; i++) {
			priority = options->get_priority(centers[i]);
			binary_heap_update_key(queue, i, priority);
		}
		bitset_add(R, pair.node);
		backtracking_recursive(G, k - 1, R, queue, centers, options, valopt, Ropt);
		bitset_remove(R, pair.node);
		centers_redistribute_undo(centers, G->m);
		for (int i = 0; i < G->m; i++) {
			priority = options->get_priority(centers[i]);
			binary_heap_update_key(queue, i, priority);
		}

		binary_heap_insert(queue); // Undo extraction
	}
}

Result* backtracking(Graph* G, int k, Options* options) {
	Center** centers = centers_new_from_graph(G, options);

	// Get priorities
	double* priorities;
	if ((priorities = malloc(G->m * sizeof * priorities)) == NULL)
		printf("ERROR - Ran out of memory: backtracking - priorities\n");
	for (int i = 0; i < G->m; i++) {
		priorities[i] = options->get_priority(centers[i]);
	}

	BinaryHeap* queue = binary_heap_new_range(priorities, G->m);
	BitSet* R = bitset_new(G->m);

	// Get result using recursion
	BitSet* Ropt = bitset_new(G->m);
	double val = INT_MAX;
	backtracking_recursive(G, k, R, queue, centers, options, &val, Ropt);

	Result* res = result_new();
	result_update(res, val, Ropt, G->S);

	bitset_free(R);
	bitset_free(Ropt);
	binary_heap_free(queue);
	centers_free(centers, G->m);
	free(priorities);

	return res;
}
