#ifndef HOCHBAUM_H
#define HOCHBAUM_H

int random_start(Graph* G);

Result hochbaum(Graph* G, int k, int (*f)(Graph* graph));

#endif