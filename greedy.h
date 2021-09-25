#ifndef GREEDY_H
#define GREEDY_H

#include "graph.h"
#include "algutils.h"

Result* greedy_basic(Graph* G, int k, Options* options);

Result* greedy_centers(Graph* G, int k, Options* options);


#endif