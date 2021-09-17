#include <stdio.h>
#include <stdlib.h>
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

//double eval_score_old(double **D, int *C, BoolSet *R, int n, int m, int k) {
//    int v;
//    double mind, maxd;
//    maxd = 0.0;
//    for (int u = 0; u < n; u++) {
//        mind = 500000.0; // TODO
//        for (int i = 0; i < m; i++) {
//            // TODO
//            v = C[i];
//            if (!boolset_contains(R, i) && D[u][v] < mind) {
//                mind = D[u][v];
//            }
//        }
//        if (mind > maxd) maxd = mind;
//    }
//    return maxd;
//}

//double eval_score(Graph* G, BitSet* R) {
//    int u, v;
//    double mind, maxd;
//    maxd = 0.0;
//    for (int iu = 0; iu < G->n; iu++) {
//        u = G->V[iu];
//        mind = -1;
//        for (int iv = 0; iv < G->m; iv++) {
//            v = G->C[iv];
//            if (!bitset_contains(R, v)) {
//                if (mind < 0 || G->D[u][v] < mind) {
//                    mind = G->D[u][v];
//                }
//            }
//        }
//        if (mind > maxd) maxd = mind;
//    }
//    return maxd;
//}

//double bruteforce_old(double** D, int* C, int n, int m, int k) {
//    // Init array of indices
//    int* idxs = (int*)malloc(k * sizeof(int));
//    for (int i = 0; i < k; i++) {
//        idxs[i] = i;
//    }
//
//    // Init R (removed centers)
//    BoolSet* R = boolset_new(m);
//
//    // Loop over all combinations
//    double minv = -1;
//    double val;
//    while (idxs[0] <= m - k) {
//        // Check result
//        boolset_add_from(R, idxs, k);
//        val = eval_score(D, C, R, n, m, k);
//        if (val < minv || minv < 0) {
//            minv = val;
//        }
//
//        printf("IDXS: ");
//        for (int i = 0; i < k; i++) {
//            printf("%d ", idxs[i]);
//        }
//        printf("|| R: ");
//        for (int i = 0; i < m; i++) {
//            if (R->set[i]) printf("%d ", i);
//        }
//        printf("|| val: %f\n", val);
//
//        // Increase indices
//        boolset_remove_from(R, idxs, k);
//        increase_index(idxs, m, k);
//    }
//
//    free(idxs);
//    return minv;
//}

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
        result_update(res, INT_MAX, R, G->S);
        return res;
    }

    // Init array of indices - used for combinations
    //int* idxs = (int*)malloc(k * sizeof(int));
    int* idxs;
    if ((idxs = malloc(k * sizeof * idxs)) == NULL) {
        printf("ERROR - Ran out of memory: bruteforce - idxs");
    }
    for (int i = 0; i < k; i++) {
        idxs[i] = i;
    }

    double minv = INT_MAX;
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

// OUTDATED
//Result bruteforce_outdated(Graph* G, int k) {
//    // Init array of indices - used for combinations
//    int* idxs = (int*)malloc(k * sizeof(int));
//    for (int i = 0; i < k; i++) {
//        idxs[i] = i;
//    }
//
//    // Init R (removed centers)
//    BitSet* R = bitset_new(G->N);
//
//    // Loop over all combinations
//    double minv = INT_MAX;
//    double val;
//    int* best_idxs = malloc(k * sizeof(int));
//    Result res = { INT_MAX, NULL };
//    //int j = 0;
//    for (; idxs[0] <= G->m - k; increase_index(idxs, G->m, k)) {
//        // Check result
//        //if (j++ % 1000 == 0) printf("%d\n", j);
//        //printf("%d\n", j++);
//        bitset_add_from_idxs(R, G->C, idxs, k);
//        //printf(" x ");
//        val = eval_score(G, R);
//        //printf(" x\n");
//        if (val < res.score) {
//            res.score = val;
//            save_removed_nodes(&res, R);
//            /*for (int i = 0; i < k; i++) {
//                best_idxs[i] = idxs[i];
//            }*/
//        }
//
//        bitset_remove_from_idxs(R, G->C, idxs, k);
//    }
//
//    /*printf("R = [");
//    for (int i = 0; i < k; i++) {
//        printf("%d ", G->C[best_idxs[i]]);
//    }
//    printf("]\n");*/
//
//    free(idxs);
//    bitset_free(R);
//    return res;
//}