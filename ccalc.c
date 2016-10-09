
#include <stdio.h>
#include <stdlib.h>
#include "eval.h"

int main(int argc, char **argv) {
	char *line;
	size_t n = 0;
	printf("> ");
	ssize_t status = getline(&line, &n, stdin);
	double x;
	int error;
	while (status >= 0) {
		if ((error = eval(line, &x))) {
			printf("syntax error: %d\n", error);
		}
		else {
			printf(" = %lf\n", x);
		}
		printf("> ");
		status = getline(&line, &n, stdin);
	}
	free(line);
	putchar('\n');
	return 0;
}
