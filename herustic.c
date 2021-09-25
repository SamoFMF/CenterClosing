#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "algutils.h"
#include "bitset.h"
#include "graph.h"
#include "heuristic.h"

Result* cluster_cardinality(Graph* G, int k, Options* options) {
	double val;
	Center** centers = centers_new_from_graph(G, options, &val);
	BitSet* R = bitset_new(G->m);

	double dist, dcenter;
	int cardinality, s;
	for (int ki = 0; ki < k; ki++) {
		// Find center with lowest cluster cardinality
		cardinality = INT_MAX;
		dcenter = DBL_MAX;
		for (int is = 0; is < G->m; is++) {
			if (!bitset_contains(R, is))
				if (centers[is]->numOfNodes < cardinality || (centers[is]->numOfNodes == cardinality && centers[is]->history->val < dcenter)) {
					dcenter = centers[is]->history->val;
					cardinality = centers[is]->numOfNodes;
					s = is;
				}
		}
		bitset_add(R, s);
		dist = centers_redistribute_oneway(centers, G, R, s, options);
		if (dist > val)
			val = dist;
	}

	Result* res = result_new();
	result_update(res, val, R, G->S);

	bitset_free(R);
	centers_free(centers, G->m);

	return res;
}

Result* cluster_radius(Graph* G, int k, Options* options) {
	double val;
	Center** centers = centers_new_from_graph(G, options, &val);
	BitSet* R = bitset_new(G->m);

	double dist, radius;
	int dcenter, s;
	for (int ki = 0; ki < k; ki++) {
		// Find center with lowest cluster radius
		radius = DBL_MAX;
		dcenter = INT_MAX;
		for (int is = 0; is < G->m; is++)
			if (!bitset_contains(R, is))
				if (centers[is]->history->val < radius || (centers[is]->history->val == radius && centers[is]->numOfNodes < dcenter)) {
					dcenter = centers[is]->history->val;
					radius = centers[is]->numOfNodes;
					s = is;
				}
		bitset_add(R, s);
		dist = centers_redistribute_oneway(centers, G, R, s, options);
		if (dist > val)
			val = dist;
	}

	Result* res = result_new();
	result_update(res, val, R, G->S);

	bitset_free(R);
	centers_free(centers, G->m);

	return res;
}