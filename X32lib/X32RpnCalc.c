/*
 * X32RpnCalc.c
 *
 *  Created on: Jul 8, 2018
 *      Author: Patrick-Gilles Maillot
 *
 *      Reverse polish notation calculator with a set of operators listed below:
 *
 *      Initial code from https://rosettacode.org/wiki/Parsing/RPN_calculator_algorithm#C
 *      modifications:
 *          removed several calls to strok() and simplified main for loop reading
 *          caller's data no longer modified by program
 *          caller's data now a pointer to string is updated to point to last read char at return
 *          accept parameters from an array of parameters (param[]) based on '$'prefix
 *          accept both double and ints depending on operations to perform
 *          accept boolean operators (~ >> << & ^ |) and modulo op (%) on ints
 *          accept test operator (?), is equal comparison (=), is different comparison (!)
 *          accept exp (e), log_n conversion (l), log_10 conversion (L)
 *          enable truncate to int (i),
 *          accept mono-operand and tri-operand (and not just bi-operand) operators
 *
 *
 */
#include <math.h>
#include <stdlib.h>
#define MAXSTACK	128		// Max depth of RPN calculator stack
#define MAXPARAM	32		// Max midi parameters in commands
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
double 		stack[MAXSTACK];			// RPN stack
endian		mparam[MAXPARAM];			// accept MAXPARAM parameters max
int 		depth;						// RPN stack index
//
// Int Calculator code
void die(const char *msg) {
#ifdef printdata
    printf("error: %s", msg);
#endif
    abort();
}

void push(double v) {
    if (depth >= MAXSTACK) die("stack overflow\n");
    stack[depth++] = v;
}

double pop() {
    if (!depth) die("stack underflow\n");
    return stack[--depth];
}

double X32RpnCalc(char **k, char* t) {
    double a, b, c;
    char *s, *w, *e;
    int i;

    for (s = w = *k, depth = 0; *s && (*s != ']'); s = e) {
#ifdef printdata
        int i, l;
        printf("%s\n", w);
        l = s-w; for (i = 0; i < l; i++) printf(" "); (w[l] == ' ') ? printf(" ^\n\n") : printf("^\n\n");
#endif
        if (*s == '$') {
        	if (t) {
				i = strtol(++s, &e, 10);
				if      (t[i] == 'f') a = mparam[i].ff;
				else if (t[i] == 'i') a = mparam[i].ii;
				else a = 0.0;
			} else {
				a = mparam[strtol(++s, &e, 10)].ii;
			}
        } else {
        	a = strtod(s, &e);
        }
        if (e > s) push(a);
#define triop(x) c = pop(), b = pop(), a = pop(), push(x)
#define binop(x) b = pop(), a = pop(), push(x)
#define monop(x) a = pop(), push(x)
#define noop()
        else if (*s == '?') triop(a ? b : c);
        else if (*s == '+')    binop(a + b);
        else if (*s == '-')    binop(a - b);
        else if (*s == '*')    binop(a * b);
        else if (*s == '/')    binop(a / b);
        else if (*s == 'e')    monop(exp(a));
        else if (*s == 'l')    monop(log(a));
        else if (*s == 'L')    monop(log10(a));
        else if (*s == '%')    binop((int)a % (int)b);
        else if (*s == '^')    binop((int)a ^ (int)b);
        else if (*s == '&')    binop((int)a & (int)b);
        else if (*s == '|')    binop((int)a | (int)b);
        else if (*s == '>')    binop((int)a >> (int)b);
        else if (*s == '<')    binop((int)a << (int)b);
        else if (*s == '=')    binop((int)a == (int)b);
        else if (*s == '!')    binop((int)a != (int)b);
        else if (*s == '~')    monop(~(int)a);
        else if (*s == 'i')    monop((int)a);
        else noop();
#undef noop
#undef monop
#undef binop
#undef triop
        e++;
    }
    if (depth != 1) die("stack leftover\n");
    *k = s;
    return pop();
}
//
// End of Expression Calculator code
//

