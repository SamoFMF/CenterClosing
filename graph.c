#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

// Raw graps (from pajek files)
GraphRaw* graph_raw_new(int n) {
	GraphRaw* graph;
	if ((graph = malloc(sizeof * graph)) == NULL) {
		printf("ERROR - Ran out of memory: graph_raw_new - graph");
	}
	else {
		graph->n = n;
		graph->lengths = NULL;
		if ((graph->G = malloc(n * sizeof * graph->G)) == NULL) {
			printf("ERROR - Ran out of memory: graph_raw_new - graph->G");
		}
		else {
			for (int i = 0; i < n; i++) {
				if ((graph->G[i] = malloc(n * sizeof * graph->G[i])) == NULL) {
					printf("ERROR - Ran out of memory: graph_raw_new - graph->G[i]");
					break;
				}
			}
		}
	}
	return graph;
}

void graph_raw_free(GraphRaw* graph) {
	if (graph == NULL) return;
	else {
		if (graph->G != NULL) {
			for (int i = 0; i < graph->n; i++) {
				if (graph->G[i] != NULL) free(graph->G[i]);
			}
			free(graph->G);
		}
		if (graph->lengths != NULL) {
			free(graph->lengths);
		}
		free(graph);
	}
}

// Graph struct used by algorithms
Graph* graph_new(double** D, int n) {
	Graph* graph;
	if ((graph = malloc(sizeof * graph)) != NULL) {
		graph->D = D;
		graph->N = n;
	}
	return graph;
}

void graph_add_from_centers_strong(Graph* G, BitSet* C_set, int m) {
	int* V;
	int* C;
	if ((V = malloc((G->N - m) * sizeof(int))) != NULL && (C = malloc(m * sizeof(int))) != NULL) {
		int iv = 0;
		int ic = 0;
		for (int i = 0; i < G->N; i++) {
			if (bitset_contains(C_set, i)) {
				C[ic++] = i;
			}
			else {
				V[iv++] = i;
			}
		}
		G->V = V;
		G->C = C;
		G->n = G->N - m;
		G->m = m;
	}
}

void graph_add_from_centers(Graph* G, BitSet* C_set, int m) {
	int* V;
	int* C;
	if ((V = malloc(G->N * sizeof(int))) != NULL && (C = malloc(m * sizeof(int))) != NULL) {
		int ic = 0;
		for (int i = 0; i < G->N; i++) {
			V[i] = i;
			if (bitset_contains(C_set, i)) C[ic++] = i;
		}
		G->V = V;
		G->C = C;
		G->n = G->N;
		G->m = m;
	}
}

// Read Pajek
GraphRaw* read_pajek(char* filename, int* num_nodes) {
	int n, m, u, v;
	double d;
	FILE* file;
	fopen_s(&file, filename, "r");
	if (fscanf_s(file, "%*s %d\n", &n) == 0) return NULL;
	for (int i = 0; i < n; i++) {
		fscanf_s(file, "%*[^\n] ");
	}
	GraphRaw* graph = graph_raw_new(n);
	int* lengths;
	if ((lengths = malloc(n * sizeof * lengths)) == NULL) {
		printf("ERROR - Ran out of memory: read_pajek - lengths");
	}
	else {
		for (int i = 0; i < n; i++) {
			lengths[i] = 0;
		}
	}

	if (fscanf_s(file, "%*s %d\n", &m) == 0) return NULL;
	for (int i = 0; i < m; i++) {
		if (fscanf_s(file, "%d %d %lf ", &u, &v, &d) == 0) return NULL;
		u--;
		v--;
		graph->G[u][lengths[u]++] = pair_new(v, d);
		graph->G[v][lengths[v]++] = pair_new(u, d);
	}
	fclose(file);

	// Remove unused memory
	for (int i = 0; i < n; i++) {
		if ((graph->G[i] = realloc(graph->G[i], lengths[i] * sizeof * graph->G[i])) == NULL) {
			printf("ERROR - Realloc: read_pajek - graph->G[i]");
		}
	}

	*num_nodes = n;
	return graph;
}

double** read_pajek_dist(char* filename, int* num_nodes) {
	double** D;
	int n, m, u, v;
	double d;
	FILE* file;
	fopen_s(&file, filename, "r");
	if (fscanf_s(file, "%*s %d ", &n) == 0) return NULL;
	if ((D = malloc(n * sizeof * D)) == NULL) {
		printf("ERROR - Ran out of memory: read_pajek_fw - D");
	}
	else {
		for (int i = 0; i < n; i++) {
			fscanf_s(file, "%*[^\n] ");
			if ((D[i] = malloc(n * sizeof * D[i])) == NULL) {
				printf("ERROR - Ran out of memory: read_pajek_fw - D[i]");
			}
			else {
				for (int j = 0; j < n; j++) D[i][j] = INT_MAX;
				D[i][i] = 0.0;
			}
		}
		fscanf_s(file, "%*s %d ", &m);
		for (int i = 0; i < m; i++) {
			if (fscanf_s(file, "%d %d %lf ", &u, &v, &d) == 0) return NULL;;
			u--;
			v--;
			D[u][v] = d;
			D[v][u] = d;
		}
	}
	fclose(file);
	*num_nodes = n;
	return D;
}

/// <summary>
/// Reads from file in Pajek format and uses Floyd-Warshall algorithm
/// to return matrix of all pairwise distances.
/// </summary>
/// <param name="filename">Name of Pajek file containing graph info.</param>
/// <returns>Matrix of doubles.</returns>
double** read_pajek_fw(char* filename, int* num_nodes) {
	double** D = read_pajek_dist(filename, num_nodes);
	floyd_warshall_algorithm(D, *num_nodes);
	return D;
}

// Write Pajek

/// <summary>
/// Writes graph in GraphRaw format into a Pajek format file.
/// </summary>
/// <param name="filename">Name of file to write to.</param>
/// <param name="graph">Graph in adjacency list representation using GraphRaw struct.</param>
void write_pajek(char* filename, GraphRaw* graph) {
	// Open file
	FILE* file;
	fopen_s(&file, filename, "w");

	// Write nodes
	fprintf_s(file, "*vertices %d\n", graph->n);
	for (int i = 1; i < graph->n + 1; i++) {
		fprintf_s(file, "%d \"%d\"\n", i, i);
	}

	// Write edges
	int m = 0;
	for (int i = 0; i < graph->n; i++) m += graph->lengths[i];
	fprintf_s(file, "*edges %d\n", m);
	PairIntDouble* pair;
	for (int i = 0; i < graph->n; i++) {
		for (int j = 0; j < graph->lengths[i]; j++) {
			pair = graph->G[i][j];
			if (pair->node > i) {
				fprintf_s(file, "%d %d %lf\n", i, pair->node, pair->value);
			}
		}
	}

	// Close file
	fclose(file);
}

/// <summary>
/// Writes graph in matrix distance format to a Pajek format file.
/// </summary>
/// <param name="filename">Name of file to write to.</param>
/// <param name="D">Matrix of distances.</param>
/// <param name="n">Number of nodes.</param>
void write_pajek_dist(char* filename, double** D, int n) {
	// Open file
	FILE* file;
	fopen_s(&file, filename, "w");

	// Write nodes
	fprintf_s(file, "*vertices %d\n", n);
	for (int i = 1; i < n + 1; i++) {
		fprintf_s(file, "%d \"%d\"\n", i, i);
	}

	// Write edges
	int m = 0;
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if (D[i][j] < INT_MAX) m++;
		}
	}
	fprintf_s(file, "*edges %d\n", m);
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if (D[i][j] < INT_MAX) fprintf_s(file, "%d %d %lf\n", i, j, D[i][j]);
		}
	}

	// Close file
	fclose(file);
}