
#include <stdio.h>
#include <stdlib.h>
#include "eval.h"

int main(int argc, char **argv) {
	char *line;
	size_t n = 0;
	ssize_t status = getline(&line, &n, stdin);
	double x;
	while (status >= 0) {

		if (eval(line, &x)) {
			printf(" = %lf\n", x);
		}
		else {
			puts("syntax error");
		}
		status = getline(&line, &n, stdin);
	}
	free(line);
	return 0;
}
