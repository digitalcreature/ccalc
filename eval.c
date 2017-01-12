
#include "eval.h"
#include <stdio.h>
#include <setjmp.h>
#include <math.h>
#include <ctype.h>

//globals? in my c???
static char *s;

typedef double semret;	// semantic return; the type to return from semantic operations by each rule
#define ruledef(name) semret rule_##name(jmp_buf *_state_parent)	//declare a rule
#define rule(name) ruledef(name) { \
	jmp_buf _state;\
	static char _name[] = #name;\
	if (!setjmp(_state))
#define or else if (!setjmp(_state))
#define fail(error) do { printf("error parsing %s: %s\n", _name, error); longjmp(*_state_parent, 1); } while(0)
#define endrule else { fail(""); } }

/*
HOW TO USE THE RULE MACROS

ruledef(example);

rule(example) {
	// try to match stuff, return results of semantics
}
or {
	return matchrule(foo); // try to match against some other rule, for example
}
or {
	if (somethings wrong) {
		fail("heres what went wrong"); 	// will handle exceptions on the fly
	}
} endrule

basically the equivelent of:

example =
	| // try to match stuff
	| foo
	| // fail if something goes wrong
	;

in things like yacc or grako
*/



#define matchrule(name) rule_##name(&_state)

ruledef(expression);
ruledef(sum);
ruledef(product);
ruledef(negate);
ruledef(exponent);
ruledef(value);
ruledef(number);

#define skipspace() while (*s && isspace(*s)) s++

int eval(char *src, double *result) {
	int error;
	jmp_buf _state;
	if ((error = setjmp(_state))) {
		return error;
	}
	else {
		s = src;
		skipspace();
		*result = matchrule(expression);
		return 0;
	}
}

rule(expression) {
	return matchrule(sum);
} endrule

rule(sum) {
	char op;
	double r = matchrule(product);
	skipspace();
	while (*s == '+' || *s == '-') {
		op =* s;
		s++;
		skipspace();
		if (op == '+')
			r += matchrule(product);
		else
			r -= matchrule(product);
		skipspace();
	}
	return r;
} endrule

rule(product) {
	char op;
	double r = matchrule(negate);
	skipspace();
	while (*s == '*' || *s == '/') {
		op =* s;
		s++;
		skipspace();
		if (op == '*')
			r *= matchrule(negate);
		else
			r /= matchrule(negate);
		skipspace();
	}
	return r;
} endrule

rule(negate) {
	if (*s == '-') {
		s ++;
		skipspace();
		return - matchrule(exponent);
	}
	else {
		return matchrule(exponent);
	}
} endrule

rule(exponent) {
	double r = matchrule(value);
	skipspace();
	while (*s == '^') {
		s++;
		skipspace();
		double e = matchrule(exponent);
		r = pow(r, e);
		skipspace();
	}
	return r;
} endrule

rule(value) {
	if (*s == '(') {
		double r;
		s ++;
		skipspace();
		r = matchrule(expression);
		skipspace();
		if (*s != ')') {
			fail("no closing parentheses");
		}
		else {
			s ++;
			return r;
		}
	}
	else {
		skipspace();
		return matchrule(number);
	}
} endrule

rule(number) {
	int sign = 1;
	double r = 0;
	if (*s == '+') { *s++; skipspace(); }
	else if (*s == '-') { sign = -1; *s++; skipspace(); }
	if (!isdigit(*s) && *s != '.') fail("unexpected token");
	while (isdigit(*s)) {
		r *= 10;
		r += *s - '0';
		s++;
	}
	if (*s == '.') {
		double e = 1;
		s ++;
		if (!isdigit(*s)) fail("unexpected token");
		while (isdigit(*s)) {
			e *= 10;
			r += (*s - '0') / e;
			s++;
		}
	}
	return r * sign;
} endrule
