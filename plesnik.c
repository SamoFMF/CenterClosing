#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "algutils.h"

Result range(Graph* G, int k, double B) {
	int k1 = G->m - k;
	Result res;

	// Covered nodes
	BitSet* covered = bitset_new(G->n);

	// Chosen centers
	int* C;
	if ((C = malloc(k1 * sizeof * C)) == NULL) {
		printf("ERROR - Ran out of memory: range - C");
	}
	int C_len = 0;

	int node, center, cstar;
	double dist;
	for (int i = 0; i < G->n; i++) {
		if (bitset_contains(covered, i)) continue;
		node = G->V[i];

		// Check if there exists a center with range at most B
		cstar = -1;
		for (int j = 0; j < G->m; j++) {
			center = G->C[j];
			dist = G->D[node][center];
			if (dist <= B) {
				cstar = center;
				break;
			}
		}

		// Check if we found center with distance at most B
		if (cstar < 0 || C_len == k1) {
			// No such center exists ... return False
			// OR We found a new center to be added, but we've already added the maximum number of centers ... return False
			res.score = 0;
			res.R = NULL;
			return res;
		}
		else {
			// Found center `cstar` ... cover all nodes within 3*B distance
			bitset_add(covered, i);
			for (int j = i + 1; j < G->n; j++) {
				node = G->V[i];
				dist = G->D[node][cstar];
				if (dist <= 3 * B) bitset_add(covered, j);
			}

			// Add `cstar` to centers `C`
			C[C_len++] = cstar;
		}
	}

	// Delete `covered` from memory
	free(covered);

	// Solution found
	res.score = 1;

	// Add removed nodes
	BitSet* R = bitset_new(G->N);
	bitset_add_from(R, G->C, G->m);
	bitset_remove_from(R, C, C_len);
	save_removed_nodes(&res, R);

	return res;
}

Result bisection(Graph* G, int k, double bmin, double bmax, Result* res, double tol) {
	if (bmax - bmin < tol) {
		res->score = 3 * bmax;
		return *res;
	}
	else {
		Result res_tmp;
		double bmid = (bmin + bmax) / 2;
		res_tmp = range(G, k, bmid);
		if (res_tmp.score == 0) {
			return bisection(G, k, bmid, bmax, res, tol);
		}
		else {
			result_free(res);
			return bisection(G, k, bmin, bmid, &res_tmp, tol);
		}
	}
}

Result plesnik(Graph* G, int k, double tol) {
	double bmin, bmax;

	// Get bmin
	double dmin, dist;
	bmin = -1;
	for (int i = 0; i < G->n; i++) {
		dmin = INT_MAX;
		for (int j = 0; j < G->m; j++) {
			dist = G->D[G->V[i]][G->C[j]];
			if (dist < dmin) dmin = dist;
		}
		if (dmin > bmin) bmin = dmin;
	}

	// Update bmin and get bmax
	Result res;
	bmax = bmin;
	for (res = range(G, k, bmax); res.score == 0; bmax *= 2);

	return bisection(G, k, bmin, bmax, &res, tol);
}