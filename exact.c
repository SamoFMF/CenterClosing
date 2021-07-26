#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "bitset.h"
#include "binaryheap.h"
#include "utils.h"
#include "algutils.h"
#include "priorityfunctions.h"

Result recursive_exact(Graph* G, int k, BinaryHeap* heap, BitSet* R, double best) {
	if (k == 0) {
		Result res;
		res.score = best;
		save_removed_nodes(&res, R);
		return res;
	}
	else if (heap->n == k) {
		// All remaining elements of heap must be removed from centers
		// TODO
		for (int i = 0; i < k; i++) {
			bitset_add(R, heap->nodes[i]);
		}
		double eval = eval_score(G, R);
		Result res;
		res.score = eval;
		save_removed_nodes(&res, R);

		for (int i = 0; i < k; i++) {
			bitset_remove(R, heap->nodes[i]);
		}

		return res;
	}
	else {
		PairIntDouble pair = binary_heap_extract_min(heap);

		// Check if pair.node is removed
		bitset_add(R, pair.node);
		double eval = eval_score(G, R);
		Result res;
		res.score = INT_MAX;
		res.R = NULL;
		Result res_tmp;
		if (eval < best) {
			Result res_tmp = recursive_exact(G, k - 1, heap, R, k == 1 ? eval : best); // TODO - if k=1 => k=0, maybe don't go into recursion and calculate eval again?
			if (res_tmp.score < best) {
				res = res_tmp;
			}
			else if (res_tmp.R != NULL) {
				free(res_tmp.R);
			}
		}
		bitset_remove(R, pair.node);

		// Check if pair.node is kept
		res_tmp = recursive_exact(G, k, heap, R, res.score < 0 ? best : res.score); // TODO - mozno, da je res.score == INT_MAX,  ce eval>=best???
		if ((res.score < 0 && res_tmp.score < best) || (res_tmp.score < res.score)) {
			if (res.R != NULL) free(res.R);
			res = res_tmp;
		}
		else if (res_tmp.R != NULL) {
			free(res_tmp.R);
		}

		binary_heap_insert(heap);
		return res;
	}
}

double* get_vals(Graph* G) {
	double* vals;
	if ((vals = malloc(G->m * sizeof * vals)) == NULL) {
		printf("ERROR: Ran out of memory - exact - vals");
	}
	for (int i = 0; i < G->m; i++) {
		vals[i] = i * i;
	}
	return vals;
}

Result exact(Graph* G, int k) {
	// Create vals - TODO
	double* vals;
	if ((vals = malloc(G->m * sizeof * vals)) == NULL) {
		printf("ERROR: Ran out of memory - exact - vals");
		Result res = { -99, NULL };
		return res;
	}
	for (int i = 0; i < G->m; i++) {
		vals[i] = i * i;
	}

	// Create queue
	BinaryHeap* queue = binary_heap_new(G->C, vals, G->n, G->m);

	// Create R (removed centers)
	BitSet* R = bitset_new(G->N);

	// TODO
	Result res = recursive_exact(G, k, queue, R, INT_MAX);

	// Free allocated memory
	free(vals);
	binary_heap_free(queue);
	bitset_free(R);

	return res;
}

/// <summary>
/// Basic exact algorithm using a queue and eval function.
/// </summary>
/// <param name="G">Graph pointer.</param>
/// <param name="k">Parameter k (number of centers removed).</param>
/// <param name="f">Eval function.</param>
/// <returns></returns>
Result exact_vals(Graph* G, int k, double* (*f)(Graph* G)) {
	// Create vals - TODO
	double* vals = f(G); // TODO - premisli, ce bi drugace? Mogoce f eval po centrih ali kaj podobnega in potem funkcija, ki gre cez vse

	// Create queue
	BinaryHeap* queue = binary_heap_new(G->C, vals, G->n, G->m);

	// Create R (removed centers)
	BitSet* R = bitset_new(G->N);

	// TODO
	Result res = recursive_exact(G, k, queue, R, INT_MAX);

	// Free allocated memory
	free(vals);
	binary_heap_free(queue);
	bitset_free(R);

	return res;
}

Result exact_final_recursive(Graph* G, int k, BinaryHeap* queue, BitSet* R, Center** centers, int* reverse_center_map, PriorityFunction* priority_function, double cur_eval, double best) {
	if (k == 0) {
		Result res;
		res.score = best;
		save_removed_nodes(&res, R);
		return res;
	}
	else if (queue->n == k) {
		// All remaining elements of heap must be removed as centers
		int center;
		double eval;
		double eval_max = cur_eval;
		for (int i = 0; i < k; i++) {
			// Take top k nodes from queue (last remaining elements)
			center = queue->nodes[i];
			eval = centers_redistribute(centers, G, R, reverse_center_map[center]);
			bitset_add(R, center);
			if (eval > eval_max) eval_max = eval;
		}

		Result res;
		if (eval_max < best) {
			res.score = eval_max;
			save_removed_nodes(&res, R);
		}
		else {
			res.score = INT_MAX;
			res.R = NULL;
		}

		// Undo and delete from memory
		for (int i = 0; i < k; i++) {
			centers_redistribute_undo(centers, G->m);
			bitset_remove(R, queue->nodes[i]);
		}


		return res;
	}
	else {
		// Pop best from queue
		PairIntDouble pair = binary_heap_extract_min(queue);

		// Prep results
		Result res, res_tmp;
		res.score = INT_MAX;
		res.R = NULL;

		// Check if center `pair.node` is removed
		double eval = centers_redistribute(centers, G, R, reverse_center_map[pair.node]); // Remove pair.node and distribute its nodes to their new nearest centers
		int revert = 0; // Should we revert queue updates? 0 = No, 1 = Yes
		int loc; // Used for updating queue
		double cur_value, value; // Used for updating queue
		if (eval < cur_eval) eval = cur_eval;
		if (eval < best) {
			bitset_add(R, pair.node); // Add pair.node to removed centers

			// Update queue
			for (int i = 0; i < G->m; i++) {
				loc = queue->loc[G->C[i]];
				cur_value = queue->heap[loc];
				value = priority_function->single(centers[i]);
				if (value > cur_value) binary_heap_increase_key(queue, G->C[i], value);
			}
			revert = 1;

			// Recursive call
			res_tmp = exact_final_recursive(G, k - 1, queue, R, centers, reverse_center_map, priority_function, eval, k == 1 ? eval : best); // If k==1, this was the last node to be removed, so we send eval instead of best, since eval is the actual value of final R

			if (res_tmp.score < best) {
				res = res_tmp;
			}
			else if (res_tmp.R != NULL) { // If res_tmp is not better than best & R is not NULL, delete it from memory
				free(res_tmp.R);
			}
			bitset_remove(R, pair.node);
		}
		centers_redistribute_undo(centers, G->m);

		if (revert) {
			// Revert queue updates
			for (int i = 0; i < G->m; i++) {
				loc = queue->loc[G->C[i]];
				cur_value = queue->heap[loc];
				value = priority_function->single(centers[i]);
				if (value < cur_value) binary_heap_decrease_key(queue, G->C[i], value);
			}
		}

		// Check if center `pair.node` is kept
		res_tmp = exact_final_recursive(G, k, queue, R, centers, reverse_center_map, priority_function, cur_eval, res.R != NULL ? res.score : best);
		if (res.R == NULL && res_tmp.score < best) {
			// No need to delete res.R from memory ... it is NULL
			res = res_tmp;
		}
		else if (res_tmp.score < res.score) {
			// If res.R == NULL, then res_tmp.score >= best, which means it can only be INT_MAX, so it can't be < res.score and we can't be here
			// This means res.R != NULL and res_tmp.score < res.score < INT_MAX
			free(res.R);
			res = res_tmp;
		}
		else {
			// res_tmp = {INT_MAX, NULL} ... no need to delete res_tmp.R from memory, it's NULL
		}

		// Add `pair` back into the queue
		binary_heap_insert(queue);

		return res;
	}
}

Result exact_final(Graph* G, int k, PriorityFunction* priority_function) {
	// Create centers
	Center** centers = centers_new_from_graph(G);

	// Create vals - TODO - glej zgoraj
	// TODO 2 - uporabi centers[...]->history->val, potem pa dodaj se, da se posodablja ob spremembah!
	// double* vals = f(G);
	/*double* vals;
	if ((vals = malloc(G->m * sizeof(double))) == NULL) {
		printf("ERROR - Ran out of memory: exact_final - vals");
	}
	for (int i = 0; i < G->m; i++) {
		vals[i] = centers[i]->history->val;
	}*/
	double* vals = priority_function->overall(G, centers);

	// Create queue
	BinaryHeap* queue = binary_heap_new(G->C, vals, G->n, G->m);

	// Create R (removed centers)
	BitSet* R = bitset_new(G->N);

	// Create reverse map: center to index of center in G->C
	int* reverse_center_map;
	if ((reverse_center_map = malloc(G->N * sizeof * reverse_center_map)) == NULL) {
		printf("ERROR: exact_final - reverse_center_map");
	}
	for (int i = 0; i < G->m; i++) {
		reverse_center_map[G->C[i]] = i;
	}

	// Get current eval
	double eval = 0.0;
	for (int i = 0; i < G->m; i++) {
		if (centers[i]->history->val > eval) {
			eval = centers[i]->history->val;
		}
	}

	// Get result using recursion
	Result res = exact_final_recursive(G, k, queue, R, centers, reverse_center_map, priority_function, eval, INT_MAX);

	// Free
	bitset_free(R);
	binary_heap_free(queue);
	centers_free(centers, G->m);
	free(vals); // TODO - kakorkoli bo to porihtano
	free(reverse_center_map); // TODO - mogoce kaj drugac porihtat?

	return res;
}

// Exact with centers for k=1
Result exact_centers_k1(Graph* G, BitSet* R, Center** centers) {
	// Init closest - closest[i] ... closest center of node i
	int* closest;
	if ((closest = malloc(G->n * sizeof * closest)) == NULL) {
		printf("ERROR: Ran out of memory - exact_centers_k1 - closest");
	}

	// Init closest - closest[i] ... closest center of node i
	int* closest_best;
	if ((closest_best = malloc(G->n * sizeof * closest_best)) == NULL) {
		printf("ERROR: Ran out of memory - exact_centers_k1 - closest_best");
	}

	Center* center;
	int node, cnode, idx_node, idx_center;
	double dist, dist_node, dist_center, dist_start, dist_opt;

	// Get start distance (before removing any center)
	dist_start = -1;
	for (int ci = 0; ci < G->m; ci++) {
		center = centers[ci];
		if (center->history->val > dist_start) {
			dist_start = center->history->val;
		}
	}

	dist_opt = INT_MAX;
	idx_center = -1;
	// Iterate over all centers
	for (int ci = 0; ci < G->m; ci++) {
		if (bitset_contains(R, G->C[ci])) continue;
		center = centers[ci];
		dist_center = dist_start;
		for (int i = 0; i < centers[ci]->numOfNodes; i++) { // Iterate over nodes that have `center` as closest center
			node = centers[ci]->nodes[i];
			dist_node = INT_MAX;
			idx_node = -1;

			// Find new closest center
			for (int j = 0; j < G->m; j++) {
				if (j != ci && !bitset_contains(R, G->C[j])) {
					cnode = G->C[j];
					dist = G->D[cnode][node];
					if (dist < dist_node) {
						dist_node = dist;
						idx_node = j;
					}
				}
			}

			closest[i] = idx_node;
			if (dist_node > dist_center) {
				dist_center = dist_node;
			}
		}

		if (dist_center < dist_opt) {
			idx_center = ci;
			dist_opt = dist_center;

			for (int i = 0; i < center->numOfNodes; i++) {
				closest_best[i] = closest[i];
			}
		}
	}

	// Add to R
	bitset_add(R, G->C[idx_center]);

	// Update centers
	centers_redistribute_closest(centers, G, R, idx_center, closest_best);

	// Free allocated memory
	free(closest);
	free(closest_best);

	Result res;
	res.score = dist_opt;
	save_removed_nodes(&res, R);
	return res;
}