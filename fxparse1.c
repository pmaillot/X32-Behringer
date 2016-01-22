//
// fxparse1.c
//
//  Created on: 25 oct. 2014
//      Author: Patrick-Gilles Maillot
//
//      This/These functions deal only with Effects, parsing scene data to float/int data sent to X32;
//      They are called in/from X32SetScene()
//
//
#include <stdio.h>
#include <string.h>
#include <math.h>

extern int Xsprint(char *bd, int index, char format, void *bs);
extern float Xr_float();

extern FILE* Xin;

extern int Xverbose;
//
// pointers to known Effect names for effects 1-4
extern char* l_factor[];
extern int ll_factor;

extern char* l_mode[];
extern int ll_mode;

extern char* l_pattern[];
extern int ll_pattern;

extern char* l_dtype[];
extern int ll_dtype;

extern char* l_ddelay[];
extern int ll_ddelay;

extern char* l_bcmb[];
extern int ll_bcmb;

extern char* l_rcmb[];
extern int ll_rcmb;

extern char* 	l_mcmb[];
extern int ll_mcmb;


// external /shared functions
extern int XSUB(char* buf, int k);
extern int XPAN(char* buf, int k);
extern int XFILT(char* buf, int k);
extern int XPHAS(char* buf, int k);
extern int XDRV_DRV2(char* buf, int k, int ifx, int DRV2, int DRV);
extern int XAMP_AMP2(char* buf, int k, int ifx, int AMP2, int AMP);
extern int XSON(char* buf, int k);
extern int XEDI(char* buf, int k);
extern int XIMG(char* buf, int k);
extern int XEXC_EXC2(char* buf, int k, int ifx, int EXC2, int EXC);
extern int XENH_ENH2(char* buf, int k, int ifx, int ENH2, int ENH);
extern int XULC_ULC2(char* buf, int k, int ifx, int ULC2, int ULC);
extern int XLEC_LEC2(char* buf, int k, int ifx, int LEC2, int LEC);
extern int XFAC_FAC1M_FAC2(char* buf, int k, int ifx, int FAC2, int FAC1M, int FAC);
extern int XLIM(char* buf, int k);
extern int XWAVD(char* buf, int k);
extern int XPQ5_PQ5S(char* buf, int k, int ifx, int PQ5S, int PQ5);
extern int XPIA_PIA2(char* buf, int k, int ifx, int P1A2, int P1A);
extern int XDES_DES2(char* buf, int k, int ifx, int DES2, int DES);
extern int XGEQ_TEQ(char* buf, int k);
extern int XGEQ2_TEQ2(char* buf, int k);

//
//FX types for effects slots 1 to 4
//
enum fxtyp4 {HALL = 0, AMBI, RPLT, ROOM, CHAM, PLAT, VREV, VRM,
	GATE, RVRS, DLY, _3TAP, _4TAP, CRS, FLNG, PHAS, DIMC, FILT,
	ROTA, PAN, SUB, D_RV, CR_R, FL_R, D_CR, D_FL, MODD, GEQ2,
	GEQ, TEQ2, TEQ, DES2, DES, P1A, P1A2, PQ5, PQ5S, WAVD,
	LIM, CMB, CMB2, FAC, FAC1M, FAC2, LEC, LEC2, ULC, ULC2,
	ENH2, ENH, EXC2, EXC, IMG, EDI, SON, AMP2, AMP, DRV2,
	DRV, PIT2, PIT};

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

//
// fxparse1()
// Parse FX data for effects 1-4
// Compute and set respective data in buffer (will be sent by caller to X32)
//
// Parameters can be ints or floats:
//
// "float" values are normalized to floats [0..1] for X32
// The range can be linear or log
//
// Linear case:	fval = (Xr_float() - xmin) / (xmax-xmin);
// Log case:	fval = ln(Xr_float() / xmin) / ln(xmax/xmin);
//
//
int fxparse1(char *buf, int k, int ifx) {
	float fval;
	int ival;
	char llread[16];

int mi; //attention: is used in a macro.
int i, j;
//
// ifx is the FX index in the fx table for FX 1..4
	switch (ifx) {
	case (HALL): //Hall - 12 parameters f f f f f f f f f f f f f f f f
	case (PLAT): //Plate - 12 parameters f f f f f f f f f f f f f f f f
		k = Xsprint(buf, k, 's', ",ffffffffffff");
		ratio2float(200.);				// pre delay 0-200
		if (ifx == HALL) 	  log2float(0.2, 3.218895825);	// decay [.2, 5] 3.218895825 = log(5/0.2)
		else if (ifx == PLAT) log2float(0.5, 2.995732274);	// decay [.5, 10] 2.995732274 = log(10/0.5)
		afine2float(2., 98.); 			// size [2, 100]
		log2float(1000., 2.995732274);	// damping HZ [1000, 20000] 2.995732274 = log(20000/1000)
		afine2float(1., 29.);			// Diffuse [1, 30]
		afine2float(-12., 24.);			// level [-12,+12]
		log2float(10., 3.912023005);	// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);	// hi cut HZ [200, 20k] 4.605170186 = log(200/20000)
		log2float(0.5, 1.386294361);	// bass multi [.5, 2] 1.386294361 = log(2/.5)
		if (ifx == HALL) 	  ratio2float(50.);					// spread [0, 50]
		else if (ifx == PLAT) log2float(10., 3.912023005);	// xover [10, 500] 3.912023005 = log(500/10)
		if (ifx == HALL) 	  ratio2float(250.);	// shape [0, 250]
		else if (ifx == PLAT) ratio2float(50.);		// mod [0, 50]
		ratio2float(100.);				// modspeed [0, 100]
		// ignore other params
		break;
	case (AMBI): //Ambiance - 10 parameters f f f f f f f f f f f f f f
		k = Xsprint(buf, k, 's', ",ffffffffff");
		ratio2float(200.);				// pre delay 0-200
		log2float(0.2, 3.597312261);	// decay [.2, 7.3] [ 3.597312261 = (log(7.3) - log(0.2))
		afine2float(2., 98.);			// size [2, 100]
		log2float(1000., 2.995732274);	// damping HZ [1000, 20000] 2.995732274 = log(20000/1000)
		afine2float(1., 29.);			// Diffuse [1, 30]
		afine2float(-12., 24.);			// level [-12,+12]
		log2float(10., 3.912023005);	// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);	// hi cut HZ [200, 20k] [ 4.605170186 = log(200/20000)]
		ratio2float(100.);				// modulate [0, 100]
		ratio2float(100.);				// tail gain [0, 100]
		// ignore other params
		break;
	case RPLT: //Rich Plate Reverb - 16 parameters f f f f f f f f f f f f f f f f f f f f
	case ROOM: //Room - 16 parameters f f f f f f f f f f f f f f f f f f f f
	case CHAM: //Chamber - 16 parameters f f f f f f f f f f f f f f f f f f f f
		k = Xsprint(buf, k, 's', ",ffffffffffffffff");
		ratio2float(200.);  									// pre delay 0-200
		log2float(0.3, 4.571268634);							// decay [.3, 29]
		if (ifx == RPLT)      fval = ((Xr_float() - 4.) / 35.);	// size [4, 39]
		else if (ifx == ROOM) fval = ((Xr_float() - 4.) / 72.);	// size [4, 76]
		else if (ifx == CHAM) fval = ((Xr_float() - 4.) / 72.);	// size [4, 76]
		k = Xsprint(buf, k, 'f', &fval);
		log2float(1000., 2.995732274);				// damping HZ [1000, 20000] 2.995732274 = log(20000/1000)
		ratio2float(100.);							// Diffuse [0, 100]
		afine2float(-12., 24.);						// level [-12,+12]
		log2float(10., 3.912023005);				// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);				// hi cut HZ [200, 20k] 4.605170186 = log(200/20000)
		log2float(.25, 2.772588722);				// bass multi [.25, 4]
		ratio2float(50.);							// spread [0, 50]
		if (ifx == RPLT)      fval = (Xr_float() / 100.);	// attack [0, 100]
		else if (ifx == ROOM) fval = (Xr_float() / 250.);	// shape [0, 250]
		else if (ifx == CHAM) fval = (Xr_float() / 250.);	// shape [0, 250]
		k = Xsprint(buf, k, 'f', &fval);
		ratio2float(100.);							// spin [0, 100]
		if (ifx == CHAM) ratio2float(500.);			// refl. L [0,500]
		else 			 ratio2float(1200.);		// echo L [0, 1200]
		if (ifx == CHAM) ratio2float(500.);			// refl. R [0,500]
		else 			 ratio2float(1200.);		// echo R [0, 1200]
		if (ifx == CHAM) ratio2float(100.);			// refl. gain L [0,100]
		else             afine2float(-100., 200.);	// echo feed L [-100, 100]
		if (ifx == CHAM) ratio2float(100.);			// refl. gain R [0,100]
		else             afine2float(-100., 200.);	// echo feed R [-100, 100]
		// ignore other params
		break;
	case VREV: //Vintage Reverb - 10 parameters f f f i i f f f f f
		k = Xsprint(buf, k, 's', ",fffiifffff");
		ratio2float(120.);  				// pre delay 0-120
		log2float(0.3, 2.708050201);		// decay [.3, 4.5]
		ratio2float(100.);					// modulate [0, 10]
		toggle2int("OFF");					// Vintage [off, on]
		toggle2int("FRONT");				// [front, rear]
		afine2float(-12., 24.);				// level [-12,+12]
		log2float(10., 3.912023005);		// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(10000., 0.693147181);		// hi cut HZ [10k, 20k] 4.605170186 = log(20000/10000)
		log2float(0.5, 1.386294361);		// lo multiply [0.5, 2]
		log2float(0.25, 1.386294361);		// hi multiply [0.25, 1]
		// ignore other params
		break;
	case VRM: //Vintage Room - 13 parameters f f f f f f f f f f f f i
		k = Xsprint(buf, k, 's', ",ffffffffffffi");
		ratio2float(200.);  				// revb delay 0-200
		log2float(0.1, 5.298317367);		// decay [.1, 20]
		ratio2float(100.);					// size [0, 10]
		afine2float(1.,29.);				// density [1, 30]
		ratio2float(100.);					// ER level [0, 190]
		afine2float(-12., 24.);				// level [-12,+12]
		log2float(0.1, 4.605170186);		// lo multiply [0.1, 10]
		log2float(0.1, 4.605170186);		// hi multiply [0.1, 10]
		log2float(10., 3.912023005);		// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);		// hi cut HZ [200, 20k] 4.605170186 = log(20000/200)
		ratio2float(200.);					// ER left [0, 10]
		ratio2float(200.);					// ER right [0, 10]
		toggle2int("OFF");					// freeze [off, on]
		// ignore other params
		break;
	case GATE: //Gated Reverb - 10 parameters f f f f f f f f f f
	case RVRS: //Reverse Reverb - 9 parameters f f f f f f f f f
		if( ifx == GATE) 		k = Xsprint(buf, k, 's', ",ffffffffff");
		else if (ifx == RVRS) 	k = Xsprint(buf, k, 's', ",fffffffff");
		ratio2float(200.);  				// pre-delay 0-200
		afine2float(140., 860.);			// decay [140, 1000]
		if (ifx == GATE) 	  ratio2float(30.);		// attack [0, 30]
		else if (ifx == RVRS) ratio2float(50.); 	//rize [0, 50]
		if (ifx == GATE) 	  afine2float(1.,49.);	// density [1, 50]
		else if (ifx == RVRS) afine2float(1.,29.);	//diffuse [1, 30]
		ratio2float(100.);					// spread [0, 100]
		afine2float(-12., 24.);				// level [-12,+12]
		log2float(10., 3.912023005);		// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);		// hi cut HZ [200, 20k] 4.605170186 = log(20000/200)
		afine2float(-30,30);				// hi shv gain [-30, 0]
		if (ifx == GATE) afine2float(1.,29.);		// diffuse [1, 30]
		// ignore other params
		break;
	case DLY: //Stereo Delay - 12 parameters f f i i i f f f f f f f
		k = Xsprint(buf, k, 's', ",ffiiifffffff");
		ratio2float(100.);  				// mix [0, 100]
		afine2float(1., 2999.);				// time [1, 3000]
		list2int(l_mode, ll_mode);			// mode [ST, X, M]
		list2int(l_factor, ll_factor);		// factor L [1/4, 3/8, 1/2, 2/3, 1, 4/3, 3/2, 2, 3]
		list2int(l_factor, ll_factor);		// factor R [1/4, 3/8, 1/2, 2/3, 1, 4/3, 3/2, 2, 3]
		afine2float(-100., 200.);			// offset L/R [-100, +100]
		log2float(10., 3.912023005);		// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);		// hi cut HZ [200, 20k] 4.605170186 = log(20000/200)
		log2float(10., 3.912023005);		// feed lo cut HZ [10, 500] 3.912023005 = log(500/10)
		ratio2float(100.);  				// feed left [0, 100]
		ratio2float(100.);  				// feed right [0, 100]
		log2float(200., 4.605170186);		// feed hi cut HZ [200, 20k] 4.605170186 = log(20000/200)
		// ignore other params
		break;
	case _3TAP: //3-tap Delay - 15 parameters f f f f f f i f f i f f i i i
		k = Xsprint(buf, k, 's', ",ffffffiffiffiii");
		afine2float(1., 2999.);				// dry [1, 3000]
		ratio2float(100.);					// gain base [0, 100]
		afine2float(-100., 200.);			// pan base [-100, 100]
		ratio2float(100.);					// feedback [0, 100]
		log2float(10., 3.912023005);		// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);		// hi cut HZ [200, 20k] 4.605170186 = log(20000/200)
		list2int(l_factor, ll_factor);		// factorA [1/4...3]
		ratio2float(100.);					// gainA [0, 100]
		afine2float(-100., 200.);			// panA [-100, 100]
		list2int(l_factor, ll_factor);		// factorB [1/4...3]
		ratio2float(100.);					// gainB [0, 100]
		afine2float(-100., 200.);			// panB [-100, 100]
		toggle2int("OFF");					// cross feed [off/on]
		toggle2int("OFF");					// mono [off/on]
		toggle2int("OFF");					// dry [off/on]
		// ignore other params
		break;
	case _4TAP: //4-tap Delay - 15 parameters f f f f f f i f i f i f i i i
		k = Xsprint(buf, k, 's', ",ffffffifififiii");
		afine2float(1., 2999.);				// time [1, 3000]
		ratio2float(100.);					// gain base [0, 100]
		ratio2float(100.);					// feedback [0, 100]
		log2float(10., 3.912023005);		// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);		// hi cut HZ [200, 20k] 4.605170186 = log(20000/200)
		ratio2float(6.);					// spread [0, 6]
		list2int(l_factor, ll_factor);		// factorA [1/4...3]
		ratio2float(100.);					// gainA [0, 100]
		list2int(l_factor, ll_factor);		// factorB [1/4...3]
		ratio2float(100.);					// gainB [0, 100]
		list2int(l_factor, ll_factor);		// factorC [1/4...3]
		ratio2float(100.);					// gainC [0, 100]
		toggle2int("OFF");					// cross feed [off/on]
		toggle2int("OFF");					// mono [off/on]
		toggle2int("OFF");					// dry [off/on]
		// ignore other params
		break;
	case CRS:  //stereo chorus - 11 parameters f f f f f f f f f f f
		k = Xsprint(buf, k, 's', ",fffffffffff");
		log2float(0.05, 4.605170186);		// speed HZ [0.05, 5]
		ratio2float(100.);					// depth L [0, 100]
		ratio2float(100.);					// depth R [0, 100]
		log2float(0.5, 4.605170186);		// delay L [0.5, 50]
		log2float(0.5, 4.605170186);		// delay R [0.5, 50]
		ratio2float(100.);					// mix [0, 100]
		log2float(10., 3.912023005);		// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);		// hi cut HZ [200, 20k] 4.605170186 = log(20000/200)
		ratio2float(180.);					// phase [0, 180]
		ratio2float(100.);					// wave [0, 100]
		ratio2float(100.);					// spread [0, 100]
		// ignore other params
		break;
	case FLNG:   //stereo flanger - 12 parameters f f f f f f f f f f f f
		k = Xsprint(buf, k, 's', ",ffffffffffff");
		log2float(0.05, 4.605170186);		// speed HZ [0.05, 5]
		ratio2float(100.);					// depth L [0, 100]
		ratio2float(100.);					// depth R [0, 100]
		log2float(0.5, 3.688879454);		// delay L [0.5, 20]
		log2float(0.5, 3.688879454);		// delay R [0.5, 20]
		ratio2float(100.);					// mix [0, 100]
		log2float(10., 3.912023005);		// lo cut HZ [10, 500] 3.912023005 = log(500/10)
		log2float(200., 4.605170186);		// hi cut HZ [200, 20k] 4.605170186 = log(20000/200)
		ratio2float(180.);					// phase [0, 180]
		log2float(10., 3.912023005);		// feed lo cut HZ [10, 500]
		log2float(200., 4.605170186);		// feed hi cut HZ [200, 20k]
		afine2float(-90., 180.);			// feed [-90, 90]]
		// ignore other params
		break;
	case PHAS: //stereo phaser - 12 parameters f f f f f f f f f f f f
		k = XPHAS(buf, k);
		break;
	case DIMC: //dimensional chorus - 7 parameters i i i i i i i
		k = Xsprint(buf, k, 's', ",iiiiiii");
		toggle2int("OFF");					// active [off/on]
		toggle2int("M");					// mode [M/ST]
		toggle2int("OFF");					// dry [off/on]
		toggle2int("OFF");					// mode 1 [off/on]
		toggle2int("OFF");					// mode 2 [off/on]
		toggle2int("OFF");					// mode 3 [off/on]
		toggle2int("OFF");					// mode 4 [off/on]
		// ignore other params
		break;
	case FILT: // mood filter - 8 parameters f f f f i f i f f f f f i i
		k = XFILT(buf, k);
		// ignore other params
		break;
	case ROTA: // rotary speaker - 8 parameters f f f f f f i i
		k = Xsprint(buf, k, 's', ",ffffffii");
		log2float(0.1, 3.688879454);		// lo speed HZ [0.1, 4]
		log2float(2, 1.609437912);			// hi speed HZ [2, 10]
		ratio2float(100.);					// accelerate [0, 100]
		ratio2float(100.);					// distance [0, 100]
		afine2float(-100.,200.);			// balance [-100, 100]
		ratio2float(100.);					// mix [0, 100]
		toggle2int("RUN");					// stop [off/on]
		toggle2int("SLOW");					// slow [off/on]
		// ignore other params
		break;
	case PAN: // tremolo/panner - 9 parameters f f f f f f f f f
		k = XPAN(buf, k);
		break;
	case SUB: // suboctaver - 10 parameters i i f f f i i f f f
		k = XSUB(buf, k);
		break;
	case D_RV: // delay/chamber - 12 parameters f i f f f f f f f f f f
	case D_CR: // delay chorus - 12 parameters f i f f f f f f f f f f
	case D_FL: // delay flanger - 12 parameters f i f f f f f f f f f f
		k = Xsprint(buf, k, 's', ",fiffffffffff");
		afine2float(1., 2999.);				// time [1, 3000]
		list2int(l_pattern, ll_pattern);	// pattern [1/4, 1/3, 3/8, 1/2, 2/3, 3/4, 1, 1/4X, 1/3X, 3/8X, 1/2X, 2/3X, 3/4X","1X]
		log2float(1000., 2.995732274);		// feed hi cut [1000, 20000]
		ratio2float(100.);					// feedback [0, 100]
		ratio2float(100.);					// cross feed [0, 100]
		afine2float(-100.,200.);			// balance [-100, 100]
		if (ifx == D_RV) {
			ratio2float(200.);					// pre delay [0, 200]
			log2float(0.1, 3.912023005);		// decay [0.1, 5]
			afine2float(2.,98.);				// size [2, 100]
			log2float(1000., 2.995732274);		// damping [1000, 20000]
			log2float(10., 3.912023005);		// lo cut [10, 500]
		} else if(ifx == D_CR) {
			log2float(0.05, 4.382026635);		// speed [0.05, 4]
			ratio2float(100.);					// depth [0, 100]
			log2float(0.5, 4.605170186);		// delay [0.5, 50]
			ratio2float(180.);					// phase [0, 180]
			ratio2float(100.);					// wave [0, 100]
		} else if(ifx == D_FL) {
			log2float(0.05, 4.382026635);		// speed [0.05, 4]
			ratio2float(100.);					// depth [0, 100]
			log2float(0.5, 3.688879454);		// delay [0.5, 20]
			ratio2float(180.);					// phase [0, 180]
			afine2float(-90.,180.);				// feed [-90, 90]
		}
		ratio2float(100.);					// mix [0, 100]
		// ignore other params
		break;
	case CR_R: // chorus/chamber - 12 parameters f f f f f f f f f f f f
	case FL_R: // flanger/chamber - 12 parameters f f f f f f f f f f f f
	k = Xsprint(buf, k, 's', ",ffffffffffff");
		log2float(0.05, 4.382026635);		// speed [0.05, 4]
		ratio2float(100.);					// depth [0, 100]
		if (ifx == CR_R)      log2float(0.5, 4.605170186);		// delay [0.5, 50]
		else if (ifx == FL_R) log2float(0.5, 3.688879454);		// delay [0.5, 20]
		ratio2float(180.);					// phase [0, 180]
		if (ifx == CR_R)      ratio2float(100.);				// wave [0, 100]
		else if (ifx == FL_R) afine2float(-90.,180.);			// feed [-90, 90]
		afine2float(-100.,200.);			// balance [-100, 100]
		ratio2float(200.);					// pre delay [0, 200]
		log2float(0.1, 3.912023005);		// decay [0.1, 5]
		afine2float(2.,98.);				// size [2, 100]
		log2float(1000., 2.995732274);		// damping [1000, 20000]
		log2float(10., 3.912023005);		// lo cut [10, 500]
		ratio2float(100.);					// mix [0, 100]
		// ignore other params
		break;
	case MODD: // modulation delay - 13 parameters f i f f f f f i i f f f f
		k = Xsprint(buf, k, 's', ",fifffffiiffff");
		afine2float(1.,2999.);			// time [1, 3000]
		list2int(l_ddelay, ll_ddelay);	// delay [1, 1/2, 2/3, 3/2]
		ratio2float(100.);				// feed [0, 100]
		log2float(10., 3.912023005);	// lo cut HZ [10, 500]
		log2float(200., 4.605170186);	// hi cut HZ [200, 20k]
		ratio2float(100.);				// depth R [0, 100]
		log2float(0.05, 5.298317367);	// rate [0.05, 10]
		toggle2int("PAR");				// par/serial [PAR, SER]
		list2int(l_dtype, ll_dtype);	// type [AMB, CLUB, HALL]
		afine2float(1.,9.);				// decay [1, 10]
		log2float(1000., 2.995732274);	// damping [1000, 20000]
		afine2float(-100.,200.);		// balance [-100, 100]
		ratio2float(100.);				// mix [0, 100]
		// ignore other params
		break;
	case GEQ2: // Dual Graphic EQ - 64 parameters f
	case TEQ2: // True Dual Graphic EQ - 64 parameters f
		k = XGEQ2_TEQ2(buf, k);
		break;
	case GEQ: // Stereo Graphic EQ - 32 parameters f
	case TEQ: // True Stereo Graphic EQ - 32 parameters f
		k = XGEQ_TEQ(buf, k);
		// ignore other params
		break;
	case DES2: // dual deesser - 6 parameters f f f f i i
	case DES:  // Stereo deesser - 6 parameters f f f f i i
		k = XDES_DES2(buf, k, ifx, DES2, DES);
		break;
	case P1A:  // stereo program equalizer - 11 parameters i f f i f f f i f i i
	case P1A2: // dual program equalizer - 2X11 parameters i f f i f f f i f i i
		k = XPIA_PIA2(buf, k, ifx, P1A2, P1A);
		break;
	case PQ5:  // stereo midrange equalizer - 9 parameters i f i f i f i f i
	case PQ5S: // dual midrange equalizer - 2X9 parameters i f i f i f i f i
		k = XPQ5_PQ5S(buf, k, ifx, PQ5S, PQ5);
		break;
	case WAVD: // wave designer - 6 parameters f f f f f f
		k = XWAVD(buf, k);
		break;
	case LIM:  // precision limiter - 8 parameters f f f f f f i i
		k = XLIM(buf, k);
		break;
	case CMB: 	// stereo combinator - 29 parameters i i f f f i f i f i i f f f f i f f i f f i f f i f f i i
	case CMB2: 	// dual combinator - 2x29 parameters i i f f f i f i f i i f f f f i f f i f f i f f i f f i i
		if (ifx == CMB) {
			k = Xsprint(buf, k, 's', ",iifffififiiffffiffiffiffiffii");
			j = 1;
		} else { //if (ifx == CMB2)	{
			k = Xsprint(buf, k, 's', ",iifffififiiffffiffiffiffiffiiiifffififiiffffiffiffiffiffii");
			j = 2;
		}
		for (i = 0; i < j; i++) {
			toggle2int("OFF");					// active [OFF / ON]
			list2int(l_bcmb, ll_bcmb);			// band solo [OFF, Bd1, Bd2, Bd3, Bd4, Bd5]
			ratio2float(100.);					// mix [0, 100]
			ratio2float(19.);					// attack [0, 19]
			log2float(20., 5.010635294);		// release[20, 3000]
			toggle2int("OFF");					// autorelease [OFF / ON]
			ratio2float(10.);					// sbc speed [0, 10]
			toggle2int("OFF");					// sbc on [OFF / ON]
			afine2float(-50., 100.);			// xover [-50, 50]
			toggle2int("12");					// xover slope [12 / 48]
			list2int(l_rcmb, ll_rcmb);			// ratio [1.1, 1.2, 1.3, 1.5, 1.7, 2, 2.5, 3, 3.5, 4, 5, 7, 10, LIM]
			afine2float(-40., 40.);				// threshold [-40, 0]
			afine2float(-10., 20.);				// gain [-10, 10]
			//
			afine2float(-10., 20.);				// band threshold [-10, 10]	|
			afine2float(-10., 20.);				// band gain [-10, 10]		} low
			toggle2int("0");					// band lock [0/1]			|
			afine2float(-10., 20.);				// band threshold [-10, 10]	}
			afine2float(-10., 20.);				// band gain [-10, 10]		} lomid
			toggle2int("0");					// band lock [0/1]			|
			afine2float(-10., 20.);				// band threshold [-10, 10]	|
			afine2float(-10., 20.);				// band gain [-10, 10]		} mid
			toggle2int("0");					// band lock [0/1]			|
			afine2float(-10., 20.);				// band threshold [-10, 10]	|
			afine2float(-10., 20.);				// band gain [-10, 10]		} himid
			toggle2int("0");					// band lock [0/1]			|
			afine2float(-10., 20.);				// band threshold [-10, 10]	|
			afine2float(-10., 20.);				// band gain [-10, 10]		} high
			toggle2int("0");					// band lock [0/1]			|
			//
			list2int(l_mcmb, ll_mcmb);			// meter mode [GR, SBC, PEAK]
		}
		// ignore other params
		break;
	case FAC:	// stereo fair compressor - 7 parameters i f f f f f f
	case FAC1M:	// M/S fair compressor - 2x7 parameters i f f f f f f
	case FAC2:	// dual fair compressor - 2x7 parameters i f f f f f f
		k = XFAC_FAC1M_FAC2(buf, k, ifx, FAC2, FAC1M, FAC);
		break;
	case LEC:  // stereo leisure compressor - 5 parameters i f f i f
	case LEC2: // dual leisure compressor - 2x5 parameters i f f i f
		k = XLEC_LEC2(buf, k, ifx, LEC2, LEC);
		break;
	case ULC:   // stereo ultimo compressor - 6 parameters i f f f f i
	case ULC2:  // dual ultimo compressor - 2x6 parameters i f f f f i
		k = XULC_ULC2(buf, k, ifx, ULC2, ULC);
		break;
	case ENH2: // dual enhancer - 2x9 parameters f f f f f f f f i
	case ENH:  // stereo enhancer - 9 parameters f f f f f f f f i
		k = XENH_ENH2(buf, k, ifx, ENH2, ENH);
		break;
	case EXC2:
	case EXC:	// stereo exciter - 7 parameters f f f f f f i
		k = XEXC_EXC2(buf, k, ifx, EXC2, EXC);
		break;
	case IMG:	// stereo imager - 7 parameters f f f f f f f
		k = XIMG(buf, k);
		break;
	case EDI:	// edison ex1 - 8 parameters i i i f f f f f
		k = XEDI(buf, k);
		break;
	case SON:	// sound maxer - 8 parameters i f f f i f f f
		k = XSON(buf, k);
		break;
	case AMP2:	// dual guitar amp - 2x9 parameters f f f f f f f f i
	case AMP:	// stereo guitar amp - 9 parameters f f f f f f f f i
		k = XAMP_AMP2(buf, k, ifx, AMP2, AMP);
		break;
	case DRV2:	//dual tube stage - 2x10 parameters f f f f f f f f f f
	case DRV:	//stereo tube stage - 10 parameters f f f f f f f f f f
		k = XDRV_DRV2(buf, k, ifx, DRV2, DRV);
		break;
	case PIT2:
	case PIT:	//stereo pitch - 6 parameters f f f f f f
		if (ifx == PIT){
			k = Xsprint(buf, k, 's', ",ffffff");
			j = 1;
		} else { //if (ifx == PIT2) {
			k = Xsprint(buf, k, 's', ",ffffffffffff");
			j = 2;
		}
		for (i = 0; i < j; i++) {
			afine2float(-12, 24.);				// semitone [-12, 12]
			afine2float(-50, 100.);				// cent [-50, 50]
			log2float(1., 6.214608098);			// delay [1, 500]
			log2float(10., 3.912023005);		// lo cut [10, 500]
			log2float(2000., 2.302585093);		// hi cut [2k, 20k]
			ratio2float(100.);					// mix [0, 100]
		}
		// ignore other params
		break;
	default:
		if (Xverbose) printf("--!!-- unknown FX / parameters for Effect: %d\n", ifx);
		break;
	}
	return (k);
}

