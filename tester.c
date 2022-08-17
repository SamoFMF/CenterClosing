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
#include "independantset.h"
#include "setcover.h"
#include "result_to_json.h"

Settings* settings_new() {
	Settings* settings;
	if ((settings = malloc(sizeof * settings)) == NULL)
		printf("ERROR - Ran out of memory: settings_new\n");
	return settings;
}

void settings_free(Settings* settings) {
	if (settings->algorithms != NULL)
		bitset_free(settings->algorithms);
	free(settings);
}

//double* test_approx(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file) {
//	double* scores;
//	if ((scores = malloc(((kend - kstart) / kstep) * sizeof * scores)) == NULL)
//		printf("ERROR - Ran out of memory: test_approximate\n");
//	double scoremin;
//	Result* res;
//	clock_t start, end;
//	/*FILE* file;
//	fopen_s(&file, outputFileName, "w");*/
//	fprintf(file, "{algorithm_type: approximate, results: [");
//	for (int k = kstart; k < kend; k += kstep) {
//		printf("k=%d\n", k);
//		scoremin = DBL_MAX;
//
//		//// Greedy: basic
//		//start = clock();
//		//res = greedy_basic(G, k, options);
//		//end = clock();
//		//write_entry(file, "greedy_basic", res, k, get_time(start, end));
//		//fprintf(file, ", ");
//		//if (res->score < scoremin)
//		//	scoremin = res->score;
//		//result_free(res);
//		//printf("\tgreedy_basic\n");
//
//		// Greedy: centers
//		start = clock();
//		res = greedy_centers(G, k, options);
//		end = clock();
//		printf("Writing: ");
//		write_entry(file, "greedy_centers", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\tgreedy_centers\n");
//
//		// Hochbaum: random
//		options->get_first = hochbaum_start_random;
//		start = clock();
//		res = hochbaum(G, k, options);
//		end = clock();
//		write_entry(file, "hochbaum_rand", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\thochbaum_random\n");
//
//		// Hochbaum: best
//		options->get_first = hochbaum_start_best;
//		start = clock();
//		res = hochbaum(G, k, options);
//		end = clock();
//		write_entry(file, "hochbaum_best", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\thochbaum_best\n");
//
//		// Hochbaum: plus
//		start = clock();
//		res = hochbaum_plus(G, k, options);
//		end = clock();
//		write_entry(file, "hochbaum_plus", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\thochbaum_plus\n");
//
//		// Plesnik: range_first
//		start = clock();
//		res = plesnik(G, k, options, range_first);
//		end = clock();
//		write_entry(file, "plesnik_first", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\tplesnik_first\n");
//
//		// Plesnik: range_closest
//		start = clock();
//		res = plesnik(G, k, options, range_closest);
//		end = clock();
//		write_entry(file, "plesnik_closest", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\tplesnik_closest\n");
//
//		// Plesnik: range_rand
//		start = clock();
//		res = plesnik(G, k, options, range_rand);
//		end = clock();
//		write_entry(file, "plesnik_rand", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\tplesnik_rand\n");
//
//		// Plesnik_unlimited: range_first
//		start = clock();
//		res = plesnik_unlimited(G, k, options, range_first, 0.5);
//		end = clock();
//		write_entry(file, "plesnik_unlimited_first", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\tplesnik_unlimited_first\n");
//
//		// Plesnik: range_closest
//		start = clock();
//		res = plesnik_unlimited(G, k, options, range_closest, 0.5);
//		end = clock();
//		write_entry(file, "plesnik_unlimited_closest", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\tplesnik_unlimited_closest\n");
//
//		// Plesnik: range_rand
//		start = clock();
//		res = plesnik_unlimited(G, k, options, range_rand, 0.5);
//		end = clock();
//		write_entry(file, "plesnik_unlimited_rand", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\tplesnik_unlimited_rand\n");
//
//		// Independant Set
//		start = clock();
//		res = independant_set(G, k, options);
//		end = clock();
//		write_entry(file, "independant_set", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\independant_set\n");
//
//		// Cluster: Cardinality
//		start = clock();
//		res = cluster_cardinality(G, k, options);
//		end = clock();
//		write_entry(file, "cluster_cardinality", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\tcluster_cardinality\n");
//
//		// Cluster: Radius
//		start = clock();
//		res = cluster_radius(G, k, options);
//		end = clock();
//		write_entry(file, "cluster_radius", res, k, get_time(start, end));
//		if (k >= kend - kstep)
//			fprintf(file, "]}");
//		else
//			fprintf(file, ", ");
//		if (res->score < scoremin)
//			scoremin = res->score;
//		result_free(res);
//		printf("\tcluster_radius\n");
//
//		scores[(k - kstart) / kstep] = scoremin;
//	}
//	//fclose(file);
//
//	return scores;
//}
//
//void test_exact_old(Graph* G, Options* options, int kstart, int kend, int kstep, double* scores, FILE* file) {
//	Result* res;
//	clock_t start, end;
//	/*FILE* file;
//	fopen_s(&file, outputFileName, "w");*/
//	fprintf(file, "{algorithm_type: exact, results: [");
//	for (int k = kstart; k < kend; k += kstep) {
//		printf("k=%d\n", k);
//		// Exact
//		start = clock();
//		res = exact(G, k, options);
//		end = clock();
//		write_entry(file, "exact", res, k, get_time(start, end));
//		fprintf(file, ", ");
//		result_free(res);
//		printf("\texact\n");
//
//		// Exact: bound
//		start = clock();
//		res = exact_bound(G, k, options, scores[(k - kstart) / kstep] + 1e-6);
//		end = clock();
//		write_entry(file, "exact_bound", res, k, get_time(start, end));
//		if (k >= kend - kstep)
//			fprintf(file, "]}");
//		else
//			fprintf(file, ", ");
//		result_free(res);
//		printf("\texact_bound\n");
//	}
//	//fclose(file);
//}
//
//void test_backtracking(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file) {
//	Result* res;
//	clock_t start, end;
//	/*FILE* file;
//	fopen_s(&file, outputFileName, "w");*/
//	fprintf(file, "{algorithm_type: backtracking, results: [");
//	for (int k = kstart; k < kend; k += kstep) {
//		printf("k=%d\n", k);
//		// Backtracking
//		start = clock();
//		res = backtracking(G, k, options);
//		end = clock();
//		write_entry(file, "backtracking", res, k, get_time(start, end));
//		if (k >= kend - kstep)
//			fprintf(file, "]}");
//		else
//			fprintf(file, ", ");
//		result_free(res);
//		printf("\tbacktracking\n");
//	}
//	//fclose(file);
//}
//
//void test_backtracking_decision(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file) {
//	Result* res;
//	clock_t start, end;
//	/*FILE* file;
//	fopen_s(&file, outputFileName, "w");*/
//	fprintf(file, "{algorithm_type: decision, results: [");
//	for (int k = kstart; k < kend; k += kstep) {
//		printf("k=%d\n", k);
//		// Backtracking
//		start = clock();
//		res = backtracking_decision_to_optimization(G, k, options);
//		end = clock();
//		write_entry(file, "backtracking_decision", res, k, get_time(start, end));
//		if (k >= kend - kstep)
//			fprintf(file, "]}");
//		else
//			fprintf(file, ", ");
//		result_free(res);
//		printf("\tbacktracking_decision\n");
//	}
//	//fclose(file);
//}
//
//void test_bruteforce(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file) {
//	Result* res;
//	clock_t start, end;
//	/*FILE* file;
//	fopen_s(&file, outputFileName, "w");*/
//	fprintf(file, "{algorithm_type: bruteforce, results: [");
//	for (int k = kstart; k < kend; k += kstep) {
//		printf("k=%d\n", k);
//		// Bruteforce
//		start = clock();
//		res = bruteforce(G, k, options);
//		end = clock();
//		write_entry(file, "bruteforce", res, k, get_time(start, end));
//		if (k >= kend - kstep)
//			fprintf(file, "]}");
//		else
//			fprintf(file, ", ");
//		result_free(res);
//		printf("\tbruteforce\n");
//	}
//	//fclose(file);
//}

void test_exact(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file, Settings* settings, double* scores) {
	Result* res;
	clock_t start, end;
	// fprintf(file, "{algorithm_type: exact, results: [");
	fprintf(file, "{\"data\": \"algorithm\", \"type\": \"exact\", \"results\": [");
	set_type end_condition = settings->algorithms->set[0] % (1<<6);
	for (int k = kstart; k < kend; k += kstep) {
		if (settings->notifications)
			printf("k=%d\n", k);

		// Exact (branch & bound)
		if (bitset_contains(settings->algorithms, 'a' - 97)) {
			start = clock();
			res = exact(G, k, options);
			end = clock();
			write_entry(file, "bnb", res, k, get_time(start, end));
			if ((end_condition < (1 << 1)) && (k + kstep >= kend))
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			result_free(res);
			if (settings->notifications)
				printf("\tbnb\n");
		}

		// Bruteforce
		if (bitset_contains(settings->algorithms, 'b' - 97)) {
			start = clock();
			res = bruteforce(G, k, options);
			end = clock();
			write_entry(file, "bf", res, k, get_time(start, end));
			if ((end_condition < (1 << 2)) && (k + kstep >= kend))
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			result_free(res);
			if (settings->notifications)
				printf("\tbf\n");
		}

		// Backtracking
		if (bitset_contains(settings->algorithms, 'c' - 97)) {
			start = clock();
			res = backtracking(G, k, options);
			end = clock();
			write_entry(file, "bt", res, k, get_time(start, end));
			if ((end_condition < (1 << 3)) && (k + kstep >= kend))
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			result_free(res);
			if (settings->notifications)
				printf("\tbt\n");
		}

		// Backtracking - Decision to Optimization
		if (bitset_contains(settings->algorithms, 'd' - 97)) {
			start = clock();
			res = backtracking_decision_to_optimization(G, k, options);
			end = clock();
			write_entry(file, "btd", res, k, get_time(start, end));
			if ((end_condition < (1 << 4)) && (k + kstep >= kend))
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			result_free(res);
			if (settings->notifications)
				printf("\tbtd\n");
		}

		// Set Cover
		if (bitset_contains(settings->algorithms, 'e' - 97)) {
			start = clock();
			res = solve_using_setcover(G, k, options);
			end = clock();
			write_entry(file, "sc", res, k, get_time(start, end));
			if ((end_condition < (1 << 5)) && (k + kstep >= kend))
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			result_free(res);
			if (settings->notifications)
				printf("\tsc\n");
		}

		// bnb+
		//if (bitset_contains(settings->algorithms, 'f' - 97) && scores != NULL) {
		if (bitset_contains(settings->algorithms, 'f' - 97)) {
			start = clock();
			//res = exact_bound(G, k, options, scores[(k - kstart) / kstep]);
			res = exact_pclstp(G, k, options);
			end = clock();
			write_entry(file, "bnb+", res, k, get_time(start, end));
			if (k + kstep >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			result_free(res);
			if (settings->notifications)
				printf("\tbnb+\n");
		}
	}
}

double* test_approximate(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file, Settings* settings) {
	double* scores;
	if ((scores = malloc(((kend - kstart) / kstep) * sizeof * scores)) == NULL)
		printf("ERROR - Ran out of memory: test_approximate\n");
	double scoremin;
	Result* res;
	clock_t start, end;
	// fprintf(file, "{algorithm_type: approximate, results: [");
	fprintf(file, "{\"data\": \"algorithm\", \"type\": \"approximate\", \"results\": [");
	for (int k = kstart; k < kend; k += kstep) {
		if (settings->notifications)
			printf("k=%d\n", k);
		scoremin = DBL_MAX;

		// Do both heuristic algorithms for no reason (seems to make everything work properly)
		res = cluster_cardinality(G, k, options);
		result_free(res);
		res = cluster_radius(G, k, options);
		result_free(res);

		// Independant Set (Hochbaum-Shmoys)
		if (bitset_contains(settings->algorithms, 'g' - 97)) {
			start = clock();
			res = independant_set(G, k, options);
			end = clock();
			write_entry(file, "hs", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1<<7)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\ths\n");
		}

		// Plesnik: range_rand
		if (bitset_contains(settings->algorithms, 'h' - 97)) {
			for (int i = 0; i < settings->rand_iter - 1; i++) {
				start = clock();
				res = plesnik(G, k, options, range_rand);
				end = clock();
				write_entry(file, "prand", res, k, get_time(start, end));
				fprintf(file, ", ");
				if (res->score < scoremin)
					scoremin = res->score;
				result_free(res);
			}

			start = clock();
			res = plesnik(G, k, options, range_rand);
			end = clock();
			write_entry(file, "prand", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 8)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tprand\n");
		}

		// Plesnik: range_first
		if (bitset_contains(settings->algorithms, 'i' - 97)) {
			start = clock();
			res = plesnik(G, k, options, range_first);
			end = clock();
			write_entry(file, "pfirst", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 9)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tpfirst\n");
		}

		// Plesnik: range_closest
		if (bitset_contains(settings->algorithms, 'j' - 97)) {
			start = clock();
			res = plesnik(G, k, options, range_closest);
			end = clock();
			write_entry(file, "pclst", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 10)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tpclst\n");
		}

		// Plesnik+: range_rand
		if (bitset_contains(settings->algorithms, 'k' - 97)) {
			for (int i = 0; i < settings->rand_iter - 1; i++) {
				start = clock();
				res = plesnik_unlimited(G, k, options, range_rand);
				end = clock();
				write_entry(file, "prand+", res, k, get_time(start, end));
				fprintf(file, ", ");
				if (res->score < scoremin)
					scoremin = res->score;
				result_free(res);
			}

			start = clock();
			res = plesnik_unlimited(G, k, options, range_rand);
			end = clock();
			write_entry(file, "prand+", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 11)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tprand+\n");
		}

		// Plesnik+: range_first
		if (bitset_contains(settings->algorithms, 'l' - 97)) {
			start = clock();
			res = plesnik_unlimited(G, k, options, range_first);
			end = clock();
			write_entry(file, "pfirst+", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 12)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tpfirst+\n");
		}

		// Plesnik+: range_closest
		if (bitset_contains(settings->algorithms, 'm' - 97)) {
			start = clock();
			res = plesnik_unlimited(G, k, options, range_closest);
			end = clock();
			write_entry(file, "pclst+", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 13)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tpclst+\n");
		}

		// Hochbaum: random
		if (bitset_contains(settings->algorithms, 'n' - 97)) {
			options->get_first = hochbaum_start_random;
			for (int i = 0; i < settings->rand_iter - 1; i++) {
				start = clock();
				res = hochbaum(G, k, options);
				end = clock();
				write_entry(file, "hrand", res, k, get_time(start, end));
				fprintf(file, ", ");
				if (res->score < scoremin)
					scoremin = res->score;
				result_free(res);
			}

			start = clock();
			res = hochbaum(G, k, options);
			end = clock();
			write_entry(file, "hrand", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 14)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\thrand\n");
		}

		// Hochbaum: best
		if (bitset_contains(settings->algorithms, 'o' - 97)) {
			options->get_first = hochbaum_start_best;
			start = clock();
			res = hochbaum(G, k, options);
			end = clock();
			write_entry(file, "hbest", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 15)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\thbest\n");
		}

		// Hochbaum: plus
		if (bitset_contains(settings->algorithms, 'p' - 97)) {
			start = clock();
			res = hochbaum_plus(G, k, options);
			end = clock();
			write_entry(file, "h+", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 16)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\th+\n");
		}

		// Greedy: centers
		if (bitset_contains(settings->algorithms, 'q' - 97)) {
			start = clock();
			res = greedy_centers(G, k, options);
			end = clock();
			write_entry(file, "greedy", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 17)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tgreedy\n");
		}

		// Cluster: Cardinality
		if (bitset_contains(settings->algorithms, 'r' - 97)) {
			start = clock();
			res = cluster_cardinality(G, k, options);
			end = clock();
			write_entry(file, "card", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 18)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tcard\n");
		}

		// Cluster: Cardinality - Basic
		if (bitset_contains(settings->algorithms, 's' - 97)) {
			start = clock();
			res = cluster_cardinality_basic(G, k, options);
			end = clock();
			write_entry(file, "cardb", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 19)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tcardb\n");
		}

		// Cluster: Radius
		if (bitset_contains(settings->algorithms, 't' - 97)) {
			start = clock();
			res = cluster_radius(G, k, options);
			end = clock();
			write_entry(file, "radius", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 20)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tradius\n");
		}

		// Cluster: Radius - Basic
		if (bitset_contains(settings->algorithms, 'u' - 97)) {
			start = clock();
			res = cluster_radius_basic(G, k, options);
			end = clock();
			write_entry(file, "radiusb", res, k, get_time(start, end));
			if ((settings->algorithms->set[0] < (1 << 21)) && (k + kstep) >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tradiusb\n");
		}

		// Greedy: basic
		if (bitset_contains(settings->algorithms, 'v' - 97)) {
			start = clock();
			res = greedy_basic(G, k, options);
			end = clock();
			write_entry(file, "greedy_basic", res, k, get_time(start, end));
			if (k + kstep >= kend)
				fprintf(file, "]}");
			else
				fprintf(file, ", ");
			if (res->score < scoremin)
				scoremin = res->score;
			result_free(res);
			if (settings->notifications)
				printf("\tgreedy_basic\n");
		}

		//scores[(k - kstart) / kstep] = scoremin;
	}

	return scores;
}