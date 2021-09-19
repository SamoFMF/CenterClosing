#include <stdio.h>
#include <stdlib.h>
#include "algutils.h"
#include "utils.h"
#include "binaryheap.h"
#include "backtracking.h"

int backtracking_decision_recursive(Graph* G, int k, double B, BitSet* R, BinaryHeap* queue, Center** centers, Options* options, double valcur, BitSet* Ropt) {
	if (k == 0) {
		// We only get here if valcur <= B
		bitset_soft_copy(Ropt, R);
		return 1;
	}
	else {
		PairIntDouble pair = binary_heap_extract_min(queue);

		// Keep center pair.node
		if (queue->n > k && backtracking_decision_recursive(G, k, B, R, queue, centers, options, valcur, Ropt))
			return 1;

		// Remove center pair.node
		double val = centers_redistribute(centers, G, R, pair.node, options);
		if (val > B) {
			// Solution does not exist
			centers_redistribute_undo(centers, G->m);
			binary_heap_insert(queue);
			return 0;
		}

		double priority;
		for (int i = 0; i < G->m; i++) {
			priority = options->get_priority(centers[i]);
			binary_heap_update_key(queue, i, priority);
		}
		bitset_add(R, pair.node);
		int res = backtracking_decision_recursive(G, k - 1, B, R, queue, centers, options, valcur, Ropt);
		bitset_remove(R, pair.node);
		centers_redistribute_undo(centers, G->m);
		for (int i = 0; i < G->m; i++) {
			priority = options->get_priority(centers[i]);
			binary_heap_update_key(queue, i, priority);
		}

		binary_heap_insert(queue); // Undo heap extraction

		return res;
	}
}

void backtracking_recursive(Graph* G, int k, BitSet* R, BinaryHeap* queue, Center** centers, Options* options, double valcur, double* valopt, BitSet* Ropt) {
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
			backtracking_recursive(G, k, R, queue, centers, options, valcur, valopt, Ropt);
		}

		// Remove center pair.node
		centers_redistribute(centers, G, R, pair.node, options);
		double priority;
		for (int i = 0; i < G->m; i++) {
			priority = options->get_priority(centers[i]);
			binary_heap_update_key(queue, i, priority);
		}
		bitset_add(R, pair.node);
		backtracking_recursive(G, k - 1, R, queue, centers, options, valcur, valopt, Ropt);
		bitset_remove(R, pair.node);
		centers_redistribute_undo(centers, G->m);
		for (int i = 0; i < G->m; i++) {
			priority = options->get_priority(centers[i]);
			binary_heap_update_key(queue, i, priority);
		}

		binary_heap_insert(queue); // Undo heap extraction
	}
}

Result* backtracking_decision(Graph* G, int k, double B, Options* options) {
	double eval;
	Center** centers = centers_new_from_graph(G, options, &eval);

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
	int val = backtracking_decision_recursive(G, k, B, R, queue, centers, options, eval, Ropt);

	Result* res = result_new();
	result_update(res, val, Ropt, G->S);

	bitset_free(R);
	bitset_free(Ropt);
	binary_heap_free(queue);
	centers_free(centers, G->m);
	free(priorities);

	return res;
}

Result* backtracking(Graph* G, int k, Options* options) {
	// TODO - to vrednost, ki jo tukaj dobimo, lahko potem posljemo dalje v rekurzijo itd. in ni na koncu potrebno racunati eval_score
	double temp_val;
	Center** centers = centers_new_from_graph(G, options, &temp_val);

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
	backtracking_recursive(G, k, R, queue, centers, options, temp_val, &val, Ropt);

	Result* res = result_new();
	result_update(res, val, Ropt, G->S);

	bitset_free(R);
	bitset_free(Ropt);
	binary_heap_free(queue);
	centers_free(centers, G->m);
	free(priorities);

	return res;
}
