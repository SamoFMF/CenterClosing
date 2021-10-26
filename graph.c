#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "graph.h"
#include "algutils.h"

// Raw graphs (from pajek files)
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
Graph* graph_new() {
	Graph* graph;
	if ((graph = malloc(sizeof * graph)) != NULL) {
		graph->G = NULL;
		graph->D = NULL;
		graph->C = NULL;
		graph->S = NULL;
		graph->H = NULL;
	}
	return graph;
}

void graph_free(Graph* graph) {
	if (graph != NULL) {
		if (graph->G != NULL) free_matrix_int(graph->G, graph->N);
		if (graph->D != NULL) free_matrix_double(graph->D, graph->N);
		if (graph->C != NULL) free(graph->C);
		if (graph->S != NULL) free(graph->S);
		if (graph->H != NULL) free(graph->H);
		free(graph);
	}
}

void graph_partition_nodes(Graph* G, BitSet* S_set, int m) {
	int n = G->N - m;
	if ((G->C = malloc(n * sizeof * (G->C))) != NULL) {
		if ((G->S = malloc(m * sizeof * (G->S))) != NULL) {
			int ic = 0;
			int is = 0;
			for (int i = 0; i < G->N; i++) {
				if (bitset_contains(S_set, i))
					G->S[is++] = i;
				else
					G->C[ic++] = i;
			}
			G->n = n;
			G->m = m;
		}
		else {
			printf("ERROR - Ran out of memory: graph_partition_nodes - G->S");
			free(G->C);
		}
	}
	else {
		printf("ERROR - Ran out of memory: graph_partition_nodes - G->C");
	}
}

void graph_add_weights(Graph* G, double* hs) {
	G->H = hs;
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
		G->C = V;
		G->S = C;
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
		G->C = V;
		G->S = C;
		G->n = G->N;
		G->m = m;
	}
}

void graph_add_info(Graph* G, int* V, int* C, double* h, double* a, int n, int m) {
	G->C = V;
	G->S = C;
	G->H = h;
	// G->a = a;
	G->n = n;
	G->m = m;
}

typedef struct Comparator {
	Graph* G;
	Options* options;
	int is_center;
	int idx;
} Comparator;

Comparator* comparator_new(Graph* G, Options* options) {
	Comparator* comparator;
	if ((comparator = malloc(sizeof * comparator)) != NULL) {
		comparator->G = G;
		comparator->options = options;
	}
	else
		printf("EROR - Ran out of memory: comparator_new");
	return comparator;
}

void comparator_update(Comparator* comparator, int idx, int is_center) {
	comparator->idx = idx;
	comparator->is_center = is_center;
}

void comparator_free(Comparator* comparator) {
	free(comparator);
}

//int compare_adj_list(void* cont, const void* x, const void* y) {
//	double* cont1 = cont;
//	if (cont1 == NULL) printf("NULL!");
//	double f = cont1[*((int*)x)];
//	double s = cont1[*((int*)y)];
//	if (f > s) return 1;
//	if (f < s) return -1;
//	return 0;
//}

int compare_adj_list(void* cont, const void* xin, const void* yin) {
	Comparator* comp = cont;
	if (comp == NULL) printf("NULL!");
	int x = *((int*)xin);
	int y = *((int*)yin);
	double f, s;
	if (comp->is_center) {
		f = comp->options->eval(x, comp->idx, comp->G);
		s = comp->options->eval(y, comp->idx, comp->G);
	}
	else {
		f = comp->options->eval(comp->idx, x, comp->G);
		s = comp->options->eval(comp->idx, y, comp->G);
	}
	if (f > s) return 1;
	if (f < s) return -1;
	return 0;
}

void graph_add_sorted_adjacency_list(Graph* G, Options* options) {
	int c, s;
	if (G->G != NULL) {
		for (int i = 0; i < G->N; i++)
			if (G->G[i] != NULL)
				free(G->G[i]);
		free(G->G);
	}
	if ((G->G = malloc(G->N * sizeof * G->G)) != NULL) {
		Comparator* comparator = comparator_new(G, options);
		for (int ic = 0; ic < G->n; ic++) {
			c = G->C[ic];
			if ((G->G[c] = malloc(G->m * sizeof * G->G[c])) != NULL) {
				for (int is = 0; is < G->m; is++)
					//G->G[c][is] = G->S[is];
					G->G[c][is] = is;
				comparator_update(comparator, ic, 0);
				qsort_s(G->G[c], G->m, sizeof * G->G[c], compare_adj_list, comparator);
			} else
				printf("ERROR - Ran out of memory: graph_add_sorted_adjacency_list - G->G[c]\n");
		}
		for (int is = 0; is < G->m; is++) {
			s = G->S[is];
			if ((G->G[s] = malloc(G->n * sizeof * G->G[s])) != NULL) {
				for (int ic = 0; ic < G->n; ic++)
					//G->G[s][ic] = G->C[ic];
					G->G[s][ic] = ic;
				comparator_update(comparator, is, 1);
				qsort_s(G->G[s], G->n, sizeof * G->G[s], compare_adj_list, comparator);
			} else
				printf("ERROR - Ran out of memory: graph_add_sorted_adjacency_list - G->G[s]\n");
		}
	}
}

// Read Pajek
GraphRaw* read_pajek_raw(char* filename, int* num_nodes) {
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

Graph* read_pajek(char* filename) {
	Graph* G = graph_new();
	int n, m, u, v;
	double d;
	FILE* file;
	fopen_s(&file, filename, "r");
	//if (fscanf_s(file, "%*s %d", &n) == 0) return NULL;
	fscanf_s(file, "%*s %d\n", &n);
	G->N = n;
	if ((G->D = malloc(n * sizeof * (G->D))) == NULL) {
		printf("ERROR - Ran out of memory: read_pajek - G->D\n");
		graph_free(G);
		return NULL;
	}
	for (int i = 0; i < n; i++) {
		fscanf_s(file, "%*[^\n]\n");
		if ((G->D[i] = malloc(n * sizeof * (G->D[i]))) != NULL) {
			for (int j = 0; j < n; j++)
				G->D[i][j] = DBL_MAX;
			G->D[i][i] = 0.0;
		}
		else {
			printf("ERROR - Ran out of memory: read_pajek - G->D[i]\n");
			graph_free(G);
			return NULL;
		}
	}

	fscanf_s(file, "%*s %d", &m);
	for (int i = 0; i < m; i++) {
		fscanf_s(file, "%d %d %lf\n", &u, &v, &d);
		u--;
		v--;
		G->D[u][v] = d;
		G->D[v][u] = d;
	}
	fclose(file);
	return G;
}

double** read_pajek_dist(char* filename, int* num_nodes) {
	double** D;
	int n, m, u, v;
	double d;
	FILE* file;
	fopen_s(&file, filename, "r");
	if (fscanf_s(file, "%*s %d ", &n) == 0) return NULL;
	if ((D = malloc(n * sizeof * D)) == NULL) {
		printf("ERROR - Ran out of memory: read_pajek_dist - D");
	}
	else {
		for (int i = 0; i < n; i++) {
			fscanf_s(file, "%*[^\n] ");
			if ((D[i] = malloc(n * sizeof * D[i])) == NULL) {
				printf("ERROR - Ran out of memory: read_pajek_dist - D[i]");
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

double** read_pajek_dist_weighted(char* filename, int* num_nodes, double** weights_ptr) {
	double** D;
	double* weights;
	int n, m, u, v;
	double d;
	FILE* file;
	fopen_s(&file, filename, "r");
	if (fscanf_s(file, "%*s %d ", &n) == 0) return NULL;
	if ((D = malloc(n * sizeof * D)) == NULL || (weights = malloc(n * sizeof * weights)) == NULL) {
		printf("ERROR - Ran out of memory: read_pajek_fw - D");
	}
	else {
		for (int i = 0; i < n; i++) {
			// fscanf_s(file, "%*[^\n] ");
			fscanf_s(file, "%d %*s %lf", &u, &weights[i]);
			// weights[u - 1] = w;
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
			if (i % 10000 == 0) printf("%d\n", i);
			if (fscanf_s(file, "%d %d %lf ", &u, &v, &d) == 0) return NULL;;
			u--;
			v--;
			D[u][v] = d;
			D[v][u] = d;
		}
		*weights_ptr = weights;
	}

	fclose(file);
	*num_nodes = n;
	return D;
}

int* read_centers(char* filename, int* num_nodes) {
	int* C;
	int n;
	FILE* file;
	fopen_s(&file, filename, "r");
	fscanf_s(file, "%d", &n);
	if ((C = malloc(n * sizeof * C)) == NULL) return NULL;
	for (int i = 0; i < n; i++) {
		fscanf_s(file, "%d", &C[i]);
	}
	*num_nodes = n;
	return C;
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

/// <summary>
/// Reads from file in Pajek format and uses Floyd-Warshall algorithm
/// to return matrix of all pairwise distances.
/// </summary>
/// <param name="filename">Name of Pajek file containing graph info.</param>
/// <returns>Matrix of doubles.</returns>
double** read_pajek_fw_weighted(char* filename, int* num_nodes, double** weights) {
	double** D = read_pajek_dist_weighted(filename, num_nodes, weights);
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

// Read OR-Library (cap)
Graph* read_or_library(char* filename) {
	Graph* G = graph_new();
	int n, m, w;
	double d;
	FILE* file;
	fopen_s(&file, filename, "r");
	fscanf_s(file, "%d %d\n", &m, &n);
	G->N = n + m;
	G->n = n;
	G->m = m;
	if ((G->D = malloc(G->N * sizeof * G->D)) == NULL)
		printf("ERROR - Ran out of memory: read_or_library - G->D\n");
	if ((G->S = malloc(m * sizeof * G->S)) == NULL)
		printf("ERROR - Ran out of memory: read_or_library - G->S\n");
	for (int i = 0; i < m; i++) {
		fscanf_s(file, "%*[^\n]\n");
		if ((G->D[i] = malloc(G->N * sizeof * G->D[i])) == NULL)
			printf("ERROR - Ran out of memory: read_or_library - G->D[i](s)\n");
		for (int j = 0; j < G->N; j++)
			G->D[i][j] = DBL_MAX;
		G->D[i][i] = 0.0;
		G->S[i] = i;
	}
	if ((G->C = malloc(n * sizeof * G->C)) == NULL)
		printf("ERROR - Ran out of memory: read_or_library - G->C\n");
	if ((G->H = malloc(n * sizeof * G->H)) == NULL)
		printf("ERROR - Ran out of memory: read_or_library - G->H\n");
	for (int i = 0; i < n; i++) {
		if ((G->D[i + m] = malloc(G->N * sizeof * G->D[i])) == NULL)
			printf("ERROR - Ran out of memory: read_or_library - G->D[i](c)\n");
		fscanf_s(file, "%d\n", &w);
		G->H[i] = (double)w;
		G->C[i] = i + m;
		for (int j = 0; j < m; j++) {
			fscanf_s(file, "%lf ", &d);
			G->D[i + m][j] = d;
			G->D[j][i + m] = d;
		}
		for (int j = 0; j < n; j++)
			G->D[i + m][j + m] = DBL_MAX;
		G->D[i + m][i + m] = 0.0;
	}
	//floyd_warshall_algorithm(G->D, G->N); // TODO - think if this is necessary?

	return G;
}

Graph* read_or_library_pmed(char* filename, int* k) {
	Graph* G = graph_new();
	int n, m, kc; // Number of nodes (ie. number of consumers and also the number of suppliers), number of edges in file and k-center parameter
	FILE* file;
	fopen_s(&file, filename, "r");
	fscanf_s(file, "%d %d %d\n", &n, &m, &kc);
	G->N = 2 * n;
	G->n = n;
	G->m = n;
	*k = G->m - kc;
	if ((G->S = malloc(n * sizeof * G->S)) == NULL)
		printf("ERROR - Ran out of memory: read_or_library - G->S\n");
	if ((G->C = malloc(n * sizeof * G->C)) == NULL)
		printf("ERROR - Ran out of memory: read_or_library - G->C\n");
	if ((G->H = malloc(n * sizeof * G->H)) == NULL)
		printf("ERROR - Ran out of memory: read_or_library - G->H\n");
	for (int i = 0; i < n; i++) {
		G->S[i] = i;
		G->C[i] = i + n;
		G->H[i] = 1.0;
	}

	if ((G->D = malloc(G->N * sizeof * G->D)) == NULL)
		printf("ERROR - Ran out of memory: read_or_library_pmed - G->D\n");
	for (int i = 0; i < n; i++) {
		if ((G->D[i] = malloc(G->N * sizeof * G->D[i])) == NULL)
			printf("ERROR - Ran out of memory: read_or_library - G->D[%d]\n", i);
		if ((G->D[i + n] = malloc(G->N * sizeof * G->D[i + n])) == NULL)
			printf("ERROR - Ran out of memory: read_or_library - G->D[%d]\n", i + n);
		for (int j = 0; j < G->N; j++) {
			G->D[i][j] = DBL_MAX;
			G->D[i + n][j] = DBL_MAX;
		}
		G->D[i][i] = 0;
		G->D[i][i + n] = 0;
		G->D[i + n][i] = 0;
		G->D[i + n][i + n] = 0;
	}
	int u, v, w;
	double d;
	for (int i = 0; i < m; i++) {
		fscanf_s(file, "%d %d %d\n", &u, &v, &w);
		u--;
		v--;
		d = (double)w;
		G->D[u][v] = d;
		G->D[v][u] = d;

		G->D[u][v + n] = d;
		G->D[v + n][u] = d;

		G->D[u + n][v] = d;
		G->D[v][u + n] = d;

		G->D[u + n][v + n] = d;
		G->D[v + n][u + n] = d;
	}

	floyd_warshall_algorithm(G->D, G->N);

	fclose(file);

	return G;
}