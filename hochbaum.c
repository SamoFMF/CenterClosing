#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "algutils.h"
#include "bitset.h"
#include "graph.h"
#include "hochbaum.h"
#include "utils.h"

int hochbaum_start_random(Graph* G, Options* options) {
	// Get random consumer
	int c = rand_lim(G->n);

	// Return closest supplier to consumer c
	double dmin = DBL_MAX;
	double dist;
	int s;
	for (int is = 0; is < G->m; is++) {
		dist = options->eval(c, is, G);
		if (dist < dmin) {
			dmin = dist;
			s = is;
		}
	}
	return s;
}

int hochbaum_start_best(Graph* G, Options* options) {
	int sbest = 0;
	double val;
	double vmin = DBL_MAX;
	double dist, dmax;
	for (int s = 0; s < G->m; s++) {
		dmax = -1;
		for (int is = 0; is < G->m; is++) {
			if (is != s)
				for (int ic = 0; ic < G->n; ic++) {
					dist = options->eval(ic, is, G);
					if (dist > dmax)
						dmax = dist;
				}
		}
		if (dmax < vmin) {
			vmin = dmax;
			sbest = s;
		}
	}

	return sbest;
}

/** @brief 3-approximation algorithm based on Hochbaum
*
* Hochbaum's implementation of closing centers problem.
* Works in reverse order, adding centers instead of removing them.
*
* @param G Pointer to Graph struct.
* @param k Number of centers to be removed.
* @param options Algorithm options.
* @param get_first_center Function used to calculate the first center to be covered.
*
* @return Result Approximation of the problem solution.
*/
Result* hochbaum(Graph* G, int k, Options* options) {
	int c;
	double dist, dmin, dmax;

	int s = options->get_first(G, options);

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
			dmin = DBL_MAX;
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
		dmin = DBL_MAX;
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
		dmin = DBL_MAX;
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

int hochbaum_start_plus(Graph* G, Options* options) {
	return options->hochbaum_start_center++;
}

Result* hochbaum_plus(Graph* G, int k, Options* options) {
	int (*prev_fun)(Graph * G, Options * options) = options->get_first;
	options->hochbaum_start_center = 0;
	options->get_first = hochbaum_start_plus;
	Result* res_opt = hochbaum(G, k, options);
	Result* res = NULL;
	for (int i = 1; i < G->m; i++) {
		res = hochbaum(G, k, options);
		if (res->score < res_opt->score) {
			result_free(res_opt);
			res_opt = res;
		}
		else {
			result_free(res);
			res = NULL;
		}
	}
	if (res != NULL)
		result_free(res);
	options->get_first = prev_fun;
	return res_opt;
}