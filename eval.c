
#include "eval.h"
#include <stdio.h>
#include <setjmp.h>
#include <math.h>
#include <ctype.h>

//globals? in my c???
static jmp_buf abortpoint;
static char *s;

double p_expression();
double p_sum();
double p_product();
double p_negate();
double p_exponent();
double p_value();
double p_number();

#define skipspace() while (*s && isspace(*s)) s++

int eval(char *src, double *result) {
	int error;
	if ((error = setjmp(abortpoint))) {
		return error;
	}
	else {
		s = src;
		skipspace();
		*result = p_expression();
		return 0;
	}
}

double p_expression() {
	return p_sum();
}

double p_sum() {
	char op;
	double r = p_product();
	skipspace();
	while (*s == '+' || *s == '-') {
		op =*s;
		s++;
		skipspace();
		if (op == '+')
			r += p_product();
		else
			r -= p_product();
		skipspace();
	}
	return r;
}

double p_product() {
	char op;
	double r = p_negate();
	skipspace();
	while (*s == '*' || *s == '/') {
		op =*s;
		s++;
		skipspace();
		if (op == '*')
			r *= p_negate();
		else
			r /= p_negate();
		skipspace();
	}
	return r;
}

double p_negate() {
	if (*s == '-') {
		s ++;
		skipspace();
		return - p_exponent();
	}
	else {
		return p_exponent();
	}
}

double p_exponent() {
	double r = p_value();
	skipspace();
	while (*s == '^') {
		s++;
		skipspace();
		double e = p_exponent();
		r = pow(r, e);
		skipspace();
	}
	return r;
}

double p_value() {
	if (*s == '(') {
		double r;
		s ++;
		skipspace();
		r = p_expression();
		skipspace();
		if (*s != ')') {
			longjmp(abortpoint, 1);
		}
		else {
			s ++;
			return r;
		}
	}
	else {
		skipspace();
		return p_number();
	}
}

double p_number() {
	int sign = 1;
	double r = 0;
	if (*s == '+') { *s++; skipspace(); }
	if (*s == '-') { sign = -1; *s++; skipspace(); }
	if (!isdigit(*s)) longjmp(abortpoint, 2);
	while (isdigit(*s)) {
		r *= 10;
		r += *s - '0';
		s++;
	}
	return r * sign;
}
