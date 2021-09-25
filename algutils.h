#ifndef ALGUTILS_H
#define ALGUTILS_H

#include "graph.h"
#include "bitset.h"
#include "utils.h"

//double f(Graph* G, int i_node, int j_center);

typedef struct Graph Graph;

// Structs
typedef struct Result {
	double score;
	int* R;
} Result;

typedef struct Center {
	int* nodes; // Nodes for which Center is the closest center
	int numOfNodes; // Number of elements in `nodes`
	int delta; // Change in numOfNodes required
	LinkedList* history; // Centers score (max dist to node in `nodes`)
} Center;

typedef struct Options Options;

struct Options {
	double (*eval)(int c, int s, Graph* G);
	int (*get_first)(Graph* G, Options* options);
	double (*get_priority)(Center* center);
	int hochbaum_start_center; // Used for HochbaumPlus
};

// Result related
Result* result_new();

void result_free(Result* result);

void result_update(Result* result, double val, BitSet* R, int* S);

double eval_score(Graph* G, BitSet* R, Options* options);

void save_removed_nodes(Result* res, BitSet* R);

// Options related
Options* options_new();

// Center related
Center* center_new(int n);

void center_free(Center* center);

void centers_free(Center** centers, int n);

void center_resize(Center* center);

void center_add(Center* center, int node);

Center** centers_new_from_graph(Graph* G, Options* options, double* eval);

double centers_redistribute(Center** centers, Graph* G, BitSet* R, int idx, Options* options);

void centers_redistribute_undo(Center** centers, int n);

double centers_redistribute_oneway(Center** centers, Graph* G, BitSet* R, int idx, Options* options);

void centers_redistribute_closest(Center** centers, Graph* G, int idx, int* closest, Options* options);

// Distance related
double* get_sorted_distances_no_duplicates(Graph* G, int* new_len, Options* options);

#endif