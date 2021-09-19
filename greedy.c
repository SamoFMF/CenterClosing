#include <stdio.h>
#include <stdlib.h>
#include "algutils.h"
#include "exact.h"

Result greedy_basic_outdated(Graph* G, int k) {
	// Create R (removed centers)
	BitSet* R = bitset_new(G->N);

	double score;
	double min_score = INT_MAX;
	int min_node, node;
	for (int ki = 0; ki < k; ki++) {
		min_score = INT_MAX;
		min_node = -1;
		for (int i = 0; i < G->m; i++) {
			node = G->C[i];
			if (!bitset_contains(R, node)) {
				bitset_add(R, node);
				// score = eval_score(G, R); // TODO - outdated
				score = 1;
				if (score < min_score) {
					min_score = score;
					min_node = node;
				}
				bitset_remove(R, node);
			}
		}
		if (min_node < 0) printf("\nERROR ERROR ERROR - greedy_basic\n\n");
		bitset_add(R, min_node);
	}
	Result res;
	res.score = min_score;
	save_removed_nodes(&res, R);

	// Free allocated memory
	bitset_free(R);

	return res;
}

Result* greedy_basic(Graph* G, int k, Options* options) {
	BitSet* R = bitset_new(G->m);

	double dist;
	double dmin = INT_MAX;
	int imin;
	for (int ki = 0; ki < k; ki++) {
		dmin = INT_MAX;
		imin = -1;
		for (int i = 0; i < G->m; i++) {
			if (!bitset_contains(R, i)) {
				bitset_add(R, i);
				dist = eval_score(G, R, options);
				if (dist < dmin) {
					dmin = dist;
					imin = i;
				}
				bitset_remove(R, i);
			}
		}
		if (imin < 0)
			printf("ERROR - Invalid index - greedy_basic\n");
		bitset_add(R, imin);
	}

	Result* res = result_new();
	result_update(res, dmin, R, G->S);
	bitset_free(R);
	return res;
}

//Result greedy_centers_old(Graph* G, int k) {
//	// Init centers (and the nodes they are the closest center to)
//	Center** centers = centers_new_from_graph(G);
//
//	// Init R (removed centers)
//	BitSet* R = bitset_new(G->N);
//
//	double val_min, val_node_min, val, val_max;
//	int idx_min, idx_node_min, idx_max;
//	for (int ki = 0; ki < k; ki++) {
//		val_min = INT_MAX;
//		for (int i = 0; i < G->m; i++) {
//			if (bitset_contains(R, G->C[i])) continue;
//			val_max = centers[i]->history->val;
//			for (int j = 0; j < centers[i]->numOfNodes; j++) {
//				val_node_min = INT_MAX;
//				for (int ci = 0; ci < G->m; ci++) {
//					if (bitset_contains(R, G->C[ci]) || ci == i) continue;
//					val = G->D[centers[i]->nodes[j]][G->C[ci]];
//					if (val < val_node_min) {
//						val_node_min = val;
//						idx_node_min = ci;
//					}
//				}
//				if (val_node_min > val_max) {
//					val_max = val_node_min;
//					idx_max = j;
//				}
//			}
//			if (val_max < val_min) {
//				val_min = val_max;
//				idx_min = i;
//			}
//		}
//
//		// Remove center G->C[idx_min] and redistribute the nodes for which it was the closest center
//		bitset_add(R, G->C[idx_min]);
//		centers_redistribute(centers, G, R, idx_min);
//		//center_free(centers[idx_min]);
//	}
//
//	//for (int i = 0; i < G->m; i++) {
//	//	//if (!bitset_contains(R, G->C[i])) center_free(centers[i]);
//	//}
//
//	Result res;
//	res.score = val_min;
//	save_removed_nodes(&res, R);
//	bitset_free(R);
//	return res;
//}

double test_k1(Graph* G, BitSet* R, Center** centers, Options* options) {
	// closest[i] ... 2nd closest center of consumer i
	int* closest;
	if ((closest = malloc(G->n * sizeof * closest)) == NULL)
		printf("ERROR: Ran out of memory - exact_centers_k1 - closest");

	double val_start = -1;
	for (int i = 0; i < G->m; i++)
		if (!bitset_contains(R, i) && centers[i]->history->val > val_start)
			val_start = centers[i]->history->val;
	
	Center* center;
	double dist, dsup, dcon;
	int sidx;
	double dopt = INT_MAX;
	int iopt;
	for (int is = 0; is < G->m; is++)
		if (!bitset_contains(R, is)) {
			center = centers[is];
			dsup = val_start;
			for (int i = 0; i < center->numOfNodes; i++) {
				dcon = INT_MAX;
				for (int j = 0; j < G->m; j++)
					if (!bitset_contains(R, j) && j != is) {
						dist = options->eval(center->nodes[i], j, G);
						if (dist < dcon) {
							dcon = dist;
							sidx = j;
						}
					}
				closest[center->nodes[i]] = sidx;
				if (dsup < dcon)
					dsup = dcon;
			}
			if (dsup < dopt) {
				dopt = dsup;
				iopt = is;
			}
		}
	
	// Update
	bitset_add(R, iopt);
	centers_redistribute_closest(centers, G, iopt, closest, options);
	free(closest);
	return dopt;
}

//Result greedy_centers(Graph* G, int k) {
//	// Init centers (and the nodes they are the closest center to)
//	Options* options = options_new();
//	Center** centers = centers_new_from_graph(G, options);
//
//	// Init R (removed centers)
//	BitSet* R = bitset_new(G->N);
//
//	// Calculate optimal solution for (k=1) k-times
//	Result res;
//	for (int ki = 0; ki < k; ki++) {
//		res = exact_centers_k1(G, R, centers);
//	}
//
//	// Free allocated memory
//	bitset_free(R);
//	centers_free(centers, G->m);
//
//	return res;
//}

Result* greedy_centers(Graph* G, int k, Options* options) {
	// TODO - premisli, ce nam ta vrednost olajsa delo v rekurziji spodaj
	double temp_val;
	Center** centers = centers_new_from_graph(G, options, &temp_val);
	BitSet* R = bitset_new(G->m);
	Result* res = result_new();

	double val;
	for (int ki = 0; ki < k; ki++) {
		val = test_k1(G, R, centers, options);
	}

	result_update(res, val, R, G->S);
	return res;
}