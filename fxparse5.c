//
// fxparse5.c
//
//  Created on: 25 oct. 2014
//      Author: Patrick-Gilles Maillot
//
//      This/These functions deal only with Effects, parsing scene data to float/int data sent to X32;
//      They are called in/from X32SetScene()
//
//
#include <stdio.h>
extern int Xverbose;
//
//FX types for effects slots 5-8
//
enum fxtyp5 {GEQ2 = 0, GEQ, TEQ2, TEQ, DES2, DES, P1A, P1A2,
	PQ5, PQ5S, WAVD, LIM, FAC, FAC1M, FAC2, LEC, LEC2, ULC,
	ULC2, ENH2, ENH, EXC2, EXC, IMG, EDI, SON, AMP2, AMP,
	DRV2, DRV, PHAS, FILT, PAN, SUB};


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
// fxparse5()
// Parse FX data for effects 5-8
// Compute and set respective data in buffer (will be sent by caller to X32)
//
//
//


int fxparse5(char *buf, int k, int ifx) {

//
// ifx is the FX index in the fx table for FX 5..8
	switch (ifx) {
	case GEQ2: // Dual Graphic EQ - 64 parameters f
	case TEQ2: // True Dual Graphic EQ - 64 parameters f
		k = XGEQ2_TEQ2(buf, k);
		break;
	case GEQ: // Stereo Graphic EQ - 32 parameters f
	case TEQ: // True Stereo Graphic EQ - 32 parameters f
		k = XGEQ_TEQ(buf, k);
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
		// ignore other params
		break;
	case SON:	// sound maxer - 8 parameters i f f f i f f f
		k = XSON(buf, k);
		break;
	case AMP2:	// dual guitar amp - 2x9 parameters f f f f f f f f i
	case AMP:	// stereo guitar amp - 9 parameters f f f f f f f f i
		k = XAMP_AMP2(buf, k, ifx, AMP2, AMP);
		// ignore other params
		break;
	case DRV2:	//dual tube stage - 2x10 parameters f f f f f f f f f f
	case DRV:	//stereo tube stage - 10 parameters f f f f f f f f f f
		k = XDRV_DRV2(buf, k, ifx, DRV2, DRV);
		break;
	case PHAS: //stereo phaser - 12 parameters f f f f f f f f f f f f
		k = XPHAS(buf, k);
		break;
	case FILT: // mood filter - 8 parameters f f f f i f i f f f f f i i
		k = XFILT(buf, k);
		break;
	case PAN: // tremolo/panner - 9 parameters f f f f f f f f f
		k = XPAN(buf, k);
		break;
	case SUB: // suboctaver - 10 parameters i i f f f i i f f f
		k = XSUB(buf, k);
		break;
	default:
		if (Xverbose) printf("--!!-- unknown FX / parameters for Effect: %d\n", ifx);
		break;
	}
	return (k);
}


