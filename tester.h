#ifndef TESTER_H
#define TESTER_H

#include <stdio.h>
#include "graph.h"
#include "algutils.h"
#include "bitset.h"

typedef struct _Settings {
    BitSet* algorithms;
    int rand_iter;
    int notifications;
} Settings;

Settings* settings_new();

void settings_free(Settings* settings);

//double* test_approx(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file);
//
//void test_exact_old(Graph* G, Options* options, int kstart, int kend, int kstep, double* scores, FILE* file);
//
//void test_backtracking(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file);
//
//void test_backtracking_decision(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file);
//
//void test_bruteforce(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file);

// Updated
void test_exact(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file, Settings* settings, double* scores);

double* test_approximate(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file, Settings* settings);

#endif