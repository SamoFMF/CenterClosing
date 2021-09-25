#ifndef TESTER_H
#define TESTER_H

#include <stdio.h>
#include "graph.h"
#include "algutils.h"

double* test_approximate(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file);

void test_exact(Graph* G, Options* options, int kstart, int kend, int kstep, double* scores, FILE* file);

void test_backtracking(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file);

void test_backtracking_decision(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file);

void test_bruteforce(Graph* G, Options* options, int kstart, int kend, int kstep, FILE* file);

#endif