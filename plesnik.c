#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "algutils.h"
#include "bitset.h"
#include "graph.h"
#include "plesnik.h"
#include "utils.h"

int compare_weights(void* cont, const void* x, const void* y) {
	double* H = cont;
	double f = H[*((int*)x)];
	double s = H[*((int*)y)];
	if (f > s) return -1;
	if (f < s) return 1;
	return 0;
}

// Requires G->G
BitSet* independant_set_old(Graph* G, int k, double B, Options* options, int* ics) {
	BitSet* R = bitset_new_full(G->m);

	//int* ics;
	//if ((ics = malloc(G->n * sizeof * ics)) == NULL)
	//	printf("ERROR - Ran out of memory: range_cur - ics\n");
	//for (int i = 0; i < G->n; i++)
	//	ics[i] = i;
	//qsort_s(ics, G->n, sizeof * ics, compare_weights, G->H);
	BitSet* covered = bitset_new(G->n);

	int c, s, c1, s1, c2, snode, cnode, snode1;
	for (int i = 0; i < G->n; i++) {
		c = ics[i];
		if (!bitset_contains(covered, c)) {
			s = G->G[G->C[c]][0];
			if (options->eval(c, s, G) > B) {
				bitset_free(R);
				bitset_free(covered);
				//free(ics);
				return NULL;
			}
			else {
				bitset_remove(R, s);
				snode = G->S[s];
				for (int i1 = 0; i1 < G->n && options->eval(G->G[snode][i1], s, G) <= B; i1++) {
					c1 = G->G[snode][i1];
					cnode = G->C[c1];
					for (int i2 = 0; i2 < G->m && options->eval(c1, G->G[cnode][i2], G) <= B; i2++) {
						s1 = G->G[cnode][i2];
						if (s1 == s)
							continue;
						snode1 = G->S[s1];
						for (int i3 = 0; i3 < G->n && options->eval(G->G[snode1][i3], s1, G) <= B; i3++) {
							c2 = G->G[snode1][i3];
							bitset_add(covered, c2);
						}
					}
				}
				/*for (int ic1 = 0; ic1 < G->n; ic1++) {
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
				}*/
			}
		}
	}

	bitset_free(covered);
	//free(ics);

	if (R->numOfElements >= k) {
		return R;
	}
	else {
		if (R != NULL)
			bitset_free(R);
		return NULL;
	}
}

BitSet* range_rand(Graph* G, int k, double B, Options* options, int* ics) {
	BitSet* R = bitset_new_full(G->m);

	//int* ics;
	//if ((ics = malloc(G->n * sizeof * ics)) == NULL)
	//	printf("ERROR - Ran out of memory: range_cur - ics\n");
	//for (int i = 0; i < G->n; i++)
	//	ics[i] = i;
	//qsort_s(ics, G->n, sizeof * ics, compare_weights, G->H);
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
				//free(ics);
				return NULL;
			}
			else {
				bitset_remove(R, s);
				for (int ic = 0; ic < G->n; ic++)
					if (options->eval(ic, s, G) <= 3 * B)
						bitset_add(covered, ic);
			}
		}
	}

	bitset_free(covered);
	//free(ics);

	if (R->numOfElements >= k) {
		return R;
	}
	else {
		if (R != NULL)
			bitset_free(R);
		return NULL;
	}
}

BitSet* range_closest(Graph* G, int k, double B, Options* options, int* ics) {
	BitSet* R = bitset_new_full(G->m);
	BitSet* covered = bitset_new(G->n);

	int c, s;
	double closest, dist;
	for (int i = 0; i < G->n; i++) {
		c = ics[i];
		if (!bitset_contains(covered, c)) {
			s = -1;
			closest = B;
			for (int is = 0; is < G->m; is++) {
				if (bitset_contains(R, is)) {
					dist = options->eval(c, is, G);
					if (dist <= closest)
						s = is;
				}
			}
			if (s < 0) {
				bitset_free(R);
				bitset_free(covered);
				return NULL;
			}
			else {
				bitset_remove(R, s);
				for (int ic = 0; ic < G->n; ic++)
					if (options->eval(ic, s, G) <= 3 * B)
						bitset_add(covered, ic);
			}
		}
	}

	bitset_free(covered);

	if (R->numOfElements >= k) {
		return R;
	}
	else {
		if (R != NULL)
			bitset_free(R);
		return NULL;
	}
}

BitSet* range_first(Graph* G, int k, double B, Options* options, int* ics) {
	BitSet* R = bitset_new_full(G->m);

	//int* ics;
	//if ((ics = malloc(G->n * sizeof * ics)) == NULL)
	//	printf("ERROR - Ran out of memory: range_cur - ics\n");
	//for (int i = 0; i < G->n; i++)
	//	ics[i] = i;
	//qsort_s(ics, G->n, sizeof * ics, compare_weights, G->H);
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
				//free(ics);
				return NULL;
			}
			else {
				bitset_remove(R, s);
				for (int ic = 0; ic < G->n; ic++)
					if (options->eval(ic, s, G) <= 3 * B)
						bitset_add(covered, ic);
			}
		}
	}

	bitset_free(covered);
	//free(ics);

	if (R->numOfElements >= k) {
		return R;
	}
	else {
		if (R != NULL)
			bitset_free(R);
		return NULL;
	}
}

void result_update_plesnik(Result* result, BitSet* R, Graph* G, int k, Options* options) { // Version of result_update where extra slots are filled (ie. if algorithm removed too many centers, they are added back)
	if (result->R != NULL)
		free(result->R);
	else if ((result->R = malloc(k * sizeof * result->R)) != NULL) {
		//result->score = val;
		int i = 0;
		int j;
		for (j = 0; j < R->n && i < k; j++)
			if (bitset_contains(R, j))
				result->R[i++] = G->S[j];
		if (R->numOfElements > k) {
			for (; j < R->n; j++)
				bitset_remove(R, j);
		}
		result->score = eval_score(G, R, options);
	}
	else {
		printf("ERROR - Ran out of memory: result_update");
	}
}

Result* plesnik(Graph* G, int k, Options* options, BitSet* (*decision_solver)(Graph* G, int k, double B, Options* options, int* ics)) {
	int* ics;
	if ((ics = malloc(G->n * sizeof * ics)) == NULL)
		printf("ERROR - Ran out of memory: range_cur - ics\n");
	for (int i = 0; i < G->n; i++)
		ics[i] = i;
	qsort_s(ics, G->n, sizeof * ics, compare_weights, G->H);

	int ilow, ihigh, imid;
	double* dists = get_sorted_distances_no_duplicates(G, &ihigh, options);

	BitSet* R = NULL;
	BitSet* Ropt;
	if ((Ropt = decision_solver(G, k, dists[0], options, ics)) == NULL) {
		ilow = 0;
		ihigh--;
		if ((Ropt = decision_solver(G, k, dists[ihigh], options, ics)) == NULL) {
			printf("ERROR - decision_to_optimization failed: no solution found\n"); // This should never happen
			return NULL;
		}

		while (ihigh - ilow > 1) {
			if (R != NULL)
				bitset_free(R);
			imid = (ihigh + ilow) / 2;
			if ((R = decision_solver(G, k, dists[imid], options, ics)) == NULL) {
				ilow = imid;
			}
			else {
				ihigh = imid;
				bitset_soft_copy(Ropt, R);
			}
		}
	}
	Result* res = result_new();
	//result_update(res, eval_score(G, Ropt, options), Ropt, G->S);
	result_update_plesnik(res, Ropt, G, k, options);

	if (R != NULL)
		bitset_free(R);
	bitset_free(Ropt);
	free(dists);
	free(ics);

	return res;
}

//Result* plesnik_unlimited(Graph* G, int k, Options* options, BitSet* (*decision_solver)(Graph* G, int k, double B, Options* options, int* ics), double tol) {
//	int* ics;
//	if ((ics = malloc(G->n * sizeof * ics)) == NULL)
//		printf("ERROR - Ran out of memory: range_cur - ics\n");
//	for (int i = 0; i < G->n; i++)
//		ics[i] = i;
//	qsort_s(ics, G->n, sizeof * ics, compare_weights, G->H);
//
//	//int ilow, ihigh, imid;
//	//double* dists = get_sorted_distances_no_duplicates(G, &ihigh, options);
//	double dlow, dhigh, dmid, d;
//	dlow = DBL_MAX;
//	dhigh = -1;
//	for (int c = 0; c < G->n; c++) {
//		for (int s = 0; s < G->m; s++) {
//			d = options->eval(c, s, G);
//			if (d < dlow)
//				dlow = d;
//			else if (d > dhigh)
//				dhigh = d;
//		}
//	}
//
//	BitSet* R = NULL;
//	BitSet* Ropt;
//	if ((Ropt = decision_solver(G, k, dlow, options, ics)) == NULL) {
//		if ((Ropt = decision_solver(G, k, dhigh, options, ics)) == NULL) {
//			printf("ERROR - decision_to_optimization failed: no solution found\n"); // This should never happen
//			return NULL;
//		}
//
//		while (dhigh - dlow > tol) {
//			if (R != NULL)
//				bitset_free(R);
//			dmid = (dhigh + dlow) / 2;
//			if ((R = decision_solver(G, k, dmid, options, ics)) == NULL) {
//				dlow = dmid;
//			}
//			else {
//				dhigh = dmid;
//				bitset_soft_copy(Ropt, R);
//			}
//		}
//	}
//
//	Result* res = result_new();
//	result_update(res, eval_score(G, Ropt, options), Ropt, G->S);
//
//	if (R != NULL)
//		bitset_free(R);
//	bitset_free(Ropt);
//	free(ics);
//
//	return res;
//}

Result* plesnik_unlimited(Graph* G, int k, Options* options, BitSet* (*decision_solver)(Graph* G, int k, double B, Options* options, int* ics)) {
	int* ics;
	if ((ics = malloc(G->n * sizeof * ics)) == NULL)
		printf("ERROR - Ran out of memory: range_cur - ics\n");
	for (int i = 0; i < G->n; i++)
		ics[i] = i;
	qsort_s(ics, G->n, sizeof * ics, compare_weights, G->H);

	int ilow, ihigh, imid;
	double* dists2 = get_sorted_distances_no_duplicates(G, &ihigh, options);
	double* dists = sorted_distances_no_duplicates_add_thirds(dists2, ihigh, &ihigh);
	free(dists2);

	BitSet* R = NULL;
	BitSet* Ropt;
	if ((Ropt = decision_solver(G, k, dists[0], options, ics)) == NULL) {
		ilow = 0;
		ihigh--;
		if ((Ropt = decision_solver(G, k, dists[ihigh], options, ics)) == NULL) {
			printf("ERROR - decision_to_optimization failed: no solution found\n"); // This should never happen
			return NULL;
		}

		while (ihigh - ilow > 1) {
			if (R != NULL)
				bitset_free(R);
			imid = (ihigh + ilow) / 2;
			if ((R = decision_solver(G, k, dists[imid], options, ics)) == NULL) {
				ilow = imid;
			}
			else {
				ihigh = imid;
				bitset_soft_copy(Ropt, R);
			}
		}
	}
	Result* res = result_new();
	//result_update(res, eval_score(G, Ropt, options), Ropt, G->S);
	result_update_plesnik(res, Ropt, G, k, options);

	if (R != NULL)
		bitset_free(R);
	bitset_free(Ropt);
	free(dists);
	free(ics);

	return res;
}