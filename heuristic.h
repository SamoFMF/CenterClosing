#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "graph.h"
#include "algutils.h"

Result* cluster_cardinality(Graph* G, int k, Options* options);

Result* cluster_cardinality_basic(Graph* G, int k, Options* options);

Result* cluster_radius(Graph* G, int k, Options* options);

Result* cluster_radius_basic(Graph* G, int k, Options* options);

#endif