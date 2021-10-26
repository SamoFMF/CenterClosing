#ifndef BITSET_H
#define BITSET_H

typedef unsigned long set_type;

typedef struct BitSet {
	set_type* set;
	int n; // number of values (bits)
	int N; // number of set_types in set
	int numOfElements; // Number of bits equal 1
	int size; // size of set entry
} BitSet;

BitSet* bitset_new(int n);

BitSet* bitset_new_full(int n);

void bitset_free(BitSet* bitset);

void bitset_reset(BitSet* bitset);

int bitset_contains(BitSet* bitset, int i);

int bitset_is_subset(BitSet* bitset, BitSet* subset);

int bitset_get_first(BitSet* bitset);

int* bitset_get_elements(BitSet* bitset);

void bitset_add(BitSet* bitset, int i);

void bitset_add_from(BitSet* boolset, int* xs, int n);

void bitset_add_from_idxs(BitSet* boolset, int* xs, int* idxs, int n);

void bitset_remove(BitSet* bitset, int i);

void bitset_remove_from(BitSet* bitset, int* xs, int n);

void bitset_remove_from_idxs(BitSet* bitset, int* xs, int* idxs, int n);

void bitset_soft_copy(BitSet* copyTo, BitSet* copyFrom);

#endif