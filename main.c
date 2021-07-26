#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bruteforce.h"
//#include "boolset.h"
#include "bitset.h"
#include "graph.h"
#include "binaryheap.h"
#include "utils.h"
#include "graphgenerators.h"
#include "exact.h"
#include "algutils.h"
#include "greedy.h"
#include "priorityfunctions.h"
#include "hochbaum.h"
#include "plesnik.h"

int create_D(double** D) {
    double x = 100;
    double y = 0.95 * x;
    double Db[8][8] = { {0, x, x, x, x / 10, y, y, y},{x, 0, x, x, y, x / 10, y, y},{x, x, 0, x, y, y, x / 10, y},{x, x, x, 0, y, y, y, x / 10},{x / 10, y, y, y, y, y, y, y},{y, x / 10, y, y, y, y, y, y},{y, y, x / 10, y, y, y, y, y},{y, y, y, x / 10, y, y, y, y} };
    int n = 8;
    D = malloc(n * sizeof(*D));
    for (int i = 0; i < n; i++) {
        D[i] = malloc(n * sizeof(*D[i]));
        for (int j = 0; j < n; j++) {
            D[i][j] = Db[i][j];
        }
    }
    return n;
}

void myfree(double** D, int n) {
    for (int i = 0; i < n; i++) {
        free(D[i]);
    }
    free(D);
}

void print_D(double** D, int n) {
    printf("D:\n");
    for (int i = 0; i < n; i++) {
        printf("[");
        for (int j = 0; j < n; j++) {
            printf("%f, ", D[i][j]);
        }
        printf("]\n");
    }
}

void print_graph(Graph* G) {
    printf("N = %d || n = %d || m = %d\n", G->N, G->n, G->m);
    printf("V: [");
    for (int i = 0; i < G->n; i++) {
        printf("%d, ", G->V[i]);
    }
    printf("]\n");
    printf("C: [");
    for (int i = 0; i < G->m; i++) {
        printf("%d, ", G->C[i]);
    }
    printf("]\n");
    print_D(G->D, G->N);
}

int main() {
    // Testing centers_redistribution
    int n1;
    double** D1 = read_pajek_dist("data/moj3.net", &n1);
    //int n1 = 6;
    Graph* G1 = graph_new(D1, n1);
    int Cb1[3] = { 0, 1, 2 };
    BitSet* C1 = bitset_new(n1);
    int m1 = 3;
    int k1 = 2;
    int Rb1[1] = { 0 };
    BitSet* R1 = bitset_new(n1);
    bitset_add_from(C1, Cb1, m1);
    bitset_add_from(R1, Rb1, 1);
    graph_add_from_centers(G1, C1, m1);

    printf("Step 1\n");
    Center** centers1 = centers_new_from_graph(G1);
    printf("Step 2\n");
    centers_redistribute(centers1, G1, R1, 0);
    for (int i = 0; i < 3; i++) {
        printf("Center %d:\n[", G1->C[i]);
        for (int j = 0; j < centers1[i]->numOfNodes; j++) {
            printf("%d ", centers1[i]->nodes[j]);
        }
        printf("]\n");
    }

    printf("\n\n");

    int nodes[10] = { 0,1,2,3,4,5,6,7,8,9 };
    double vals[10] = { 3, 10, 540, 30, 20, 100, 1, 96, 99, 2000 };
    BinaryHeap* heap = binary_heap_new(nodes, vals, 10, 10);
    printf("HEAP:\n");
    for (int i = 0; i < 10; i++) {
        printf("(%d,%f) ", heap->nodes[i], heap->heap[i]);
    }
    printf("\n\n");
    PairIntDouble res1;
    for (int i = 0; i < 5; i++) {
        res1 = binary_heap_extract_min(heap);
        printf("%d: %f\n", res1.node, res1.value);
    }
    printf("\n");
    binary_heap_insert(heap);
    binary_heap_insert(heap);
    printf("HEAP:\n");
    for (int i = 0; i < 10; i++) {
        printf("(%d,%f) ", heap->nodes[i], heap->heap[i]);
    }
    printf("\n\n");
    binary_heap_free(heap);



    /*double x = 100;
    double y = 0.95 * x;
    double Db[8][8] = { {0, x, x, x, x / 10, y, y, y},{x, 0, x, x, y, x / 10, y, y},{x, x, 0, x, y, y, x / 10, y},{x, x, x, 0, y, y, y, x / 10},{x / 10, y, y, y, 0, y, y, y},{y, x / 10, y, y, y, 0, y, y},{y, y, x / 10, y, y, y, 0, y},{y, y, y, x / 10, y, y, y, 0} };
    int n = 8;
    double** D;
    D = malloc(n * sizeof(*D));
    for (int i = 0; i < n; i++) {
        D[i] = malloc(n * sizeof(*D[i]));
        for (int j = 0; j < n; j++) {
            D[i][j] = Db[i][j];
        }
    }

    Graph* G = graph_new(D, n);
    int Cb[4] = { 0,1,2,4 };
    BitSet* C = bitset_new(n);
    int m = 4;
    int k = 3;
    bitset_add_from(C, Cb, m);

    graph_add_from_centers(G, C, m);*/




    //double** D;
    //int n = 150;
    //D = read_pajek_dist("data/moj2.net", &n);
    //printf("START\n");
    ////D = create_random_distances(n);

    //printf("DONE DONE \n\n");
    ////print_D(D, n);

    //Graph* G = graph_new(D, n);

    ////int Cb[4] = { 0, 1, 2, 4 };
    //int Cb[30];
    //for (int i = 0; i < 30; i++) {
    //    Cb[i] = i;
    //}
    //BitSet* C = bitset_new(n);
    //int m = 30;
    //int k = 15;
    //bitset_add_from(C, Cb, m);

    //graph_add_from_centers(G, C, m);

    /*BitSet* R = bitset_new(G->N);
    int Rb[7] = { 0,1,2,3,5,6,7 };
    bitset_add_from(R, Rb, 7);*/


    //double** D;
    //int n;
    //D = read_pajek_dist("data/circle.net", &n);
    //printf("START\n");
    ////D = create_random_distances(n);

    //printf("DONE DONE \n\n");
    ////print_D(D, n);

    //Graph* G = graph_new(D, n);

    //int Cb[4] = { 0, 3, 6, 9 };
    //BitSet* C = bitset_new(n);
    //int m = 4;
    //int k = 3;
    //bitset_add_from(C, Cb, m);

    //graph_add_from_centers(G, C, m);

    int n = 301;
    Graph* G = create_circle_graph(n, 1000, 1);
    n -= (n - 1) % 3;
    int k = n / 3;

    PriorityFunction* priority_function = priority_functions_new(priority_eval_center, priority_eval);
    //PriorityFunction* priority_function = priority_functions_new(priority_num_closest_center, priority_num_closest);




    /*for (int ui = 0; ui < 150; ui++) {
        printf("%d: [", ui);
        for (int i = 0; i < 150; i++) {
            if (bitset_contains(C, i) && !bitset_contains(R, i)) printf("%.2f ", D[ui][i]);
        }
        printf("]\n");
    }
    printf("\n\n");*/

    /*for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%f ", D[i][j]);
        }
        printf("\n");
    }
    for (int i = 0; i < 20; i++) {
        printf("%f ", D[27][i]);
    }*/

    printf("\n\n");

    //printf("Eval = %f\n", eval_score(G, R));

    //print_graph(G);
    //printf("\n");
    //

    printf("BRUTEFORCING\n");
    
    clock_t start, end;
    double cpu_time_used;

    /*start = clock();
    Result res = bruteforce(G, k);
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Bruteforce done in %f\n", cpu_time_used);

    printf("\n[");
    for (int i = 0; i < k; i++) {
        printf("%d ", res.R[i]);
    }
    printf("]\n");

    printf("RESULT = %f \n", res.score);*/

    printf("\n\nSTART OF EXACT:\n");

    start = clock();
    Result res_exact = exact_final(G, k, priority_function);
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Exact done in %f\n", cpu_time_used);

    printf("\n[");
    for (int i = 0; i < k; i++) {
        printf("%d ", res_exact.R[i]);
    }
    printf("]\n");
    printf("RESULT EXACT = %f\n", res_exact.score);

    // GREEDY BASIC
    /*printf("\n\nSTART OF GREEDY:\n");

    start = clock();
    Result res_greedy = greedy_basic(G, k);
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Greedy done in %f\n", cpu_time_used);

    printf("\n[");
    for (int i = 0; i < k; i++) {
        printf("%d ", res_greedy.R[i]);
    }
    printf("]\n");
    printf("RESULT GREEDY = %f\n", res_greedy.score);*/

    // GREEDY CENTERS
    printf("\n\nSTART OF GREEDY:\n");

    start = clock();
    Result res_greedy2 = greedy_centers(G, k);
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Greedy done in %f\n", cpu_time_used);

    printf("\n[");
    for (int i = 0; i < k; i++) {
        printf("%d ", res_greedy2.R[i]);
    }
    printf("]\n");
    printf("RESULT GREEDY 2 = %f\n", res_greedy2.score);

    // HOCHBAUM
    printf("\n\nSTART OF HOCHBAUM:\n");

    start = clock();
    Result res_hochbaum = hochbaum(G, k, random_start);
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Hochbaum done in %f\n", cpu_time_used);

    printf("\n[");
    for (int i = 0; i < k; i++) {
        printf("%d ", res_hochbaum.R[i]);
    }
    printf("]\n");
    printf("RESULT HOCHBAUM = %f\n", res_hochbaum.score);

    // PLESNIK
    printf("\n\nSTART OF PLESNIK:\n");

    start = clock();
    Result res_plesnik = plesnik(G, k, 1e-8);
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Plesnik done in %f\n", cpu_time_used);

    printf("\n[");
    for (int i = 0; i < k; i++) {
        printf("%d ", res_plesnik.R[i]);
    }
    printf("]\n");
    printf("RESULT PLESNIK = %f\n", res_plesnik.score);



   /* int is[1] = { 0 };
    int C[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };*/

    //bool R[4] = { false, false, true, false };
    //
    ///*double res = eval_score(D, is, C, n, 4, 1);
    //printf("RESITEV = %f\n\n", res);*/

    //double r = bruteforce(D, C, n, 8, 4);
    //printf("RESITEV = %f\n\n", r);

    //BoolSet* bs = boolset_new(5);
    //boolset_add(bs, 3);
    //for (int i = 0; i < 5; i++) {
    //    printf("%d ", bs->set[i] ? 1 : 0);
    //}
    return 0;
}