#ifndef RESULT_TO_JSON_H
#define RESULT_TO_JSON_H

#include <stdio.h>
#include "algutils.h"

void write_entry(FILE* file, char* algorithm, Result* res, int k, double t);

#endif