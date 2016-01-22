//
// Xscene2X.c
//
//  Created on: 25 oct. 2014
//      Author: Patrick-Gilles Maillot
//
#include <string.h>
#include <stdio.h>
#include <math.h>

// External calls used
extern int Xsprint(char *bd, int index, char format, void *bs);

extern FILE* Xin;
//
// Used in this file:

int XOff_On(char *buf, int k, char* Crit_0) {
	int ival;
	char llread[16]; // max length for enum string argument
//
// "Crit_0 = 0" will generate a 0; any other value will set a 1
	ival = fscanf(Xin, "%s", llread);
	k = Xsprint(buf, k, 's', ",i");
	ival = (strcmp(llread, Crit_0) == 0)? 0 : 1;
	k = Xsprint(buf, k, 'i', &ival);
	return (k);
}

float Xr_float() {
char llread[16]; // max length for float argument when read as a string
float fval;
int i, l, ival, idec;
//
// float number, in the form nnn, nnn.ff, or nnnkff
// let's read data and search for punctuation ('.' or 'k')
	i = fscanf(Xin, "%s", llread);
	l = strlen(llread);
	for (i = 0; i < l; i++) {
		if (llread[i] == '.') {
			sscanf(llread, "%f", &fval);
			return (fval);
		} else if (llread[i] == 'k') {
			ival = 0; idec = 0;
			llread[i] = 0;
			if (i > 0) sscanf(llread, "%d", &ival);
			if (i < l) sscanf(llread + i + 1, "%d", &idec);
			fval = (float)ival * 1000.;
			if (l-i == 2) fval += (float)idec * 100.;
			else if (l-i == 3) fval += (float)idec * 10.;
			else if (l-i == 4) fval += (float)idec;
			return (fval);
		}
	}
//no punctuation mark case
	sscanf(llread, "%f", &fval);
	return (fval);
}

int Xp_percent(char *buf, int k) {
	float fval;
	int i;

	i = fscanf(Xin, "%f", &fval);
	i = k; // This is to avoid warning (i initialized but not used)
	fval /= 100.;
	k = Xsprint(buf, i, 's', ",f");
	k = Xsprint(buf, k, 'f', &fval);
	return (k);
}

int Xp_linf(char *buf, int k, float xmin, float lmaxmin, float xstep) {
	float fval;
	int i;

	fval = Xr_float();
//	fval = (fval - xmin) / (xmax-xmin) and lmaxmin = (xmax-xmin)
	fval = (fval - xmin) / lmaxmin;
// round to xstep value
	xstep = lmaxmin/xstep;
//	fval = ceilf(fval*xstep) / xstep;
	fval = roundf(fval*xstep) / xstep;
	if (fval <= 0.) fval = 0.; // avoid -0.0 values (0x80000000)
	if (fval > 1.) fval = 1.;
	i = k; // This is to avoid warning (i initialized but not used)
	k = Xsprint(buf, i, 's', ",f");
	k = Xsprint(buf, k, 'f', &fval);
	return (k);
}

int Xp_logf(char *buf, int k, float xmin, float lmaxmin, int nsteps) {
	float fval;
	int i;

//	nsteps -= 1;
//	i = fscanf(Xin, "%f", &fval);
	fval = Xr_float();
//	fval = log(fval / xmin) / log(xmax / xmin); // short for log(fval) - log(xmin) / log(xmax) - log (xmin)
	fval = log(fval / xmin) / lmaxmin; // lmaxmin = log(xmax / xmin)
// round to nsteps' value of log()
	fval = roundf(fval * nsteps) / nsteps;
//	if (lmaxmin > 0.) fval = roundf(fval * nsteps) / nsteps;
//	else              fval = floorf(fval * nsteps) / nsteps;
	if (fval <= 0.) fval = 0.; // avoid -0.0 values (0x80000000)
	if (fval > 1.) fval = 1.;
	i = k; // This is to avoid warning (i initialized but not used)
	k = Xsprint(buf, i, 's', ",f");
	k = Xsprint(buf, k, 'f', &fval);
	return (k);
}

int Xp_int(char *buf, int k) {
	int ival, i;

	i = fscanf(Xin, "%d", &ival);
	i = k; // This is to avoid warning (i initialized but not used)
	k = Xsprint(buf, i, 's', ",i");
	k = Xsprint(buf, k, 'i', &ival);
	return (k);
}

int Xp_str(char *buf, int k) {
	int i;
	char llread[32], ch;

// ! spaces are authorized. Names are between '"' characters
// search for first '"'
	do {
		ch = (char)fgetc(Xin);
	} while(ch != '"');
// now read name until next '"' found, and replace it with \0
	i = 0;
	do {
		llread[i++] = ch = (char)fgetc(Xin);
	} while(ch != '"');
	llread[i - 1] = 0;
	k = Xsprint(buf, k, 's', ",s");
	k = Xsprint(buf, k, 's', llread);
	return(k);
}

int Xp_list(char *buf, int k, char **list, int list_max) {
	int ival;
	char llread[16];

	ival = fscanf(Xin, "%s", llread);
//	printf("got %s\n", llread);
	for (ival = 0; ival < list_max; ival++) {
//		printf("comparing to %s\n", list[ival]);
		if (strcmp(llread, list[ival]) == 0) {
			k = Xsprint(buf, k, 's', ",i");
			k = Xsprint(buf, k, 'i', &ival);
			break;
		}
	}
	return(k);
}

int Xp_fxlist(char *buf, int k, char **list, int list_max, int *p_ival) {
	int i;
	char llread[16];

	i = fscanf(Xin, "%s", llread);
//	printf("got %s\n", llread);
	for (i = 0; i < list_max; i++) {
//		printf("comparing to %s\n", list[ival]);
		if (strcmp(llread, list[i]) == 0) {
			*p_ival = i; // we need the index value back
			k = Xsprint(buf, k, 's', ",i");
			k = Xsprint(buf, k, 'i', p_ival);
			break;
		}
	}
	return(k);
}

int Xp_bit(char *buf, int k) {
	int		i, j, ival;
	char	ch, llread[40]; // can easily have 32 bits

	i = fscanf(Xin, "%s", llread);
	i = strlen(llread) - 1;
	j = 0;
	ival = 0;
	while ((ch = llread[i]) != '%') {
//		ival |= ((ch - '0') << j);
		if (ch == '1') ival |= (1<<j); // safer than (ch - '0')
		j++;
		i--;
	}
	k = Xsprint(buf, k, 's', ",i");
	k = Xsprint(buf, k, 'i', &ival);
return (k);
}



int Xp_frequency(char *buf, int k, int nsteps) {
	float fval;
//
	fval = Xr_float();
// now have to convert between 0 (20) and 1 (20000)
//	fval = (log(fval) - log(20.)) / (log(20000.) - log(20.));
//	fval = (log(fval/20.)) / (log(1000.));
	fval = (log(fval/20.)) / 6.907755279;
	fval = roundf(fval * nsteps) / nsteps;
	if (fval <= 0.) fval = 0.; // avoid -0.0 values (0x80000000)
	if (fval > 1.) fval = 1.;
	k = Xsprint(buf, k, 's', ",f");
	k = Xsprint(buf, k, 'f', &fval);
	return (k);
}

int Xp_level(char *buf, int k, int nsteps) {
char llread[16];
float fval;
int i;
//
//	level: a float with value in [0.0...1.0 (+10 dB), steps]  ->  4	"linear" dB ranges:
//	0.0...0.0625 (-oo, -90...-60 dB),
//	0.0625...0.25 (-60...-30 dB),
//	0.25...0.5 (-30...-10dB) and
//	0.5...1.0 (-10...+10dB)
	i = fscanf(Xin, "%s", llread);
// manage "minus infinity"
	if (llread[0] == '-' && llread[1] == 'o' && llread[2] == 'o') fval = 0.0;
	else {
		i = sscanf(llread, "%f", &fval);
		if (fval < -60.) {
// first slope, make sure we don't generate negative values
//			if ((fval = 0.0625 / 30. * (fval + 90.)) < 0.0) fval = 0.0;
			fval = fval * 0.00208333333 + 0.1875;
			fval = roundf(fval * nsteps) / nsteps;
			if (fval < 0.0) fval = 0.0;
		} else if (fval < -30.) {
// second slope
//			fval = 0.0625 + (0.25 - 0.0625) / 30. * (fval + 60.);
			fval = 0.00625 * fval + 0.4375;
			fval = roundf(fval * nsteps) / nsteps;
		} else if (fval < -10.) {
// third slope
//			fval = 0.25 + 0.25 / 20. * (fval + 30.);
			fval = 0.0125 * fval + 0.625;
			fval = roundf(fval * nsteps) / nsteps;
		} else if (fval <= 10.) {
// fourth and high values slope; make sure we don't go over 1.0
//			if ((fval = 0.5 + 0.5 / 20. * (fval + 10.)) > 1.0) fval = 1.0;
			if ((fval = (fval * 0.025 + 0.75)) > 1.0) fval = 1.0;
		}
	}
	i = k;
	k = Xsprint(buf, i, 's', ",f");
	k = Xsprint(buf, k, 'f', &fval);
	return (k);
}


