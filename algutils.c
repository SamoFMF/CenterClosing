#include <stdio.h>
#include <stdlib.h>
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
        result->score = INT_MAX;
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
    int c, s;
    double mind, maxd, d;
    maxd = -1;
    for (int ic = 0; ic < G->n; ic++) {
        c = G->C[ic];
        mind = INT_MAX;
        for (int is = 0; is < G->m; is++) {
            s = G->S[is];
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
        printf("ERROR - center_resize");
    }
}

void center_add(Center* center, int node) {
    center->nodes[center->numOfNodes++] = node;
}

//Center** centers_new_from_graph(Graph* G) {
//    Center** centers;
//    if ((centers = malloc(G->m * sizeof * centers)) == NULL) {
//        printf("ERROR - Ran out of memory: centers_new_from_graph - centers");
//        return centers;
//    }
//    for (int i = 0; i < G->m; i++) {
//        centers[i] = center_new(G->n);
//    }
//
//    double dist, min_dist;
//    int min_idx;
//    int u, v;
//    for (int i = 0; i < G->n; i++) {
//        u = G->C[i];
//        min_dist = INT_MAX;
//        for (int j = 0; j < G->m; j++) {
//            v = G->C[j];
//            dist = G->D[u][v];
//            if (dist < min_dist) {
//                min_dist = dist;
//                min_idx = j;
//            }
//        }
//        v = G->C[min_idx];
//        center_add(centers[min_idx], u);
//        if (centers[min_idx]->history->val < min_dist) {
//            centers[min_idx]->history->val = min_dist;
//        }
//    }
//
//    for (int i = 0; i < G->m; i++) {
//        center_resize(centers[i]);
//    }
//
//    return centers;
//}

Center** centers_new_from_graph(Graph* G, Options* options) {
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
        dmin = INT_MAX;
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

    for (int i = 0; i < G->m; i++)
        center_resize(centers[i]);

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
        dmin = INT_MAX;

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

double centers_redistribute_outdated(Center** centers, Graph* G, BitSet* R, int idx) { // TODO - outdated, update with options etc.
    int node, cnode, idxmin;
    double dmin, dist;
    double dmax = -1;
    Center* center;

    int* closest;
    if ((closest = malloc(centers[idx]->numOfNodes * sizeof * closest)) == NULL) {
        printf("ERROR: Ran out of memory - centers_redistribute - closest");
    }

    // Update deltas
    for (int i = 0; i < centers[idx]->numOfNodes; i++) {
        node = centers[idx]->nodes[i];
        dmin = INT_MAX;

        // Find new closest
        for (int j = 0; j < G->m; j++) {
            if (j == idx) continue;
            cnode = G->C[j];
            if (!bitset_contains(R, cnode)) {
                dist = G->D[node][cnode];
                if (dist < dmin) {
                    dmin = dist;
                    idxmin = j;
                }
            }
        }
        centers[idxmin]->delta++;
        closest[i] = idxmin;
    }

    // Resize centers
    for (int i = 0; i < G->m; i++) {
        center = centers[i];
        center->history = linkedlist_next(center->history, center->delta);
        if (i != idx && center->delta > 0) {
            //printf("Step 3.1 %d %d\n", i, center->delta);
            //realloc(center->nodes, (center->numOfNodes + center->delta) * sizeof * center->nodes);
            center_resize(center);
            //printf("Step 3.2 %d %d\n", i, center->delta);
            centers[i]->delta = 0;
        }
    }

    //printf("Step 4\n");

    // Add new nodes
    for (int i = 0; i < centers[idx]->numOfNodes; i++) {
        center = centers[closest[i]];
        cnode = G->C[closest[i]];
        node = centers[idx]->nodes[i];
        // center_add(center, centers[idx]->nodes[i]);
        center_add(center, node);
        dist = G->D[node][cnode];
        if (dist > center->history->val) {
            center->history->val = dist;
            if (dist > dmax) {
                dmax = dist;
            }
        }
    }
    
    return dmax; // TODO - premisli, ce je dovolj, da je to max od novih vrednosti, ki pokvarijo trenutno vrednost, ali potrebujemo vse
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
        icenter = closest[inode]; // TODO - premisli, ce je to okej. Ce je, morda naredi preko indeksov in zmanjsas closest?
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