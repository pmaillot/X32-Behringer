/*
 * X32RpnCalc.c
 *
 *  Created on: Jul 8, 2018
 *	  Author: Patrick-Gilles Maillot
 *
 *	  Reverse polish notation calculator with a set of operators listed below:
 *
 *	  Initial code from https://rosettacode.org/wiki/Parsing/RPN_calculator_algorithm#C
 *	  modifications:
 *		  removed several calls to strok() and simplified main for loop reading
 *		  caller's data no longer modified by program
 *		  caller's data now a pointer to string is updated to point to last read char at return
 *		  accept parameters from an array of parameters (extern endian mparam[]) based on '$'prefix
 *		  accept both double and ints depending on operations to perform
 *		  accept boolean and shift operators (~ > < & ^ |) and modulo op (%) on ints
 *		  accept test operator (?), is equal comparison (=), is different comparison (!)
 *		  accept exp (e), log_n conversion (l), log_10 conversion (L)
 *		  enable truncate to int (i),
 *		  accept mono-operand and tri-operand (and not just bi-operand) operators
 *		  Added a 1 slot Memory component this enables passing results from one calculation to the next
 *		  	(M) pops from RPN stack and adds to memory
 *		  	(m) pushes memory to RPN stack
 *		  	(Z) zeroes memory
 *		  enable absolute value (a),
 *
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXSTACK	128		// Max depth of RPN calculator stack
#define MAXPARAM	32		// Max number of command parameters
//#define printdata	1
//
// Expression Calculator code
//
double X32RpnCalc(char **s, char* t);
void die(const char *msg);
void push(double v);
double pop();
//
//
typedef union {
	char    cc[4];
	int     ii;
	float   ff;
} endian;
//
double 		stack[MAXSTACK], M;			// RPN stack and Memory slot
endian		mparam[MAXPARAM];			// accept MAXPARAM parameters max
int 		depth;						// RPN stack index
//
// Int Calculator code
//
void die(const char *msg) {
#ifdef _WIN32
	MessageBox(NULL, msg, "stack error", MB_OK);
#else
	printf("error: stack %s\n", msg);
#endif
}

void push(double v) {
	if (depth >= MAXSTACK) {
		die("overflow");
	} else {
		stack[depth++] = v;
	}
}

double pop() {
	if (!depth) {
		die("underflow");
		return 0.0;
	}
	return stack[--depth];
}

double X32RpnCalc(char **k, char* t) {
	double a, b, c;
	char *s, *w, *e;
	int i;

	for (s = w = *k, depth = 0; *s && (*s != ']'); s = ++e) {
#ifdef printdata
		int l;
		printf("%s\n", w);
		l = s-w; for (i = 0; i < l; i++) printf(" "); (w[l] == ' ') ? printf(" ^") : printf("^");
		printf(", M: %.2f, Stk:", M);
		for (i = 0; i < depth; i++) printf(" %.2f,", stack[i]); printf("\n\n");
#endif
		// check whether s points to some kind of numerical data
		if (*s == '$') {			// data comes from mparam, depending on type
			if (t) {
				i = strtol(++s, &e, 10);
				if      (t[i] == 'f') a = mparam[i].ff;
				else if (t[i] == 'i') a = mparam[i].ii;
				else a = 0.0;
			} else {
				a = mparam[strtol(++s, &e, 10)].ii;
			}
		} else {					// data comes from inline
			a = strtod(s, &e);
		}
		// push to stack if valid numerical data (e > s then)
		if (e > s) push(a);
		// else (e == s) this may be an operator... check for known ones
#define triop(x) c = pop(), b = pop(), a = pop(), push(x)
#define binop(x) b = pop(), a = pop(), push(x)
#define monop(x) a = pop(), push(x)
#define noop()
		else if (*s == '?')	triop(a ? b : c);
		else if (*s == '+')	binop(a + b);
		else if (*s == '-')	binop(a - b);
		else if (*s == '*')	binop(a * b);
		else if (*s == '/')	binop(a / b);
		else if (*s == 'e')	monop(exp(a));
		else if (*s == 'l')	monop(log(a));
		else if (*s == 'L')	monop(log10(a));
		else if (*s == '%')	binop((int)a % (int)b);
		else if (*s == '^')	binop((int)a ^ (int)b);
		else if (*s == '&')	binop((int)a & (int)b);
		else if (*s == '|')	binop((int)a | (int)b);
		else if (*s == '>')	binop((int)a >> (int)b);
		else if (*s == '<')	binop((int)a << (int)b);
		else if (*s == '=')	binop((int)a == (int)b);
		else if (*s == '!')	binop((int)a != (int)b);
		else if (*s == '~')	monop(~(int)a);
		else if (*s == 'i')	monop((int)a);
		else if (*s == 'a')	monop(fabs(a));
		else if (*s == 'M')	M += pop();
		else if (*s == 'm')	push (M);
		else if (*s == 'Z')	M = 0.;
		else noop();
#undef noop
#undef monop
#undef binop
#undef triop
		if ((*s == ']') || (*e == ']')) break;
	}
	// done ; check stack empty, update string pointer and return final result
	*k = s;
	if (depth > 1) {
		die("leftover");
		return 0.0;
	} else {
		return pop();
	}
}
//
// End of Expression Calculator code
//

