#ifndef GRAPHGENERATORS_H
#define GRAPHGENERATORS_H

double** create_random_distances(int n);

Graph* create_circle_graph(int n, double R, double x);

double** create_erdos_renyi_gnm(int n, int m);

double** create_erdos_renyi_gnm_full(int n, int m);

Graph* create_star_graph(int n, double R, double x);

Graph* create_circle_graph(int n, double R, double x);


#endif