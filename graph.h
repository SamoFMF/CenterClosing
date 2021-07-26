#ifndef GRAPH_H
#define GRAPH_H

#include "bitset.h"
#include "utils.h"

// Graph Raw
typedef struct GraphRaw {
	PairIntDouble*** G;
	int* lengths;
	int n;
} GraphRaw;

GraphRaw* graph_raw_new(int n);

void graph_raw_free(GraphRaw* graph);

// Graph
typedef struct Graph {
	double** D;
	int* V;
	int* C;
	int N, n, m;
} Graph;

Graph* graph_new(double** D, int n);

void graph_add_from_centers_strong(Graph* G, BitSet* C_set, int m);

void graph_add_from_centers(Graph* G, BitSet* C_set, int m);

// Pajek
GraphRaw* read_pajek(char* filename, int* num_nodes);

double** read_pajek_dist(char* filename, int* num_nodes);

double** read_pajek_fw(char* filename, int* num_nodes);

void write_pajek(char* filename, GraphRaw* graph);

void write_pajek_dist(char* filename, double** D, int n);

#endif