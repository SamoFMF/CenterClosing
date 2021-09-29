#ifndef PLESNIK_H
#define PLESNIK_H

#include "algutils.h"
#include "bitset.h"
#include "graph.h"

BitSet* range_first(Graph* G, int k, double B, Options* options, int* ics);

BitSet* range_rand(Graph* G, int k, double B, Options* options, int* ics);

BitSet* range_closest(Graph* G, int k, double B, Options* options, int* ics);

Result* plesnik(Graph* G, int k, Options* options, BitSet* (*decision_solver)(Graph* G, int k, double B, Options* options, int* ics));

Result* plesnik_unlimited(Graph* G, int k, Options* options, BitSet* (*decision_solver)(Graph* G, int k, double B, Options* options, int* ics), double tol);

#endif