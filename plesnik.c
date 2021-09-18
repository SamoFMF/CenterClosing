#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "algutils.h"
#include "utils.h"

int compare(void* cont, const void* x, const void* y) {
	double* cont1 = cont;
	if (cont1 == NULL) printf("NULL!");
	double f = cont1[*((int*)x)];
	double s = cont1[*((int*)y)];
	if (f > s) return 1;
	if (f < s) return -1;
	return 0;
}

int compare_weights(void* cont, const void* x, const void* y) {
	double* H = cont;
	double f = H[*((int*)x)];
	double s = H[*((int*)y)];
	if (f > s) return -1;
	if (f < s) return 1;
	return 0;
}

// Requires G->G
BitSet* range_adj(Graph* G, int k, double B, Options* options) {
	BitSet* R = bitset_new_full(G->m);

	int* ics;
	if ((ics = malloc(G->n * sizeof * ics)) == NULL)
		printf("ERROR - Ran out of memory: range_cur - ics\n");
	for (int i = 0; i < G->n; i++)
		ics[i] = i;
	qsort_s(ics, G->n, sizeof * ics, compare_weights, G->H);
	BitSet* covered = bitset_new(G->n);

	int c, s, c1, s1, c2;
	for (int i = 0; i < G->n; i++) {
		c = ics[i];
		if (!bitset_contains(covered, c)) {
			s = G->G[G->C[c]][0];
			if (options->eval(c, s, G) > B) {
				bitset_free(R);
				bitset_free(covered);
				free(ics);
				return NULL;
			}
			else {
				bitset_remove(R, s);
				for (int ic1 = 0; ic1 < G->n; ic1++) {
					c1 = G->G[G->S[s]][ic1];
					if (options->eval(c1, s, G) <= B) {
						for (int is1 = 0; is1 < G->m; is1++) {
							s1 = G->G[G->C[c1]][is1];
							if (options->eval(c1, s1, G) <= B) {
								for (int ic2 = 0; ic2 < G->n; ic2++) {
									c2 = G->G[G->S[s1]][ic2];
									if (options->eval(c2, s1, G) <= B) {
										bitset_add(covered, c2);
									}
									else
										break;
								}
							}
							else
								break;
						}
					}
					else
						break;
				}
			}
		}
	}

	bitset_free(covered);
	free(ics);

	if (R->numOfElements >= k) {
		return R;
	}
	else {
		return NULL;
	}
}

BitSet* range_rand(Graph* G, int k, double B, Options* options) {
	BitSet* R = bitset_new_full(G->m);

	int* ics;
	if ((ics = malloc(G->n * sizeof * ics)) == NULL)
		printf("ERROR - Ran out of memory: range_cur - ics\n");
	for (int i = 0; i < G->n; i++)
		ics[i] = i;
	qsort_s(ics, G->n, sizeof * ics, compare_weights, G->H);
	BitSet* covered = bitset_new(G->n);

	int c, s, nums;
	for (int i = 0; i < G->n; i++) {
		c = ics[i];
		if (!bitset_contains(covered, c)) {
			nums = 0;
			for (int is = 0; is < G->m; is++) {
				if (bitset_contains(R, is) && options->eval(c, is, G) <= B && rand_lim(nums + 1) == 0) {
					s = is;
					nums++;
				}
			}
			if (nums == 0) {
				bitset_free(R);
				bitset_free(covered);
				free(ics);
				return NULL;
			}
			else {
				bitset_remove(R, s);
				for (int ic = 0; ic < G->m; ic++)
					if (options->eval(ic, s, G) <= 3 * B)
						bitset_add(covered, ic);
			}
		}
	}

	bitset_free(covered);
	free(ics);

	if (R->numOfElements >= k) {
		return R;
	}
	else {
		return NULL;
	}
}

BitSet* range_first(Graph* G, int k, double B, Options* options) {
	BitSet* R = bitset_new_full(G->m);

	int* ics;
	if ((ics = malloc(G->n * sizeof * ics)) == NULL)
		printf("ERROR - Ran out of memory: range_cur - ics\n");
	for (int i = 0; i < G->n; i++)
		ics[i] = i;
	qsort_s(ics, G->n, sizeof * ics, compare_weights, G->H);
	BitSet* covered = bitset_new(G->n);

	int c, s;
	for (int i = 0; i < G->n; i++) {
		c = ics[i];
		if (!bitset_contains(covered, c)) {
			s = -1;
			for (int is = 0; is < G->m; is++) {
				if (bitset_contains(R, is) && options->eval(c, is, G) <= B) {
					s = is;
					break;
				}
			}
			if (s < 0) {
				bitset_free(R);
				bitset_free(covered);
				free(ics);
				return NULL;
			}
			else {
				bitset_remove(R, s);
				for (int ic = 0; ic < G->m; ic++)
					if (options->eval(ic, s, G) <= 3 * B)
						bitset_add(covered, ic);
			}
		}
	}

	bitset_free(covered);
	free(ics);

	if (R->numOfElements >= k) {
		return R;
	}
	else {
		return NULL;
	}
}

Result* decision_to_optimization(Graph* G, int k, Options* options, BitSet* (*decision_solver)(Graph* G, int k, double B, Options* options)) {
	int ilow, ihigh, imid;
	double* dists = get_sorted_distances_no_duplicates(G, &ihigh, options);
	
	BitSet* R;
	if ((R = decision_solver(G, k, dists[0], options)) == NULL) {
		ilow = 0;
		ihigh--;
		if ((R = decision_solver(G, k, dists[ihigh], options)) == NULL) {
			printf("ERROR - decision_to_optimization failed: no solution found\n"); // This should never happen
			return NULL;
		}
		while (ihigh - ilow > 1) {
			if (R != NULL)
				bitset_free(R);
			imid = (ihigh + ilow) / 2;
			if ((R = decision_solver(G, k, dists[imid], options)) == NULL) {
				ilow = imid;
			}
			else {
				ihigh = imid;
			}
		}
	}
	Result* res = result_new();
	result_update(res, eval_score(G, R, options), R, G->S);
	return res;
}

//Result range_new(Graph* G, int k, double B) {
//	int k1 = G->m - k;
//	Result res;
//
//	BitSet* covered = bitset_new(G->n);
//	int* R = bitset_new_full(G->N);
//
//	// Sort nodes by h-score
//	int* idxs;
//	if ((idxs = malloc(G->n * sizeof * idxs)) == NULL) {
//		printf("ERROR - Ran out of memory: range - idxs");
//	}
//	else {
//		for (int i = 0; i < G->n; i++) idxs[i] = i;
//	}
//	qsort_s(idxs, G->n, sizeof(int), compare, G->H);
//
//	int node, center, cstar, cstar_idx, i, j;
//	double dist;
//	for (int idx = 0; idx < G->n; idx++) {
//		i = idxs[idx];
//		if (bitset_contains(covered, i)) continue;
//		node = G->C[i];
//
//		// Check ifi there exists a center with range at most B
//		cstar = -1;
//		cstar_idx = -1;
//		for (int j = 0; j < G->m; j++) {
//			center = G->C[j];
//			dist = f(G, i, j);
//			if (dist <= B) {
//				cstar = center;
//				cstar_idx = j;
//				break;
//			}
//		}
//
//		// Check if center at range at most B was found
//		if (cstar < 0) {
//			// TODO - free things
//			res.score = 0;
//			res.R = NULL;
//			return res;
//		}
//		else {
//			// Found center `cstar` => cover all nodes within 3*B distance
//			bitset_add(covered, i);
//			bitset_remove(R, cstar);
//			for (int jdx = idx + 1; jdx < G->n; jdx++) {
//				j = idxs[jdx];
//				node = G->C[j];
//				dist = f(G, i, cstar_idx);
//				if (dist <= 3 * B) bitset_add(covered, j);
//			}
//		}
//	}
//
//	// TODO
//}
//
//Result range(Graph* G, int k, double B) {
//	int k1 = G->m - k;
//	Result res;
//
//	// Covered nodes
//	BitSet* covered = bitset_new(G->n);
//
//	// Chosen centers
//	int* C;
//	if ((C = malloc(k1 * sizeof * C)) == NULL) {
//		printf("ERROR - Ran out of memory: range - C");
//	}
//	int C_len = 0;
//
//	int node, center, cstar;
//	double dist;
//	for (int i = 0; i < G->n; i++) {
//		if (bitset_contains(covered, i)) continue;
//		node = G->C[i];
//
//		// Check if there exists a center with range at most B
//		cstar = -1;
//		for (int j = 0; j < G->m; j++) {
//			center = G->C[j];
//			dist = G->D[node][center];
//			if (dist <= B) {
//				cstar = center;
//				break;
//			}
//		}
//
//		// Check if we found center with distance at most B
//		if (cstar < 0 || C_len == k1) {
//			// No such center exists ... return False
//			// OR We found a new center to be added, but we've already added the maximum number of centers ... return False
//			res.score = 0;
//			res.R = NULL;
//			return res;
//		}
//		else {
//			// Found center `cstar` ... cover all nodes within 3*B distance
//			bitset_add(covered, i);
//			for (int j = i + 1; j < G->n; j++) {
//				node = G->C[j];
//				dist = G->D[node][cstar];
//				if (dist <= 3 * B) bitset_add(covered, j);
//			}
//
//			// Add `cstar` to centers `C`
//			C[C_len++] = cstar;
//		}
//	}
//
//	// Delete `covered` from memory
//	free(covered);
//
//	// Solution found
//	res.score = 1;
//
//	// Add removed nodes
//	BitSet* R = bitset_new(G->N);
//	bitset_add_from(R, G->C, G->m);
//	bitset_remove_from(R, C, C_len);
//	save_removed_nodes(&res, R);
//
//	return res;
//}


//Result bisection(Graph* G, int k, double bmin, double bmax, Result* res, double tol) {
//	if (bmax - bmin < tol) {
//		res->score = 3 * bmax;
//		return *res;
//	}
//	else {
//		Result res_tmp;
//		double bmid = (bmin + bmax) / 2;
//		res_tmp = range(G, k, bmid);
//		if (res_tmp.score == 0) {
//			return bisection(G, k, bmid, bmax, res, tol);
//		}
//		else {
//			result_free(res);
//			return bisection(G, k, bmin, bmid, &res_tmp, tol);
//		}
//	}
//}
//
//Result plesnik(Graph* G, int k, double tol) {
//	double bmin, bmax;
//
//	// Get bmin
//	double dmin, dist;
//	bmin = -1;
//	for (int i = 0; i < G->n; i++) {
//		dmin = INT_MAX;
//		for (int j = 0; j < G->m; j++) {
//			dist = G->D[G->C[i]][G->S[j]];
//			if (dist < dmin) dmin = dist;
//		}
//		if (dmin > bmin) bmin = dmin;
//	}
//
//	// Update bmin and get bmax
//	Result res;
//	bmax = bmin;
//	for (res = range(G, k, bmax); res.score == 0; bmax *= 2);
//
//	return bisection(G, k, bmin, bmax, &res, tol);
//}