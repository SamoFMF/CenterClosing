#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "algutils.h"
#include "bitset.h"
#include "greedy.h"

Result* greedy_basic(Graph* G, int k, Options* options) {
	BitSet* R = bitset_new(G->m);

	double dist;
	double dmin = DBL_MAX;
	int imin;
	for (int ki = 0; ki < k; ki++) {
		printf("ki=%d\n", ki);
		dmin = DBL_MAX;
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


double exact_k1(Graph* G, BitSet* R, Center** centers, Options* options, double val_start) {
	// closest[i] ... 2nd closest center of consumer i
	int* closest;
	if ((closest = malloc(G->n * sizeof * closest)) == NULL)
		printf("ERROR: Ran out of memory - exact_centers_k1 - closest");
	
	Center* center;
	double dist, dsup, dcon;
	int sidx;
	double dopt = DBL_MAX;
	int iopt;
	for (int is = 0; is < G->m; is++)
		if (!bitset_contains(R, is)) {
			center = centers[is];
			dsup = val_start;
			//dsup = 0; // TODO - premisli, ce je ta vrstica zgoraj smiselna?? Ce ni, odstrani val_start iz parametra itd. Naceloma bi morala kvaliteta resitve biti ista, ampak mogoce je z zgornjo kancek hitreje? Testiraj! (mogoce zgornja malo hitrejsa, ampak mogoce vraca slabe priblizke tekom celotnega greedy algoritma)
			for (int i = 0; i < center->numOfNodes; i++) {
				dcon = DBL_MAX;
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
	return dopt > val_start? dopt: val_start;
}

Result* greedy_centers(Graph* G, int k, Options* options) {
	double val;
	Center** centers = centers_new_from_graph(G, options, &val);
	BitSet* R = bitset_new(G->m);

	for (int ki = 0; ki < k; ki++) {
		val = exact_k1(G, R, centers, options, val);
	}

	Result* res = result_new();
	result_update(res, val, R, G->S);

	centers_free(centers, G->m);
	bitset_free(R);


	return res;
}