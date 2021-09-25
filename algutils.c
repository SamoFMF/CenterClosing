#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "graph.h"
#include "bitset.h"
#include "algutils.h"

double f(Graph* G, int i_node, int j_center) {
    int node = G->C[i_node];
    int center = G->C[j_center];
    //return G->H[i_node] * G->D[node][center] + G->a[i_node];
    return G->H[i_node] * G->D[node][center];
}

// Options
Options* options_new() {
    Options* options;
    if ((options = malloc(sizeof * options)) == NULL)
        printf("ERROR - Ran out of memory: options_new");
    return options;
}

// Result
Result* result_new() {
    Result* result;
    if ((result = malloc(sizeof * result)) != NULL) {
        result->R = NULL;
        result->score = DBL_MAX;
    }
    else {
        printf("ERROR: Ran out of memory - result_new");
    }
    return result;
}

void result_free(Result* result) {
    if (result->R != NULL)
        free(result->R);
    free(result);
}

void result_update(Result* result, double val, BitSet* R, int* S) {
    if (result->R != NULL)
        free(result->R);
    if ((result->R = malloc(R->n * sizeof * result->R)) != NULL) {
        result->score = val;
        int i = 0;
        for (int j = 0; j < R->n; j++)
            if (bitset_contains(R, j))
                result->R[i++] = S[j];
    }
    else {
        printf("ERROR - Ran out of memory: result_update");
    }
}

// Result related
double eval_score(Graph* G, BitSet* R, Options* options) {
    //int c, s;
    double mind, maxd, d;
    maxd = -1;
    for (int ic = 0; ic < G->n; ic++) {
        //c = G->C[ic];
        mind = DBL_MAX;
        for (int is = 0; is < G->m; is++) {
            //s = G->S[is];
            if (!bitset_contains(R, is)) {
                d = options->eval(ic, is, G);
                if (d < mind)
                    mind = d;
            }
        }
        if (mind > maxd)
            maxd = mind;
    }
    return maxd;
}

//double eval_score(Graph* G, BitSet* R) {
//    int u, v;
//    double mind, maxd;
//    maxd = 0.0;
//    for (int iu = 0; iu < G->n; iu++) {
//        u = G->C[iu];
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

// OUTDATED
void save_removed_nodes(Result* res, BitSet* R) {
    if ((res->R = malloc(R->numOfElements * sizeof * res->R)) != NULL) {
        int i = 0;
        for (int j = 0; j < R->n; j++) {
            if (bitset_contains(R, j)) res->R[i++] = j;
        }
    }
    else {
        res->R = NULL;
    }
}

// Center related
Center* center_new(int n) {
    Center* center;
    if ((center = malloc(sizeof * center)) != NULL) {
        if ((center->nodes = malloc(n * sizeof * center->nodes)) != NULL && (center->history = linkedlist_new()) != NULL) {
            center->numOfNodes = 0;
            center->delta = 0;
        }
        else {
            printf("ERROR - Ran out of memory: center_new - inner");
        }
    }
    else {
        printf("ERROR: Ran out of memory: center_new - outter");
    }
    return center;
}

void center_free(Center* center) {
    free(center->nodes);
    linkedlist_free_all(center->history);
    free(center);
}

void centers_free(Center** centers, int n) {
    for (int i = 0; i < n; i++) {
        center_free(centers[i]);
    }
    free(centers);
}

void center_resize(Center* center) {
    if ((center->nodes = realloc(center->nodes, (center->numOfNodes + center->delta) * sizeof * center->nodes)) == NULL) {
        if (center->numOfNodes + center->delta != 0)
            printf("ERROR - center_resize");
    }
}

void center_add(Center* center, int node) {
    center->nodes[center->numOfNodes++] = node;
}

Center** centers_new_from_graph(Graph* G, Options* options, double* eval) {
    Center** centers;
    if ((centers = malloc(G->m * sizeof * centers)) == NULL) {
        printf("ERROR - Ran out of memory: centers_new_from_graph - centers");
        return centers;
    }
    for (int i = 0; i < G->m; i++) {
        centers[i] = center_new(G->n);
    }

    double dist, dmin;
    int min_idx;
    int c, s;
    for (int i = 0; i < G->n; i++) {
        c = G->C[i];
        dmin = DBL_MAX;
        for (int j = 0; j < G->m; j++) {
            s = G->S[j];
            dist = options->eval(i, j, G);
            if (dist < dmin) {
                dmin = dist;
                min_idx = j;
            }
        }
        s = G->S[min_idx];
        center_add(centers[min_idx], i);
        if (centers[min_idx]->history->val < dmin)
            centers[min_idx]->history->val = dmin;
    }

    double dmax = -1;
    for (int i = 0; i < G->m; i++) {
        center_resize(centers[i]);
        if (centers[i]->history->val > dmax)
            dmax = centers[i]->history->val;
    }
    *eval = dmax;

    return centers;
}

void centers_resize(Center** centers, int n) {
    Center* center;
    for (int i = 0; i < n; i++) {
        center = centers[i];
        center->history = linkedlist_next(center->history, center->delta);
        if (center->delta > 0) { // Prej smo imeli se i != idx, kjer je idx trenutno obravnavan v centers_redistribute, ampak takrat bi mogla vedno biti delta = 0
            center_resize(center);
            center->delta = 0;
        }
    }
}

void centers_resize_oneway(Center** centers, int n) {
    Center* center;
    for (int i = 0; i < n; i++) {
        center = centers[i];
        if (center->delta > 0) { // Prej smo imeli se i != idx, kjer je idx trenutno obravnavan v centers_redistribute, ampak takrat bi mogla vedno biti delta = 0
            center->history = linkedlist_next(center->history, center->delta);
            linkedlist_free(center->history->prev);
            center->history->prev = NULL;
            center_resize(center);
            center->delta = 0;
        }
    }
}

/** @brief Redistribute nodes from center to remaining centers
*
* Redistributes nodes for which i-th center is the closest center, to the
* remaining nodes that are not in R.
*
* @param centers Double-pointer to Center structs.
* @param G Pointer to Graph struct.
* @param R Pointer to BitSet struct containing removed centers.
* @param idx Index of the center whose nodes are being redistributed.
*
* @return void
*/
double centers_redistribute(Center** centers, Graph* G, BitSet* R, int idx, Options* options) {
    int c, s;
    double dist, dmin, dmax;
    Center* center = centers[idx];

    int* closest;
    if ((closest = malloc(center->numOfNodes * sizeof * closest)) == NULL) {
        printf("ERROR - Ran out of memory: centers_redistribute\n");
    }

    // Update deltas & fill in closest
    for (int i = 0; i < center->numOfNodes; i++) {
        c = center->nodes[i];
        dmin = DBL_MAX;

        // Find new closest
        for (int j = 0; j < G->m; j++) {
            if (!bitset_contains(R, j) && j != idx) {
                dist = options->eval(c, j, G);
                if (dist < dmin) {
                    dmin = dist;
                    s = j;
                }
            }
        }
        
        centers[s]->delta++;
        closest[i] = s;
    }

    // Resize centers
    centers_resize(centers, G->m);

    // Redistribute consumers
    dmax = -1;
    for (int i = 0; i < centers[idx]->numOfNodes; i++) {
        c = centers[idx]->nodes[i];
        s = closest[i];
        center = centers[s];
        center_add(center, c);
        dist = options->eval(c, s, G);
        if (dist > center->history->val) {
            center->history->val = dist;
            if (dist > dmax)
                dmax = dist;
        }
    }

    return dmax;
}

void centers_redistribute_undo(Center** centers, int n) {
    Center* center;

    // Iterate over centers
    for (int ci = 0; ci < n; ci++) {
        center = centers[ci];
        center->numOfNodes -= center->history->delta;
        center->history = linkedlist_remove_last(center->history);
        center_resize(center);
    }
}

double centers_redistribute_oneway(Center** centers, Graph* G, BitSet* R, int idx, Options* options) {
    int c, s;
    double dist, dmin, dmax;
    Center* center = centers[idx];

    int* closest;
    if ((closest = malloc(center->numOfNodes * sizeof * closest)) == NULL) {
        printf("ERROR - Ran out of memory: centers_redistribute\n");
    }

    // Update deltas & fill in closest
    for (int i = 0; i < center->numOfNodes; i++) {
        c = center->nodes[i];
        dmin = DBL_MAX;

        // Find new closest
        for (int j = 0; j < G->m; j++) {
            if (!bitset_contains(R, j) && j != idx) {
                dist = options->eval(c, j, G);
                if (dist < dmin) {
                    dmin = dist;
                    s = j;
                }
            }
        }

        centers[s]->delta++;
        closest[i] = s;
    }

    // Resize centers
    centers_resize_oneway(centers, G->m);

    // Redistribute consumers
    dmax = -1;
    for (int i = 0; i < centers[idx]->numOfNodes; i++) {
        c = centers[idx]->nodes[i];
        s = closest[i];
        center = centers[s];
        center_add(center, c);
        dist = options->eval(c, s, G);
        if (dist > center->history->val) {
            center->history->val = dist;
            if (dist > dmax)
                dmax = dist;
        }
    }

    return dmax;
}

/** @brief Redistribute nodes from center to remaining centers
*
* Redistributes nodes for which i-th center is the closest center, to the
* remaining nodes that are not in R.
*
* @param centers Double-pointer to Center structs.
* @param G Pointer to Graph struct.
* @param R Pointer to BitSet struct containing removed centers.
* @param idx Index of the center whose nodes are being redistributed.
* @param closest Integer array: closest[i] = closest center of node at index i
*
* @return void
*/
void centers_redistribute_closest(Center** centers, Graph* G, int idx, int* closest, Options* options) {
    double dist;
    Center* center;

    // Update deltas
    center = centers[idx];
    for (int i = 0; i < center->numOfNodes; i++)
        centers[closest[center->nodes[i]]]->delta++;

    // Resize centers
    /*for (int i = 0; i < G->m; i++) {
        center = centers[i];
        center->history = linkedlist_next(center->history, center->delta);
        if (i != idx && center->delta > 0) {
            center_resize(centers[i]);
            centers[i]->delta = 0;
        }
    }*/
    centers_resize(centers, G->m);

    // Redistribute consumers
    int inode, icenter;
    for (int i = 0; i < centers[idx]->numOfNodes; i++) {
        inode = centers[idx]->nodes[i];
        icenter = closest[inode]; // TODO - premisli, ce je to okej. Ce je, morda naredi preko indeksov in zmanjsas closest? (DONE)
        center = centers[icenter];
        center_add(center, inode);
        dist = options->eval(inode, icenter, G);
        if (dist > center->history->val)
            center->history->val = dist;
    }
}

//void centers_redistribute_closest(Center** centers, Graph* G, BitSet* R, int idx, int* closest) {
//    int node, cnode;
//    double dist;
//    Center* center;
//
//    // Update deltas
//    //for (int i = 0; i < centers[idx]->numOfNodes; i++) { // TODO - premisli: to bi morda lahko kar direkt naredil, ko se racuna `closest`
//    //    centers[closest[i]]->delta++;
//    //}
//    center = centers[idx];
//    for (int i = 0; i < center->numOfNodes; i++)
//        centers[closest[center->nodes[i]]]->delta++;
//
//    //printf("Step 3\n");
//    // Resize centers
//    for (int i = 0; i < G->m; i++) {
//        center = centers[i];
//        center->history = linkedlist_next(center->history, center->delta);
//        if (i != idx && center->delta > 0) {
//            //printf("Step 3.1 %d %d\n", i, center->delta);
//            //realloc(center->nodes, (center->numOfNodes + center->delta) * sizeof * center->nodes);
//            center_resize(centers[i]);
//            //printf("Step 3.2 %d %d\n", i, center->delta);
//            centers[i]->delta = 0;
//        }
//    }
//
//    //printf("Step 4\n");
//
//    // Add new nodes
//    for (int i = 0; i < centers[idx]->numOfNodes; i++) {
//        center = centers[closest[i]];
//        cnode = G->C[closest[i]];
//        node = centers[idx]->nodes[i];
//        // center_add(center, centers[idx]->nodes[i]);
//        center_add(center, node);
//        dist = G->D[node][cnode];
//        if (dist > center->history->val) {
//            center->history->val = dist;
//        }
//    }
//}

// Distance related
int compare_dists(const void* x, const void* y) {
    double f = *((double*)x);
    double s = *((double*)y);
    if (f > s) return 1;
    if (f < s) return -1;
    return 0;
}

double* get_sorted_distances_no_duplicates(Graph* G, int* new_len, Options* options) {
    double* dists;
    int len = G->n * G->m;
    int idx = 0;
    if ((dists = malloc(len * sizeof * dists)) != NULL) {
        for (int i = 0; i < G->n; i++) {
            for (int j = 0; j < G->m; j++) {
                dists[idx++] = options->eval(i, j, G);
            }
        }
        qsort(dists, len, sizeof * dists, compare_dists);
        idx = 0;
        for (int i = 1; i < len; i++) {
            if (dists[i] > dists[idx]) {
                idx++;
                dists[idx] = dists[i];
            }
        }
        if (idx < len) {
            if ((dists = realloc(dists, (idx + 1) * sizeof * dists)) == NULL)
                printf("ERROR - Reallocating memory: get_sorted_distances_no_duplicates");
            else
                *new_len = idx + 1;
        }
    }
    else
        printf("ERROR - Ran out of memory: get_sorted_distances_no_duplicates - dists");
    return dists;
}