#ifndef ALGUTILS_H
#define ALGUTILS_H

#include "graph.h"
#include "bitset.h"
#include "utils.h"

// Result related
typedef struct Result {
	double score;
	int* R;
} Result;

void result_free(Result* result);

double eval_score(Graph* G, BitSet* R);

void save_removed_nodes(Result* res, BitSet* R);

// Center related
typedef struct Center {
	int* nodes; // Nodes for which Center is the closest center
	int numOfNodes; // Number of elements in `nodes`
	int delta; // Change in numOfNodes required
	LinkedList* history; // Centers score (max dist to node in `nodes`)
} Center;

Center* center_new(int n);

void center_free(Center* center);

void centers_free(Center** centers, int n);

void center_resize(Center* center);

void center_add(Center* center, int node);

Center** centers_new_from_graph(Graph* G);

double centers_redistribute(Center** centers, Graph* G, BitSet* R, int idx);

void centers_redistribute_undo(Center** centers, int n);

void centers_redistribute_closest(Center** centers, Graph* G, BitSet* R, int idx, int* closest);

#endif