#include <stdio.h>
#include <stdlib.h>
#include "bitset.h"
#include "setcover.h"
#include "graph.h"
#include "algutils.h"

SetCover* graph_to_setcover(Graph* G, Options* options, double B) {
	SetCover* SC;
	if ((SC = malloc(sizeof * SC)) == NULL)
		printf("ERROR - Ran out of memory: graph_to_setcover - SC\n");
	if ((SC->S = malloc(G->N * sizeof * SC->S)) == NULL)
		printf("ERROR - Ran out of memory: graph_to_setcover - SC->S\n");
	for (int i = 0; i < G->N; i++)
		SC->S[i] = bitset_new(G->N);
	int node;
	for (int c = 0; c < G->n; c++) {
		node = G->C[c];
		for (int i = 0; i < G->m && options->eval(c, G->G[node][i], G) <= B; i++) {
			bitset_add(SC->S[node], G->S[G->G[node][i]]);
		}
		bitset_add(SC->S[node], node); // Closed neighbourhood
	}
	for (int s = 0; s < G->m; s++) {
		node = G->S[s];
		for (int i = 0; i < G->n && options->eval(G->G[node][i], s, G) <= B; i++) {
			bitset_add(SC->S[node], G->C[G->G[node][i]]);
		}
		for (int s1 = 0; s1 < G->m; s1++) { // Also includes node => closed neighbourhood
			bitset_add(SC->S[node], G->S[s1]);
		}
	}
	SC->contains = bitset_new_full(G->N);
	SC->n = G->N;
	return SC;
}

SetCover* center_closing_to_setcover(Graph* G, Options* options, double B) {
	SetCover* SC;
	if ((SC = malloc(sizeof * SC)) == NULL)
		printf("ERROR - Ran out of memory: graph_to_setcover - SC\n");
	if ((SC->S = malloc(G->m * sizeof * SC->S)) == NULL)
		printf("ERROR - Ran out of memory: graph_to_setcover - SC->S\n");
	int node;
	SC->contains = bitset_new_full(G->m);
	for (int i = 0; i < G->m; i++) {
		SC->S[i] = bitset_new(G->n);
		node = G->S[i];
		for (int j = 0; j < G->n && options->eval(G->G[node][j], i, G) <= B; j++)
			bitset_add(SC->S[i], G->G[node][j]);
		if (SC->S[i]->numOfElements == 0)
			bitset_remove(SC->contains, i);
	}
	SC->n = G->m;
	return SC;
}

ResultSetCover* resultsetcover_new() {
	ResultSetCover* res;
	if ((res = malloc(sizeof * res)) == NULL) {
		printf("ERROR - Ran out of memory: resultsetcover_new\n");
	}
	else {
		res->C = NULL;
		res->length = 0;
	}
	return res;
}

void resultsetcover_free(ResultSetCover* res) {
	if (res->C != NULL)
		free(res->C);
	free(res);
}

void resultsetcover_add(ResultSetCover* res, int x) {
	res->length++;
	if ((res->C = realloc(res->C, res->length * sizeof * res->C)) == NULL)
		printf("ERROR - Reallocating memory: resultsetcover_add\n");
	res->C[res->length - 1] = x;
}

ResultSetCover* minimum_set_cover(SetCover* SC, BitSet* U) {
	if (SC->contains->numOfElements == 0) {
		if (U->numOfElements == 0) {
			return resultsetcover_new();
		}
		else {
			return NULL;
		}
	}

	// Let $S \in SC->S$ be of maximum cardinality
	int max_cardinality = 0;
	int max_i;
	for (int i = 0; i < SC->n; i++) {
		if (bitset_contains(SC->contains, i)) {
			if (SC->S[i]->numOfElements > max_cardinality) {
				max_cardinality = SC->S[i]->numOfElements;
				max_i = i;
			}
		}
	}

	// Get C2
	//bitset_remove(SC->contains, max_i);
	int removed_max_i = bitset_contains(SC->contains, max_i);
	if (removed_max_i)
		bitset_remove(SC->contains, max_i);
	ResultSetCover* C2 = minimum_set_cover(SC, U);

	// Get C1
	// Setup
	int* S;
	int nS = SC->S[max_i]->numOfElements;
	if ((S = malloc(nS * sizeof * S)) == NULL)
		printf("ERROR - Ran out of memory: minimum_set_cover - S\n");
	int idx = 0;
	for (int i = 0; i < SC->S[max_i]->n; i++)
		if (bitset_contains(SC->S[max_i], i))
			S[idx++] = i;
	int** removed;
	if ((removed = malloc(SC->n * sizeof * removed)) == NULL)
		printf("ERROR - Ran out of memory: minimum_set_cover - removed\n");
	for (int i = 0; i < SC->n; i++) {
		if ((removed[i] = malloc(nS * sizeof * removed[i])) == NULL)
			printf("ERROR - Ran out of memory: minimum_set_cover - removed[%d]\n", i);
		for (int j = 0; j < nS; j++)
			removed[i][j] = -1;
	}
	// Remove elements
	int* removed_contains;
	if ((removed_contains = malloc(nS * sizeof * removed_contains)) == NULL)
		printf("ERROR - Ran out of memory: minimum_set_cover - removed_contains\n");
	for (int i = 0; i < nS; i++)
		removed_contains[i] = -1;
	/*int removed_max_i = bitset_contains(SC->contains, max_i);
	if (removed_max_i)
		bitset_remove(SC->contains, max_i);*/
	idx = 0;
	for (int i = 0; i < nS; i++)
		if (bitset_contains(U, S[i])) {
			bitset_remove(U, S[i]);
			removed_contains[idx++] = S[i];
		}
	for (int i = 0; i < SC->n; i++) {
		if (bitset_contains(SC->contains, i)) {
			idx = 0;
			for (int j = 0; j < nS; j++)
				if (bitset_contains(SC->S[i], S[j])) {
					bitset_remove(SC->S[i], S[j]);
					removed[i][idx++] = S[j];
					if (SC->S[i]->numOfElements == 0)
						bitset_remove(SC->contains, i);
				}
		}
	}
	ResultSetCover* C1 = minimum_set_cover(SC, U);
	if (C1 != NULL)
		resultsetcover_add(C1, max_i);

	// Add removed elements back to corresponding sets
	for (int i = 0; i < SC->n; i++) {
		if (removed[i][0] < 0)
			continue;
		if (!bitset_contains(SC->contains, i))
			bitset_add(SC->contains, i);
		for (int j = 0; j < nS && removed[i][j] >= 0; j++)
			bitset_add(SC->S[i], removed[i][j]);
	}
	for (int i = 0; i < nS && removed_contains[i] >= 0; i++)
		bitset_add(U, removed_contains[i]);
	if (removed_max_i)
		bitset_add(SC->contains, max_i);

	// Free memory
	free(S);
	for (int i = 0; i < SC->n; i++)
		free(removed[i]);
	free(removed);
	free(removed_contains);

	// Return smaller set cover out of C1 and C2, or NULL if both are NULL
	if (C1 == NULL && C2 == NULL)
		return NULL;
	else if (C1 == NULL)
		return C2;
	else if (C2 == NULL)
		return C1;
	else if (C1->length < C2->length) {
		resultsetcover_free(C2);
		return C1;
	}
	else {
		resultsetcover_free(C1);
		return C2;
	}
}

Result* solve_using_setcover(Graph* G, int k, Options* options) {
	int ilow, ihigh, imid;
	double* dists = get_sorted_distances_no_duplicates(G, &ihigh, options);

	ResultSetCover* resOpt;
	ResultSetCover* resTemp;
	BitSet* U = bitset_new_full(G->n);
	if (((resOpt = minimum_set_cover(center_closing_to_setcover(G, options, dists[0]), U)) == NULL) || (resOpt->length > (G->m - k))) {
		ihigh--;
		if (((resOpt = minimum_set_cover(center_closing_to_setcover(G, options, dists[ihigh]), U)) == NULL) || (resOpt->length > (G->m - k))) {
			printf("ERROR - backtracking_decision_to_optimization failed: no solution found\n"); // This should never happen
			return NULL;
		}
		ilow = 0;
		while (ihigh - ilow > 1) {
			imid = (ihigh + ilow) / 2;
			if (((resTemp = minimum_set_cover(center_closing_to_setcover(G, options, dists[imid]), U)) == NULL) || (resTemp->length > (G->m - k))) {
				ilow = imid;
				if (resTemp != NULL)
					resultsetcover_free(resTemp);
			}
			else {
				resultsetcover_free(resOpt);
				resOpt = resTemp;
				ihigh = imid;
			}
		}
	}

	// Create Result*
	Result* res = result_new();
	res->score = dists[ihigh];
	BitSet* removed_centers = bitset_new_full(G->m);
	for (int i = 0; i < resOpt->length; i++)
		bitset_add(removed_centers, resOpt->C[i]);
	result_update(res, dists[ihigh], removed_centers, G->S);
	bitset_free(removed_centers);
	resultsetcover_free(resOpt);
	free(dists);
	return res;
}