#ifndef EXACT_H
#define EXACT_H

#include "priorityfunctions.h"

Result exact(Graph* G, int k);

double* get_vals(Graph* G);
Result exact_vals(Graph* G, int k, double* (*f)(Graph* G));

Result exact_final(Graph* G, int k, PriorityFunction* priority_function);

Result exact_centers_k1(Graph* G, BitSet* R, Center** centers);

#endif