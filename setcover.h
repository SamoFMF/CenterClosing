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

Result* solve_using_setcover_slow(Graph* G, int k, Options* options);

Result* solve_using_setcover(Graph* G, int k, Options* options);

#endif