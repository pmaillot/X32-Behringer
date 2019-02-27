/*
 * X32CopyFX.c
 *
 *  Created on: Nov 6, 2016
 *      Author: Patrick-Gilles Maillot
 *
 *	X32CopyFX will copy one fx to another, or one side of a selected FX to its other side
 *	The utility is aimed at EQ (TEQ or GEQ), with a few options included
 *
 * log history:
 * 	ver 1.00: initial release
 * 	ver 1.10: added reset capability
 * 	ver 1.20: added reset From->To, with real default values respective of each effects
 * 	ver 1.30: users can overwrite standards default with their own default parameter sets (-f option)
 * 	ver 1.31: source code cosmetic changes, additional comments
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//
#include <winsock2.h>	// Windows functions for std GUI & sockets
#include <mmsystem.h>	// Multimedia functions (such as MIDI) for Windows
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//
#define MAXFXNUM				8				// X32 supports up to 8 effects
#define BSIZE					512				// UDP buffers max length values
#define BSIZEm1					BSIZE - 1		// max buffer fgets() length values
//
// Effect ref names
char* FXref[] = {
		"HALL",
		"AMBI",
		"RPLT",
		"ROOM",
		"CHAM",
		"PLAT",
		"VREV",
		"VRM",
		"GATE",
		"RVRS",
		"DLY",
		"3TAP",
		"4TAP",
		"CRS",
		"FLNG",
		"PHAS",
		"DIMC",
		"FILT",
		"ROTA",
		"PAN",
		"SUB",
		"D/RV",
		"CR/R",
		"FL/R",
		"D/CR",
		"D/FL",
		"MODD",
		"GEQ2",
		"GEQ",
		"TEQ2",
		"TEQ",
		"DES2",
		"DES",
		"P1A",
		"P1A2",
		"PQ5",
		"PQ5S",
		"WAVD",
		"LIM",
		"CMB",
		"CMB2",
		"FAC",
		"FAC1M",
		"FAC2",
		"LEC",
		"LEC2",
		"ULC",
		"ULC2",
		"ENH2",
		"ENH",
		"EXC2",
		"EXC",
		"IMG",
		"EDI",
		"SON",
		"AMP2",
		"AMP",
		"DRV2",
		"DRV",
		"PIT2",
		"PIT"
};
int   FXrefMAX = sizeof(FXref) / sizeof(char*);
//
// Effects defaults. The trailing zeroes are not necessary - No big deal but todo: remove them!
char* FXdef[] = {
		"20 1.57 60 5k74 25 0.0 83 7k2 0.95 25 50 30 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"4 0.84 60 5k06 30 0.0 71 7k9 20 50 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"10 1.70 27 4k47 100 0.0 89 5k0 0.81 18 26 50 90 80 34 -28 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"6 0.43 18 3k94 68 1.5 83 5k0 1.00 18 40 25 10 20 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"14 1.70 56 4k47 100 0.0 60 5k0 1.00 33 70 50 115 135 54 66 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"32 2.11 60 6k50 30 0.0 60 7k2 1.09 211.44 20 20 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"40 3.0 100 OFF FRONT 0.0 76 11k9 1.00 0.70 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"20 2.16 24 30 22 0.0 1.10 0.69 89 10k4 28 34 OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
//		"20 1.94 24 30 22 0.0 1.10 0.69 97 10k4 28 34 OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"8 312 6 22 60 0.0 65 5k0 -12.0 30 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"30 624 28 24 78 0.0 105 7k9 -12.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"100 223 X 1 1 13 10 20k0 97 30 30 20k0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
//		"100 223 ST 1 1 13 10 20k0 97 30 30 20k0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"200 100 0 30.0 10 20k0 4/3 50 -100 3/2 60 100 OFF OFF OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"200 100 30.0 10 20k0 5 4/3 50 1 50 3/2 50 OFF OFF OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.48 20 20 15.1 16.6 100 83 10k4 120 100 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.32 80 80 4.2 4.2 100 83 15k1 180 122 11k5 70 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"1.44 54 42 28 4 70 -25 0 0 21 52 209 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON ST OFF ON OFF ON OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.20 0 70 104.7 LP 100 TRI 90 64 12 143 60 4POL OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.66 5.01 38 0 -15 100 RUN SLOW 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"1.44 180 0 100 0 0 21 52 209 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON LO 100 30 0 ON LO 100 30 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"240 3/4 7k6 40.0 40 +0 24 1.43 52 5k6 89 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.47 20 15.1 180 100 +10 20 1.32 40 6k4 83 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.41 80 2.2 25 70 -15 14 0.97 24.0 7k2 113 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"201 1 9k7 30.0 40 +60 1.07 20 15.1 100 100 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"185 1 9k7 38.0 30 +50 0.56 80 4.2 180 80 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"300 1 30.0 97 9k5 20 1.08 SER CLUB 5.0 5k6 +0 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0",
		"0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0",
		"0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.0 0.0 0.0 0.0 FEM FEM 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.0 0.0 0.0 0.0 FEM ST 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON 0.0 0.0 20 0.0 0.6 0.0 12k 0.0 20k ON 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON 0.0 0.0 20 0.0 0.6 0.0 12k 0.0 20k ON ON 0.0 0.0 20 0.0 0.6 0.0 12k 0.0 20k ON 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON 0.0 200 0.0 1k5 0.0 4k 0.0 ON 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"OFF 0.0 200 0.0 200 0.0 1k5 0.0 OFF OFF 0.0 200 0.0 200 0.0 1k5 0.0 OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0 0 0.0 0 0 0.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.0 -0.5 0 3 0.05 662 ON OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON OFF 100 5 494 ON 5 ON 0 48 3 0.0 0.0 0.0 0.0 0 0.0 0.0 0 0.0 0.0 0 0.0 0.0 0 0.0 0.0 0 GR 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON OFF 100 5 494 ON 5 ON 0 48 3 0.0 0.0 0.0 0.0 0 0.0 0.0 0 0.0 0.0 0 0.0 0.0 0 0.0 0.0 0 GR ON OFF 100 5 494 ON 5 ON 0 48 3 0.0 0.0 0.0 0.0 0 0.0 0.0 0 0.0 0.0 0 0.0 0.0 0 0.0 0.0 0 GR 0 0 0 0 0 0",
		"ON -8.0 5.0 2 50 -8.5 55 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON -8.0 5.0 2 50 -8.5 55 -8.0 5.0 2 50 -8.5 55 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON -8.0 5.0 2 50 -8.5 55 ON -8.0 5.0 2 50 -8.5 55 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON 44 48 COMP 1.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON 44 48 COMP 1.0 ON 44 48 COMP 1.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON -24 -23 3.7 1.9 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON -24 -23 3.7 1.9 4 ON -24 -23 3.7 1.9 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.0 14 20 12 38 16 50 OFF 0.0 14 20 12 38 16 50 OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0.0 22 16 11 16 25 12 40 OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"6k91 0 0 0 0 30 OFF 6k91 0 0 0 0 30 OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"3k31 28 24 14 30 30 OFF 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0 0 0 0.0 261 1.96 0.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON ST ST 0 0 0 0 0.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"ON 0.6 0.8 0.0 ON 0.6 0.8 0.0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"5.0 5.0 5.0 5.0 5.0 5.0 5.0 5.0 ON 5.0 5.0 5.0 5.0 5.0 5.0 5.0 5.0 ON 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"5.0 5.0 5.0 5.0 5.0 5.0 5.0 5.0 ON 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"50 20 20 0.0 100 6k9 0.0 197 0.0 3k0 50 20 20 0.0 100 6k9 0.0 197 0.0 3k0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"25 20 28 -6.5 55 18k7 0.0 154 0.0 4k1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0 -10 5.0 100 -50 100 0 10 6.9 100 50 15k8 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		"0 0 5.0 52 15k8 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
};
//
int					keep_on = 1;				// 1 as long as program runs
int					Xconnected;					// whether X32 is connected or not
//
char 				r_buf[BSIZE], s_buf[BSIZE];	// X32 receive and send buffers
int 				r_len, s_len;				// associated data lengths
int 				p_status;					// "polling" status
//
WSADATA				wsa;						// Windows Sockets
int 				Xfd;						// X32 socket
struct sockaddr_in	Xip;						// IP structure
struct sockaddr		*Xip_addr = (struct sockaddr*) &Xip;
struct timeval		timeout;					// timeout for non blocking udp comm
fd_set 				ufds;						// file descriptor
int 				Xip_len = sizeof(Xip);		// length of addresses
char				xremote[12] = "/xremote";	// automatic trailing zeroes
char				Xip_str[32];				// X32 IP address as a string
time_t				before, now;				// timers for /xremote
char				str1[64];
//
int					FXnum;						// current selected FX number
char*				FXNames[MAXFXNUM] = {0, 0, 0, 0, 0, 0, 0, 0};	// array of pointers to FX name strings
time_t				FXcpy = 0;					// fx time stamp indicator
char*				FXcpOptions[] = {"Reset FROM:", "Reset FROM -> TO", "Copy FROM: A->B",
									 "Copy FROM: B->A", "Copy FROM -> TO"};
int					FXcpOptMAX = sizeof(FXcpOptions) / sizeof(char*);
int					FXcpmstr = 0;				// copy master flag
//
char*				*FXref_ram;	// array of FX names
char*				*FXdef_ram;	// array of FX parameter data strings
//
// Windows Declarations
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE		hInstance = 0;
HWND			hwndipaddr, hwndlistFXfrom, hwndlistFXto, hwndFXcpy, hwndconx, hwndcpmstr, hwndcpdir;
HFONT			hfont, htmp;
HDC				hdc;
PAINTSTRUCT		ps;
MSG				wMsg;
//
//
// Macros:
//
#define RPOLL													\
do {															\
	FD_ZERO (&ufds);											\
	FD_SET (Xfd, &ufds);										\
	p_status = select(Xfd+1, &ufds, NULL, NULL, &timeout);		\
} while (0);
//
#define RECV													\
do {															\
	r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);				\
} while (0);
//
#define SEND													\
do {															\
	if (sendto (Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {	\
	perror ("Error while sending data");						\
	exit (EXIT_FAILURE);										\
	}															\
} while(0);
//
#define SENDR													\
do {															\
	if (sendto (Xfd, r_buf, r_len, 0, Xip_addr, Xip_len) < 0) {	\
	perror ("Error while sending data");						\
	exit (EXIT_FAILURE);										\
	}															\
} while(0);
//
// External calls used
extern int  Xsprint(char *buffer, int index, char format, void *data);
extern int  Xfprint(char *buffer, int index, char *header, char format, void *data);
extern int  X32Connect(int Xconnected, char* Xip_str, int btime);
extern int  validateIP4Dotted(const char *s);
//
// Private functions
void XlistFX(int fill);
void XcopyFX();
void XReset(int j);
//
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdFile) {
//
	WNDCLASSW wc = {0};
	wc.lpszClassName = L"X32CopyFX";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"X32CopyFX",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 50, 50, 300, 230, 0, 0, hInstance, 0);
//
	Xconnected = 0;
// Main loop
	while (keep_on) {
		if (PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		}
		if (Xconnected) {
			// check if copy time stamp expired
			now = time(NULL);
			if (FXcpy) {
				if (now > FXcpy + 1) {
					SetWindowText(hwndFXcpy, "APPLY");
					FXcpy = 0;
				}
			}
		}
		Sleep(5); // avoid high CPU by waiting 5ms
	}
	return (int) wMsg.wParam;
}
//
//
// Windows Callbacks
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int i;
//
	switch (msg) {
	case WM_CREATE:
		//
		hwndipaddr = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 40, 120, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndconx = CreateWindow("button", "Connect", WS_VISIBLE | WS_CHILD,
				130, 40, 145, 20, hwnd, (HMENU )1, NULL, NULL);
		// Propose FX list as combo box. The list will be populated when connecting to X32
		// set height so there's enough room for 8 lines (170 ... > 8*20)
		hwndlistFXfrom = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				5, 80, 180, 170, hwnd, (HMENU)0, NULL, NULL);
		hwndlistFXto = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				5, 120, 180, 170, hwnd, (HMENU)0, NULL, NULL);
		hwndcpdir = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				5, 160, 150, 170, hwnd, (HMENU)0, NULL, NULL);
		hwndcpmstr = CreateWindow("BUTTON", "Copy Master",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
				170, 163, 120, 20, hwnd, (HMENU)2, NULL, NULL);
		hwndFXcpy = CreateWindow("button", "APPLY", WS_VISIBLE | WS_CHILD | BS_MULTILINE,
				195, 80, 80, 63, hwnd, (HMENU )3, NULL, NULL);
		break;
//
	case WM_PAINT:
		//
		hdc = BeginPaint(hwnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		// update user information
		SetWindowText(hwndipaddr, (LPSTR) Xip_str);
		//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 5, 0, str1, wsprintf(str1, "X32CopyFX - ©2016 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		// update diverse texts
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, FIXED_PITCH, NULL);//TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 235, 15, str1, wsprintf(str1, "v 1.31"));
		TextOut(hdc, 5, 25, str1, wsprintf(str1, "Set X32 IP below:"));
		TextOut(hdc, 5, 65, str1, wsprintf(str1, "Select FX FROM:"));
		TextOut(hdc, 5, 105, str1, wsprintf(str1, "Select FX TO:"));
		TextOut(hdc, 5, 145, str1, wsprintf(str1, "Action selection:"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		EndPaint(hwnd, &ps);
		break;
//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			switch (LOWORD(wParam)) {
			case 1: // connect X32
				GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32Connect(Xconnected, Xip_str, 5000);
					if (Xconnected)	{
						SetWindowText(hwndconx, "Connected");
						// Load dropdown item list
						XlistFX(1);
						sprintf(s_buf, "X32CopyFX: Please note the program does not\ncheck for the result of the copy, nor does it\ncheck if the operation is a valid one or not.\n\nX32CopyFX will just apply the selected operations\nto the up to 64 parameters of the selected effect(s).\n");
						MessageBox(NULL, s_buf, "Note", MB_OK);
					} else {
						SetWindowText(hwndconx, "Connect");
						// free allocated name spaces
						XlistFX(0);
					}
				} else {
					MessageBox(NULL, "Incorrect IP string form", NULL, MB_OK);
				}
				break;
			case 2:
				// Copy FX "from" to "to", according to options/settings
				FXcpmstr ^= 1;
				SendMessage(hwndcpmstr, BM_SETCHECK, FXcpmstr, 0);
				break;
			case 3:
				// Copy FX "from" to "to", according to options/settings
				XcopyFX();
				SetWindowText(hwndFXcpy, "DONE");
				FXcpy = time(NULL);
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		if (Xconnected) close(Xfd);
		Xconnected = 0;
		// Free allocated name spaces
		for (i = 0; i <MAXFXNUM; i++) {
			if (FXNames[i]) free(FXNames[i]);
			FXNames[i] = 0;
		}
		// Stop mainloop
		keep_on = 0;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
// XlistFX(fill):
// fill - 1: run through all 8 FX from X32, get their names and fill combo box list with
// 			 names in the form "n: fxname".
// 			 name fields are dynamically allocated.
// fill = 0: free name space memory if needed
//			 reset combo box names
void XlistFX(int fill) {
	int i, j;

	if (fill) {
		// there's something to display; fetch it from the console
		for (i = 0; i < MAXFXNUM; i++) {
			sprintf(r_buf, "fx/%d", i+1);
			s_len = Xfprint (s_buf, 0, "/node", 's', r_buf);
			SEND;
			RECV;
			FXNames[i] = 0;
			j = 0;
			while (j < r_len && r_buf[j] != ' ') j += 1;
			if ((FXNames[i] = malloc(sizeof(char) * r_len - j + 4)) != NULL) {
				FXNames[i][0] = i + 1 + (int)'0';
				FXNames[i][1] = ':';
				FXNames[i][2] = ' ';
				FXNames[i][3] = 0;
				strcat(FXNames[i], r_buf + j + 1);
				FXNames[i][strlen(FXNames[i]) - 1] = 0;
			}
			SendMessage(hwndlistFXfrom, CB_ADDSTRING, (WPARAM)0, (LPARAM)FXNames[i]);
			SendMessage(hwndlistFXto, CB_ADDSTRING, (WPARAM)0, (LPARAM)FXNames[i]);
		}
		// set list of possible operations
		for (i = 0; i < FXcpOptMAX; i++) {
			SendMessage(hwndcpdir, CB_ADDSTRING, (WPARAM)0, (LPARAM)FXcpOptions[i]);
		}
		// display initial item 0 in the selection field
		SendMessage(hwndlistFXfrom, CB_SETCURSEL, (WPARAM)FXnum, (LPARAM)0);
		SendMessage(hwndlistFXto, CB_SETCURSEL, (WPARAM)FXnum, (LPARAM)0);
		SendMessage(hwndcpdir, CB_SETCURSEL, (WPARAM)FXnum, (LPARAM)0);
	} else {
		// not connected - remove effects and operations
		for (i = 0; i <MAXFXNUM; i++) {
			if (FXNames[i]) free(FXNames[i]);
			FXNames[i] = 0;
			SendMessage(hwndlistFXfrom, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
			SendMessage(hwndlistFXto, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
		}
		for (i = 0; i < FXcpOptMAX; i++) {
			SendMessage(hwndcpdir, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
		}
	}
}
//
//
// XReset: reset the effect at index J [1-8] with its default values.
// values are retrieved from the default table in FXdef_ram
void XReset(int j) {
	int i;
//
	for (i = 0; i < FXrefMAX; i++ ) {
		// below, the "+ 3" is to point to the effect name, avoiding the "1: " before the name
		if (strcmp(FXref_ram[i], FXNames[j - 1] + 3) == 0) {
			// found: send default values
			memcpy(s_buf, "/\0\0\0,s\0\0fx/1/par \0", 17);
			strcpy(s_buf + 17, FXdef_ram[i]);			// s_buf almost ready
			s_len = strlen(FXdef_ram[i]) + 17;			// length
			s_buf[s_len++] = 0;							// 0 ending
			while(s_len & 3) s_buf[s_len++] = 0;	// set length to multiple of 4
			s_buf[11] = j + '0';					// set FX slot number [1..8]
			SEND
			RECV
			return;
		}
	}
	return;
}
//
// XcopyFX() Manage the actual copy of FX data, after getting source, destination
// and options
void XcopyFX() {
	int i;
	int FXfrom, FXto, FXdir, FXmstr;
	// get source FX number; X32 FX numbers start at 1
	FXfrom = 1 + SendMessage(hwndlistFXfrom, CB_GETCURSEL,(WPARAM) 0, (LPARAM) 0);
	// get destination FX number
	FXto = 1 + SendMessage(hwndlistFXto, CB_GETCURSEL,(WPARAM) 0, (LPARAM) 0);
	// get operation & Copy Master values
	FXdir = SendMessage(hwndcpdir, CB_GETCURSEL,(WPARAM) 0, (LPARAM) 0);
	FXmstr = SendMessage(hwndcpmstr, BM_GETCHECK,(WPARAM) 0, (LPARAM) 0);
	//
	switch (FXdir) {
	case 0: // Reset From:
		// search effect name within list of FXnames
		XReset(FXfrom);
		break;
	case 1: // Reset From: to To:
		for (i = FXfrom; i < FXto + 1; i++) XReset(i);
		break;
	case 2: // copy From: A->B
		for (i = 1; i < 32; i++) { // par source = [01..31]
			sprintf(r_buf, "/fx/%d/par/%02d", FXfrom, i);
			s_len = Xsprint (s_buf, 0, 's', r_buf);
			SEND
			RECV
			sprintf(r_buf+10, "%02d", i+32);
			SENDR			// SENDR sends to X32 using read buffer
		}
		if (FXmstr) {
			sprintf(r_buf, "/fx/%d/par/32", FXfrom);
			s_len = Xsprint (s_buf, 0, 's', r_buf);
			SEND
			RECV
			r_buf[10] = '6';
			r_buf[11] = '4';
			SENDR			// SENDR sends to X32 using read buffer
		}
		break;
	case 3: // copy From: B->A
		for (i = 33; i < 64; i++) { // par source = [33..63]
			sprintf(r_buf, "/fx/%d/par/%02d", FXfrom, i);
			s_len = Xsprint (s_buf, 0, 's', r_buf);
			SEND
			RECV
			sprintf(r_buf+10, "%02d", i-32);
			SENDR			// SENDR sends to X32 using read buffer
		}
		if (FXmstr) {
			sprintf(r_buf, "/fx/%d/par/64", FXfrom);
			s_len = Xsprint (s_buf, 0, 's', r_buf);
			SEND
			RECV
			r_buf[10] = '3';
			r_buf[11] = '2';
			SENDR			// SENDR sends to X32 using read buffer
		}
		break;
	case 4: // copy From to To
		for (i = 1; i < 32; i++) { // par = [01..31] and [33..63]
			sprintf(r_buf, "/fx/%d/par/%02d", FXfrom, i);
			s_len = Xsprint (s_buf, 0, 's', r_buf);
			SEND
			RECV
			r_buf[4] = (char)FXto + '0';
			SENDR;
			sprintf(r_buf, "/fx/%d/par/%02d", FXfrom, i + 32);
			s_len = Xsprint (s_buf, 0, 's', r_buf);
			SEND
			RECV
			r_buf[4] = (char)FXto + '0';
			SENDR;
		}
		if (FXmstr) {
			sprintf(r_buf, "/fx/%d/par/32", FXfrom);
			s_len = Xsprint (s_buf, 0, 's', r_buf);
			SEND
			RECV
			r_buf[4] = (char)FXto + '0';
			SENDR;
			sprintf(r_buf, "/fx/%d/par/64", FXfrom);
			s_len = Xsprint (s_buf, 0, 's', r_buf);
			SEND
			RECV
			r_buf[4] = (char)FXto + '0';
			SENDR;
		}
		break;
	default:
		break;
	}
}
//
//
// Main program
int main(int argc, char **argv) {
	HINSTANCE hPrevInstance = 0;
	PWSTR 	pCmdLine = 0;
	int		nCmdFile = 0;
	int		filein, i;
	FILE	*UserFXfile;
	char	input_ch;

	filein = 0;
	while ((input_ch = getopt(argc, argv, "f:h")) != (char)0xff) {
		switch (input_ch) {
		case 'f':
			filein = 1;
			sscanf(optarg, "%s", r_buf);
			break;
		case 'h':
		default:
			printf("Usage:\n");
			printf("./X32CopyFx [-f <file>]\n");
			printf("\tWhere if -f is set and <file> is provided, file is in the form of pair of lines:\n");
			printf("\t\tFX name\n");
			printf("\t\tparameter data string associated to FX name\n");
			sleep(30);
			return(0);
			break;
		}
	}
	// copy default FX tables parameter data
	FXref_ram = malloc(FXrefMAX * sizeof(char*));
	FXdef_ram = malloc(FXrefMAX * sizeof(char*));
	for (i = 0; i < FXrefMAX; i++) {
		FXref_ram[i] = FXref[i];	// copy pointers only
		FXdef_ram[i] = FXdef[i];	// copy pointers only
	}
	// if filein is set, open user description and update default table values
	// accordingly
	if (filein) {
		if ((UserFXfile = fopen(r_buf, "r")) != NULL) {
			// valid (?) file from the user - read and set buffers in place
			while (fgets(r_buf, BSIZEm1, UserFXfile) > 0) {
				// the first line of a pair is the FX name; search for it in our list
				for (i = 0; i < FXrefMAX; i++) {
					r_len = sizeof(FXref[i]);	// avoids comiler warning
					if (strncmp(FXref[i], r_buf, r_len) == 0) {
						// found
						// the next line of a pair is the FX default parameters data string
						if (fgets(r_buf, BSIZEm1, UserFXfile) > 0) {
							// free a potentially already replaced set
							if (FXdef_ram[i] != FXdef[i]) free(FXdef_ram[i]);
							// allocate a new buffer for user default parameters set
							FXdef_ram[i] = malloc(sizeof(r_buf));
							strcpy(FXdef_ram[i], r_buf);	// save user default parameters set
							break;
						}
					}
				}
			}
		}
	}
	// done
// hide command window
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
// start man window and loop
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
//
// free memory allocated to save user parameter default sets
	for (i = 0; i < FXrefMAX; i++) {
		if (FXdef_ram[i] != FXdef[i]) free(FXdef_ram[i]);
	}
	free(FXref_ram);
	free(FXdef_ram);
	return(0);
}
