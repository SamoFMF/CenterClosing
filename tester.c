#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include "graph.h"
#include "algutils.h"
#include "utils.h"
#include "tester.h"
#include "greedy.h"
#include "plesnik.h"
#include "hochbaum.h"
#include "heuristic.h"
#include "exact.h"
#include "backtracking.h"
#include "bruteforce.h"
#include "result_to_json.h"

double* test_approximate(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file) {
	double* scores;
	if ((scores = malloc(((kend - kstart) / kstep) * sizeof * scores)) == NULL)
		printf("ERROR - Ran out of memory: test_approximate\n");
	double scoremin;
	Result* res;
	clock_t start, end;
	/*FILE* file;
	fopen_s(&file, outputFileName, "w");*/
	fprintf(file, "{algorithm_type: approximate, results: [");
	for (int k = kstart; k < kend; k += kstep) {
		printf("k=%d\n", k);
		scoremin = DBL_MAX;

		//// Greedy: basic
		//start = clock();
		//res = greedy_basic(G, k, options);
		//end = clock();
		//write_entry(file, "greedy_basic", res, k, get_time(start, end));
		//fprintf(file, ", ");
		//if (res->score < scoremin)
		//	scoremin = res->score;
		//result_free(res);
		//printf("\tgreedy_basic\n");

		// Greedy: centers
		start = clock();
		res = greedy_centers(G, k, options);
		end = clock();
		printf("Writing: ");
		write_entry(file, "greedy_centers", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\tgreedy_centers\n");

		// Hochbaum: random
		options->get_first = hochbaum_start_random;
		start = clock();
		res = hochbaum(G, k, options);
		end = clock();
		write_entry(file, "hochbaum_rand", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\thochbaum_random\n");

		// Hochbaum: best
		options->get_first = hochbaum_start_best;
		start = clock();
		res = hochbaum(G, k, options);
		end = clock();
		write_entry(file, "hochbaum_best", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\thochbaum_best\n");

		// Hochbaum: plus
		start = clock();
		res = hochbaum_plus(G, k, options);
		end = clock();
		write_entry(file, "hochbaum_plus", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\thochbaum_plus\n");

		// Plesnik: range_first
		start = clock();
		res = plesnik(G, k, options, range_first);
		end = clock();
		write_entry(file, "plesnik_first", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\tplesnik_first\n");

		// Plesnik: range_closest
		start = clock();
		res = plesnik(G, k, options, range_closest);
		end = clock();
		write_entry(file, "plesnik_closest", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\tplesnik_closest\n");

		// Plesnik: range_rand
		start = clock();
		res = plesnik(G, k, options, range_rand);
		end = clock();
		write_entry(file, "plesnik_rand", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\tplesnik_rand\n");

		// Plesnik_unlimited: range_first
		start = clock();
		res = plesnik_unlimited(G, k, options, range_first, 0.5);
		end = clock();
		write_entry(file, "plesnik_unlimited_first", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\tplesnik_unlimited_first\n");

		// Plesnik: range_closest
		start = clock();
		res = plesnik_unlimited(G, k, options, range_closest, 0.5);
		end = clock();
		write_entry(file, "plesnik_unlimited_closest", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\tplesnik_unlimited_closest\n");

		// Plesnik: range_rand
		start = clock();
		res = plesnik_unlimited(G, k, options, range_rand, 0.5);
		end = clock();
		write_entry(file, "plesnik_unlimited_rand", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\tplesnik_unlimited_rand\n");

		//// Plesnik: range_adj
		//start = clock();
		//res = plesnik(G, k, options, range_adj);
		//end = clock();
		//write_entry(file, "plesnik_adj", res, k, get_time(start, end));
		//fprintf(file, ", ");
		//if (res->score < scoremin)
		//	scoremin = res->score;
		//result_free(res);
		//printf("\tplesnik_adj\n");

		// Cluster: Cardinality
		start = clock();
		res = cluster_cardinality(G, k, options);
		end = clock();
		write_entry(file, "cluster_cardinality", res, k, get_time(start, end));
		fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\tcluster_cardinality\n");

		// Cluster: Radius
		start = clock();
		res = cluster_radius(G, k, options);
		end = clock();
		write_entry(file, "cluster_radius", res, k, get_time(start, end));
		if (k >= kend - kstep)
			fprintf(file, "]}");
		else
			fprintf(file, ", ");
		if (res->score < scoremin)
			scoremin = res->score;
		result_free(res);
		printf("\tcluster_radius\n");

		scores[(k - kstart) / kstep] = scoremin;
	}
	//fclose(file);

	return scores;
}

void test_exact(Graph* G, Options* options, int kstart, int kend, int kstep, double* scores, FILE* file) {
	Result* res;
	clock_t start, end;
	/*FILE* file;
	fopen_s(&file, outputFileName, "w");*/
	fprintf(file, "{algorithm_type: exact, results: [");
	for (int k = kstart; k < kend; k += kstep) {
		printf("k=%d\n", k);
		// Exact
		start = clock();
		res = exact(G, k, options);
		end = clock();
		write_entry(file, "exact", res, k, get_time(start, end));
		fprintf(file, ", ");
		result_free(res);
		printf("\texact\n");

		// Exact: bound
		start = clock();
		res = exact_bound(G, k, options, scores[(k - kstart) / kstep] + 1e-6);
		end = clock();
		write_entry(file, "exact_bound", res, k, get_time(start, end));
		if (k >= kend - kstep)
			fprintf(file, "]}");
		else
			fprintf(file, ", ");
		result_free(res);
		printf("\texact_bound\n");
	}
	//fclose(file);
}

void test_backtracking(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file) {
	Result* res;
	clock_t start, end;
	/*FILE* file;
	fopen_s(&file, outputFileName, "w");*/
	fprintf(file, "{algorithm_type: backtracking, results: [");
	for (int k = kstart; k < kend; k += kstep) {
		printf("k=%d\n", k);
		// Backtracking
		start = clock();
		res = backtracking(G, k, options);
		end = clock();
		write_entry(file, "backtracking", res, k, get_time(start, end));
		if (k >= kend - kstep)
			fprintf(file, "]}");
		else
			fprintf(file, ", ");
		result_free(res);
		printf("\tbacktracking\n");
	}
	//fclose(file);
}

void test_backtracking_decision(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file) {
	Result* res;
	clock_t start, end;
	/*FILE* file;
	fopen_s(&file, outputFileName, "w");*/
	fprintf(file, "{algorithm_type: decision, results: [");
	for (int k = kstart; k < kend; k += kstep) {
		printf("k=%d\n", k);
		// Backtracking
		start = clock();
		res = backtracking_decision_to_optimization(G, k, options);
		end = clock();
		write_entry(file, "backtracking_decision", res, k, get_time(start, end));
		if (k >= kend - kstep)
			fprintf(file, "]}");
		else
			fprintf(file, ", ");
		result_free(res);
		printf("\tbacktracking_decision\n");
	}
	//fclose(file);
}

void test_bruteforce(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file) {
	Result* res;
	clock_t start, end;
	/*FILE* file;
	fopen_s(&file, outputFileName, "w");*/
	fprintf(file, "{algorithm_type: bruteforce, results: [");
	for (int k = kstart; k < kend; k += kstep) {
		printf("k=%d\n", k);
		// Bruteforce
		start = clock();
		res = bruteforce(G, k, options);
		end = clock();
		write_entry(file, "bruteforce", res, k, get_time(start, end));
		if (k >= kend - kstep)
			fprintf(file, "]}");
		else
			fprintf(file, ", ");
		result_free(res);
		printf("\tbruteforce\n");
	}
	//fclose(file);
}