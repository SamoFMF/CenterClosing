#ifndef PLESNIK_H
#define PLESNIK_H

#include "graph.h"
#include "algutils.h"

BitSet* range_adj(Graph* G, int k, double B, Options* options);

BitSet* range_first(Graph* G, int k, double B, Options* options);

BitSet* range_rand(Graph* G, int k, double B, Options* options);

Result plesnik(Graph* G, int k, double tol);

Result* decision_to_optimization(Graph* G, int k, Options* options, BitSet* (*decision_solver)(Graph* G, int k, double B, Options* options));

#endif