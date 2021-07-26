#include <stdio.h>
#include <stdlib.h>
#include "algutils.h"
#include "exact.h"

Result greedy_basic(Graph* G, int k) {
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
				score = eval_score(G, R);
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

Result greedy_centers(Graph* G, int k) {
	// Init centers (and the nodes they are the closest center to)
	Center** centers = centers_new_from_graph(G);

	// Init R (removed centers)
	BitSet* R = bitset_new(G->N);

	// Calculate optimal solution for (k=1) k-times
	Result res;
	for (int ki = 0; ki < k; ki++) {
		res = exact_centers_k1(G, R, centers);
	}

	// Free allocated memory
	bitset_free(R);
	centers_free(centers, G->m);

	return res;
}