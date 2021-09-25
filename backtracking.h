#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#include "graph.h"
#include "algutils.h"

Result* backtracking(Graph* G, int k, Options* options);

Result* backtracking_decision_to_optimization(Graph* G, int k, Options* options);

#endif