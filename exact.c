#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "graph.h"
#include "bitset.h"
#include "binaryheap.h"
#include "utils.h"
#include "algutils.h"
#include "priorityfunctions.h"
#include "plesnik.h"


void exact_recursive(Graph* G, int k, BitSet* R, BinaryHeap* queue, Center** centers, Options* options, double valcur, double* valopt, BitSet* Ropt) {
	if (k == 0) {
		// We only come here if valcur < *valopt => new best solution
		*valopt = valcur;
		bitset_soft_copy(Ropt, R);
	}
	else if (queue->n == k) {
		// All remaining elements of heap (centers) must be removed
		double eval;
		int s;

		// Add remaining centers to R
		for (int i = 0; i < k; i++) {
			s = queue->nodes[i];
			bitset_add(R, s);
		}

		// Redistribute consumers of removed centers
		int isbest = -1;
		for (int i = 0; i < k; i++) {
			s = queue->nodes[i];
			eval = centers_redistribute(centers, G, R, s, options);
			if (eval > valcur) {
				valcur = eval;
				if (valcur > *valopt) {// Solution is not optimal
					isbest = i;
					break;
				}
			}
		}
		if (isbest < 0) { // Found new optimal solution
			*valopt = valcur;
			bitset_soft_copy(Ropt, R);
			isbest = k - 1;
		}

		for (int i = 0; i < isbest + 1; i++) {
			s = queue->nodes[i];
			bitset_remove(R, s);
			centers_redistribute_undo(centers, G->m);
		}
		for (int i = isbest + 1; i < k; i++) {
			s = queue->nodes[i];
			bitset_remove(R, s);
		}
	}
	else {
		PairIntDouble pair = binary_heap_extract_min(queue);

		// Keep center `pair.node`
		exact_recursive(G, k, R, queue, centers, options, valcur, valopt, Ropt);

		// Remove node `pair.node`
		int revert = 0;
		double priority;
		double eval = centers_redistribute(centers, G, R, pair.node, options);
		if (eval > valcur) valcur = eval;
		if (valcur < *valopt) {
			// This path still has potential to be optimal
			revert = 1;
			bitset_add(R, pair.node);
			for (int i = 0; i < G->m; i++) {
				priority = options->get_priority(centers[i]);
				binary_heap_update_key(queue, i, priority);
			}

			exact_recursive(G, k - 1, R, queue, centers, options, valcur, valopt, Ropt);

			bitset_remove(R, pair.node);
		}
		centers_redistribute_undo(centers, G->m);
		if (revert)
			for (int i = 0; i < G->m; i++) {
				priority = options->get_priority(centers[i]);
				binary_heap_update_key(queue, i, priority);
			}

		binary_heap_insert(queue);
	}
}

//Result* exact(Graph* G, int k, Options* options) {
//	double valcur;
//	Center** centers = centers_new_from_graph(G, options, &valcur);
//
//	// Get priorities
//	double* priorities;
//	if ((priorities = malloc(G->m * sizeof * priorities)) == NULL)
//		printf("ERROR - Ran out of memory: backtracking - priorities\n");
//	for (int i = 0; i < G->m; i++) {
//		priorities[i] = options->get_priority(centers[i]);
//	}
//
//	BinaryHeap* queue = binary_heap_new_range(priorities, G->m);
//	BitSet* R = bitset_new(G->m);
//
//	double valopt = DBL_MAX;
//	BitSet* Ropt = bitset_new(G->m);
//	// Recursive call
//	exact_recursive(G, k, R, queue, centers, options, valcur, &valopt, Ropt);
//	Result* res = result_new();
//	result_update(res, valopt, Ropt, G->S);
//
//	// Free
//	bitset_free(R);
//	bitset_free(Ropt);
//	free(priorities);
//	binary_heap_free(queue);
//	centers_free(centers, G->m);
//
//	return res;
//}

Result* exact_bound(Graph* G, int k, Options* options, double valmax) {
	double valcur;
	Center** centers = centers_new_from_graph(G, options, &valcur);

	// Get priorities
	double* priorities;
	if ((priorities = malloc(G->m * sizeof * priorities)) == NULL)
		printf("ERROR - Ran out of memory: backtracking - priorities\n");
	for (int i = 0; i < G->m; i++) {
		priorities[i] = options->get_priority(centers[i]);
	}

	BinaryHeap* queue = binary_heap_new_range(priorities, G->m);
	BitSet* R = bitset_new(G->m);

	double valopt = valmax;
	BitSet* Ropt = bitset_new(G->m);
	// Recursive call
	exact_recursive(G, k, R, queue, centers, options, valcur, &valopt, Ropt);
	Result* res = result_new();
	result_update(res, valopt, Ropt, G->S);

	// Free
	bitset_free(R);
	bitset_free(Ropt);
	free(priorities);
	binary_heap_free(queue);
	centers_free(centers, G->m);

	return res;
}

Result* exact_pclstp(Graph* G, int k, Options* options) {
	// Run pclst+
	Result* res_pclstp = plesnik_unlimited(G, k, options, range_closest);
	double valopt = res_pclstp->score;
	result_free(res_pclstp);

	double valcur;
	Center** centers = centers_new_from_graph(G, options, &valcur);

	// Get priorities
	double* priorities;
	if ((priorities = malloc(G->m * sizeof * priorities)) == NULL)
		printf("ERROR - Ran out of memory: backtracking - priorities\n");
	for (int i = 0; i < G->m; i++) {
		priorities[i] = options->get_priority(centers[i]);
	}

	BinaryHeap* queue = binary_heap_new_range(priorities, G->m);
	BitSet* R = bitset_new(G->m);

	BitSet* Ropt = bitset_new(G->m);
	// Recursive call
	exact_recursive(G, k, R, queue, centers, options, valcur, &valopt, Ropt);
	Result* res = result_new();
	result_update(res, valopt, Ropt, G->S);

	// Free
	bitset_free(R);
	bitset_free(Ropt);
	free(priorities);
	binary_heap_free(queue);
	centers_free(centers, G->m);

	return res;
}

Result* exact(Graph* G, int k, Options* options) {
	return exact_bound(G, k, options, DBL_MAX);
}