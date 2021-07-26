#ifndef BITSET_H
#define BITSET_H

typedef struct BitSet {
	unsigned long* set;
	int n; // number of values (bits)
	int N; // number of unsigned longs in set
	int numOfElements; // Number of bits equal 1 (TODO - currently not correct if adding an element that's already in ... check bitset_add/remove)
	int size; // size of set entry
} BitSet;

BitSet* bitset_new(int n);

void bitset_free(BitSet* bitset);

int bitset_contains(BitSet* bitset, int i);

void bitset_add(BitSet* bitset, int i);

void bitset_add_from(BitSet* boolset, int* xs, int n);

void bitset_add_from_idxs(BitSet* boolset, int* xs, int* idxs, int n);

void bitset_remove(BitSet* bitset, int i);

void bitset_remove_from(BitSet* bitset, int* xs, int n);

void bitset_remove_from_idxs(BitSet* bitset, int* xs, int* idxs, int n);



#endif