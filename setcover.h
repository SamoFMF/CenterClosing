#ifndef SETCOVER_H
#define SETCOVER_H

#include "bitset.h"
#include "algutils.h"
#include "graph.h"

typedef struct SetCover {
	BitSet** S;
	BitSet* contains;
	int n; // length of S
} SetCover;

typedef struct ResultSetCover {
	int* C;
	int length;
} ResultSetCover;

SetCover* graph_to_setcover(Graph* G, Options* options, double B);

SetCover* center_closing_to_setcover(Graph* G, Options* options, double B);

ResultSetCover* resultsetcover_new();

ResultSetCover* minimum_set_cover(SetCover* G, BitSet* U);

ResultSetCover* minimum_set_cover_upgraded(SetCover* SC, BitSet* U);

Result* solve_using_setcover(Graph* G, int k, Options* options);

#endif