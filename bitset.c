#include <stdio.h>
#include <stdlib.h>
#include "bitset.h"

BitSet* bitset_new(int n) {
	BitSet* bitset;
	if ((bitset = malloc(sizeof * bitset)) != NULL) {
		bitset->n = n;
		int size = 8 * sizeof(unsigned long);
		int N = (size - 1 + n) / size;
		bitset->N = N;
		bitset->size = size;
		bitset->numOfElements = 0;
		if ((bitset->set = malloc(N * sizeof(unsigned long))) != NULL) {
			for (int i = 0; i < bitset->N; i++) {
				bitset->set[i] = 0;
			}
		}
	}
	return bitset;
}

void bitset_free(BitSet* bitset) {
	free(bitset->set);
	free(bitset);
}

int bitset_contains(BitSet* bitset, int i) {
	if (i < bitset->n) {
		int block = i / bitset->size;
		int pos = i % bitset->size;
		return 1 & (bitset->set[block] >> pos);
	}
	else {
		return 0;
	}
}

void bitset_add(BitSet* bitset, int i) {
	if (i < bitset->n) {
		int block = i / bitset->size;
		int pos = i % bitset->size;
		bitset->set[block] |= (unsigned long)1 << pos;
		bitset->numOfElements++;
	}
}

void bitset_add_from(BitSet* boolset, int* xs, int n) {
	for (int i = 0; i < n; i++) {
		bitset_add(boolset, xs[i]);
	}
}

void bitset_add_from_idxs(BitSet* boolset, int* xs, int* idxs, int n) {
	for (int i = 0; i < n; i++) {
		bitset_add(boolset, xs[idxs[i]]);
	}
}

void bitset_remove(BitSet* bitset, int i) {
	if (i < bitset->n) {
		int block = i / bitset->size;
		int pos = i % bitset->size;
		bitset->set[block] &= ~((unsigned long)1 << pos);
		bitset->numOfElements--;
	}
}

void bitset_remove_from(BitSet* bitset, int* xs, int n) {
	for (int i = 0; i < n; i++) {
		bitset_remove(bitset, xs[i]);
	}
}

void bitset_remove_from_idxs(BitSet* bitset, int* xs, int* idxs, int n) {
	for (int i = 0; i < n; i++) {
		bitset_remove(bitset, xs[idxs[i]]);
	}
}