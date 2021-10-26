#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bruteforce.h"
#include "bitset.h"
#include "graph.h"
#include "binaryheap.h"
#include "utils.h"
#include "graphgenerators.h"
#include "exact.h"
#include "algutils.h"
#include "greedy.h"
#include "priorityfunctions.h"
#include "hochbaum.h"
#include "plesnik.h"
#include "backtracking.h"
#include "heuristic.h"
#include "result_to_json.h"
#include "tester.h"
#include "setcover.h"
#include "independantset.h"

#define NUM_ALGS 22

char* concat(const char* s1, const char* s2, int n)
{
    const size_t len = strlen(s2);
    char* res = malloc(n + len + 1);
    memcpy(res, s1, n);
    memcpy(res + n, s2, len + 1);
    return res;
}

void usage() {
    printf("NAROBE!\n");
    exit(8);
}

//int main_tmp(int argc, char* argv[]) {
//    if (argc < 2) {
//        printf("No input!\n");
//        usage();
//    }
//    char* programname = argv[0];
//    size_t filename_len = strlen(argv[1]);
//    size_t len_in;
//    size_t len_folder = 0;
//    for (len_in = 0; len_in < filename_len; len_in++) {
//        if (argv[1][len_in] == '/')
//            len_folder = len_in;
//        if (argv[1][len_in] == '.')
//            break;
//    }
//    if (len_folder > 0)
//        len_folder++;
//    len_in -= len_folder;
//    char* folder = malloc(len_folder + 1);
//    memcpy(folder, argv[1], len_folder);
//    char* filename = malloc(len_in + 1);
//    memcpy(filename, argv[1] + len_folder, len_in);
//    char* filename_type = malloc(filename_len - len_in + 1);
//    memcpy(filename_type, argv[1] + len_folder + len_in, filename_len - len_in - len_folder + 1);
//    char* folder_out;
//    size_t len_folder_out;
//    int missing_flag_f = 1;
//    argc--;
//    argv++;
//
//    // Algorithms
//    BitSet* algorithms = bitset_new(NUM_ALGS);
//    int missing_flag_a = 1;
//
//    // Info
//    int input_type = 0;
//    int input_iterate = 0;
//    int input_iterate_start = 0;
//    int notifications = 0;
//
//    // Settings
//    int weighted = 0;
//    int kflag = 0;
//    int rflag = 0;
//    int range[3] = { 0,0,0 };
//    int rand_iter = 1;
//
//    // Helpers
//    int double_step;
//    char* end;
//
//    while ((argc > 1) && (argv[1][0] == '-')) {
//        double_step = 0;
//        switch (argv[1][1]) {
//        case 'a':
//            double_step = 1;
//            missing_flag_a = 0;
//            for (int i = 0; argv[2][i] != '\0'; i++)
//                bitset_add(algorithms, argv[2][i] - 97);
//            argc--;
//            argv++;
//            break;
//        case 't':
//            input_type = strtol(argv[2], &end, 10);
//            argc--;
//            argv++;
//            break;
//        case 'i':
//            input_iterate = strtol(argv[2], &end, 10);
//            argc--;
//            argv++;
//            break;
//        case 'j':
//            input_iterate_start = strtol(argv[2], &end, 10);
//            argc--;
//            argv++;
//            break;
//        case 'w':
//            weighted = 1;
//            break;
//        case 'n':
//            notifications = 1;
//            break;
//        case 'k':
//            kflag = strtol(argv[2], &end, 10);
//            argc--;
//            argv++;
//            break;
//        case 'r':
//            rflag = 1;
//            range[0] = strtol(argv[2], &end, 10);
//            range[1] = strtol(argv[3], &end, 10);
//            range[2] = strtol(argv[4], &end, 10);
//            argc -= 3;
//            argv += 3;
//            break;
//        case 's':
//            rand_iter = strtol(argv[2], &end, 10);
//            argc--;
//            argv++;
//            break;
//        case 'f':
//            folder_out = argv[2];
//            missing_flag_f = 0;
//            argc--;
//            argv++;
//            break;
//        default:
//            usage();
//        }
//        --argc;
//        ++argv;
//    }
//
//    /*if (missing_flag_a)
//        bitset_add(algorithms, 0);*/
//    if (missing_flag_f) {
//        folder_out = "results/";
//    }
//    len_folder_out = strlen(folder_out);
//
//    char* algorithm_names[NUM_ALGS] = { "bnb", "bf", "bt", "btd", "sc", "bnb+", "hs", "prand", "pfirst", "pclst", "prand+", "pfirst+", "pclst+", "hrand", "hbest", "h+", "greedy", "card", "cardb", "radius", "radiusb", "greedyb" };
//    printf("Running program: %s\n", programname);
//    printf("Using algorithms:");
//    for (int i = 0; i < NUM_ALGS; i++)
//        if (bitset_contains(algorithms, i))
//            printf(" %s", algorithm_names[i]);
//    printf("\n");
//    printf("Using input type: %s\n", input_type == 0 ? "orlib-pmed" : "orlib-cap");
//    printf("Input iteration: ");
//    if (input_iterate == 0)
//        printf("None\n");
//    else {
//        printf("%d", input_iterate);
//        if (input_iterate_start > 0)
//            printf(" from %d", input_iterate_start);
//        printf("\n");
//    }
//    printf("Notifications: ");
//    if (notifications)
//        printf("ON\n");
//    else
//        printf("OFF\n");
//
//    // Parameter k
//    printf("Parameter k: ");
//    int k, kstart, kend, kstep;
//    if (rflag) {
//        kstart = range[0];
//        kend = range[1];
//        kstep = range[2];
//        printf("from %d to %d \\w step %d\n", kstart, kend, kstep);
//    }
//    else if (kflag) { // If -k flag used, overwrite k
//        kstart = kflag;
//        kend = kflag + 1;
//        kstep = 1;
//        printf("%d\n", kstart);
//    }
//    else
//        printf("Default\n");
//
//    printf("\n");
//
//    // Algorithm options
//    Options* options = options_new();
//    options->eval = weighted ? weighted_eval : unweighted_eval;
//    options->get_first = hochbaum_start_best;
//    options->get_priority = priority_eval_center;
//
//    // Settings
//    Settings* settings = settings_new();
//    settings->algorithms = algorithms;
//    settings->rand_iter = rand_iter;
//    settings->notifications = notifications;
//
//    // Run code
//    FILE* file;
//    Graph* G;
//    char* filename_in;
//    char* filename_out;
//    int iterations = input_iterate > 0 ? input_iterate : 1;
//    double* scores;
//    char iterations_string[16];
//
//    // Time
//    clock_t start_t, end_t;
//
//    for (int i = 0; i < iterations; i++) {
//        if (input_iterate == 0) {
//            filename_in = concat(concat(folder, filename, len_folder), filename_type, len_in + len_folder);
//            filename_out = concat(filename, ".json", len_in);
//            //filename_in = concat(filename, filename_type, len_in);
//        }
//        else {
//            sprintf_s(iterations_string, 16, "%d", input_iterate_start + i + 1);
//            filename_in = concat(concat(concat(folder, filename, len_folder), iterations_string, len_in + len_folder), filename_type, len_in + len_folder + strlen(iterations_string));
//            filename_out = concat(concat(filename, iterations_string, len_in), ".json", len_in + strlen(iterations_string));
//            //filename_in = concat(concat(filename, iterations_string, len_in), filename_type, len_in + strlen(iterations_string));
//        }
//        // Output
//        //filename_out = concat(concat(folder, filename_in, strlen(folder)), "_results.out", len_in + strlen(folder));
//        filename_out = concat(concat(folder, folder_out, len_folder), filename_out, len_folder + len_folder_out);
//        printf("Reading from file: %s\n", filename_in);
//        printf("Writing to file: %s\n", filename_out);
//
//        if (input_type == 0) {
//            // read_or_library_pmed
//            G = read_or_library_pmed(filename_in, &k);
//            if (!kflag && !rflag) {
//                kstart = k;
//                kend = k + 1;
//                kstep = 1;
//            }
//        }
//        else if (input_type == 1) {
//            // read_or_library (cap) ... also used for generated powerlaw (barabasi-albert) graphs
//            G = read_or_library(filename_in);
//            if (!kflag && !rflag) {
//                kstart = 1;
//                kend = 2;
//                kstep = 1;
//            }
//        }
//        else {
//            G = read_or_library_pmed(filename_in, &k);
//        }
//
//        if (notifications)
//            printf("Creating G->G\n");
//
//        start_t = clock();
//        graph_add_sorted_adjacency_list(G, options);
//        end_t = clock();
//
//        if (notifications)
//            printf("Opening filename_out\n");
//        fopen_s(&file, filename_out, "w");
//        fprintf(file, "[");
//
//        // Write info about input
//        fprintf(file, "{\"data\": \"info\", \"n\": %d, \"m\": %d, \"krange\": [%d, %d, %d], \"setup\": %d, \"algorithms\": [", G->n, G->m, kstart, kstep, kend, (int)(1000 * get_time(start_t, end_t) + 0.1));
//        int count_algs = 0;
//        for (int i = 0; i < NUM_ALGS; i++)
//            if (bitset_contains(algorithms, i)) {
//                fprintf(file, "\"%s\"", algorithm_names[i]);
//                count_algs++;
//                if (count_algs < algorithms->numOfElements)
//                    fprintf(file, ", ");
//            }
//        fprintf(file, "]}");
//
//        // Run tests
//        if (notifications)
//            printf("RUNNING TESTS\n");
//        if (algorithms->set[0] >= 64) {
//            fprintf(file, ", ");
//            scores = test_approximate(G, options, kstart, kend, kstep, file, settings);
//        }
//        else
//            scores = NULL;
//        if (algorithms->set[0] % 64 > 0) {
//            fprintf(file, ", ");
//            test_exact(G, options, kstart, kend, kstep, file, settings, scores);
//        }
//
//        if (scores != NULL)
//            free(scores);
//
//        fprintf(file, "]");
//        fclose(file);
//        graph_free(G);
//        printf("\n");
//    }
//    if (notifications)
//        printf("DONE\n");
//
//    return 0;
//}