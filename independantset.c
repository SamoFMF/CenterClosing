#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "algutils.h"
#include "bitset.h"
#include "independantset.h"

// Requires G->G
BitSet* independant_set_decision(Graph* G, int k, double B, Options* options) {
	BitSet* R = bitset_new_full(G->m);
	BitSet* covered = bitset_new(G->n);

	double asd;

	int s, c1, s1, c2, snode, cnode, snode1;
	BitSet* visited_C = bitset_new(G->n);
	BitSet* visited_S = bitset_new(G->m);
	for (int c = 0; c < G->n; c++) {
		if (!bitset_contains(covered, c)) {
			s = G->G[G->C[c]][0];
			printf("\ts=%d\n", s);
			if (options->eval(c, s, G) > B) {
				bitset_free(R);
				bitset_free(covered);
				return NULL;
			}
			else {
				bitset_remove(R, s);
				snode = G->S[s];
				for (int i1 = 0; i1 < G->n && options->eval(G->G[snode][i1], s, G) <= B; i1++) {
					c1 = G->G[snode][i1];
					if (bitset_contains(visited_C, c1))
						continue;
					bitset_add(visited_C, c1);
					cnode = G->C[c1];
					for (int i2 = 0; i2 < G->m && options->eval(c1, G->G[cnode][i2], G) <= B; i2++) {
						s1 = G->G[cnode][i2];
						/*if (s1 == s || bitset_contains(visited_S, s1))*/
						if (bitset_contains(visited_S, s1))
							continue;
						bitset_add(visited_S, s1);
						snode1 = G->S[s1];
						for (int i3 = 0; i3 < G->n && options->eval(G->G[snode1][i3], s1, G) <= B; i3++) {
							c2 = G->G[snode1][i3];
							if (c2 == 6) {
								asd = options->eval(c2, s1, G);
								asd = options->eval(c1, s1, G);
								asd = options->eval(c1, s, G);
								asd = 0.0;
							}
							bitset_add(covered, c2);
						}
					}
				}
			}
		}
	}

	bitset_free(covered);
	bitset_free(visited_C);
	bitset_free(visited_S);

	if (R->numOfElements >= k) {
		return R;
	}
	else {
		if (R != NULL)
			bitset_free(R);
		return NULL;
	}
}

Result* independant_set(Graph* G, int k, Options* options) {
	int ilow, ihigh, imid;

	double* dists = get_sorted_distances_no_duplicates(G, &ihigh, options);

	BitSet* R = NULL;
	BitSet* Ropt;
	if ((Ropt = independant_set_decision(G, k, dists[0], options)) == NULL) {
		ilow = 0;
		ihigh--;
		if ((Ropt = independant_set_decision(G, k, dists[ihigh], options)) == NULL) {
			printf("ERROR - decision_to_optimization failed: no solution found\n"); // This should never happen
			return NULL;
		}

		while (ihigh - ilow > 1) {
			if (R != NULL)
				bitset_free(R);
			imid = (ihigh + ilow) / 2;
			if ((R = independant_set_decision(G, k, dists[imid], options)) == NULL) {
				ilow = imid;
			}
			else {
				ihigh = imid;
				printf("imid = %u && R->set[0] = %u\n", imid, R->set[0]);
				bitset_soft_copy(Ropt, R);
			}
		}
	}
	Result* res = result_new();
	printf("Ropt->set[0] = %u\n", Ropt->set[0]);
	result_update(res, eval_score(G, Ropt, options), Ropt, G->S);

	if (R != NULL)
		bitset_free(R);
	bitset_free(Ropt);
	free(dists);

	return res;
}