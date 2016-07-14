//
// fxparse.c
//
//  Created on: 25 oct. 2014
//      Author: Patrick-Gilles Maillot
//
//      This/These functions deal only with Effects, parsing scene data to float/int data sent to X32;
//      They are called in/from X32SetScene()
//		They are common to fparse1.c and fparse5.c
//
//
// change Log: July 14, 2016 - line 203, test should be done against PQ5S, not PQ5
//
#include <stdio.h>
#include <string.h>
#include <math.h>

extern int Xsprint(char *bd, int index, char format, void *bs);
extern float Xr_float();

extern FILE* Xin;

// pointers to known Effect names for effects 5-8
extern char* l_fmode[];
extern int ll_fmode;

extern char* l_fwave[];
extern int ll_fwave;

extern char* l_srange[];
extern int ll_srange;

extern char* l_plfreq[];
extern int ll_plfreq;

extern char* l_phfreq[];
extern int ll_phfreq;

extern char* l_pasel[];
extern int ll_pasel;

extern char* l_qllfreq[];
extern int ll_qllfreq;

extern char* l_qmlfreq[];
extern int ll_qmlfreq;

extern char* l_qhlfreq[];
extern int ll_qhlfreq;

extern char* l_rulc[];
extern int ll_rulc;

//
// Macros
//
// log2float: The principle is to convert to y [0, 1] a data comprised between
// xmin and xmax in the log domain. We apply an afine conversion to get the value:
// y = (log(x) - log(xmin)) / (log(xmax) - log(xmin)), all log are base 10.
// -1- logs can simplify to ln (neper base) easily (log10(x) = ln(x)/ln(10))
// -2- ln(a) - ln(b) = ln(a/b)
// The applied formula becomes
// y = ln(x / xmin) / ln (xmax / xmin)
// in the macro: x is the returned value of Xr_float(), a = xmin, and b = ln (xmax / xmin)
#define log2float(a,b) do {						\
		fval = log(Xr_float() / a) / b;			\
		k = Xsprint(buf, k, 'f', &fval);		\
	} while (0)

// afine2float: The principle is to convert to y [0, 1] a data comprised between
// xmin and xmax in the linear domain. We apply an afine conversion to get the value:
// y = (x - xmin) / (xmax - xmin)
// in the macro: x is the returned value of Xr_float(), a = xmin, and b = (xmax - xmin)
#define afine2float(a,b) do {					\
		fval = ((Xr_float() - a) / b);			\
		k = Xsprint(buf, k, 'f', &fval);		\
	} while (0)

// ratio2float: The principle is to convert to y [0, 1] a data comprised between
// xmin and xmax in the linear domain with a special case of xmin = 0. We apply a
// simple conversion to get the value:
// y = x / xmax
// in the macro: x is the retuned value of Xr_float(), a = xmax
#define ratio2float(a) do {						\
		fval = (Xr_float() / a); 				\
		k = Xsprint(buf, k, 'f', &fval);		\
	} while (0)

// toggle2int: The principle is to convert to y [0 or 1]integer a string data
// that can take two values. We only test the value corresponding to result = 0.
// All other string values will convert to result = 1.
// in the macro: a points to the character string corresponding to result = 0.
#define toggle2int(a) do {						\
		ival = fscanf(Xin, "%s", llread); 		\
		if (strcmp(llread, a) == 0) ival = 0;	\
		else ival = 1;							\
		k = Xsprint(buf, k, 'i', &ival);		\
	} while (0)

// list2int: The principle is to convert to y [0 to n]integer a string data
// that can take multiple values. We test values until we get a corresponding result.
// in the macro: l_list points to an array of character strings corresponding valid
// values and l_len is the array size. If the data is not found, the macro returns
// value of 0.
#define list2int(l_list, l_len) do {				\
		ival = fscanf(Xin, "%s", llread); 			\
		ival = 0;									\
		for (mi = 0; mi < l_len; mi++)	{			\
			if (strcmp(llread, l_list[mi]) == 0) {	\
				ival = mi;							\
				break;								\
			}										\
		}											\
		k = Xsprint(buf, k, 'i', &ival);			\
	} while (0)




int XGEQ2_TEQ2(char* buf, int k) {
	int i;
	float fval;

	k = Xsprint(buf, k, 's', ",ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
	for (i = 0; i < 64; i++) afine2float(-15.,30.); // EQ values [-15, 15]
	return (k);
}

int XGEQ_TEQ(char* buf, int k) {
	int i;
	float fval;

	k = Xsprint(buf, k, 's', ",ffffffffffffffffffffffffffffffff");
	for (i = 0; i < 32; i++) afine2float(-15.,30.); // EQ values [-15, 15]
	return (k);
}

int XDES_DES2(char* buf, int k, int ifx, int DES2, int DES) {
	float fval;
	int ival;
	char llread[16];

	k = Xsprint(buf, k, 's', ",ffffii");
	ratio2float(50.);				// lo band [0, 50]
	ratio2float(50.);				// hi band [0, 50]
	ratio2float(50.);				// lo band [0, 50]
	ratio2float(50.);				// hi band [0, 50]
	toggle2int("FEM");				// voice [FEM / MALE]
	if (ifx == DES2) 		toggle2int("FEM");	// voice [FEM / MALE]
	else if (ifx == DES) 	toggle2int("ST");	// voice [ST / M/S]
	return (k);
}

int XPIA_PIA2(char* buf, int k, int ifx, int P1A2, int P1A) {
	float fval;
	int mi, ival;
	char llread[16];

	if (ifx == P1A)			k = Xsprint(buf, k, 's', ",iffifffifii");
	else if (ifx == P1A2)	k = Xsprint(buf, k, 's', ",iffifffifiiiffifffifii");
	toggle2int("OFF");				// active [OFF / ON]
	afine2float(-12., 24.);			// gain [-12, 12]
	ratio2float(10.);				// lo boost [0, 10]
	list2int(l_plfreq, ll_plfreq);	// lo freq [0, 30, 60, 100]
	ratio2float(10.);				// lo attenuation [0, 10]
	ratio2float(10.);				// hi width [0, 10]
	ratio2float(10.);				// hi boost [0, 10]
	list2int(l_phfreq, ll_phfreq);	// lo freq [3K, 4K, 5K, 8K, 10K, 12K, 16K]
	ratio2float(10.);				// hi attenuation [0, 10]
	list2int(l_pasel, ll_pasel);	// lo freq [5K, 10K, 20K]
	toggle2int("OFF");				// transformer [OFF / ON]
	if (ifx == P1A2) {
		toggle2int("OFF");				// active [OFF / ON]
		afine2float(-12., 24.);			// gain [-12, 12]
		ratio2float(10.);				// lo boost [0, 10]
		list2int(l_plfreq, ll_plfreq);	// lo freq [0, 30, 60, 100]
		ratio2float(10.);				// lo attenuation [0, 10]
		ratio2float(10.);				// hi width [0, 10]
		ratio2float(10.);				// hi boost [0, 10]
		list2int(l_phfreq, ll_phfreq);	// lo freq [3K, 4K, 5K, 8K, 10K, 12K, 16K]
		ratio2float(10.);				// hi attenuation [0, 10]
		list2int(l_pasel, ll_pasel);	// lo freq [5K, 10K, 20K]
		toggle2int("OFF");				// transformer [OFF / ON]
	}
	return (k);
}

int XPQ5_PQ5S(char* buf, int k, int ifx, int PQ5S, int PQ5) {
	float fval;
	int mi, ival;
	char llread[16];

	if (ifx == PQ5)			k = Xsprint(buf, k, 's', ",ififififi");
	else if (ifx == PQ5S)	k = Xsprint(buf, k, 's', ",ififififiififififi");
	toggle2int("OFF");					// active [OFF / ON]
	afine2float(-12., 24.);				// gain [-12, 12]
	list2int(l_qllfreq, ll_qllfreq);	// lo freq [200, 300, 500, 700, 1000]
	ratio2float(10.);					// lo boost [0, 10]
	list2int(l_qmlfreq, ll_qmlfreq);	// mid freq [200, 300, 500, 700, 1k, 1k5, 2k, 3k, 4k, 5k, 7k]
	ratio2float(10.);					// mid boost [0, 10]
	list2int(l_qhlfreq, ll_qhlfreq);	// hi freq [1k5, 2k, 3k, 4k, 5k]
	ratio2float(10.);					// hi boost [0, 10]
	toggle2int("OFF");					// transformer [OFF / ON]
	if (ifx == PQ5S) {
		toggle2int("OFF");					// active [OFF / ON]
		afine2float(-12., 24.);				// gain [-12, 12]
		list2int(l_qllfreq, ll_qllfreq);	// lo freq [200, 300, 500, 700, 1000]
		ratio2float(10.);					// lo boost [0, 10]
		list2int(l_qmlfreq, ll_qmlfreq);	// mid freq [200, 300, 500, 700, 1k, 1k5, 2k, 3k, 4k, 5k, 7k]
		ratio2float(10.);					// mid boost [0, 10]
		list2int(l_qhlfreq, ll_qhlfreq);	// hi freq [1k5, 2k, 3k, 4k, 5k]
		ratio2float(10.);					// hi boost [0, 10]
		toggle2int("OFF");					// transformer [OFF / ON]
	}
	return (k);
}

int XWAVD(char* buf, int k) {
	float fval;

	k = Xsprint(buf, k, 's', ",ffffff");
	afine2float(-100., 200.);			// attack A [-100, 100]
	afine2float(-100., 200.);			// sustain A [-100, 100]
	afine2float(-24., 48.);				// gain A [-24, 24]]
	afine2float(-100., 200.);			// attack B [-100, 100]
	afine2float(-100., 200.);			// sustain B [-100, 100]
	afine2float(-24., 48.);				// gain B [-24, 24]]
	return (k);
}


int XLIM(char* buf, int k) {
	float fval;
	int ival;
	char llread[16];

	k = Xsprint(buf, k, 's', ",ffffffii");
	ratio2float(18.);					// input gain [0, 18]
	afine2float(-18., 18.);				// out gain [-18, 0]
	ratio2float(100.);					// squeeze [0, 100]
	ratio2float(10.);					// knee [0, 10]
	log2float(0.05, 2.995732274);		// attack[0.05, 1]
	log2float(20., 4.605170186);		// release[20, 2000]
	toggle2int("OFF");					// stereo link [OFF / ON]
	toggle2int("OFF");					// auto gain [OFF / ON]
	return (k);
}


int XFAC_FAC1M_FAC2(char* buf, int k, int ifx, int FAC2, int FAC1M, int FAC) {
	float fval;
	int ival, i, j;
	char llread[16];

	j = 1;
	if (ifx == FAC) {
		k = Xsprint(buf, k, 's', ",iffffff");
	}
	else if (ifx == FAC1M || ifx == FAC2) {
		k = Xsprint(buf, k, 's', ",iffffffiffffff");
		j = 2;
	}
	for (i = 0; i < j; i++) {
		toggle2int("OFF");					// active [OFF / ON]
		afine2float(-20., 20.);				// input gain [-20, 0]
		ratio2float(10.);					// threshold [0, 10]
		ratio2float(6.);					// time [0, 6]
		ratio2float(100.);					// bias [0, 100]
		afine2float(-18., 24.);				// gain [-18, 6]
		afine2float(-100., 200.);			// balance [-100, 100]
	}
	return(k);
}

int XLEC_LEC2(char* buf, int k, int ifx, int LEC2, int LEC) {
	float fval;
	int ival, i, j;
	char llread[16];

	j = 1;
	if (ifx == LEC){
		k = Xsprint(buf, k, 's', ",iffif");
	} else if (ifx == LEC2) {
		k = Xsprint(buf, k, 's', ",iffififfif");
		j = 2;
	}
	for (i = 0; i < j; i++) {
		toggle2int("OFF");					// active [OFF / ON]
		ratio2float(100.);					// gain [0, 100]
		ratio2float(100.);					// peak [0, 100]
		toggle2int("COMP");					// mode [COMP, LIM]
		afine2float(-18., 24.);				// gain [-18, 6]
	}
	return (k);
}

int XULC_ULC2(char* buf, int k, int ifx, int ULC2, int ULC) {
	float fval;
	int ival, mi, i, j;
	char llread[16];

	j = 1;
	if (ifx == ULC){
		k = Xsprint(buf, k, 's', ",iffffi");
	} else if (ifx == ULC2) {
		k = Xsprint(buf, k, 's', ",iffffiiffffi");
		j = 2;
	}
	for (i = 0; i < j; i++) {
		toggle2int("OFF");					// active [OFF / ON]
		afine2float(-48., 48.);				// input gain [-48, 0]
		afine2float(-48., 48.);				// out gain [-48, 0]
		afine2float(1., 6.);				// attack [1, 7]
		afine2float(1., 6.);				// release [1, 7]
		list2int(l_rulc, ll_rulc);			// ratio [4, 8, 12, 20, ALL]
	}
	return (k);
}

int XENH_ENH2(char* buf, int k, int ifx, int ENH2, int ENH) {
	float fval;
	int ival, i, j;
	char llread[16];

	j = 1;
	if (ifx == ENH){
		k = Xsprint(buf, k, 's', ",ffffffffi");
	} else if (ifx == ENH2) {
		k = Xsprint(buf, k, 's', ",ffffffffiffffffffi");
		j = 2;
	}
	for (i = 0; i < j; i++) {
		afine2float(-12., 24.);				// out gain [-12, 12]
		ratio2float(100.);					// spead [0, 100]
		ratio2float(100.);					// bass gain [0, 100]
		afine2float(1., 49.);				// bass freq [1, 50]
		ratio2float(100.);					// mid gain [0, 100]
		afine2float(1., 49.);				// mid q [1, 50]
		ratio2float(100.);					// hi gain [0, 100]
		afine2float(1., 49.);				// hi freq [1, 50]
		toggle2int("OFF");					// solo [OFF / ON]
	}
	return (k);
}

int XEXC_EXC2(char* buf, int k, int ifx, int EXC2, int EXC) {
	float fval;
	int ival, i, j;
	char llread[16];

	j = 1;
	if (ifx == EXC){
		k = Xsprint(buf, k, 's', ",ffffffi");
	} else if (ifx == EXC2) {
		k = Xsprint(buf, k, 's', ",ffffffiffffffi");
		j = 2;
	}
	for (i = 0; i < j; i++) {
		log2float(1000., 2.302585093);		// tune [1k, 10k]
		ratio2float(100.);					// peak [0, 100]
		ratio2float(100.);					// zero fill [0, 100]
		afine2float(-50., 100);				// timbre [-50, 50]
		ratio2float(100.);					// harmonics [0, 100]
		ratio2float(100.);					// mix [0, 100]
		toggle2int("OFF");					// solo [OFF / ON]
	}
	return (k);
}

int XIMG(char* buf, int k) {
	float fval;

	k = Xsprint(buf, k, 's', ",fffffff");
	afine2float(-100., 200.);			// balance [-100, 100]
	afine2float(-100., 200.);			// mono pan [-100, 100]
	afine2float(-100., 200.);			// stereo pan [-100, 100]
	ratio2float(12.);					// shv gain [0, 12]
	log2float(100., 2.302585093);		// shv freq [100, 1000]
	log2float(1., 2.302585093);			// shv q [1, 10]
	afine2float(-12., 24.);				// out gain [-12, 12]
	return (k);
}

int XEDI(char* buf, int k) {
	float fval;
	int ival;
	char llread[16];

	k = Xsprint(buf, k, 's', ",iiifffff");
	toggle2int("OFF");					// active [OFF / ON]
	toggle2int("ST");					// stereo input [ST / M/S]
	toggle2int("ST");					// stereo output [ST / M/S]
	afine2float(-50., 100);				// st spread [-50, 50]
	afine2float(-50., 100);				// lmf spread [-50, 50]
	afine2float(-50., 100);				// balance [-50, 50]
	afine2float(-50., 100);				// center distance [-50, 50]
	afine2float(-12., 24.);				// out gain [-12, 12]
	return (k);
}

int XSON(char* buf, int k) {
	float fval;
	int ival;
	char llread[16];

	k = Xsprint(buf, k, 's', ",ifffifff");
	toggle2int("OFF");					// active [OFF / ON]
	ratio2float(10.);					// lo contour [0, 10]
	ratio2float(10.);					// process [0, 10]
	afine2float(-12., 24.);				// out gain [-12, 12]
	toggle2int("OFF");					// active [OFF / ON]
	ratio2float(10.);					// lo contour [0, 10]
	ratio2float(10.);					// process [0, 10]
	afine2float(-12., 24.);				// out gain [-12, 12]
	return (k);
}

int XAMP_AMP2(char* buf, int k, int ifx, int AMP2, int AMP) {
	float fval;
	int  ival, i, j;
	char llread[16];

	j = 1;
	if (ifx == AMP){
		k = Xsprint(buf, k, 's', ",ffffffffi");
	} else if (ifx == AMP2) {
		k = Xsprint(buf, k, 's', ",ffffffffiffffffffi");
		j = 2;
	}
	for (i = 0; i < j; i++) {
		ratio2float(10.);					// preamp [0, 10]
		ratio2float(10.);					// buzz [0, 10]
		ratio2float(10.);					// punch [0, 10]
		ratio2float(10.);					// crunch [0, 10]
		ratio2float(10.);					// drive [0, 10]
		ratio2float(10.);					// low [0, 10]
		ratio2float(10.);					// high [0, 10]
		ratio2float(10.);					// level [0, 10]
		toggle2int("OFF");					// cabinet [OFF / ON]
	}
	return (k);
}

int XDRV_DRV2(char* buf, int k, int ifx, int DRV2, int DRV) {
	float fval;
	int  i, j;

	j = 1;
	if (ifx == DRV){
		k = Xsprint(buf, k, 's', ",ffffffffff");
	} else if (ifx == DRV2) {
		k = Xsprint(buf, k, 's', ",ffffffffffffffffffff");
		j = 2;
	}
	for (i = 0; i < j; i++) {
		ratio2float(100.);					// drive [0, 100]
		ratio2float(50.);					// even ear [0, 50]
		ratio2float(50.);					// odd ear [0, 50]
		afine2float(-12, 24.);				// gain [-12, 12]
		log2float(20., 2.302585093);		// lo cut [20, 200]
		log2float(4000., 1.609437912);		// hi cut [4k, 20k]
		afine2float(-12, 24.);				// lo gain [-12, 12]
		log2float(50., 2.079441542);		// lo freq [50, 400]
		afine2float(-12, 24.);				// hi gain [-12, 12]
		log2float(1000., 2.302585093);		// hi freq [1k, 10k]
	}
	return (k);
}

int XPHAS(char* buf, int k) {
	float fval;

	k = Xsprint(buf, k, 's', ",ffffffffffff");
	log2float(0.05, 4.605170186);		// speed HZ [0.05, 5]
	ratio2float(100.);					// depth [0, 100]
	ratio2float(80.);					// resonance [0, 80]
	ratio2float(50.);					// base [0, 50]
	afine2float(2., 10.);				// stages [2, 12]
	ratio2float(100.);					// mix [0, 100]
	afine2float(-50.,100.);				// wave [-50, 50]
	ratio2float(180.);					// phase [0, 180]
	afine2float(-100.,200.);			// env. modulation [-100, 100]
	log2float(10., 4.605170186);		// attack [10, 1000]
	log2float(1., 7.60090246);			// hold [1, 2000]
	log2float(10., 4.605170186);		// release [10, 1000]
	return (k);
}

int XFILT(char* buf, int k) {
	float fval;
	int ival, mi;
	char llread[16];

	k = Xsprint(buf, k, 's', ",ffffififffffii");
	log2float(0.05, 5.991464547);		// speed HZ [0.05, 20]
	ratio2float(100.);					// depth [0, 100]
	ratio2float(100.);					// resonance [0, 100]
	log2float(20., 6.620073207);		// base [20 , 15000]
	list2int(l_fmode, ll_fmode);		// mode [LP, HP, BP, NO]
	ratio2float(100.);					// mix [0, 100]
	list2int(l_fwave, ll_fwave);		// wave [TRI, SIN, SAW, SAW-, RMP, SQU. RND]
	ratio2float(180.);					// phase [0, 180]
	afine2float(-100.,200.);			// env. modulation [-100, 100]
	log2float(10., 3.218875825);		// attack [10, 250]
	log2float(10., 3.912023005);		// Release [10, 500]
	ratio2float(100.);					// drive [0, 100]
	toggle2int("2POL");					// 4 pole [off/on]
	toggle2int("OFF");					// sidechain [off/on]
	return (k);
}

int XPAN(char* buf, int k) {
	float fval;

	k = Xsprint(buf, k, 's', ",fffffffff");
	log2float(0.05, 4.605170186);		// speed HZ [0.05, 5]
	ratio2float(180.);					// phase [0, 180]
	afine2float(-50.,100.);				// wave [-50, 50]
	ratio2float(100.);					// depth [0, 100]
	ratio2float(100.);					// env speed [0, 100]
	ratio2float(100.);					// env depth [0, 100]
	log2float(10., 4.605170186);		// attack [10, 1000]
	log2float(1., 7.60090246);			// hold [1, 2000]
	log2float(10., 4.605170186);		// release [10, 1000]
	return (k);
}

int XSUB(char* buf, int k) {
	float fval;
	int ival, mi;
	char llread[16];

	k = Xsprint(buf, k, 's', ",iifffiifff");
	toggle2int("OFF");					// active [off/on]
	list2int(l_srange, ll_srange);		// range [LO, MID, HI]
	ratio2float(100.);					// dry [0, 100]
	ratio2float(100.);					// octave -1 [0, 100]
	ratio2float(100.);					// octave -2 [0, 100]
	toggle2int("OFF");					// active [off/on]
	list2int(l_srange, ll_srange);		// range [LO, MID, HI]
	ratio2float(100.);					// dry [0, 100]
	ratio2float(100.);					// octave -1 [0, 100]
	ratio2float(100.);					// octave -2 [0, 100]
	return (k);
}
