#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "graph.h"

double** create_random_distances(int n) {
	double** D = distance_matrix_new(n);
	double val;
	if (D != NULL) {
		for (int i = 0; i < n; i++) {
			D[i][i] = 0;
			for (int j = i + 1; j < n; j++) {
				val = rand_lim(RAND_MAX / 2);
				D[i][j] = val;
				D[j][i] = val;
			}
		}

		// Make sure triangle inequality holds
		int changes = 1;
		while (changes) {
			changes = 0;
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					for (int k = 0; k < n; k++) {
						val = D[i][k] + D[k][j];
						if (val < D[i][j]) {
							D[i][j] = val;
							D[j][i] = val;
							changes = 1;
						}
					}
				}
			}
		}
	}
	return D;
}

/// <summary>
/// Gnm model of Erdos-Renyi random graph. Modified to ensure 1 connected component.
/// Node `0` is a special connecting node - it connects to every other node with very high distance.
/// </summary>
/// <param name="n">Number of nodes.</param>
/// <param name="m">Number of edges.</param>
/// <returns>Distance matrix of Erdos-Renyi random graph.</returns>
double** create_erdos_renyi_gnm(int n, int m) {
	double** D = distance_matrix_new(n);
	if (D != NULL) {
		int edges = 0;
		int u, v;
		double d;
		while (edges < m) {
			u = rand_lim(n);
			v = rand_lim(n);
			if (u != v) {
				d = rand_lim(RAND_MAX / 2);
				D[u][v] = d;
				D[v][u] = d;
				edges++;
			}
		}

		// Ensure only 1 connected component.
		for (int i = 0; i < n; i++) {
			D[0][i] = RAND_MAX / 2;
			D[i][0] = RAND_MAX / 2;
		}
	}
	return D;
}

/// <summary>
/// Gnm model of Erdos-Renyi random graph. Modified to ensure 1 connected component. And distances added as shortest path between any pair of nodes - using Floyd-Warshall algorithm.
/// Node `0` is a special connecting node - it connects to every other node with very high distance
/// </summary>
/// <param name="n">Number of nodes.</param>
/// <param name="m">Number of edges.</param>
/// <returns>Distance matrix of Erdos-Renyi random graph.</returns>
double** create_erdos_renyi_gnm_full(int n, int m) {
	double** D = create_erdos_renyi_gnm(n, m);
	floyd_warshall_algorithm(D, n);
	return D;
}

// Greedy: 2-approx
Graph* create_star_graph(int n, double R, double x) {
	n -= (n - 1) % 2;
	Graph* graph = NULL;
	double** D = distance_matrix_new(n);
	if (D != NULL) {
		// Set everything to INT_MAX (D[i][i] = 0)
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				D[i][j] = INT_MAX;
			}
			D[i][i] = 0.0;
		}
		int mid = (n - 1) / 2;
		for (int i = 0; i < mid; i++) {
			D[i][mid + i] = R;
			D[mid + i][i] = R;
			D[mid + i][n - 1] = R + x;
			D[n - 1][mid + i] = R + x;
		}
		floyd_warshall_algorithm(D, n);
		graph = graph_new(D, n);
		BitSet* C = bitset_new(n);
		if (graph != NULL && C != NULL) {
			for (int i = 0; i < mid; i++) bitset_add(C, i);
			bitset_add(C, n - 1);
			//graph_add_from_centers(graph, C, mid + 1);
			graph_add_from_centers_strong(graph, C, mid + 1);
		}
	}
	return graph;
}

// This one gives 3/2 for small enough x for greedy method
Graph* create_circle_graph(int n, double R, double x) {
	n -= (n - 1) % 3;
	double** D;
	if ((D = malloc(n * sizeof * D)) == NULL) {
		printf("ERROR - Ran out of memory: create_circle_graph - D");
		return NULL;
	}
	else {
		for (int i = 0; i < n; i++) {
			if ((D[i] = malloc(n * sizeof * D[i])) == NULL) {
				printf("ERROR - Ran out of memory: create_circle_graph - D[i]");
				return NULL;
			}
			else {
				for (int j = 0; j < n; j++) {
					D[i][j] = -1;
				}
				D[i][i] = 0;
			}
		}
	}

	int u, v;
	int n3 = n / 3;
	for (int i = 0; i < n3; i++) {
		u = 3 * i;

		// Add actual links in graph
		D[n - 1][u] = R;
		D[u][u + 1] = R + x;
		D[u + 1][u + 2] = 2 * R;
		D[u + 2][(u + 3) % (n - 1)] = R + x;
		D[u + 2][(u + 4) % (n - 1)] = 2 * (R + x);

		// Other distances from center `u` (and 2 vertices after it)
		D[u][u + 2] = 3 * R + x;
		v = u;
		for (int j = 1; j < n3 - 1; j++) {
			v += 3;
			v %= n - 1;

			D[u][v] = 2 * R;
			D[u][v + 1] = 3 * R + x;
			D[u][v + 2] = 3 * R + x;
		}
		v += 3;
		v %= n - 1;
		D[u][v + 1] = 3 * R + x;

		// Distances from `u+1` and `u+2`
		D[u + 1][n - 1] = 2 * R + x;
		D[u + 2][n - 1] = 2 * R + x;
		D[u + 1][(u + 4) % (n - 1)] = 2 * (2 * R + x);
		D[u + 1][(u + 5) % (n - 1)] = 2 * (2 * R + x);
		D[u + 2][(u + 5) % (n - 1)] = 2 * (2 * R + x);
		v = u;
		for (int j = 2; j < n3 - 1; j++) {
			v += 3;
			v %= n - 1;

			D[u + 1][v + 1] = 2 * (2 * R + x);
			D[u + 1][v + 2] = 2 * (2 * R + x);
			D[u + 2][v + 1] = 2 * (2 * R + x);
			D[u + 2][v + 2] = 2 * (2 * R + x);
		}
		v += 3;
		v %= n - 1;
		D[u + 1][v + 1] = 2 * (2 * R + x);
		D[u + 2][v + 1] = 2 * (2 * R + x);
		D[u + 2][v + 2] = 2 * (2 * R + x);
	}

	for (int i = 0; i < n; i++) {
		for (int j = i+1; j < n; j++) {
			if (D[i][j] > 0) {
				D[j][i] = D[i][j];
			}
			else {
				D[i][j] = D[j][i];
			}
		}
	}

	Graph* G = graph_new(D, n);

	BitSet* C = bitset_new(n);
	for (int i = 0; i < n3; i++) {
		bitset_add(C, 3 * i);
	}
	bitset_add(C, n - 1);

	graph_add_from_centers(G, C, n3 + 1);

	return G;
}