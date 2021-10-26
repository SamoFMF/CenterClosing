#ifndef EXACT_H
#define EXACT_H

Result* exact(Graph* G, int k, Options* options);

Result* exact_bound(Graph* G, int k, Options* options, double valmax);

Result* exact_pclstp(Graph* G, int k, Options* options);

#endif