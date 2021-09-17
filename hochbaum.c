#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "utils.h"
#include "algutils.h"

//int random_start(Graph* G) {
//	int node = rand_lim(G->n);
//	int center, cmin;
//	double dist;
//	double dmin = INT_MAX;
//	for (int i = 0; i < G->m; i++) {
//		center = G->C[i];
//		dist = G->D[node][center];
//		if (dist < dmin) {
//			dmin = dist;
//			cmin = center;
//		}
//	}
//	return cmin;
//}

int random_start(Graph* G) {
	return rand_lim(G->n);
}

/** @brief 3-approximation algorithm based on Hochbaum
*
* Hochbaum's implementation of closing centers problem.
* Works in reverse order, adding centers instead of removing them.
*
* @param G Pointer to Graph struct.
* @param k Number of centers to be removed.
* @param f Function used to calculate the first center to add.
*
* @return Result Approximation of the problem solution.
*/
Result* hochbaum(Graph* G, int k, Options* options) {
	int c, s;
	double dist, dmin, dmax;
	
	c = options->get_first(G);
	dmin = INT_MAX;
	for (int i = 0; i < G->m; i++) {
		dist = options->eval(c, i, G);
		if (dist < dmin) {
			dmin = dist;
			s = i;
		}
	}
	// BitSet* R = bitset_new(G->m);
	// bitset_add_from(R, G->S, G->m); // TODO - maybe add "bitset_new_full" which creates a bitset with every element already in, should be faster to just set values to max
	BitSet* R = bitset_new_full(G->m);
	bitset_remove(R, s);
	int* covered; // Already covered centers - somewhat redundant but speeds up the process of finding the furthest consumer
	if ((covered = malloc((G->m - k) * sizeof * covered)) == NULL) {
		printf("ERROR - Ran out of memory: hochbaum - covered");
	}
	covered[0] = s;

	for (int i = 0; i < G->m - k - 1; i++) {
		// Find c furthest from S\R
		dmax = -1;
		for (int ic = 0; ic < G->n; ic++) {
			dmin = INT_MAX;
			for (int is = 0; is < i + 1; is++) {
				dist = options->eval(ic, covered[is], G);
				if (dist < dmin)
					dmin = dist;
			}
			if (dmin > dmax) {
				dmax = dmin;
				c = ic;
			}
		}

		// Find s closest to c from R
		dmin = INT_MAX;
		for (int is = 0; is < G->m; is++) {
			if (bitset_contains(R, is)) {
				dist = options->eval(c, is, G);
				if (dist < dmin) {
					dmin = dist;
					s = is;
				}
			}
		}
		bitset_remove(R, s);
		covered[i + 1] = s;
	}

	// Eval R
	dmax = -1;
	for (int c = 0; c < G->n; c++) {
		dmin = INT_MAX;
		for (int is = 0; is < G->m - k; is++) {
			dist = options->eval(c, covered[is], G);
			if (dist < dmin)
				dmin = dist;
		}
		if (dmin > dmax)
			dmax = dmin;
	}

	Result* res = result_new();
	result_update(res, dmax, R, G->S);

	bitset_free(R);
	free(covered);

	return res;
}

Result hochbaum_outdated(Graph* G, int k, int (*f)(Graph* graph)) {
	int k1 = G->m - k;
	// Array of added centers
	int* Carr;
	if ((Carr = malloc(k1 * sizeof * Carr)) == NULL) {
		printf("ERROR - Ran out of memory: hochbaum - Rarr");
	}

	// Get first center to remove
	// Carr[0] = f(G);

	int node, center;
	int vmax, closest;
	double dmax, dist, dmin;
	closest = f(G);
	for (int ki = 0; ki < k1; ki++) {
		// Add to Carr
		Carr[ki] = closest;

		// Find the furthest node
		dmax = -1.0;
		for (int i = 0; i < G->n; i++) {
			node = G->C[i];
			dmin = INT_MAX;
			for (int j = 0; j < ki + 1; j++) {
				center = Carr[j];
				dist = G->D[node][center];
				if (dist < dmin) dmin = dist;
			}
			if (dmin > dmax) {
				dmax = dmin;
				vmax = G->C[i];
			}
		}

		// Find closest center to vmax || TODO - premisli, ce bi mel se bitset R in bi tukaj moral preveriti samo centre, ki se niso odstranjeni? Sicer poisces najdlje oddaljen node, torej verjetno njemu najblizji ni noben od teh? Not sure. Mogoce pa se lahko zgodi, ampak ni pomembno?
		dmin = INT_MAX;
		for (int i = 0; i < G->m; i++) {
			center = G->C[i];
			dist = G->D[vmax][center];
			if (dist < dmin) {
				dmin = dist;
				closest = center;
			}
		}

		// Carr[ki] = closest;
	}

	Result res;
	res.score = dmax;
	
	// Bitset of removed nodes
	BitSet* R = bitset_new(G->N);
	bitset_add_from(R, G->C, G->m);
	bitset_remove_from(R, Carr, k1);
	save_removed_nodes(&res, R);

	return res;
}