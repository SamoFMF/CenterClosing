#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "backtracking.h"
#include "heuristic.h"
#include "result_to_json.h"
#include "tester.h"
#include "setcover.h"
#include "independantset.h"

//double get_time(clock_t start, clock_t end) {
//    return ((double)(end - start)) / CLOCKS_PER_SEC;
//}

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
    printf("GRAPH G:\n");
    printf("N = %d || n = %d || m = %d\n", G->N, G->n, G->m);
    if (G->D != NULL) {
        //printf("Distance matrix:\n");
        //print_matrix_double(G->D, G->N);
    }
    if (G->S != NULL) {
        printf("S: ");
        print_array_int(G->S, G->m);
    }
    if (G->C != NULL) {
        printf("C: ");
        print_array_int(G->C, G->n);
    }
    if (G->H != NULL) {
        printf("H: ");
        print_array_double(G->H, G->n);
    }
}

void print_result(char* name, Result* R, int k, double ms) {
    printf("\nRESULT %s (k=%d):\nR: ", name, k);
    print_array_int(R->R, k);
    printf("Val: %f\n", R->score);
    printf("Solved in %4.0fms\n", 1000*ms);
}

//int compare(void* cont, const void* x, const void* y) {
//    //int fx = *((int*)x);
//    //int sx = *((int*)y);
//    double* cont1 = cont;
//    if (cont1 == NULL) printf("NULL!");
//    double f = cont1[*((int*)x)];
//    double s = cont1[*((int*)y)];
//    if (f > s) return 1;
//    if (f < s) return -1;
//    return 0;
//}

int mycompare(void* cont, const void* x, const void* y) {
    double* cont1 = cont;
    if (cont1 == NULL) printf("NULL!");
    double f = cont1[*((int*)x)];
    double s = cont1[*((int*)y)];
    if (f > s) return 1;
    if (f < s) return -1;
    return 0;
}
    
// qsort_s(idxs, G->n, sizeof(int), compare, G->H);

double test_eval(int c, int s, Graph* G) {
    return G->D[G->C[c]][G->S[s]];
}

double test_eval1(int c, int s, Graph* G) {
    return G->H[c] * G->D[G->C[c]][G->S[s]];
}

void print_binary(unsigned long x) {
    int b;
    for (int i = 0; i < 8 * sizeof(unsigned long); i++) {
        b = 1 & x;
        x >>= 1;
        printf("%d ", b);
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    Options* options = options_new();
    //options->eval = unweighted_eval;
    options->eval = weighted_eval;
    options->get_first = hochbaum_start_best;
    options->get_priority = priority_eval_center;

    int k;
    //Graph* G = read_or_library_pmed("or_library/pmed/data/pmed3.txt", &k);
    //Graph* G = read_or_library_pmed("generated/test1.txt", &k);
    Graph* G = read_or_library("test/powerlaw1.txt");
    graph_add_sorted_adjacency_list(G, options);
    k = 1;

    printf("k=%d\n", k);
    clock_t start = clock();
    //Result* res = solve_using_setcover(G, k, options);
    Result* res = independant_set(G, k, options);
    clock_t end = clock();
    printf("score=%lf\n", res->score);
    printf("time=%lf\n\n", get_time(start, end));
    print_result("test", res, k, get_time(start, end));

    /*printf("\n\nSTART:\n");
    char filename[128];
    Result* res1;
    for (int i = 1; i < 31; i++) {
        printf("i=%d\n", i);
        sprintf_s(filename, 128, "generated/test%d.txt", i);
        G = read_or_library_pmed(filename, &k);
        graph_add_sorted_adjacency_list(G, options);
        for (int ki = 1; ki < 5; ki++) {
            printf("\tk=%d\n", ki);
            res = solve_using_setcover(G, G->m - ki, options);
            res1 = exact(G, G->m - ki, options);
            if (res->score != res1->score)
                printf("i=%d || %lf != %lf\n", i, res->score, res1->score);
        }
    }*/

    //BitSet* R = bitset_new_full(G->m);
    //bitset_remove(R, 0);
    //bitset_remove(R, 13);
    //bitset_remove(R, 20);
    //bitset_remove(R, 142);
    //bitset_remove(R, 199);
    //bitset_remove(R, 223);
    //double val = eval_score(G, R, options);
    //printf("val=%lf\n", val);

    //int xs[6] = { 142,199,20,223,0,13 };
    //for (int i = 0; i < 6; i++) {
    //    bitset_add(R, xs[i]);
    //    printf("removed=%d, score=%lf\n", xs[i], eval_score(G, R, options));
    //    bitset_remove(R, xs[i]);
    //}

    //k = 2;
    //start = clock();
    //res = exact_bound(G, k, options, res->score);
    //end = clock();
    ////printf("exact=%lf\n", res->score);
    ////printf("time=%f\n", get_time(start, end));
    ////printf("\n\n");
    //print_result("bruteforce_old", res, k, get_time(start, end));

    //start = clock();
    //res = bruteforce_old(G, k, options);
    //end = clock();
    ////printf("bruteforce=%lf\n", res->score);
    ////printf("time=%f\n", get_time(start, end));
    //print_result("bruteforce_old", res, k, get_time(start, end));

    //start = clock();
    //res = bruteforce(G, 5, options);
    //end = clock();
    ////printf("bruteforce=%lf\n", res->score);
    ////printf("time=%f\n", get_time(start, end));
    //print_result("bruteforce_old", res, 5, get_time(start, end));


    return 0;
}

//int main_old() {
//    Graph* G;
//    Options* options;
//    double* scores;
//    FILE* file;
//    char* filenameIn = malloc(100 * sizeof * filenameIn);
//    char* filenameOut = malloc(100 * sizeof * filenameOut);
//    int k;
//
//    options = options_new();
//    options->eval = test_eval1;
//    options->get_first = hochbaum_start_best;
//    options->get_priority = priority_eval_center;
//
//    /*printf("start ..");
//    for (int i = 1; i < 41; i++) {
//        sprintf_s(filenameIn, 100, "or_library/pmed/data/pmed%d.txt", i);
//        printf("%s\n", filenameIn);
//        G = read_or_library_pmed(filenameIn, &k);
//
//        printf("k=%d\n", k);
//
//        sprintf_s(filenameOut, 100, "or_library/pmed/testing/pmed%d_results.txt", i);
//        fopen_s(&file, filenameOut, "w");
//        fprintf(file, "[");
//        scores = test_approximate(G, options, k, k + 1, 1, file);
//        fprintf(file, "]");
//        fclose(file);
//
//        graph_free(G);
//        free(scores);
//    }*/
//
//    /*G = read_or_library_pmed("or_library/pmed/data/pmed1.txt", &k);
//    Result* res;
//    for (int ki = 1; ki < 30; ki++) {
//        res = exact(G, ki, options);
//        print_result("exact", res, ki, 0.0);
//        result_free(res);
//    }
//
//    printf("Done!\n");*/
//
//    // G = read_or_library("data/wilson_100_200_m50_plus.txt");
//    G = read_pajek("data/tree_270_36315_20.txt");
//    int m = 20;
//    int n = 250;
//    G->m = m;
//    G->n = n;
//    floyd_warshall_algorithm(G->D, G->N);
//    printf("\n");
//    // print_matrix_double(G->D, G->N);
//    G->S = malloc(m * sizeof * G->S);
//    for (int i = 0; i < m; i++)
//        G->S[i] = i;
//    G->C = malloc(n * sizeof * G->C);
//    for (int i = m; i < G->N; i++)
//        G->C[i - m] = i;
//    G->H = malloc(n * sizeof * G->H);
//    for (int i = 0; i < 2; i++)
//        G->H[i] = 5*5;
//    for (int i = 2; i < 24; i++)
//        G->H[i] = 4*4;
//    for (int i = 24; i < 59; i++)
//        G->H[i] = 3*3;
//    for (int i = 59; i < 157; i++)
//        G->H[i] = 2*2;
//    for (int i = 157; i < n; i++)
//        G->H[i] = 1;
//
//    printf("DONE READING!\n");
//    Result* res;
//    clock_t start, end;
//    clock_t start_out = clock();
//    for (int ki = 1; ki < G->m; ki++) {
//        start = clock();
//        res = exact(G, ki, options);
//        end = clock();
//        print_result("tree", res, ki, get_time(start, end));
//    }
//    clock_t end_out = clock();
//    printf("TIME = %f\n", get_time(start_out, end_out));
//
//    // TEST
//    G = read_or_library("data/barabasi_albert_100_10.txt");
//    floyd_warshall_algorithm(G->D, G->N);
//    graph_add_sorted_adjacency_list(G, options);
//    start = clock();
//    res = solve_using_setcover_slow(G, 5, options);
//    end = clock();
//    printf("\n\n");
//    print_result("testing", res, 5, get_time(start, end));
//    printf("\n\n");
//
//    ////////////////////////
//    // SET COVER TEST
//    SetCover* Gsc = malloc(sizeof * Gsc);
//    Gsc->contains = bitset_new_full(5);
//    Gsc->n = 5;
//    Gsc->S = malloc(5 * sizeof * Gsc->S);
//    Gsc->S[0] = bitset_new(13);
//    bitset_add(Gsc->S[0], 0);
//    bitset_add(Gsc->S[0], 1);
//
//    Gsc->S[1] = bitset_new(13);
//    bitset_add(Gsc->S[1], 1);
//    bitset_add(Gsc->S[1], 2);
//    bitset_add(Gsc->S[1], 3);
//    bitset_add(Gsc->S[1], 4);
//
//    Gsc->S[2] = bitset_new(13);
//    bitset_add(Gsc->S[2], 5);
//    bitset_add(Gsc->S[2], 6);
//    bitset_add(Gsc->S[2], 7);
//    bitset_add(Gsc->S[2], 8);
//    bitset_add(Gsc->S[2], 9);
//    bitset_add(Gsc->S[2], 10);
//    bitset_add(Gsc->S[2], 11);
//    bitset_add(Gsc->S[2], 12);
//
//    Gsc->S[3] = bitset_new(13);
//    bitset_add(Gsc->S[3], 0);
//    bitset_add(Gsc->S[3], 2);
//    bitset_add(Gsc->S[3], 4);
//    bitset_add(Gsc->S[3], 6);
//    bitset_add(Gsc->S[3], 8);
//    bitset_add(Gsc->S[3], 10);
//    bitset_add(Gsc->S[3], 12);
//
//    Gsc->S[4] = bitset_new(13);
//    bitset_add(Gsc->S[4], 1);
//    bitset_add(Gsc->S[4], 3);
//    bitset_add(Gsc->S[4], 5);
//    bitset_add(Gsc->S[4], 7);
//    bitset_add(Gsc->S[4], 9);
//    bitset_add(Gsc->S[4], 11);
//    bitset_add(Gsc->S[4], 12);
//
//    BitSet* U = bitset_new_full(13);
//
//    ResultSetCover* MSC = minimum_set_cover(Gsc, U);
//    printf("\nRESULT OF SET COVER:\nlength = %d\n", MSC->length);
//    print_array_int(MSC->C, MSC->length);
//
//
//    ////////////////
//    // TEST: -> domset (set cover)
//    Graph* G1 = graph_new();
//    G1->D = malloc(6 * sizeof * G1->D);
//    for (int i = 0; i < 6; i++)
//        G1->D[i] = malloc(6 * sizeof * G1->D[i]);
//    double temp[6][6] = { {0,2,2,1,1,3},
//                          {2,0,2,1,1,1},
//                          {2,2,0,3,1,1},
//                          {1,1,3,0,2,2},
//                          {1,1,1,2,0,2},
//                          {3,1,1,2,2,0} };
//    for (int i = 0; i < 6; i++) {
//        for (int j = 0; j < 6; j++) {
//            G1->D[i][j] = temp[i][j];
//        }
//    }
//    G1->C = malloc(3 * sizeof * G1->C);
//    G1->C[0] = 0;
//    G1->C[1] = 1;
//    G1->C[2] = 2;
//    G1->S = malloc(3 * sizeof * G1->S);
//    G1->S[0] = 3;
//    G1->S[1] = 4;
//    G1->S[2] = 5;
//    G1->H = malloc(3 * sizeof * G1->H);
//    for (int i = 0; i < 3; i++)
//        G1->H[i] = 1.0;
//    G1->N = 6;
//    G1->n = 3;
//    G1->m = 3;
//    graph_add_sorted_adjacency_list(G1, options);
//
//    printf("DONE\n");
//
//    SetCover* SC1 = graph_to_setcover(G1, options, 1);
//    U = bitset_new_full(6);
//    MSC = minimum_set_cover(SC1, U);
//    printf("\nRESULT OF SET COVER:\nlength = %d\n", MSC->length);
//    print_array_int(MSC->C, MSC->length);
//
//    printf("DONE\n");
//
//    // Primer 2: data/moj1.net
//    printf("\n\n");
//    G = read_pajek("data/moj1.net");
//    BitSet* S_set = bitset_new(G->N);
//    int* S_arr = malloc(4 * sizeof(int));
//    S_arr[0] = 0;
//    S_arr[1] = 1;
//    S_arr[2] = 2;
//    S_arr[3] = 3;
//    bitset_add_from(S_set, S_arr, 4);
//    G->H = malloc(4 * sizeof(double));
//    G->H[0] = 5;
//    G->H[1] = 4;
//    G->H[2] = 3;
//    G->H[3] = 2;
//
//    graph_partition_nodes(G, S_set, 4);
//
//    print_graph(G);
//
//    k = 2;
//    options = options_new();
//    options->eval = test_eval1;
//    options->get_first = hochbaum_start_best;
//    options->get_priority = priority_eval_center;
//
//    graph_add_sorted_adjacency_list(G, options);
//
//    // EDGE CASE TEST
//    res = plesnik_unlimited(G, k, options, range_first);
//    printf("\n\nHERE:");
//    print_result("plesnik", res, k, 0.0);
//    printf("\n\n");
//
//    SC1 = graph_to_setcover(G, options, 190);
//    U = bitset_new_full(G->N);
//    MSC = minimum_set_cover(SC1, U);
//    printf("\nRESULT OF SET COVER:\nlength = %d\n", MSC->length);
//    print_array_int(MSC->C, MSC->length);
//
//    //////////// NEW
//    //int B = 190;
//    /*SetCover* SC2 = malloc(sizeof * SC2);
//    SC2->S = malloc(G->m * sizeof * SC2->S);
//    for (int i = 0; i < G->m; i++) {
//        SC2->S[i] = bitset_new(G->n);
//        int node = G->S[i];
//        for (int j = 0; j < G->n && options->eval(G->G[node][j], i, G) <= B; j++) {
//            bitset_add(SC2->S[i], G->G[node][j]);
//        }
//    }
//    SC2->contains = bitset_new_full(G->m);
//    SC2->n = G->m;*/
//    SetCover* SC2 = center_closing_to_setcover(G, options, 190);
//    printf("HERE\n");
//
//    U = bitset_new_full(G->n);
//    MSC = minimum_set_cover(SC2, U);
//    printf("\nRESULT OF SET COVER:\nlength = %d\n", MSC->length);
//    print_array_int(MSC->C, MSC->length);
//
//    // Test
//    Result* resTest = solve_using_setcover(G, 2, options);
//    print_result("setcover test", resTest, 2, 0.0);
//
//    resTest = independant_set(G, 1, options);
//    print_result("independent set", resTest, 1, 0.0);
//
//
//
//    // Primer 3: data/moj5.net
//    printf("\n\n");
//    G = read_pajek("data/moj5v2.net");
//    S_set = bitset_new(G->N);
//    S_arr = malloc(4 * sizeof(int));
//    S_arr[0] = 0;
//    S_arr[1] = 1;
//    S_arr[2] = 2;
//    S_arr[3] = 3;
//    bitset_add_from(S_set, S_arr, 4);
//    G->H = malloc(4 * sizeof(double));
//    G->H[0] = 5;
//    G->H[1] = 3;
//    G->H[2] = 1;
//    G->H[3] = 2;
//
//    graph_partition_nodes(G, S_set, 4);
//    floyd_warshall_algorithm(G->D, 8);
//
//    print_graph(G);
//
//    k = 2;
//    options = options_new();
//    options->eval = test_eval1;
//    options->get_first = hochbaum_start_best;
//    options->get_priority = priority_eval_center;
//
//    graph_add_sorted_adjacency_list(G, options);
//
//    // EDGE CASE TEST
//    res = plesnik(G, k, options, range_first);
//    printf("\n\nHERE:");
//    print_result("plesnik", res, k, 0.0);
//    printf("\n\n");
//
//    // SLOVENIA
//    FILE* f;
//    start = clock();
//    int num = 6470;
//    double** D2 = malloc(num * sizeof * D2);
//    for (int i = 0; i < num; i++)
//        D2[i] = malloc(num * sizeof * D2[i]);
//    fopen_s(&f, "data/distances_6470", "rb");
//    for (int i = 0; i < num; i++)
//        fread(D2[i], sizeof(double), num, f);
//    fclose(f);
//    end = clock();
//    printf("Read in %f time.\n", get_time(start, end));
//
//    for (int i = 0; i < 5; i++) {
//        for (int j = 0; j < 5; j++)
//            printf("%7.3f ", D2[i][j]);
//        printf("\n");
//    }
//    printf("\n");
//
//    int m1 = 463;
//    int n1 = 6007;
//    int N1 = n1 + m1;
//    G1 = malloc(sizeof * G1);
//    G1->D = D2;
//    G1->N = N1;
//    int* Sarr1 = malloc(m1 * sizeof * Sarr1);
//    for (int i = 0; i < m1; i++) {
//        Sarr1[i] = n1 + i;
//    }
//    BitSet* Sset1 = bitset_new(N1);
//    bitset_add_from(Sset1, Sarr1, m1);
//
//    graph_partition_nodes(G1, Sset1, m1);
//
//    G1->H = malloc(n1 * sizeof * G1->H);
//    fopen_s(&f, "data/population_6007", "rb");
//    fread(G1->H, sizeof(double), n1, f);
//    fclose(f);
//
//    //free(options);
//    options = options_new();
//    options->eval = test_eval1;
//    options->get_first = hochbaum_start_random;
//    options->get_priority = priority_eval_center;
//    k = G1->m - 1;
//
//    start = clock();
//    Result* res3 = cluster_radius(G1, k, options);
//    end = clock();
//    print_result("cluster_cardinality", res3, k, get_time(start, end));
//
//    start = clock();
//    Result* res4 = cluster_radius_basic(G1, k, options);
//    end = clock();
//    print_result("cluster_cardinality_basic", res4, k, get_time(start, end));
//
//    BitSet* R3 = bitset_new(G1->m);
//    for (int i = 0; i < k; i++)
//        bitset_add(R3, res3->R[i] - G1->n);
//
//    BitSet* R4 = bitset_new(G1->m);
//    for (int i = 0; i < k; i++)
//        bitset_add(R4, res4->R[i] - G1->n);
//
//    double eval3 = eval_score(G1, R3, options);
//    double eval4 = eval_score(G1, R4, options);
//    printf("eval3=%f\neval4=%f\n", eval3, eval4);
//
//    Result* res5 = exact_bound(G1, 1, options, 1);
//    printf("DONE\n");
//
//    /*start = clock();
//    for (int ki = 1; ki < G->m; ki++) {
//        res = backtracking(G, ki, options);
//        print_result("back", res, ki, 0.0);
//    }
//    end = clock();
//    printf("TIME = %f\n", get_time(start, end));*/
//
//    //Graph* G = read_pajek("data/moj4.net");
//    //BitSet* S_set = bitset_new(G->N);
//    //int* S_arr = malloc(2 * sizeof(int));
//    //S_arr[0] = 2;
//    //S_arr[1] = 3;
//    //bitset_add_from(S_set, S_arr, 2);
//    //G->H = malloc(2 * sizeof(double));
//    //G->H[0] = 4;
//    //G->H[1] = 1;
//
//    //graph_partition_nodes(G, S_set, 2);
//
//    ////int k;
//    ////Graph* G = read_or_library_pmed("or_library/pmed/data/pmed1.txt", &k);
//
//    //print_graph(G);
//
//    //int k = 1;
//    //Options* options = options_new();
//    //options->eval = test_eval1;
//    //options->get_first = hochbaum_start_random;
//    //options->get_priority = priority_eval_center;
//
//    //graph_add_sorted_adjacency_list(G, options);
//
//    //Result* res = plesnik(G, k, options, range_adj);
//    //print_result("asd", res, k, 0.0);
//
//    //printf(" after\n");
//
//    /*sprintf_s(filenameIn, 50, "or_library/cap/datc/capa.txt");
//    printf("%s\n", filenameIn);
//    G = read_or_library(filenameIn);
//
//    options = options_new();
//    options->eval = test_eval1;
//    options->get_first = hochbaum_start_best;
//    options->get_priority = priority_eval_center;
//
//    graph_add_sorted_adjacency_list(G, options);
//
//    sprintf_s(filenameOut, 60, "or_library/cap/results/capc_results_weighted.txt");
//
//    fopen_s(&file, filenameOut, "w");
//    fprintf(file, "[");
//    scores = test_approximate(G, options, 1, G->m, 1, file);
//    fprintf(file, ", ");
//    test_exact(G, options, 1, G->m, 1, scores, file);
//    fprintf(file, ", ");
//    test_backtracking(G, options, 1, 3, 1, file);
//    fprintf(file, ", ");
//    test_backtracking(G, options, G->m - 2, G->m, 1, file);
//    fprintf(file, ", ");
//    test_backtracking_decision(G, options, 1, G->m, 1, file);
//    fprintf(file, ", ");
//    test_bruteforce(G, options, 1, 3, 1, file);
//    fprintf(file, ", ");
//    test_bruteforce(G, options, G->m - 2, G->m, 1, file);
//    fprintf(file, "]");
//    fclose(file);
//    free(scores);*/
//
//    //options = options_new();
//    //options->eval = test_eval;
//    //options->get_first = hochbaum_start_best;
//    //options->get_priority = priority_eval_center;
//
//    //int k;
//    //G = read_or_library_pmed("or_library/pmed/data/pmed40.txt", &k);
//    ///*fopen_s(&file, "or_library/pmed/results/pmed40_results.txt", "w");
//    //fprintf(file, "[");
//    //test_approximate(G, options, k, k + 1, 1, file);
//    //fprintf(file, "]");
//    //fclose(file);*/
//
//    //clock_t start = clock();
//    //Result* res = greedy_basic(G, k, options);
//    //clock_t end = clock();
//    //print_result("basic", res, k, get_time(start, end));
//
//    return 0;
//}

//int main_old() {
//    //clock_t start, end;
//
//    /*FILE* file;
//    fopen_s(&file, "data/output_test.txt", "w");
//    fprintf(file, "{");*/
//
//    // Primer 1: data/moj4.net (tudi v magistrski)
//    Graph* G = read_pajek("data/moj4.net");
//    BitSet* S_set = bitset_new(G->N);
//    int* S_arr = malloc(2 * sizeof(int));
//    S_arr[0] = 2;
//    S_arr[1] = 3;
//    bitset_add_from(S_set, S_arr, 2);
//    G->H = malloc(2 * sizeof(double));
//    G->H[0] = 4;
//    G->H[1] = 1;
//
//    graph_partition_nodes(G, S_set, 2);
//
//    print_graph(G);
//
//    int k = 1;
//    Options* options = options_new();
//    options->eval = test_eval1;
//    options->get_first = hochbaum_start_random;
//    options->get_priority = priority_eval_center;
//
//    graph_add_sorted_adjacency_list(G, options);
//
//    FILE* file;
//    fopen_s(&file, "data/moj4_results.txt", "w");
//    fprintf(file, "[");
//    double* scores = test_approximate(G, options, 1, G->m, 1, file);
//    fprintf(file, ", ");
//    test_exact(G, options, 1, G->m, 1, scores, file);
//    fprintf(file, "]");
//    fclose(file);
//    free(scores);
//
//    /*start = clock();
//    Result* res = bruteforce(G, k, options);
//    end = clock();
//    print_result("bruteforce", res, k, get_time(start, end));
//    write_entry(file, "bruteforce", res, k, get_time(start, end));
//    fprintf(file, ", ");
//
//    start = clock();
//    Result* res1 = backtracking(G, k, options);
//    end = clock();
//    print_result("backtracking", res1, k, get_time(start, end));
//    write_entry(file, "backtracking", res1, k, get_time(start, end));
//    fprintf(file, ", ");
//
//    start = clock();
//    Result* res2 = exact(G, k, options);
//    end = clock();
//    print_result("exact", res2, k, get_time(start, end));
//
//    start = clock();
//    Result* res3 = greedy_centers(G, k, options);
//    end = clock();
//    print_result("greedy_centers", res3, k, get_time(start, end));
//
//    start = clock();
//    Result* res4 = greedy_basic(G, k, options);
//    end = clock();
//    print_result("greedy_basic", res4, k, get_time(start, end));
//
//    start = clock();
//    Result* res5 = hochbaum(G, k, options);
//    end = clock();
//    print_result("hochbaum", res5, k, get_time(start, end));
//
//    start = clock();
//    Result* res6 = plesnik(G, k, options, range_adj);
//    end = clock();
//    print_result("plesnik", res6, k, get_time(start, end));
//
//    start = clock();
//    Result* res7 = cluster_cardinality(G, k, options);
//    end = clock();
//    print_result("cardinality", res7, k, get_time(start, end));
//
//    start = clock();
//    Result* res8 = cluster_radius(G, k, options);
//    end = clock();
//    printf("f\n", 2.3);
//    print_result("radius", res8, k, get_time(start, end));
//
//    fclose(file);*/
//
//
//    graph_free(G);
//    bitset_free(S_set);
//    free(S_arr);
//    free(options);
//    /*result_free(res);
//    result_free(res1);
//    result_free(res2);
//    result_free(res3);
//    result_free(res4);
//    result_free(res5);
//    result_free(res6);
//    result_free(res7);
//    result_free(res8);*/
//
//    // Primer 2: data/moj1.net
//    printf("\n\n");
//    G = read_pajek("data/moj1.net");
//    S_set = bitset_new(G->N);
//    S_arr = malloc(4 * sizeof(int));
//    S_arr[0] = 0;
//    S_arr[1] = 1;
//    S_arr[2] = 2;
//    S_arr[3] = 3;
//    bitset_add_from(S_set, S_arr, 4);
//    G->H = malloc(4 * sizeof(double));
//    G->H[0] = 5;
//    G->H[1] = 4;
//    G->H[2] = 3;
//    G->H[3] = 2;
//
//    graph_partition_nodes(G, S_set, 4);
//
//    print_graph(G);
//
//    k = 2;
//    options = options_new();
//    options->eval = test_eval1;
//    options->get_first = hochbaum_start_best;
//    options->get_priority = priority_eval_center;
//
//    graph_add_sorted_adjacency_list(G, options);
//
//    fopen_s(&file, "data/moj1_results.txt", "w");
//    fprintf(file, "[");
//    scores = test_approximate(G, options, 1, G->m, 1, file);
//    fprintf(file, ", ");
//    test_exact(G, options, 1, G->m, 1, scores, file);
//    fprintf(file, ", ");
//    test_backtracking(G, options, 1, G->m, 1, file);
//    fprintf(file, ", ");
//    test_bruteforce(G, options, 1, G->m, 1, file);
//    fprintf(file, "]");
//    fclose(file);
//    free(scores);
//
//    graph_free(G);
//    bitset_free(S_set);
//    free(S_arr);
//    free(options);
//
//    G = read_or_library("data/or_library/cap41.txt");
//    //print_graph(Gtest);
//    //printf("\n");
//    //for (int i = 0; i < 5; i++) {
//    //    for (int j = 0; j < 5; j++)
//    //        printf("%7.2lf ", Gtest->D[i][j]);
//    //    printf("\n");
//    //}
//
//    options = options_new();
//    options->eval = test_eval;
//    options->get_first = hochbaum_start_best;
//    options->get_priority = priority_eval_center;
//
//    graph_add_sorted_adjacency_list(G, options);
//
//    fopen_s(&file, "data/or_library/results/cap41_results.txt", "w");
//    fprintf(file, "[");
//    scores = test_approximate(G, options, 1, G->m, 1, file);
//    fprintf(file, ", ");
//    test_exact(G, options, 1, G->m, 1, scores, file);
//    fprintf(file, ", ");
//    test_backtracking(G, options, 1, G->m, 1, file);
//    fprintf(file, ", ");
//    test_backtracking_decision(G, options, 1, G->m, 1, file);
//    fprintf(file, ", ");
//    test_bruteforce(G, options, 1, G->m, 1, file);
//    fprintf(file, "]");
//    fclose(file);
//    free(scores);
//
//    //start = clock();
//    //res = bruteforce(G, k, options);
//    //end = clock();
//    //print_result("bruteforce", res, k, get_time(start, end));
//
//    //start = clock();
//    //res1 = backtracking(G, k, options);
//    //end = clock();
//    //print_result("backtracking", res1, k, get_time(start, end));
//
//    //start = clock();
//    //Result* res2 = exact_bound(G, k, options, 285 + 1e-6);
//    //end = clock();
//    //print_result("exact", res2, k, get_time(start, end));
//
//    //start = clock();
//    //res3 = greedy_centers(G, k, options);
//    //end = clock();
//    //print_result("greedy_centers", res3, k, get_time(start, end));
//
//    //start = clock();
//    //res4 = greedy_basic(G, k, options);
//    //end = clock();
//    //print_result("greedy_basic", res4, k, get_time(start, end));
//
//    //start = clock();
//    //res5 = hochbaum(G, k, options);
//    //end = clock();
//    //print_result("hochbaum", res5, k, get_time(start, end));
//
//    //start = clock();
//    //res6 = plesnik(G, k, options, range_adj);
//    //end = clock();
//    //print_result("plesnik", res6, k, get_time(start, end));
//
//    //start = clock();
//    //res7 = cluster_cardinality(G, k, options);
//    //end = clock();
//    //print_result("cardinality", res7, k, get_time(start, end));
//
//    //start = clock();
//    //res8 = cluster_radius(G, k, options);
//    //end = clock();
//    //print_result("radius", res8, k, get_time(start, end));
//
//    //start = clock();
//    //Result* res10 = plesnik_unlimited(G, k, options, range_first, 10);
//    //end = clock();
//    //print_result("plesnik++", res10, k, get_time(start, end));
//
//    //graph_free(G);
//    //bitset_free(S_set);
//    //free(S_arr);
//    //free(options);
//    //result_free(res);
//    //result_free(res1);
//    //result_free(res2);
//    //result_free(res3);
//    //result_free(res4);
//    //result_free(res5);
//    //result_free(res6);
//    //result_free(res7);
//    //result_free(res8);
//
//    ////////////////////////////
//    //// Primer 3: data/moj2.net
//    //printf("\n\n");
//    //G = read_pajek("data/moj2.net");
//    //S_set = bitset_new(G->N);
//    //int m = 10;
//    //int n = G->N - m;
//    //S_arr = malloc(m * sizeof(int));
//    //for (int i = 0; i < m; i++)
//    //    S_arr[i] = i;
//    //bitset_add_from(S_set, S_arr, m);
//    //G->H = malloc(n * sizeof(double));
//    //for (int i = 0; i < n; i++)
//    //    G->H[i] = i + 1;
//
//
//    //graph_partition_nodes(G, S_set, m);
//
//    ////print_graph(G);
//    //printf("START SOLVING\n");
//
//    //k = 5;
//    //options = options_new();
//    //options->eval = test_eval1;
//    //options->get_first = random_start;
//    //options->get_priority = priority_eval_center;
//
//    //graph_add_sorted_adjacency_list(G, options);
//
//    ////start = clock();
//    ////res = bruteforce(G, k, options);
//    ////end = clock();
//    ////print_result("bruteforce", res, k, get_time(start, end));
//
//    ////start = clock();
//    ////res1 = backtracking(G, k, options);
//    ////end = clock();
//    ////print_result("backtracking", res1, k, get_time(start, end));
//
//    //start = clock();
//    //res2 = exact(G, k, options);
//    //end = clock();
//    //print_result("exact", res2, k, get_time(start, end));
//
//    //start = clock();
//    //res3 = greedy_centers(G, k, options);
//    //end = clock();
//    //print_result("greedy_centers", res3, k, get_time(start, end));
//
//    //start = clock();
//    //res4 = greedy_basic(G, k, options);
//    //end = clock();
//    //print_result("greedy_basic", res4, k, get_time(start, end));
//
//    //start = clock();
//    //res5 = hochbaum(G, k, options);
//    //end = clock();
//    //print_result("hochbaum", res5, k, get_time(start, end));
//
//    //start = clock();
//    //res6 = plesnik(G, k, options, range_adj);
//    //end = clock();
//    //print_result("plesnik", res6, k, get_time(start, end));
//
//    //start = clock();
//    //res7 = cluster_cardinality(G, k, options);
//    //end = clock();
//    //print_result("cardinality", res7, k, get_time(start, end));
//
//    //start = clock();
//    //res8 = cluster_radius(G, k, options);
//    //end = clock();
//    //print_result("radius", res8, k, get_time(start, end));
//
//    //start = clock();
//    //Result* res9 = backtracking_decision_to_optimization(G, k, options);
//    //end = clock();
//    //print_result("back_dec_opt", res9, k, get_time(start, end));
//
//    //n = 5;
//    //int* xs = malloc(n * sizeof * xs);
//    //for (int i = 0; i < n; i++)
//    //    xs[i] = i + 1;
//
//    //printf("START\n");
//    //FILE* f;
//    //fopen_s(&f, "testdata", "wb");
//    //fwrite(xs, sizeof(int), n, f);
//    //fwrite(xs, sizeof(int), n, f);
//    //fclose(f);
//
//    //printf("READ\n");
//
//    //int* ys = malloc(2 * n * sizeof * ys);
//    //fopen_s(&f, "testdata", "rb");
//    //fread(ys, sizeof(int), 2*n, f);
//    //fclose(f);
//
//    //printf("DONE\n");
//    //for (int i = 0; i < 2*n; i++)
//    //    printf("%d ", ys[i]);
//    //printf("\n");
//
//    //double** D = malloc(n * sizeof * D);
//    //for (int i = 0; i < n; i++) {
//    //    D[i] = malloc(n * sizeof * D[i]);
//    //    for (int j = 0; j < n; j++)
//    //        D[i][j] = i * j;
//    //}
//
//    //fopen_s(&f, "testdata1", "wb");
//    //for (int i = 0; i < n; i++)
//    //    fwrite(D[i], sizeof(double), n, f);
//    //fclose(f);
//
//    //double** D1 = malloc(n * sizeof * D1);
//    //for (int i = 0; i < n; i++)
//    //    D1[i] = malloc(n * sizeof * D1[i]);
//    //fopen_s(&f, "testdata1", "rb");
//    //for (int i = 0; i < n; i++)
//    //    fread(D1[i], sizeof(double), n, f);
//
//    //for (int i = 0; i < n; i++) {
//    //    for (int j = 0; j < n; j++) {
//    //        printf("%2.0f ", D1[i][j]);
//    //    }
//    //    printf("\n");
//    //}
//    //fclose(f);
//
//    //fopen_s(&f, "testdata3", "wb");
//    //for (int i = 0; i < G->N; i++)
//    //    fwrite(G->D[i], sizeof(double), G->N, f);
//    //fclose(f);
//
//    //FILE* f;
//    //printf("\n\nMALLOC\n");
//    //int n = 6470;
//    //double** dists = malloc(n * sizeof * dists);
//    //for (int i = 0; i < n; i++)
//    //    dists[i] = malloc(n * sizeof * dists[i]);
//    //printf("READ\n");
//    //fopen_s(&f, "data/gmaps_distances10_6470", "rb");
//    //for (int i = 0; i < n; i++)
//    //    fread(dists[i], sizeof(double), n, f);
//    //fclose(f);
//    //printf("FLOYD-WARSHALL .. ");
//    //start = clock();
//    //floyd_warshall_algorithm(dists, n);
//    //end = clock();
//    //printf("%f\nWRITING\n", get_time(start, end));
//    //fopen_s(&f, "data/gmaps_distances10_full_6470", "wb");
//    //for (int i = 0; i < n; i++)
//    //    fwrite(dists[i], sizeof(double), n, f);
//    //fclose(f);
//    //printf("\nDONE\n");
//
//
//
//    //FILE* f;
//    //start = clock();
//    //int num = 6470;
//    //double** D2 = malloc(num * sizeof * D2);
//    //for (int i = 0; i < num; i++)
//    //    D2[i] = malloc(num * sizeof * D2[i]);
//    //fopen_s(&f, "data/distances_6470", "rb");
//    //for (int i = 0; i < num; i++)
//    //    fread(D2[i], sizeof(double), num, f);
//    //fclose(f);
//    //end = clock();
//    //printf("Read in %f time.\n", get_time(start, end));
//
//    //for (int i = 0; i < 5; i++) {
//    //    for (int j = 0; j < 5; j++)
//    //        printf("%7.3f ", D2[i][j]);
//    //    printf("\n");
//    //}
//    //printf("\n");
//
//    //int m1 = 463;
//    //int n1 = 6007;
//    //int N1 = n1 + m1;
//    //Graph* G1 = malloc(sizeof * G1);
//    //G1->D = D2;
//    //G1->N = N1;
//    //int* Sarr1 = malloc(m1 * sizeof * Sarr1);
//    //for (int i = 0; i < m1; i++) {
//    //    Sarr1[i] = n1 + i;
//    //}
//    //BitSet* Sset1 = bitset_new(N1);
//    //bitset_add_from(Sset1, Sarr1, m1);
//
//    //graph_partition_nodes(G1, Sset1, m1);
//
//    //G1->H = malloc(n1 * sizeof * G1->H);
//    //fopen_s(&f, "data/population_6007", "rb");
//    //fread(G1->H, sizeof(double), n1, f);
//    //fclose(f);
//
//    ////free(options);
//    //options = options_new();
//    //options->eval = test_eval1;
//    //options->get_first = hochbaum_start_random;
//    //options->get_priority = priority_eval_center;
//    //k = 1;
//
//    //scores = test_approximate(G1, options, 100, 101, 1, "data/slovenija_approx.txt"); // 12194.492462
//
//    /*start = clock();
//    Result* res3 = greedy_centers(G1, k, options, range_first);
//    end = clock();
//    print_result("greedy_centers", res3, k, get_time(start, end));*/
//
//    /*scores = test_approximate(G1, options, 80, 101, 5, "data/slovenija_approx.txt");
//    free(scores);*/
//
//    //k = 30;
//    //start = clock();
//    ////Result* res3 = exact_bound(G1, k, options, 51000);
//    //Result* res3 = exact(G1, k, options);
//    //end = clock();
//    //print_result("exact", res3, k, get_time(start, end));
//
//    //double d;
//    //int sm;
//    //for (int c = 0; c < G1->n; c++) {
//    //    d = INT_MAX;
//    //    for (int s = 0; s < G1->m; s++)
//    //        if (options->eval(c, s, G1) < d) {
//    //            d = options->eval(c, s, G1);
//    //            sm = s;
//    //        }
//    //    if (abs(d - 50088.021004) < 0.1)
//    //        printf("c=%d, s=%d\n", c, sm);
//    //}
//
//    return 0;
//}

//int main() {
//    // Testing centers_redistribution
//    int n1;
//    double** D1 = read_pajek_dist("data/moj3.net", &n1);
//    //int n1 = 6;
//    Graph* G1 = graph_new(D1, n1);
//    int Cb1[3] = { 0, 1, 2 };
//    BitSet* C1 = bitset_new(n1);
//    int m1 = 3;
//    int k1 = 2;
//    int Rb1[1] = { 0 };
//    BitSet* R1 = bitset_new(n1);
//    bitset_add_from(C1, Cb1, m1);
//    bitset_add_from(R1, Rb1, 1);
//    graph_add_from_centers(G1, C1, m1);
//
//    printf("Step 1\n");
//    Center** centers1 = centers_new_from_graph(G1);
//    printf("Step 2\n");
//    centers_redistribute(centers1, G1, R1, 0);
//    for (int i = 0; i < 3; i++) {
//        printf("Center %d:\n[", G1->C[i]);
//        for (int j = 0; j < centers1[i]->numOfNodes; j++) {
//            printf("%d ", centers1[i]->nodes[j]);
//        }
//        printf("]\n");
//    }
//
//    printf("\n\n");
//
//    int nodes[10] = { 0,1,2,3,4,5,6,7,8,9 };
//    double vals[10] = { 3, 10, 540, 30, 20, 100, 1, 96, 99, 2000 };
//    BinaryHeap* heap = binary_heap_new(nodes, vals, 10, 10);
//    printf("HEAP:\n");
//    for (int i = 0; i < 10; i++) {
//        printf("(%d,%f) ", heap->nodes[i], heap->heap[i]);
//    }
//    printf("\n\n");
//    PairIntDouble res1;
//    for (int i = 0; i < 5; i++) {
//        res1 = binary_heap_extract_min(heap);
//        printf("%d: %f\n", res1.node, res1.value);
//    }
//    printf("\n");
//    binary_heap_insert(heap);
//    binary_heap_insert(heap);
//    printf("HEAP:\n");
//    for (int i = 0; i < 10; i++) {
//        printf("(%d,%f) ", heap->nodes[i], heap->heap[i]);
//    }
//    printf("\n\n");
//    binary_heap_free(heap);
//
//
//
//    /*double x = 100;
//    double y = 0.95 * x;
//    double Db[8][8] = { {0, x, x, x, x / 10, y, y, y},{x, 0, x, x, y, x / 10, y, y},{x, x, 0, x, y, y, x / 10, y},{x, x, x, 0, y, y, y, x / 10},{x / 10, y, y, y, 0, y, y, y},{y, x / 10, y, y, y, 0, y, y},{y, y, x / 10, y, y, y, 0, y},{y, y, y, x / 10, y, y, y, 0} };
//    int n = 8;
//    double** D;
//    D = malloc(n * sizeof(*D));
//    for (int i = 0; i < n; i++) {
//        D[i] = malloc(n * sizeof(*D[i]));
//        for (int j = 0; j < n; j++) {
//            D[i][j] = Db[i][j];
//        }
//    }
//
//    Graph* G = graph_new(D, n);
//    int Cb[4] = { 0,1,2,4 };
//    BitSet* C = bitset_new(n);
//    int m = 4;
//    int k = 3;
//    bitset_add_from(C, Cb, m);
//
//    graph_add_from_centers(G, C, m);*/
//
//
//
//
//    //double** D;
//    //int n = 150;
//    //D = read_pajek_dist("data/moj2.net", &n);
//    //printf("START\n");
//    ////D = create_random_distances(n);
//
//    //printf("DONE DONE \n\n");
//    ////print_D(D, n);
//
//    //Graph* G = graph_new(D, n);
//
//    ////int Cb[4] = { 0, 1, 2, 4 };
//    //int Cb[30];
//    //for (int i = 0; i < 30; i++) {
//    //    Cb[i] = i;
//    //}
//    //BitSet* C = bitset_new(n);
//    //int m = 30;
//    //int k = 15;
//    //bitset_add_from(C, Cb, m);
//
//    //graph_add_from_centers(G, C, m);
//
//    /*BitSet* R = bitset_new(G->N);
//    int Rb[7] = { 0,1,2,3,5,6,7 };
//    bitset_add_from(R, Rb, 7);*/
//
//
//    //double** D;
//    //int n;
//    //D = read_pajek_dist("data/circle.net", &n);
//    //printf("START\n");
//    ////D = create_random_distances(n);
//
//    //printf("DONE DONE \n\n");
//    ////print_D(D, n);
//
//    //Graph* G = graph_new(D, n);
//
//    //int Cb[4] = { 0, 3, 6, 9 };
//    //BitSet* C = bitset_new(n);
//    //int m = 4;
//    //int k = 3;
//    //bitset_add_from(C, Cb, m);
//
//    //graph_add_from_centers(G, C, m);
//
//    int n = 301;
//    Graph* G = create_circle_graph(n, 1000, 1);
//    n -= (n - 1) % 3;
//    int k = n / 3;
//
//    PriorityFunction* priority_function = priority_functions_new(priority_eval_center, priority_eval);
//    //PriorityFunction* priority_function = priority_functions_new(priority_num_closest_center, priority_num_closest);
//
//
//
//
//    /*for (int ui = 0; ui < 150; ui++) {
//        printf("%d: [", ui);
//        for (int i = 0; i < 150; i++) {
//            if (bitset_contains(C, i) && !bitset_contains(R, i)) printf("%.2f ", D[ui][i]);
//        }
//        printf("]\n");
//    }
//    printf("\n\n");*/
//
//    /*for (int i = 0; i < 5; i++) {
//        for (int j = 0; j < 8; j++) {
//            printf("%f ", D[i][j]);
//        }
//        printf("\n");
//    }
//    for (int i = 0; i < 20; i++) {
//        printf("%f ", D[27][i]);
//    }*/
//
//    printf("\n\n");
//
//    //printf("Eval = %f\n", eval_score(G, R));
//
//    //print_graph(G);
//    //printf("\n");
//    //
//
//    printf("BRUTEFORCING\n");
//    
//    clock_t start, end;
//    double cpu_time_used;
//
//    /*start = clock();
//    Result res = bruteforce(G, k);
//    end = clock();
//
//    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//    printf("Bruteforce done in %f\n", cpu_time_used);
//
//    printf("\n[");
//    for (int i = 0; i < k; i++) {
//        printf("%d ", res.R[i]);
//    }
//    printf("]\n");
//
//    printf("RESULT = %f \n", res.score);*/
//
//    printf("\n\nSTART OF EXACT:\n");
//
//    start = clock();
//    Result res_exact = exact_final(G, k, priority_function);
//    end = clock();
//
//    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//    printf("Exact done in %f\n", cpu_time_used);
//
//    printf("\n[");
//    for (int i = 0; i < k; i++) {
//        printf("%d ", res_exact.R[i]);
//    }
//    printf("]\n");
//    printf("RESULT EXACT = %f\n", res_exact.score);
//
//    // GREEDY BASIC
//    /*printf("\n\nSTART OF GREEDY:\n");
//
//    start = clock();
//    Result res_greedy = greedy_basic(G, k);
//    end = clock();
//
//    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//    printf("Greedy done in %f\n", cpu_time_used);
//
//    printf("\n[");
//    for (int i = 0; i < k; i++) {
//        printf("%d ", res_greedy.R[i]);
//    }
//    printf("]\n");
//    printf("RESULT GREEDY = %f\n", res_greedy.score);*/
//
//    // GREEDY CENTERS
//    printf("\n\nSTART OF GREEDY:\n");
//
//    start = clock();
//    Result res_greedy2 = greedy_centers(G, k);
//    end = clock();
//
//    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//    printf("Greedy done in %f\n", cpu_time_used);
//
//    printf("\n[");
//    for (int i = 0; i < k; i++) {
//        printf("%d ", res_greedy2.R[i]);
//    }
//    printf("]\n");
//    printf("RESULT GREEDY 2 = %f\n", res_greedy2.score);
//
//    // HOCHBAUM
//    printf("\n\nSTART OF HOCHBAUM:\n");
//
//    start = clock();
//    Result res_hochbaum = hochbaum(G, k, random_start);
//    end = clock();
//
//    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//    printf("Hochbaum done in %f\n", cpu_time_used);
//
//    printf("\n[");
//    for (int i = 0; i < k; i++) {
//        printf("%d ", res_hochbaum.R[i]);
//    }
//    printf("]\n");
//    printf("RESULT HOCHBAUM = %f\n", res_hochbaum.score);
//
//    // PLESNIK
//    printf("\n\nSTART OF PLESNIK:\n");
//
//    start = clock();
//    Result res_plesnik = plesnik(G, k, 1e-8);
//    end = clock();
//
//    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//    printf("Plesnik done in %f\n", cpu_time_used);
//
//    printf("\n[");
//    for (int i = 0; i < k; i++) {
//        printf("%d ", res_plesnik.R[i]);
//    }
//    printf("]\n");
//    printf("RESULT PLESNIK = %f\n", res_plesnik.score);
//
//
//
//   /* int is[1] = { 0 };
//    int C[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };*/
//
//    //bool R[4] = { false, false, true, false };
//    //
//    ///*double res = eval_score(D, is, C, n, 4, 1);
//    //printf("RESITEV = %f\n\n", res);*/
//
//    //double r = bruteforce(D, C, n, 8, 4);
//    //printf("RESITEV = %f\n\n", r);
//
//    //BoolSet* bs = boolset_new(5);
//    //boolset_add(bs, 3);
//    //for (int i = 0; i < 5; i++) {
//    //    printf("%d ", bs->set[i] ? 1 : 0);
//    //}
//
//
//    // TEST CENTROV
//    int num_centers;
//    int* Craw = read_centers("data/slovenia_post_offices.net", &num_centers);
//    for (int i = 0; i < num_centers; i++) {
//        printf("%d ", Craw[i]);
//    }
//    printf("\n");
//    printf("%d\n", num_centers);
//
//    int num_nodes;
//    double* weights;
//    double** Dslo = read_pajek_dist_weighted("data/slovenia_d_full_1_0.net", &num_nodes, &weights);
//    printf("HERE\n");
//
//    Graph* Gslo = graph_new(Dslo, num_nodes);
//    BitSet* Cslo = bitset_new(num_nodes);
//    bitset_add_from(Cslo, Craw, num_centers);
//    graph_add_from_centers(Gslo, Cslo, num_centers);
//
//    printf("%d %d %d\n", Gslo->N, Gslo->n, Gslo->m);
//
//    if (weights == NULL) printf("NULL\n");
//
//    //for (int i = 0; i < num_nodes; i++) {
//    //    printf("%.2lf ", weights[i]);
//    //}
//
//    printf("\n\nSTART OF SLOVENIA:\n");
//
//    start = clock();
//    Result res_slovenia = hochbaum(Gslo, 2, random_start);
//    end = clock();
//
//    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//    printf("Slovenia done in %f\n", cpu_time_used);
//
//    printf("\n[");
//    for (int i = 0; i < k; i++) {
//        printf("%d ", res_slovenia.R[i]);
//    }
//    printf("]\n");
//    printf("RESULT EXACT = %f\n", res_slovenia.score);
//
//    return 0;
//}