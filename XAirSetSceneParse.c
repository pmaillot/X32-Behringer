//
// XAirSetSceneParse.c
//
//  Created on: 6 jan. 2015 (SetSceneParse.c)
//      Author: Patrick-Gilles Maillot
//
//  Modified to Support XAir Series on: Jul 10, 2016
//      Author: Ken Mitchell

// Scene and Snippet files parser. Basically a very big switch statement checking the syntax of incoming data and
// based on parameters, sending data to XAir accordingly.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "XAirSetScene.h"

#ifdef __WIN32__
#include <windows.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

extern int fxparse1(char *buf, int k, int ifx);
extern void Xfdump(char *header, char *buf, int len, int debug);
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int XOff_On(char *buf, int k, char* Crit_0);
extern int Xp_percent(char *buf, int k);
extern int Xp_linf(char *buf, int k, float xmin, float lmaxmin, float xstep);
extern int Xp_logf(char *buf, int k, float xmin, float lmaxmin, int nsteps);
extern int Xp_int(char *buf, int k);
extern int Xp_str(char *buf, int k);
extern int Xp_bit(char *buf, int k);
extern int Xp_list(char *buf, int k, char **list, int list_max);
extern int Xp_fxlist(char *buf, int k, char **list, int list_max, int *p_ival);
extern int Xp_frequency(char *buf, int k, int nsteps);
extern int Xp_level(char *buf, int k, int nsteps);
extern void Xlogf(char *header, char *buf, int len);


extern int					Xdebug;
extern int					Xverbose;
extern int					Xdelay;
extern int					X32SHOW;

extern struct sockaddr		*Xip_pt;
extern int					Xip_len;	// length of addresses
extern int 					Xfd;		// our socket
extern int					fx[4]; 		// saves the FX current type for each of the fx slots
extern FILE					*Xin, *log_file;


#ifdef __WIN32__

#define MILLISLEEP(t)													\
	do {																\
		Sleep((t));														\
	} while (0);
#else

#define MILLISLEEP(t)													\
	do {																\
		usleep((t)*1000);												\
	} while (0);
#endif

#define SendDataToXAir do {														\
		if (X32SHOW) {if (Xverbose) Xlogf("->X", buf, k);}						\
		else {if (Xverbose) {Xfdump("->X", buf, k, Xdebug); fflush(stdout);}}	\
		if (sendto(Xfd, buf, k, 0, Xip_pt, Xip_len) < 0) {						\
			perror("coundn't send data");										\
			return (-1);														\
		}																		\
		if (Xdelay > 0) MILLISLEEP(Xdelay);										\
		if (Early_End()) return (0);											\
	} while (0);
//
//	Private functions
int Early_End() {
	int i;
//
	while ((i = fgetc(Xin)) == (int)' ');	// ignore/skip spaces
	ungetc(i, Xin);							// re-install just read char
	if ((i == (int)'/') || (i == (int)10)) return 1; // Early end is either a new line of a command
	return 0;
}
//
//
//
int SetSceneParse(char *l_read) {
int 				Xsc_index;
int					ch, rt, mx, k, bus;
char 				c1;
char				buf[512];	//512 is to take care of large number of
char				tmp[512];	//parameters i.e. 64 floats in FX (each is max 6 chars)

	for (Xsc_index = 0; Xsc_index < Xsc_max; Xsc_index++) {
		if (strcmp(l_read, Xsc[Xsc_index]) == 0) {
			// found
			if (X32SHOW) {
				if(Xverbose) fprintf (log_file, "scene data in: %s - found: %d %s\r\n", l_read, Xsc_index, Xsc[Xsc_index]);
			} else {
				if(Xverbose) printf ("scene data in: %s - found: %d %s\n", l_read, Xsc_index, Xsc[Xsc_index]);
			}

			switch (Xsc_index) {
			case config_chlink: // /config/chlink 1-2 ... 15-16
				for (ch = 1; ch < 16; ch+=2) {
					sprintf(tmp, "/config/chlink/%d-%d", ch, ch+1);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToXAir // send to XAir
				}
				break;
			case config_buslink: // /config/buslink 1-2 ... 15-16
				for (ch = 1; ch < 6; ch+=2) {
					sprintf(tmp, "/config/buslink/%d-%d", ch, ch+1);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToXAir // send to XAir
				}
				break;
			case config_linkcfg: // /config/linkcfg
				k = Xsprint(buf, 0, 's', "/config/linkcfg/preamp");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/linkcfg/eq");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/linkcfg/dyn");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/linkcfg/fdrmute");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case config_solo: // /config/solo
				k = Xsprint(buf, 0, 's', "/config/solo/level");
				k = Xp_level(buf, k, 161);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/solo/source");
				k = Xp_list(buf, k, Xsource, Xsource_max);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/solo/sourcetrim");
				// rev 1.09 spec says -18..+12db log(73)
				// but firmware 1.12 acts like -18..+18db lin(72)
				k = Xp_linf(buf, k, -18., 36., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/solo/chmode");
				k = XOff_On(buf, k, "PFL");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/solo/busmode");
				k = XOff_On(buf, k, "PFL");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/solo/dimatt");
				k = Xp_linf(buf, k, -40., 40., 1.);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/solo/dim");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/solo/mono");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/solo/mute");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/solo/dimplf");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case config_amixenable: // /config/amixenable
				k = Xsprint(buf, 0, 's', "/config/amixenable/X");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/amixenable/Y");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case config_amixlock: // /config/amixenable
				k = Xsprint(buf, 0, 's', "/config/amixlock/X");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/config/amixlock/Y");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case config_mute: // /config/mute 1-4
				for (ch = 1; ch < 5; ch++) {
					sprintf(tmp, "/config/mute/%d", ch);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToXAir // send to XAir
				}
				break;
			case ch_01_config:case ch_02_config:case ch_03_config:case ch_04_config: // /ch/01/config
			case ch_05_config:case ch_06_config:case ch_07_config:case ch_08_config:
			case ch_09_config:case ch_10_config:case ch_11_config:case ch_12_config:
			case ch_13_config:case ch_14_config:case ch_15_config:case ch_16_config:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/config/name", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/config/color", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/config/insrc", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinsrc, Xinsrc_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/config/rtnsrc", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xchrtnsrc, Xchrtnsrc_max);
				SendDataToXAir // send to XAir
				break;
			case ch_01_preamp:case ch_02_preamp:case ch_03_preamp:case ch_04_preamp: // /ch/01/preamp
			case ch_05_preamp:case ch_06_preamp:case ch_07_preamp:case ch_08_preamp:
			case ch_09_preamp:case ch_10_preamp:case ch_11_preamp:case ch_12_preamp:
			case ch_13_preamp:case ch_14_preamp:case ch_15_preamp:case ch_16_preamp:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/preamp/rtntrim", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -18., 36., 0.25);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/preamp/rtnsw", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/preamp/invert", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/preamp/hpon", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/preamp/hpf", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 20., 2.9957322735, 100); // log(400/20) = 2.9957322735
				SendDataToXAir // send to XAir
				break;
			case ch_01_gate:case ch_02_gate:case ch_03_gate:case ch_04_gate: // /ch/01/gate
			case ch_05_gate:case ch_06_gate:case ch_07_gate:case ch_08_gate:
			case ch_09_gate:case ch_10_gate:case ch_11_gate:case ch_12_gate:
			case ch_13_gate:case ch_14_gate:case ch_15_gate:case ch_16_gate:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/gate/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/gate/mode", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xgatemode, Xgatemode_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/gate/thr", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -80., 80., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/gate/range", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 3., 57., 1.);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/gate/attack", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/gate/hold", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log(2000/0.02) = 11.512925465
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/gate/release", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log (4000/5) = 6.684611728
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/gate/keysrc", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xkeysrc, Xkeysrc_max);
				SendDataToXAir // send to XAir
				break;
			case ch_01_gate_filter:case ch_02_gate_filter:case ch_03_gate_filter:case ch_04_gate_filter: // /ch/01/gate/filter
			case ch_05_gate_filter:case ch_06_gate_filter:case ch_07_gate_filter:case ch_08_gate_filter:
			case ch_09_gate_filter:case ch_10_gate_filter:case ch_11_gate_filter:case ch_12_gate_filter:
			case ch_13_gate_filter:case ch_14_gate_filter:case ch_15_gate_filter:case ch_16_gate_filter:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/gate/filter/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/gate/filter/type", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xfiltertype, Xfiltertype_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/gate/filter/f", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToXAir // send to XAir
				break;
			case ch_01_dyn:case ch_02_dyn:case ch_03_dyn:case ch_04_dyn: // /ch/01/dyn
			case ch_05_dyn:case ch_06_dyn:case ch_07_dyn:case ch_08_dyn:
			case ch_09_dyn:case ch_10_dyn:case ch_11_dyn:case ch_12_dyn:
			case ch_13_dyn:case ch_14_dyn:case ch_15_dyn:case ch_16_dyn:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/dyn/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/mode", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "COMP");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/det", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PEAK");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/env", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "LIN");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/thr", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -60., 60., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/ratio", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xdynratio, Xdynratio_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/knee", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 5., 1.);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/mgain", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 24., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/attack", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/hold", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log (2000/0.02) = 11.512925465
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/release", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log(4000/5) = 6.684611728
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/mix", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_percent(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/keysrc", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xkeysrc, Xkeysrc_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/auto", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case ch_01_dyn_filter:case ch_02_dyn_filter:case ch_03_dyn_filter:case ch_04_dyn_filter: // /ch/01/dyn/filter
			case ch_05_dyn_filter:case ch_06_dyn_filter:case ch_07_dyn_filter:case ch_08_dyn_filter:
			case ch_09_dyn_filter:case ch_10_dyn_filter:case ch_11_dyn_filter:case ch_12_dyn_filter:
			case ch_13_dyn_filter:case ch_14_dyn_filter:case ch_15_dyn_filter:case ch_16_dyn_filter:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/dyn/filter/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/filter/type", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xfiltertype, Xfiltertype_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/dyn/filter/f", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToXAir // send to XAir
				break;
			case ch_01_insert:case ch_02_insert:case ch_03_insert:case ch_04_insert: // /ch/01/insert
			case ch_05_insert:case ch_06_insert:case ch_07_insert:case ch_08_insert:
			case ch_09_insert:case ch_10_insert:case ch_11_insert:case ch_12_insert:
			case ch_13_insert:case ch_14_insert:case ch_15_insert:case ch_16_insert:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/insert/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/insert/fxslot", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xchfxslot, Xchfxslot_max);
				SendDataToXAir // send to XAir
				break;
			case ch_01_eq:case ch_02_eq:case ch_03_eq:case ch_04_eq: // /ch/01/eq
			case ch_05_eq:case ch_06_eq:case ch_07_eq:case ch_08_eq:
			case ch_09_eq:case ch_10_eq:case ch_11_eq:case ch_12_eq:
			case ch_13_eq:case ch_14_eq:case ch_15_eq:case ch_16_eq:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/eq/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case ch_01_eq_1:case ch_02_eq_1:case ch_03_eq_1:case ch_04_eq_1: // /ch/01/eq/1
			case ch_05_eq_1:case ch_06_eq_1:case ch_07_eq_1:case ch_08_eq_1:
			case ch_09_eq_1:case ch_10_eq_1:case ch_11_eq_1:case ch_12_eq_1:
			case ch_13_eq_1:case ch_14_eq_1:case ch_15_eq_1:case ch_16_eq_1:
			case ch_01_eq_2:case ch_02_eq_2:case ch_03_eq_2:case ch_04_eq_2: // /ch/01/eq/2
			case ch_05_eq_2:case ch_06_eq_2:case ch_07_eq_2:case ch_08_eq_2:
			case ch_09_eq_2:case ch_10_eq_2:case ch_11_eq_2:case ch_12_eq_2:
			case ch_13_eq_2:case ch_14_eq_2:case ch_15_eq_2:case ch_16_eq_2:
			case ch_01_eq_3:case ch_02_eq_3:case ch_03_eq_3:case ch_04_eq_3: // /ch/01/eq/3
			case ch_05_eq_3:case ch_06_eq_3:case ch_07_eq_3:case ch_08_eq_3:
			case ch_09_eq_3:case ch_10_eq_3:case ch_11_eq_3:case ch_12_eq_3:
			case ch_13_eq_3:case ch_14_eq_3:case ch_15_eq_3:case ch_16_eq_3:
			case ch_01_eq_4:case ch_02_eq_4:case ch_03_eq_4:case ch_04_eq_4: // /ch/01/eq/4
			case ch_05_eq_4:case ch_06_eq_4:case ch_07_eq_4:case ch_08_eq_4:
			case ch_09_eq_4:case ch_10_eq_4:case ch_11_eq_4:case ch_12_eq_4:
			case ch_13_eq_4:case ch_14_eq_4:case ch_15_eq_4:case ch_16_eq_4:
				sscanf(l_read + 4, "%d", &ch);
				sscanf(l_read + 10, "%c", &c1);
				sprintf(tmp, "/ch/%02d/eq/%c/type", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqtyp, Xeqtyp_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/eq/%c/f", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/eq/%c/g", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/eq/%c/q", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log(0.3/10) = -3.506557897
				SendDataToXAir // send to XAir
				break;
			case ch_01_mix:case ch_02_mix:case ch_03_mix:case ch_04_mix: // /ch/01/mix
			case ch_05_mix:case ch_06_mix:case ch_07_mix:case ch_08_mix:
			case ch_09_mix:case ch_10_mix:case ch_11_mix:case ch_12_mix:
			case ch_13_mix:case ch_14_mix:case ch_15_mix:case ch_16_mix:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/mix/lr", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/mix/pan", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToXAir // send to XAir
				break;
	// "pan" is only available on sends _01, _03, and _05
			case ch_01_mix_01:case ch_02_mix_01:case ch_03_mix_01:case ch_04_mix_01: // /ch/01/mix/01
			case ch_05_mix_01:case ch_06_mix_01:case ch_07_mix_01:case ch_08_mix_01:
			case ch_09_mix_01:case ch_10_mix_01:case ch_11_mix_01:case ch_12_mix_01:
			case ch_13_mix_01:case ch_14_mix_01:case ch_15_mix_01:case ch_16_mix_01:
			case ch_01_mix_03:case ch_02_mix_03:case ch_03_mix_03:case ch_04_mix_03: // /ch/01/mix/03
			case ch_05_mix_03:case ch_06_mix_03:case ch_07_mix_03:case ch_08_mix_03:
			case ch_09_mix_03:case ch_10_mix_03:case ch_11_mix_03:case ch_12_mix_03:
			case ch_13_mix_03:case ch_14_mix_03:case ch_15_mix_03:case ch_16_mix_03:
			case ch_01_mix_05:case ch_02_mix_05:case ch_03_mix_05:case ch_04_mix_05: // /ch/01/mix/05
			case ch_05_mix_05:case ch_06_mix_05:case ch_07_mix_05:case ch_08_mix_05:
			case ch_09_mix_05:case ch_10_mix_05:case ch_11_mix_05:case ch_12_mix_05:
			case ch_13_mix_05:case ch_14_mix_05:case ch_15_mix_05:case ch_16_mix_05:
				sscanf(l_read + 4, "%d", &ch);
				sscanf(l_read + 11, "%d", &mx);
				sprintf(tmp, "/ch/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/mix/%02d/grpon", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/mix/%02d/tap", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtap, Xmxtap_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/mix/%02d/pan", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToXAir // send to XAir
				break;
	// All others don't have "pan"
			case ch_01_mix_02:case ch_02_mix_02:case ch_03_mix_02:case ch_04_mix_02: // /ch/01/mix/02
			case ch_05_mix_02:case ch_06_mix_02:case ch_07_mix_02:case ch_08_mix_02:
			case ch_09_mix_02:case ch_10_mix_02:case ch_11_mix_02:case ch_12_mix_02:
			case ch_13_mix_02:case ch_14_mix_02:case ch_15_mix_02:case ch_16_mix_02:
			case ch_01_mix_04:case ch_02_mix_04:case ch_03_mix_04:case ch_04_mix_04: // /ch/01/mix/04
			case ch_05_mix_04:case ch_06_mix_04:case ch_07_mix_04:case ch_08_mix_04:
			case ch_09_mix_04:case ch_10_mix_04:case ch_11_mix_04:case ch_12_mix_04:
			case ch_13_mix_04:case ch_14_mix_04:case ch_15_mix_04:case ch_16_mix_04:
			case ch_01_mix_06:case ch_02_mix_06:case ch_03_mix_06:case ch_04_mix_06: // /ch/01/mix/06
			case ch_05_mix_06:case ch_06_mix_06:case ch_07_mix_06:case ch_08_mix_06:
			case ch_09_mix_06:case ch_10_mix_06:case ch_11_mix_06:case ch_12_mix_06:
			case ch_13_mix_06:case ch_14_mix_06:case ch_15_mix_06:case ch_16_mix_06:
			case ch_01_mix_07:case ch_02_mix_07:case ch_03_mix_07:case ch_04_mix_07: // /ch/01/mix/07
			case ch_05_mix_07:case ch_06_mix_07:case ch_07_mix_07:case ch_08_mix_07:
			case ch_09_mix_07:case ch_10_mix_07:case ch_11_mix_07:case ch_12_mix_07:
			case ch_13_mix_07:case ch_14_mix_07:case ch_15_mix_07:case ch_16_mix_07:
			case ch_01_mix_08:case ch_02_mix_08:case ch_03_mix_08:case ch_04_mix_08: // /ch/01/mix/08
			case ch_05_mix_08:case ch_06_mix_08:case ch_07_mix_08:case ch_08_mix_08:
			case ch_09_mix_08:case ch_10_mix_08:case ch_11_mix_08:case ch_12_mix_08:
			case ch_13_mix_08:case ch_14_mix_08:case ch_15_mix_08:case ch_16_mix_08:
			case ch_01_mix_09:case ch_02_mix_09:case ch_03_mix_09:case ch_04_mix_09: // /ch/01/mix/09
			case ch_05_mix_09:case ch_06_mix_09:case ch_07_mix_09:case ch_08_mix_09:
			case ch_09_mix_09:case ch_10_mix_09:case ch_11_mix_09:case ch_12_mix_09:
			case ch_13_mix_09:case ch_14_mix_09:case ch_15_mix_09:case ch_16_mix_09:
			case ch_01_mix_10:case ch_02_mix_10:case ch_03_mix_10:case ch_04_mix_10: // /ch/01/mix/10
			case ch_05_mix_10:case ch_06_mix_10:case ch_07_mix_10:case ch_08_mix_10:
			case ch_09_mix_10:case ch_10_mix_10:case ch_11_mix_10:case ch_12_mix_10:
			case ch_13_mix_10:case ch_14_mix_10:case ch_15_mix_10:case ch_16_mix_10:
				sscanf(l_read + 4, "%d", &ch);
				sscanf(l_read + 11, "%d", &mx);
				sprintf(tmp, "/ch/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/mix/%02d/grpon", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/mix/%02d/tap", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtap, Xmxtap_max);
				SendDataToXAir // send to XAir
				break;

			case ch_01_grp:case ch_02_grp:case ch_03_grp:case ch_04_grp: // /ch/01/grp
			case ch_05_grp:case ch_06_grp:case ch_07_grp:case ch_08_grp:
			case ch_09_grp:case ch_10_grp:case ch_11_grp:case ch_12_grp:
			case ch_13_grp:case ch_14_grp:case ch_15_grp:case ch_16_grp:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/grp/dca", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/grp/mute", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				break;
			case ch_01_automix:case ch_02_automix:case ch_03_automix:case ch_04_automix:
			case ch_05_automix:case ch_06_automix:case ch_07_automix:case ch_08_automix:
			case ch_09_automix:case ch_10_automix:case ch_11_automix:case ch_12_automix:
			case ch_13_automix:case ch_14_automix:case ch_15_automix:case ch_16_automix:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/automix/group", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xamgroup, Xamgroup_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/automix/weight", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -24., 36., 0.5);
				SendDataToXAir // send to XAir
				break;
			case rtn_aux_config:
				k = Xsprint(buf, 0, 's', "/rtn/aux/config/name");
				k = Xp_str(buf, k);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/rtn/aux/config/color");
				k = Xp_int(buf, k);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/rtn/aux/config/rtnsrc");
				k = Xp_list(buf, k, Xrtnrtnsrc, Xrtnrtnsrc_max);
				SendDataToXAir // send to XAir
				break;
			case rtn_aux_preamp:
				k = Xsprint(buf, 0, 's', "/rtn/aux/preamp/rtntrim");
				k = Xp_linf(buf, k, -18., 36., 0.25);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/rtn/aux/preamp/rtnsw");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case rtn_aux_eq:
				k = Xsprint(buf, 0, 's', "/rtn/aux/eq/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case rtn_aux_eq_1: case rtn_aux_eq_2: case rtn_aux_eq_3: case rtn_aux_eq_4:
				sscanf(l_read + 12, "%c", &c1);
				sprintf(tmp, "/rtn/aux/eq/%c/type", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqtyp, Xeqtyp_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/aux/eq/%c/f", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/aux/eq/%c/g", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/aux/eq/%c/q", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log(0.3/10) = -3.506557897
				SendDataToXAir // send to XAir
				break;
			case rtn_aux_mix:
				k = Xsprint(buf, 0, 's', "/rtn/aux/mix/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/rtn/aux/mix/fader");
				k = Xp_level(buf, k, 1023);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "rtn/aux/mix/lr");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "rtn/aux/mix/pan");
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToXAir // send to XAir
				break;
	// Only _01, _03, _05 have "pan"
			case rtn_aux_mix_01: case rtn_aux_mix_03: case rtn_aux_mix_05:
				sscanf(l_read + 14, "%d", &mx);
				sprintf(tmp, "/rtn/aux/mix/%02d/level", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/aux/mix/%02d/grpon", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/aux/mix/%02d/tap", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtap, Xmxtap_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/aux/mix/%02d/pan", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToXAir // send to XAir
				break;
// _02, _04, _06 and _07 - _10 don't have "pan"
			case rtn_aux_mix_02: case rtn_aux_mix_04: case rtn_aux_mix_06: case rtn_aux_mix_07:
			case rtn_aux_mix_08: case rtn_aux_mix_09: case rtn_aux_mix_10:
				sscanf(l_read + 14, "%d", &mx);
				sprintf(tmp, "/rtn/aux/mix/%02d/level", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/aux/mix/%02d/grpon", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/aux/mix/%02d/tap", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtap, Xmxtap_max);
				SendDataToXAir // send to XAir
				break;
			case rtn_aux_grp:
				k = Xsprint(buf, 0, 's', "/rtn/aux/grp/dca");
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/rtn/aux/grp/mute");
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				break;
			case rtn_1_config: case rtn_2_config: case rtn_3_config: case rtn_4_config:
				sscanf(l_read + 5, "%d", &rt);
				sprintf(tmp, "/rtn/%d/config/name", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/config/color", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/config/rtnsrc", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xrtnrtnsrc, Xrtnrtnsrc_max);
				SendDataToXAir // send to XAir
				break;
			case rtn_1_preamp: case rtn_2_preamp: case rtn_3_preamp: case rtn_4_preamp:
				sscanf(l_read + 5, "%d", &rt);
				sprintf(tmp, "/rtn/%d/preamp/rtntrim", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -18., 36., 0.25);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/preamp/rtnsw", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case rtn_1_eq: case rtn_2_eq: case rtn_3_eq: case rtn_4_eq:
				sscanf(l_read + 5, "%d", &rt);
				sprintf(tmp, "/rtn/%d/eq/on", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case rtn_1_eq_1: case rtn_2_eq_1: case rtn_3_eq_1: case rtn_4_eq_1:
			case rtn_1_eq_2: case rtn_2_eq_2: case rtn_3_eq_2: case rtn_4_eq_2:
			case rtn_1_eq_3: case rtn_2_eq_3: case rtn_3_eq_3: case rtn_4_eq_3:
			case rtn_1_eq_4: case rtn_2_eq_4: case rtn_3_eq_4: case rtn_4_eq_4:
				sscanf(l_read + 5, "%d", &rt);
				sscanf(l_read + 10, "%c", &c1);
				sprintf(tmp, "/rtn/%d/eq/%c/type", rt, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqtyp, Xeqtyp_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/eq/%c/f", rt, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/eq/%c/g", rt, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/eq/%c/q", rt, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log(0.3/10) = -3.506557897
				SendDataToXAir // send to XAir
				break;
			case rtn_1_mix: case rtn_2_mix: case rtn_3_mix: case rtn_4_mix:
				sscanf(l_read + 5, "%d", &rt);
				sprintf(tmp, "/rtn/%d/mix/on", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/mix/fader", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/mix/lr", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/mix/pan", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToXAir // send to XAir
				break;
	// "pan" is only available on sends _01, _03, and _05
			case rtn_1_mix_01: case rtn_2_mix_01: case rtn_3_mix_01: case rtn_4_mix_01:
			case rtn_1_mix_03: case rtn_2_mix_03: case rtn_3_mix_03: case rtn_4_mix_03:
			case rtn_1_mix_05: case rtn_2_mix_05: case rtn_3_mix_05: case rtn_4_mix_05:
				sscanf(l_read + 5, "%d", &rt);
				sscanf(l_read + 11, "%d", &mx);
				sprintf(tmp, "/rtn/%d/mix/%02d/level", rt, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/mix/%02d/grpon", rt, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/mix/%02d/tap", rt, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtap, Xmxtap_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/mix/%02d/pan", rt, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToXAir // send to XAir
				break;
	// All others don't have "pan"
			case rtn_1_mix_02: case rtn_2_mix_02: case rtn_3_mix_02: case rtn_4_mix_02:
			case rtn_1_mix_04: case rtn_2_mix_04: case rtn_3_mix_04: case rtn_4_mix_04:
			case rtn_1_mix_06: case rtn_2_mix_06: case rtn_3_mix_06: case rtn_4_mix_06:
			case rtn_1_mix_07: case rtn_2_mix_07: case rtn_3_mix_07: case rtn_4_mix_07:
			case rtn_1_mix_08: case rtn_2_mix_08: case rtn_3_mix_08: case rtn_4_mix_08:
			case rtn_1_mix_09: case rtn_2_mix_09: case rtn_3_mix_09: case rtn_4_mix_09:
			case rtn_1_mix_10: case rtn_2_mix_10: case rtn_3_mix_10: case rtn_4_mix_10:
				sscanf(l_read + 5, "%d", &rt);
				sscanf(l_read + 11, "%d", &mx);
				sprintf(tmp, "/rtn/%d/mix/%02d/level", rt, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/mix/%02d/grpon", rt, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/mix/%02d/tap", rt, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtap, Xmxtap_max);
				SendDataToXAir // send to XAir
				break;
			case rtn_1_grp: case rtn_2_grp: case rtn_3_grp: case rtn_4_grp:
				sscanf(l_read + 5, "%d", &rt);
				sprintf(tmp, "/rtn/%d/grp/dca", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/rtn/%d/grp/mute", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				break;
			case bus_1_config: case bus_2_config: case bus_3_config: case bus_4_config:
			case bus_5_config: case bus_6_config:
				sscanf(l_read + 5, "%d", &bus);
				sprintf(tmp, "/bus/%d/config/name", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/config/color", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToXAir // send to XAir
				break;
			case bus_1_dyn: case bus_2_dyn: case bus_3_dyn: case bus_4_dyn:
			case bus_5_dyn: case bus_6_dyn:
				sscanf(l_read + 5, "%d", &bus);
				sprintf(tmp, "/ch/%d/dyn/on", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/mode", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "COMP");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/det", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PEAK");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/env", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "LIN");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/thr", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -60., 60., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/ratio", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xdynratio, Xdynratio_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/knee", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 5., 1.);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/mgain", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 24., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/attack", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/hold", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log (2000/0.02) = 11.512925465
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/release", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log(4000/5) = 6.684611728
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/mix", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_percent(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/keysrc", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xkeysrc, Xkeysrc_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/dyn/auto", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case bus_1_dyn_filter: case bus_2_dyn_filter: case bus_3_dyn_filter: case bus_4_dyn_filter:
			case bus_5_dyn_filter: case bus_6_dyn_filter:
				sscanf(l_read + 5, "%d", &bus);
				sprintf(tmp, "/bus/%d/dyn/filter/on", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/dyn/filter/type", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xfiltertype, Xfiltertype_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/dyn/filter/f", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToXAir // send to XAir
				break;
			case bus_1_insert:case bus_2_insert:case bus_3_insert:case bus_4_insert:
			case bus_5_insert:case bus_6_insert:
				sscanf(l_read + 5, "%d", &bus);
				sprintf(tmp, "/bus/%d/insert/on", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/insert/fxslot", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xchfxslot, Xchfxslot_max);
				SendDataToXAir // send to XAir
				break;
			case bus_1_eq:case bus_2_eq:case bus_3_eq:case bus_4_eq:
			case bus_5_eq:case bus_6_eq:
				sscanf(l_read + 5, "%d", &bus);
				sprintf(tmp, "/bus/%02d/eq/on", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/eq/mode", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqmode, Xeqmode_max);
				SendDataToXAir // send to XAir
				break;
			case bus_1_eq_1:case bus_2_eq_1:case bus_3_eq_1:case bus_4_eq_1:
			case bus_5_eq_1:case bus_6_eq_1:
			case bus_1_eq_2:case bus_2_eq_2:case bus_3_eq_2:case bus_4_eq_2:
			case bus_5_eq_2:case bus_6_eq_2:
			case bus_1_eq_3:case bus_2_eq_3:case bus_3_eq_3:case bus_4_eq_3:
			case bus_5_eq_3:case bus_6_eq_3:
			case bus_1_eq_4:case bus_2_eq_4:case bus_3_eq_4:case bus_4_eq_4:
			case bus_5_eq_4:case bus_6_eq_4:
			case bus_1_eq_5:case bus_2_eq_5:case bus_3_eq_5:case bus_4_eq_5:
			case bus_5_eq_5:case bus_6_eq_5:
			case bus_1_eq_6:case bus_2_eq_6:case bus_3_eq_6:case bus_4_eq_6:
			case bus_5_eq_6:case bus_6_eq_6:
				sscanf(l_read + 5, "%d", &bus);
				sscanf(l_read + 10, "%c", &c1);
				sprintf(tmp, "/bus/%d/eq/%c/type", bus, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqtyp, Xeqtyp_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/eq/%c/f", bus, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/eq/%c/g", bus, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/eq/%c/q", bus, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log(0.3/10) = -3.506557897
				SendDataToXAir // send to XAir
				break;
//
// 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1k,
// 1k25, 1k6, 2k, 2k5, 3k15, 4k, 5k, 6k3, 8k, 10k, 12k5, 16k, 20k
			case bus_1_geq: case bus_2_geq: case bus_3_geq: case bus_4_geq:
			case bus_5_geq: case bus_6_geq:
				sscanf(l_read + 5, "%d", &bus);
				sprintf(tmp, "/bus/%d/geq/20", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/25", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/31.5", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/40", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/50", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/63", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/80", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/100", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/125", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/160", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/200", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/250", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/315", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/400", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/500", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/630", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/800", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/1k", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/1k25", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/1k6", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/2k", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/2k5", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/3k15", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/4k", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/5k", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/6k3", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/8k", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/10k", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/12k5", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/16k", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/geq/20k", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				break;
			case bus_1_mix:case bus_2_mix:case bus_3_mix:case bus_4_mix:
			case bus_5_mix:case bus_6_mix:
				sscanf(l_read + 5, "%d", &bus);
				sprintf(tmp, "/bus/%d/mix/on", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/mix/fader", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/mix/lr", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/bus/%d/mix/pan", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToXAir // send to XAir
				break;
			case bus_1_grp:case bus_2_grp:case bus_3_grp:case bus_4_grp:
			case bus_5_grp:case bus_6_grp:
				sscanf(l_read + 5, "%d", &bus);
				sprintf(tmp, "/ch/%d/grp/dca", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/grp/mute", bus);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				break;
			case fxsend_1_config: case fxsend_2_config: case fxsend_3_config: case fxsend_4_config:
				sscanf(l_read + 8, "%d", &ch);
				sprintf(tmp, "/fxsend/%d/config/name", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/fxsend/%d/config/color", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToXAir // send to XAir
				break;
			case fxsend_1_mix: case fxsend_2_mix: case fxsend_3_mix: case fxsend_4_mix:
				sscanf(l_read + 8, "%d", &ch);
				sprintf(tmp, "/ch/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToXAir // send to XAir
				break;
			case fxsend_1_grp: case fxsend_2_grp: case fxsend_3_grp: case fxsend_4_grp:
				sscanf(l_read + 8, "%d", &ch);
				sprintf(tmp, "/ch/%d/grp/dca", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/ch/%d/grp/mute", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToXAir // send to XAir
				break;
			case lr_config:
				k = Xsprint(buf, 0, 's', "/lr/config/name");
				k = Xp_str(buf, k);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/config/color");
				k = Xp_int(buf, k);
				SendDataToXAir // send to XAir
				break;
			case lr_dyn:
				k = Xsprint(buf, 0, 's', "/lr/dyn/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/mode");
				k = XOff_On(buf, k, "COMP");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/det");
				k = XOff_On(buf, k, "PEAK");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/env");
				k = XOff_On(buf, k, "LIN");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/thr");
				k = Xp_linf(buf, k, -60., 60., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/ratio");
				k = Xp_list(buf, k, Xdynratio, Xdynratio_max);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/knee");
				k = Xp_linf(buf, k, 0., 5., 1.);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/mgain");
				k = Xp_linf(buf, k, 0., 24., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/attack");
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/hold");
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log (2000/0.02) = 11.512925465
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/release");
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log(4000/5) = 6.684611728
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/mix");
				k = Xp_percent(buf, k);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/auto");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case lr_dyn_filter:
				k = Xsprint(buf, 0, 's', "/lr/dyn/filter/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/filter/type");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xfiltertype, Xfiltertype_max);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/dyn/filter/f");
				k = Xp_frequency(buf, k, 200);
				SendDataToXAir // send to XAir
				break;
			case lr_insert:
				k = Xsprint(buf, 0, 's', "/lr/insert/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/insert/fxslot");
				k = Xp_list(buf, k, Xlrfxslot, Xlrfxslot_max);
				SendDataToXAir // send to XAir
				break;
			case lr_eq:
				k = Xsprint(buf, 0, 's', "/lr/eq/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/eq/mode");
				k = Xp_list(buf, k, Xeqmode, Xeqmode_max);
				SendDataToXAir // send to XAir
				break;
			case lr_eq_1: case lr_eq_2: case lr_eq_3: case lr_eq_4:
			case lr_eq_5: case lr_eq_6:
				sscanf(l_read + 7, "%c", &c1);
				sprintf(tmp, "/lr/eq/%c/type", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqtyp, Xeqtyp_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/lr/eq/%c/f", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/lr/eq/%c/g", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/lr/eq/%c/q", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log(0.3/10) = -3.506557897
				SendDataToXAir // send to XAir
				break;
//
// 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1k,
// 1k25, 1k6, 2k, 2k5, 3k15, 4k, 5k, 6k3, 8k, 10k, 12k5, 16k, 20k
			case lr_geq:
				k = Xsprint(buf, 0, 's', "/lr/geq/20");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/25");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/31.5");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/40");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/50");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/63");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/80");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/100");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/125");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/160");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/200");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/250");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/315");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/400");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/500");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/630");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/800");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/1k");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/1k25");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/1k6");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/2k");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/2k5");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/3k15");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/4k");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/5k");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/6k3");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/8k");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/10k");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/12k5");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/16k");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/geq/20k");
				k = Xp_linf(buf, k, -15., 30., 0.5);
				SendDataToXAir // send to XAir
				break;
			case lr_mix:
				k = Xsprint(buf, 0, 's', "lr/mix/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/mix/fader");
				k = Xp_level(buf, k, 1023);
				SendDataToXAir // send to XAir
				k = Xsprint(buf, 0, 's', "/lr/mix/pan");
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToXAir // send to XAir
				break;
			case dca_1: case dca_2: case dca_3: case dca_4:
				sscanf(l_read + 5, "%c", &c1);
				sprintf(tmp, "/dca/%c/on", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				sprintf(tmp, "/dca/%c/fader", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToXAir // send to XAir
				break;
			case dca_1_config: case dca_2_config: case dca_3_config: case dca_4_config:
				sscanf(l_read + 5, "%c", &c1);
				sprintf(tmp, "/dca/%c/config/name", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/dca/%c/config/color", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToXAir // send to XAir
				break;
			case fx_1: case fx_2: case fx_3: case fx_4:
				sscanf(l_read + 4, "%c", &c1);
				sprintf(tmp, "/fx/%c/type", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_fxlist(buf, k, Xfxtyp4, Xfxtyp4_max, &(fx[(int)c1 - (int)'1']));
				SendDataToXAir // send to XAir
				sprintf(tmp, "/fx/%c/insert", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case fx_1_par: case fx_2_par: case fx_3_par: case fx_4_par:
				sscanf(l_read + 4, "%c", &c1);
				sprintf(tmp, "/fx/%c/par", c1);
				k = Xsprint(buf, 0, 's', tmp);
				//fx['c1'- 1] is the current FX type
				k = fxparse1(buf, k, fx[(int)c1 - (int)'1']);
				SendDataToXAir // send to XAir
				break;
			case routing_main_01: case routing_main_02: // 01: Main 02: Phones
				sscanf(l_read + 14, "%d", &rt);
				sprintf(tmp, "/routing/main/%02d/src", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmainsrc, Xmainsrc_max);
				SendDataToXAir // send to XAir
				break;
			case routing_aux_01: case routing_aux_02: case routing_aux_03: case routing_aux_04:
			case routing_aux_05: case routing_aux_06:
				sscanf(l_read + 13, "%d", &rt);
				sprintf(tmp, "/routing/aux/%02d/src", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xauxsrc, Xauxsrc_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/routing/aux/%02d/pos", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xsrcpos, Xsrcpos_max);
				SendDataToXAir // send to XAir
				break;
			case routing_p16_01: case routing_p16_02: case routing_p16_03: case routing_p16_04:
			case routing_p16_05: case routing_p16_06: case routing_p16_07: case routing_p16_08:
			case routing_p16_09: case routing_p16_10: case routing_p16_11: case routing_p16_12:
			case routing_p16_13: case routing_p16_14: case routing_p16_15: case routing_p16_16:
				sscanf(l_read + 13, "%d", &rt);
				sprintf(tmp, "/routing/p16/%02d/src", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xauxsrc, Xauxsrc_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/routing/p16/%02d/pos", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xsrcpos, Xsrcpos_max);
				SendDataToXAir // send to XAir
				break;
			case routing_usb_01: case routing_usb_02: case routing_usb_03: case routing_usb_04:
			case routing_usb_05: case routing_usb_06: case routing_usb_07: case routing_usb_08:
			case routing_usb_09: case routing_usb_10: case routing_usb_11: case routing_usb_12:
			case routing_usb_13: case routing_usb_14: case routing_usb_15: case routing_usb_16:
			case routing_usb_17: case routing_usb_18:
				sscanf(l_read + 13, "%d", &rt);
				sprintf(tmp, "/routing/usb/%02d/src", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xusbsrc, Xusbsrc_max);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/routing/usb/%02d/pos", rt);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xsrcpos, Xsrcpos_max);
				SendDataToXAir // send to XAir
				break;
		// Only _01 - _16 have phantom power
			case headamp_01: case headamp_02: case headamp_03: case headamp_04:
			case headamp_05: case headamp_06: case headamp_07: case headamp_08:
			case headamp_09: case headamp_10: case headamp_11: case headamp_12:
			case headamp_13: case headamp_14: case headamp_15: case headamp_16:
				sscanf(l_read + 9, "%2d", &ch);
				sprintf(tmp, "/headamp/%02d/gain", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -12., 72., 0.5);
				SendDataToXAir // send to XAir
				sprintf(tmp, "/headamp/%02d/phantom", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToXAir // send to XAir
				break;
			case headamp_17: case headamp_18: case headamp_19: case headamp_20:
			case headamp_21: case headamp_22: case headamp_23: case headamp_24:
				sscanf(l_read + 9, "%2d", &ch);
				sprintf(tmp, "/headamp/%02d/gain", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -12., 32., 0.5);
				SendDataToXAir // send to XAir
				break;
			default:
				break;
			} // all taken care of, Exit for loop!
			break;
		}
	}
	return(0);
}
