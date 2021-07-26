#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "bitset.h"
#include "algutils.h"

void result_free(Result* result) {
    if (result->R != NULL) {
        bitset_free(result->R);
    }
    free(result);
}

// Result related
double eval_score(Graph* G, BitSet* R) {
    int u, v;
    double mind, maxd;
    maxd = 0.0;
    for (int iu = 0; iu < G->n; iu++) {
        u = G->V[iu];
        mind = -1;
        for (int iv = 0; iv < G->m; iv++) {
            v = G->C[iv];
            if (!bitset_contains(R, v)) {
                if (mind < 0 || G->D[u][v] < mind) {
                    mind = G->D[u][v];
                }
            }
        }
        if (mind > maxd) maxd = mind;
    }
    return maxd;
}

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

Center** centers_new_from_graph(Graph* G) {
    Center** centers;
    if ((centers = malloc(G->m * sizeof * centers)) == NULL) {
        printf("ERROR - Ran out of memory: centers_new_from_graph - centers");
        return centers;
    }
    for (int i = 0; i < G->m; i++) {
        centers[i] = center_new(G->n);
    }

    double dist, min_dist;
    int min_idx;
    int u, v;
    for (int i = 0; i < G->n; i++) {
        u = G->V[i];
        min_dist = INT_MAX;
        for (int j = 0; j < G->m; j++) {
            v = G->C[j];
            dist = G->D[u][v];
            if (dist < min_dist) {
                min_dist = dist;
                min_idx = j;
            }
        }
        v = G->C[min_idx];
        center_add(centers[min_idx], u);
        if (centers[min_idx]->history->val < min_dist) {
            centers[min_idx]->history->val = min_dist;
        }
    }

    for (int i = 0; i < G->m; i++) {
        center_resize(centers[i]);
    }

    return centers;
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
double centers_redistribute(Center** centers, Graph* G, BitSet* R, int idx) {
    int node, cnode, idxmin;
    double dmin, dist;
    double dmax = -1;
    Center* center;

    int* closest;
    if ((closest = malloc(centers[idx]->numOfNodes * sizeof(int))) == NULL) {
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

    //printf("Step 3\n");
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
void centers_redistribute_closest(Center** centers, Graph* G, BitSet* R, int idx, int* closest) {
    int node, cnode;
    double dist;
    Center* center;

    // Update deltas
    for (int i = 0; i < centers[idx]->numOfNodes; i++) { // TODO - premisli: to bi morda lahko kar direkt naredil, ko se racuna `closest`
        centers[closest[i]]->delta++;
    }

    //printf("Step 3\n");
    // Resize centers
    for (int i = 0; i < G->m; i++) {
        center = centers[i];
        center->history = linkedlist_next(center->history, center->delta);
        if (i != idx && center->delta > 0) {
            //printf("Step 3.1 %d %d\n", i, center->delta);
            //realloc(center->nodes, (center->numOfNodes + center->delta) * sizeof * center->nodes);
            center_resize(centers[i]);
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
        }
    }
}