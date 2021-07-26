#ifndef PRIORITYFUNCTIONS_H
#define PRIORITYFUNCTIONS_H

#include "algutils.h"

typedef struct PriorityFunction {
	double (*single)(Center* center);
	double* (*overall)(Graph* G, Center** centers);
} PriorityFunction;

PriorityFunction* priority_functions_new(double (*single)(Center* center), double* (*overall)(Graph* G, Center** centers));

double priority_eval_center(Center* center);

double* priority_eval(Graph* G, Center** centers);

double priority_num_closest_center(Center* center);

double* priority_num_closest(Graph* G, Center** centers);

#endif