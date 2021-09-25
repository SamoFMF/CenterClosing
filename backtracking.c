#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "algutils.h"
#include "utils.h"
#include "binaryheap.h"

int backtracking_decision_recursive(Graph* G, int k, double B, BitSet* R, BinaryHeap* queue, Center** centers, Options* options, double valcur, BitSet* Ropt) {
	if (valcur > B)
		return 0;
	else if (k == 0) {
		// We only get here if valcur <= B
		bitset_soft_copy(Ropt, R);
		return 1;
	}
	else {
		PairIntDouble pair = binary_heap_extract_min(queue);

		// Keep center `pair.node`
		if (queue->n > k && backtracking_decision_recursive(G, k, B, R, queue, centers, options, valcur, Ropt)) {
			binary_heap_insert(queue);
			return 1;
		}

		// Remove center `pair.node`
		double val = centers_redistribute(centers, G, R, pair.node, options);
		if (val > B) {
			// Solution does not exist
			centers_redistribute_undo(centers, G->m);
			binary_heap_insert(queue);
			return 0;
		}
		else if (val > valcur)
			valcur = val;

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
		//double val = eval_score(G, R, options);
		if (valcur < *valopt) {
			*valopt = valcur;
			bitset_soft_copy(Ropt, R);
		}
	}
	else {
		PairIntDouble pair = binary_heap_extract_min(queue);

		// Keep center `pair.node`
		if (queue->n > k) {
			backtracking_recursive(G, k, R, queue, centers, options, valcur, valopt, Ropt);
		}

		// Remove center `pair.node`
		double val = centers_redistribute(centers, G, R, pair.node, options);
		if (val > valcur)
			valcur = val;
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

Result* backtracking_decision_to_optimization(Graph* G, int k, Options* options) {
	double eval;
	Center** centers = centers_new_from_graph(G, options, &eval);

	int ilow, ihigh, imid;
	double* dists = get_sorted_distances_no_duplicates(G, &ihigh, options);

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
	BitSet* Rtemp = bitset_new(G->m);
	if (!backtracking_decision_recursive(G, k, eval, R, queue, centers, options, eval, Ropt)) {
		ihigh--;
		if (!backtracking_decision_recursive(G, k, dists[ihigh], R, queue, centers, options, eval, Ropt)) {
			printf("ERROR - backtracking_decision_to_optimization failed: no solution found\n"); // This should never happen
			return NULL;
		}

		// Find `eval` in `dists`
		int jlow = 0;
		int jhigh = ihigh;
		int jmid;
		while (jhigh - jlow > 1) {
			jmid = (jhigh + jlow) / 2;
			if (dists[jmid] < eval) // TODO - maybe add some epsilon > 0 for comparisons?
				jlow = jmid;
			else {
				jhigh = jmid;
				if (dists[jhigh] == eval)
					break;
			}
		}

		if (jhigh == 0)
			ilow = 0;
		else
			ilow = jhigh - 1;
		while (ihigh - ilow > 1) {
			imid = (ihigh + ilow) / 2;
			if (backtracking_decision_recursive(G, k, dists[imid], R, queue, centers, options, eval, Rtemp)) {
				ihigh = imid;
				bitset_soft_copy(Ropt, Rtemp);
			}
			else {
				ilow = imid;
			}
		}
	}

	Result* res = result_new();
	result_update(res, eval_score(G, Ropt, options), Ropt, G->S);

	bitset_free(R);
	bitset_free(Ropt);
	bitset_free(Rtemp);
	binary_heap_free(queue);
	centers_free(centers, G->m);
	free(priorities);

	return res;
}

Result* backtracking(Graph* G, int k, Options* options) {
	double val_start;
	Center** centers = centers_new_from_graph(G, options, &val_start);

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
	double val = DBL_MAX;
	backtracking_recursive(G, k, R, queue, centers, options, val_start, &val, Ropt);

	Result* res = result_new();
	result_update(res, val, Ropt, G->S);

	bitset_free(R);
	bitset_free(Ropt);
	binary_heap_free(queue);
	centers_free(centers, G->m);
	free(priorities);

	return res;
}
