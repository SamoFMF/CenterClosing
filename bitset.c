#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "bitset.h"

BitSet* bitset_new(int n) {
	BitSet* bitset;
	if ((bitset = malloc(sizeof * bitset)) != NULL) {
		bitset->n = n;
		int size = 8 * sizeof(set_type);
		int N = (size - 1 + n) / size;
		bitset->N = N;
		bitset->size = size;
		bitset->numOfElements = 0;
		if ((bitset->set = calloc(N, sizeof(set_type))) == NULL)
			printf("ERROR - Ran out of memory: bitset_new - bitset->set\n");
		/*if ((bitset->set = malloc(N * sizeof(set_type))) != NULL) {
			for (int i = 0; i < bitset->N; i++) {
				bitset->set[i] = 0;
			}
		}
		else {
			printf("ERROR - Ran out of memory: bitset_new - bitset->set");
		}*/
	}
	else {
		printf("ERROR - Ran out of memory: bitset_new - bitset\n");
	}
	return bitset;
}

BitSet* bitset_new_full(int n) {
	BitSet* bitset;
	if ((bitset = malloc(sizeof * bitset)) != NULL) {
		bitset->n = n;
		int size = 8 * sizeof(*bitset->set);
		int N = (size - 1 + n) / size;
		bitset->N = N;
		bitset->size = size;
		bitset->numOfElements = n;
		if ((bitset->set = malloc(N * sizeof(*bitset->set))) != NULL) {
			for (int i = 0; i < bitset->N; i++) {
				bitset->set[i] = (set_type)-1;
			}
		}
		else {
			printf("ERROR - Ran out of memory: bitset_new_full - bitset->set");
		}
	}
	else {
		printf("ERROR - Ran out of memory: bitset_new_full - bitset");
	}
	return bitset;
}

void bitset_free(BitSet* bitset) {
	if (bitset->set != NULL) free(bitset->set);
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
	if (i < bitset->n && !bitset_contains(bitset, i)) {
		int block = i / bitset->size;
		int pos = i % bitset->size;
		bitset->set[block] |= (set_type)1 << pos;
		bitset->numOfElements++;
	}
}

void bitset_add_from(BitSet* bitset, int* xs, int n) {
	for (int i = 0; i < n; i++) {
		bitset_add(bitset, xs[i]);
	}
}

void bitset_add_from_idxs(BitSet* boolset, int* xs, int* idxs, int n) {
	for (int i = 0; i < n; i++) {
		bitset_add(boolset, xs[idxs[i]]);
	}
}

void bitset_remove(BitSet* bitset, int i) {
	if (i < bitset->n && bitset_contains(bitset, i)) {
		int block = i / bitset->size;
		int pos = i % bitset->size;
		bitset->set[block] &= ~((set_type)1 << pos);
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

void bitset_soft_copy(BitSet* copyTo, BitSet* copyFrom) { // Assume n, N and size are the same
	for (int i = 0; i < copyTo->N; i++)
		copyTo->set[i] = copyFrom->set[i];
	copyTo->numOfElements = copyFrom->numOfElements;
}