#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

double** create_random_distances(int n) {
	double** D;
	double val;
	if ((D = malloc(n * sizeof * D)) != NULL) {
		for (int i = 0; i < n; i++) {
			if ((D[i] = malloc(n * sizeof * D[i])) == NULL) {
				return NULL;
			}
		}
		for (int i = 0; i < n; i++) {
			D[i][i] = 0;
			for (int j = i + 1; j < n; j++) {
				val = rand_lim(RAND_MAX / 2);
				D[i][j] = val;
				D[j][i] = val;
			}
		}

		// Make sure triangle inequality holds
		int changes = 1;
		while (changes) {
			changes = 0;
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					for (int k = 0; k < n; k++) {
						val = D[i][k] + D[k][j];
						if (val < D[i][j]) {
							D[i][j] = val;
							D[j][i] = val;
							changes = 1;
						}
					}
				}
			}
		}
	}
	return D;
}