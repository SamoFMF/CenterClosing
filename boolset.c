#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "boolset.h"


// BOOLSET
BoolSet * boolset_new(int n) {
	BoolSet* boolset;
	if ((boolset = malloc(sizeof * boolset)) != NULL) {
		boolset->n = n;
		if ((boolset->set = malloc(n * sizeof(bool))) != NULL) {
			for (int i = 0; i < n; i++) {
				boolset->set[i] = false;
			}
		}
	}
	return boolset;
}

void boolset_add(BoolSet *boolset, int u) {
	boolset->set[u] = true;
}

void boolset_add_from(BoolSet* boolset, int* xs, int n) {
	for (int i = 0; i < n; i++) {
		boolset_add(boolset, xs[i]);
	}
}

void boolset_add_from_idxs(BoolSet* boolset, int* xs, int* idxs, int n) {
	int idx;
	for (int i = 0; i < n; i++) {
		idx = idxs[i];
		boolset_add(boolset, xs[idx]);
	}
}

void boolset_remove(BoolSet *boolset, int u) {
	boolset->set[u] = false;
}

void boolset_remove_from(BoolSet* boolset, int* xs, int n) {
	for (int i = 0; i < n; i++) {
		boolset_remove(boolset, xs[i]);
	}
}

void boolset_remove_from_idxs(BoolSet* boolset, int* xs, int* idxs, int n) {
	for (int i = 0; i < n; i++) {
		boolset_remove(boolset, xs[idxs[i]]);
	}
}

bool boolset_contains(BoolSet* boolset, int i) {
	return boolset->set[i];
}

void boolset_free(BoolSet* boolset) {
	free(boolset->set);
	free(boolset);
}

// BOOLSET ITER
BoolSetIter * boolset_iter_new(BoolSet* boolset) {
	BoolSetIter* boolset_iter;
	if ((boolset_iter = malloc(sizeof * boolset_iter)) != NULL) {
		boolset_iter->boolset = boolset;
	}
	return boolset_iter;
}

int boolset_iter_next(BoolSetIter* boolset_iter) {
	for (int i = boolset_iter->i; i < boolset_iter->boolset->n; i++) {
		if (boolset_contains(boolset_iter->boolset, i)) {
			boolset_iter->i = i + 1;
			return i;
		}
	}
	return NULL;
}