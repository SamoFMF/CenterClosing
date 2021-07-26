#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "utils.h"
#include "algutils.h"

int random_start(Graph* G) {
	int node = rand_lim(G->n);
	int center, cmin;
	double dist;
	double dmin = INT_MAX;
	for (int i = 0; i < G->m; i++) {
		center = G->C[i];
		dist = G->D[node][center];
		if (dist < dmin) {
			dmin = dist;
			cmin = center;
		}
	}
	return cmin;
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
Result hochbaum(Graph* G, int k, int (*f)(Graph* graph)) {
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
			node = G->V[i];
			dmin = INT_MAX;
			for (int j = 0; j < ki + 1; j++) {
				center = Carr[j];
				dist = G->D[node][center];
				if (dist < dmin) dmin = dist;
			}
			if (dmin > dmax) {
				dmax = dmin;
				vmax = G->V[i];
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