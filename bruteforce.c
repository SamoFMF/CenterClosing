#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "bitset.h"
#include "graph.h"
#include "algutils.h"

void increase_index(int* idxs, int n, int k) {
    int i = k - 1;
    idxs[i]++;
    while (i > 0 && idxs[i] > n - k + i) {
        idxs[--i]++;
    }
    if (idxs[0] <= n-k) {
        for (int j = i + 1; j < k; j++) {
            idxs[j] = idxs[j - 1] + 1;
        }
    }
}

Result* bruteforce(Graph* G, int k, Options* options) {
    BitSet* R = bitset_new(G->m); // Removed centers
    Result* res = result_new();
    double val;
    if (k == 0) {
        val = eval_score(G, R, options);
        result_update(res, val, R, G->S);
        return res;
    }
    else if (k == G->m) {
        for (int i = 0; i < G->m; i++)
            bitset_add(R, i);
        result_update(res, DBL_MAX, R, G->S);
        return res;
    }

    // Init array of indices - used for combinations
    int* idxs;
    if ((idxs = malloc(k * sizeof * idxs)) == NULL) {
        printf("ERROR - Ran out of memory: bruteforce - idxs");
    }
    for (int i = 0; i < k; i++) {
        idxs[i] = i;
    }

    double minv = DBL_MAX;
    int* best_idxs;
    if ((best_idxs = malloc(k * sizeof * best_idxs)) == NULL) {
        printf("ERROR - Ran out of memory: bruteforce - best_idxs");
        exit(1);
    }
    for (; idxs[0] <= G->m - k; increase_index(idxs, G->m, k)) {
        bitset_add_from(R, idxs, k);
        val = eval_score(G, R, options);
        if (val < res->score) {
            result_update(res, val, R, G->S);
        }
        bitset_remove_from(R, idxs, k);
    }

    free(idxs);
    bitset_free(R);
    return res;
}