#ifndef GRAPH_H
#define GRAPH_H

#include "bitset.h"
#include "utils.h"
#include "algutils.h"

typedef struct Options Options;

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
	int** G; // Sorted adjacency list
	double** D; // Distance matrix
	int* C; // Consumers
	int* S; // Suppliers
	double* H; // Weights
	// double* a; // Addends
	int N, n, m; // number of nodes, consumers and suppliers
} Graph;

// Graph* graph_new(double** D, int n);
Graph* graph_new();

void graph_free(Graph* graph);

void graph_partition_nodes(Graph* G, BitSet* S_set, int m);

void graph_add_from_centers_strong(Graph* G, BitSet* C_set, int m);

void graph_add_from_centers(Graph* G, BitSet* C_set, int m);

void graph_add_info(Graph* G, int* V, int* C, double* h, double* a, int n, int m);

void graph_add_sorted_adjacency_list(Graph* G, Options* options);

// Pajek
GraphRaw* read_pajek_raw(char* filename, int* num_nodes);

Graph* read_pajek(char* filename);

double** read_pajek_dist(char* filename, int* num_nodes);

double** read_pajek_fw(char* filename, int* num_nodes);

double** read_pajek_dist_weighted(char* filename, int* num_nodes, double** weights_ptr);

double** read_pajek_fw_weighted(char* filename, int* num_nodes, double** weights);

int* read_centers(char* filename, int* num_nodes);

void write_pajek(char* filename, GraphRaw* graph);

void write_pajek_dist(char* filename, double** D, int n);

// OR-Library
Graph* read_or_library(char* filename);

Graph* read_or_library_pmed(char* filename, int* k);

#endif