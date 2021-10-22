#include <stdio.h>
#include "algutils.h"
#include "result_to_json.h"

void write_entry(FILE* file, char* algorithm, Result* res, int k, double t) {
	static char myStringStatic[100];
	fprintf(file, "{\"algorithm\": \"%s\", \"k\": %d, \"score\": %lf, \"time\": %d, \"removed\": ", algorithm, k, res->score, (int)(1000*t + 0.1));
	if (res->R == NULL) {
		fprintf(file, "null}");
	}
	else {
		fprintf(file, "[");
		for (int i = 0; i < k - 1; i++)
			fprintf(file, "%d, ", res->R[i]);
		fprintf(file, "%d]}", res->R[k - 1]);
	}
}