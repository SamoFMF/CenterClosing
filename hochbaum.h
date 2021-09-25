#ifndef HOCHBAUM_H
#define HOCHBAUM_H

#include "graph.h"
#include "algutils.h"

int hochbaum_start_random(Graph* G, Options* options);

int hochbaum_start_best(Graph* G, Options* options);

Result* hochbaum(Graph* G, int k, Options* options);

Result* hochbaum_plus(Graph* G, int k, Options* options);

#endif