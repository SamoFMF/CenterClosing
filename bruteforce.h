#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

#include <stdbool.h>
#include "graph.h"
#include "bitset.h"
#include "algutils.h"

void increase_index(int* idxs, int n, int k);

double eval_score(Graph* G, BitSet* R);

Result bruteforce(Graph* G, int k);




#endif