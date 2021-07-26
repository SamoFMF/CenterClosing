#ifndef BOOLSET_H
#define BOOLSET_H

#include <stdbool.h>

typedef struct BoolSet {
	bool* set;
	int n;
} BoolSet;

typedef struct BoolSetIter {
	BoolSet* boolset;
	int i, imax;
} BoolSetIter;

BoolSet* boolset_new(int n);

void boolset_add(BoolSet *boolset, int u);

void boolset_add_from(BoolSet *boolset, int* xs, int n);

void boolset_add_from_idxs(BoolSet* boolset, int* xs, int* idxs, int n);

void boolset_remove(BoolSet *boolset, int u);

void boolset_remove_from(BoolSet *boolset, int* xs, int n);

void boolset_remove_from_idxs(BoolSet* boolset, int* xs, int* idxs, int n);

void boolset_free(BoolSet *boolset);

bool boolset_contains(BoolSet* boolset, int i);

BoolSetIter* boolset_iter_new(BoolSet* boolset);

int boolset_iter_next(BoolSetIter* boolset_iter);

#endif