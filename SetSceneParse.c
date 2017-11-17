//
// SetSceneParse.c
//
//  Created on: 6 janv. 2015
//      Author: Patrick-Gilles Maillot
//
// Scene and Snippet files parser. Basically a very big switch statement checking the syntax of incoming data and
// based on parameters, sending data to X32 accordingly.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "X32SetScene.h"

#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

extern int fxparse1(char *buf, int k, int ifx);
extern int fxparse5(char *buf, int k, int ifx);
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
extern int					X32VER;
extern int					X32SHOW;
extern int					X32PRESET;

extern struct sockaddr		*Xip_pt;
extern int					Xip_len;	// length of addresses
extern int 					Xfd;		// our socket
extern int					fx[8]; 		// saves the FX current type for each of the fx slots
extern FILE					*Xin, *log_file;


#ifdef __WIN32__

#define MILLISLEEP(t)													\
	do {																\
		Sleep(t);														\
	} while (0);
#else

#define MILLISLEEP(t)													\
	do {																\
		usleep(t*1000);													\
	} while (0);
#endif

#define SendDataToX32 do {														\
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
int					ch, mx, k;
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
			case config_chlink: // /config/chlink 1-2 ... 31-32
				for (ch = 1; ch < 32; ch+=2) {
					sprintf(tmp, "/config/chlink/%d-%d", ch, ch+1);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case config_auxlink: // /config/auxlink 1-2 ... 7-8
				for (ch = 1; ch < 8; ch+=2) {
					sprintf(tmp, "/config/auxlink/%d-%d", ch, ch+1);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case config_fxlink: // /config/fxlink 1-2 ... 7-8
				for (ch = 1; ch < 8; ch+=2) {
					sprintf(tmp, "/config/fxlink/%d-%d", ch, ch+1);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case config_buslink: // /config/buslink 1-2 ... 15-16
				for (ch = 1; ch < 16; ch+=2) {
					sprintf(tmp, "/config/buslink/%d-%d", ch, ch+1);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case config_mtxlink: // /config/mtxlink 1-2 ... 5-6
				for (ch = 1; ch < 6; ch+=2) {
					sprintf(tmp, "/config/mtxlink/%d-%d", ch, ch+1);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case config_mute: // /config/mute 1-6
				for (ch = 1; ch < 7; ch++) {
					sprintf(tmp, "/config/mute/%d", ch);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case config_linkcfg: // /config/linkcfg
				k = Xsprint(buf, 0, 's', "/config/linkcfg/hadly");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/linkcfg/eq");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/linkcfg/dyn");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/linkcfg/fdrmute");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case config_mono: // /config/mono
				k = Xsprint(buf, 0, 's', "/config/mono/mode");
				k = XOff_On(buf, k, "LR+M");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/mono/link");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case config_solo: // /config/solo
				k = Xsprint(buf, 0, 's', "/config/solo/level");
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/source");
				k = Xp_list(buf, k, Xsource, Xsource_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/sourcetrim");
				k = Xp_linf(buf, k, -18., 36., 0.5);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/chmode");
				k = XOff_On(buf, k, "PFL");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/Busmode");
				k = XOff_On(buf, k, "PFL");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/dcamode");
				k = XOff_On(buf, k, "PFL");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/exclusive");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/followsel");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/followsolo");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/dimatt");
				k = Xp_linf(buf, k, -40., 40., 1.);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/dim");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/mono");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/delay");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/delaytime");
				k = Xp_linf(buf, k, 0.3, 499.7, 0.1);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/masterctrl");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/mute");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/solo/dimplf");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case config_talk: // /config/talk
				k = Xsprint(buf, 0, 's', "/config/talk/enable");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/talk/source");
				k = XOff_On(buf, k, "INT");
				SendDataToX32 // send to X32
			break;
			case config_talk_A: // /config/talk/A
				k = Xsprint(buf, 0, 's', "/config/talk/A/level");
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/talk/A/dim");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/talk/A/latch");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/talk/A/destmap");
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				break;
			case config_talk_B: // /config/talk/B
				k = Xsprint(buf, 0, 's', "/config/talk/B/level");
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/talk/B/dim");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/talk/B/latch");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/talk/B/destmap");
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				break;
			case config_osc: // /config/osc
				k = Xsprint(buf, 0, 's', "/config/osc/level");
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/osc/f1");
				k = Xp_frequency(buf, k, 120);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/osc/f2");
				k = Xp_frequency(buf, k, 120);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/osc/fsel");
				k = XOff_On(buf, k, "F1");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/osc/type");
				k = Xp_list(buf, k, Xosctype, Xosctype_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/osc/dest");
				k = Xp_int(buf, k);
// not a list as expected
//						k = Xp_list(buf, k, Xoscdest, Xoscdest_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_routswitch: // /config/routing/routswitch
				k = Xsprint(buf, 0, 's', "config/routing/routswitch");
				k = Xp_list(buf, k, Xroutswitch, Xroutswitch_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_IN: // /config/routing/IN
				k = Xsprint(buf, 0, 's', "/config/routing/IN/1-8");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/routing/IN/9-16");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/routing/IN/17-24");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/routing/IN/25-32");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/routing/IN/AUX");
				k = Xp_list(buf, k, Xauxrtng, Xauxrtng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_IN1: // /config/routing/IN/1-8
				k = Xsprint(buf, 0, 's', "/config/routing/IN/1-8");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_IN9: // /config/routing/IN/9-16
				k = Xsprint(buf, 0, 's', "/config/routing/IN/9-16");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_IN17: // /config/routing/IN/17-24
				k = Xsprint(buf, 0, 's', "/config/routing/IN/17-24");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_IN25: // /config/routing/IN/25-32
				k = Xsprint(buf, 0, 's', "/config/routing/IN/25-32");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_INAUX: // /config/routing/IN/AUX
				k = Xsprint(buf, 0, 's', "/config/routing/IN/AUX");
				k = Xp_list(buf, k, Xauxrtng, Xauxrtng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_AES50A:case config_routing_AES50B: // /config/routing/AES50A|B
				sscanf(l_read +21, "%c", &c1);
				for (ch = 1; ch < 48; ch +=8) {
					sprintf(tmp, "/config/routing/AES50%c/%d-%d", c1, ch, ch+7);
					k = Xsprint(buf, 0, 's', tmp);
					k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
					SendDataToX32 // send to X32
				}
				break;
			case config_routing_AES50A1:case config_routing_AES50B1: // /config/routing/AES50A1|B1
				sscanf(l_read +21, "%c", &c1);
				ch = 1;
				sprintf(tmp, "/config/routing/AES50%c/%d-%d", c1, ch, ch+7);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_AES50A9:case config_routing_AES50B9: // /config/routing/AES50A9|B9
				sscanf(l_read +21, "%c", &c1);
				ch = 8;
				sprintf(tmp, "/config/routing/AES50%c/%d-%d", c1, ch, ch+7);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_AES50A17:case config_routing_AES50B17: // /config/routing/AES50A17|B17
				sscanf(l_read +21, "%c", &c1);
				ch = 17;
				sprintf(tmp, "/config/routing/AES50%c/%d-%d", c1, ch, ch+7);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_AES50A25:case config_routing_AES50B25: // /config/routing/AES50A25|B25
				sscanf(l_read +21, "%c", &c1);
				ch = 25;
				sprintf(tmp, "/config/routing/AES50%c/%d-%d", c1, ch, ch+7);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_AES50A33:case config_routing_AES50B33: // /config/routing/AES50A33|B33
				sscanf(l_read +21, "%c", &c1);
				ch = 33;
				sprintf(tmp, "/config/routing/AES50%c/%d-%d", c1, ch, ch+7);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_AES50A41:case config_routing_AES50B41: // /config/routing/AES50A41|B41
				sscanf(l_read +21, "%c", &c1);
				ch = 41;
				sprintf(tmp, "/config/routing/AES50%c/%d-%d", c1, ch, ch+7);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_CARD: // /config/routing/CARD
				for (ch = 1; ch < 32; ch +=8) {
					sprintf(tmp, "/config/routing/CARD/%d-%d", ch, ch+7);
					k = Xsprint(buf, 0, 's', tmp);
					k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
					SendDataToX32 // send to X32
				}
				break;
			case config_routing_CARD1: // /config/routing/CARD/1-8
				sprintf(tmp, "/config/routing/CARD/1-8");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_CARD9: // /config/routing/CARD/9-16
				sprintf(tmp, "/config/routing/CARD/9-16");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_CARD17: // /config/routing/CARD/17-24
				sprintf(tmp, "/config/routing/CARD/17-24");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_CARD25: // /config/routing/CARD/25-32
				sprintf(tmp, "/config/routing/CARD/25-32");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_OUT: // /config/routing/OUT
				sprintf(tmp, "/config/routing/OUT/1-4");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xo14rtng, Xo14rtng_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/config/routing/OUT/5-8");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xo58rtng, Xo58rtng_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/config/routing/OUT/9-12");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xo14rtng, Xo14rtng_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/config/routing/OUT/13-16");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xo58rtng, Xo58rtng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_OUT1: // /config/routing/OUT/1-4
				sprintf(tmp, "/config/routing/OUT/1-4");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xo14rtng, Xo14rtng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_OUT5: // /config/routing/OUT/5-8
				sprintf(tmp, "/config/routing/OUT/5-8");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xo58rtng, Xo58rtng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_OUT9: // /config/routing/OUT/9-12
				sprintf(tmp, "/config/routing/OUT/9-12");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xo14rtng, Xo14rtng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_OUT13: // /config/routing/OUT/13-16
				sprintf(tmp, "/config/routing/OUT/13-16");
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xo58rtng, Xo58rtng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_PLAY: // /config/routing/PLAY
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/1-8");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/9-16");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/17-24");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/25-32");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/AUX");
				k = Xp_list(buf, k, Xauxrtng, Xauxrtng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_PLAY1: // /config/routing/PLAY/1-8
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/1-8");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_PLAY9: // /config/routing/PLAY/9-16
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/9-16");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_PLAY17: // /config/routing/PLAY/17-24
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/17-24");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_PLAY25: // /config/routing/PLAY/25-32
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/25-32");
				k = Xp_list(buf, k, Xinaertng, Xinaertng_max);
				SendDataToX32 // send to X32
				break;
			case config_routing_PLAYAUX: // /config/routing/PLAY/AUX
				k = Xsprint(buf, 0, 's', "/config/routing/PLAY/AUX");
				k = Xp_list(buf, k, Xauxrtng, Xauxrtng_max);
				SendDataToX32 // send to X32
				break;
			case config_userctrl_A:case config_userctrl_B:case config_userctrl_C: // /config/userctrl
				sscanf(l_read +17, "%c", &c1);
				sprintf(tmp, "/config/userctrl/%c", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xcolor, Xcolor_max);
				SendDataToX32 // send to X32
				break;
			case config_userctrl_A_enc:case config_userctrl_B_enc:case config_userctrl_C_enc: // /config/userctrl/A|B|C/enc
				sscanf(l_read +17, "%c", &c1);
				for (ch = 1; ch < 5; ch++) {
					sprintf(tmp, "/config/userctrl/%c/enc/%d", c1, ch);
					k = Xsprint(buf, 0, 's', tmp);
					k = Xsprint(buf, k, 's', ",s");
					mx = fscanf(Xin, "%s", tmp);
					mx = 1; // search for end of sting (char '"')
					while (tmp[mx] != '"') mx++; // search for end of string (char '"')
					if (mx == 1) tmp[mx++] = '-';
					tmp[mx] = 0;	//mark end of string
					k = Xsprint(buf, k, 's', tmp + 1); //skip the first char!
					SendDataToX32 // send to X32
				}
				break;
			case config_userctrl_A_btn:case config_userctrl_B_btn:case config_userctrl_C_btn: // /config/userctrl/A|B|C/btn
				sscanf(l_read +17, "%c", &c1);
				for (ch = 5; ch < 13; ch++) {
					sprintf(tmp, "/config/userctrl/%c/btn/%d", c1, ch);
					k = Xsprint(buf, 0, 's', tmp);
					k = Xsprint(buf, k, 's', ",s");
					mx = fscanf(Xin, "%s", tmp);
					mx = 1; // search for end of sting (char '"')
					while (tmp[mx] != '"') mx++; // search for end of string (char '"')
					if (mx == 1) tmp[mx++] = '-';
					tmp[mx] = 0;	//mark end of string
					k = Xsprint(buf, k, 's', tmp+ 1); //skip the first char!
					SendDataToX32 // send to X32
				}
				break;
			case config_tape: // /config/tape
				k = Xsprint(buf, 0, 's', "/config/tape/gainL");
				k = Xp_linf(buf, k, -6., 30., 0.5);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/tape/gainR");
				k = Xp_linf(buf, k, -6., 30., 0.5);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/config/tape/autoplay");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case config_amixenable: // /config/amixenable
				k = Xsprint(buf, 0, 's', "/config/amixenable");
				k = XOff_On(buf, k, "OFF");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case ch_01_config:case ch_02_config:case ch_03_config:case ch_04_config: // /ch/01/config
			case ch_05_config:case ch_06_config:case ch_07_config:case ch_08_config:
			case ch_09_config:case ch_10_config:case ch_11_config:case ch_12_config:
			case ch_13_config:case ch_14_config:case ch_15_config:case ch_16_config:
			case ch_17_config:case ch_18_config:case ch_19_config:case ch_20_config:
			case ch_21_config:case ch_22_config:case ch_23_config:case ch_24_config:
			case ch_25_config:case ch_26_config:case ch_27_config:case ch_28_config:
			case ch_29_config:case ch_30_config:case ch_31_config:case ch_32_config:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/config/name", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/config/icon", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/config/color", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xcolor, Xcolor_max);
				SendDataToX32 // send to X32
				if (!X32PRESET) {
					sprintf(tmp, "/ch/%02d/config/source", ch);
					k = Xsprint(buf, 0, 's', tmp);
					k = Xp_int(buf, k);
					SendDataToX32 // send to X32
				}
				break;
			case ch_01_delay:case ch_02_delay:case ch_03_delay:case ch_04_delay: // /ch/01/delay
			case ch_05_delay:case ch_06_delay:case ch_07_delay:case ch_08_delay:
			case ch_09_delay:case ch_10_delay:case ch_11_delay:case ch_12_delay:
			case ch_13_delay:case ch_14_delay:case ch_15_delay:case ch_16_delay:
			case ch_17_delay:case ch_18_delay:case ch_19_delay:case ch_20_delay:
			case ch_21_delay:case ch_22_delay:case ch_23_delay:case ch_24_delay:
			case ch_25_delay:case ch_26_delay:case ch_27_delay:case ch_28_delay:
			case ch_29_delay:case ch_30_delay:case ch_31_delay:case ch_32_delay:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/delay/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/delay/time", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0.3, 499.7, 0.1);
				SendDataToX32 // send to X32
				break;
			case ch_01_preamp:case ch_02_preamp:case ch_03_preamp:case ch_04_preamp: // /ch/01/preamp
			case ch_05_preamp:case ch_06_preamp:case ch_07_preamp:case ch_08_preamp:
			case ch_09_preamp:case ch_10_preamp:case ch_11_preamp:case ch_12_preamp:
			case ch_13_preamp:case ch_14_preamp:case ch_15_preamp:case ch_16_preamp:
			case ch_17_preamp:case ch_18_preamp:case ch_19_preamp:case ch_20_preamp:
			case ch_21_preamp:case ch_22_preamp:case ch_23_preamp:case ch_24_preamp:
			case ch_25_preamp:case ch_26_preamp:case ch_27_preamp:case ch_28_preamp:
			case ch_29_preamp:case ch_30_preamp:case ch_31_preamp:case ch_32_preamp:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/preamp/trim", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -18., 36., 0.25);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/preamp/invert", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/preamp/hpon", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/preamp/hpslope", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/preamp/hpf", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 20., 2.9957322735, 100); // log(400/20) = 2.9957322735
				SendDataToX32 // send to X32
				break;
			case ch_01_gate:case ch_02_gate:case ch_03_gate:case ch_04_gate: // /ch/01/gate
			case ch_05_gate:case ch_06_gate:case ch_07_gate:case ch_08_gate:
			case ch_09_gate:case ch_10_gate:case ch_11_gate:case ch_12_gate:
			case ch_13_gate:case ch_14_gate:case ch_15_gate:case ch_16_gate:
			case ch_17_gate:case ch_18_gate:case ch_19_gate:case ch_20_gate:
			case ch_21_gate:case ch_22_gate:case ch_23_gate:case ch_24_gate:
			case ch_25_gate:case ch_26_gate:case ch_27_gate:case ch_28_gate:
			case ch_29_gate:case ch_30_gate:case ch_31_gate:case ch_32_gate:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/gate/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/gate/mode", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xgatemode, Xgatemode_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/gate/thr", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -80., 80., 0.5);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/gate/range", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 3., 57., 1.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/gate/attack", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/gate/hold", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log(2000/0.02) = 11.512925465
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/gate/release", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log (4000/5) = 6.684611728
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/gate/keysrc", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				break;
			case ch_01_gate_filter:case ch_02_gate_filter:case ch_03_gate_filter:case ch_04_gate_filter: // /ch/01/gate/filter
			case ch_05_gate_filter:case ch_06_gate_filter:case ch_07_gate_filter:case ch_08_gate_filter:
			case ch_09_gate_filter:case ch_10_gate_filter:case ch_11_gate_filter:case ch_12_gate_filter:
			case ch_13_gate_filter:case ch_14_gate_filter:case ch_15_gate_filter:case ch_16_gate_filter:
			case ch_17_gate_filter:case ch_18_gate_filter:case ch_19_gate_filter:case ch_20_gate_filter:
			case ch_21_gate_filter:case ch_22_gate_filter:case ch_23_gate_filter:case ch_24_gate_filter:
			case ch_25_gate_filter:case ch_26_gate_filter:case ch_27_gate_filter:case ch_28_gate_filter:
			case ch_29_gate_filter:case ch_30_gate_filter:case ch_31_gate_filter:case ch_32_gate_filter:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/gate/filter/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/gate/filter/type", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xgateftype, Xgateftype_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/gate/filter/f", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				break;
			case ch_01_dyn:case ch_02_dyn:case ch_03_dyn:case ch_04_dyn: // /ch/01/dyn
			case ch_05_dyn:case ch_06_dyn:case ch_07_dyn:case ch_08_dyn:
			case ch_09_dyn:case ch_10_dyn:case ch_11_dyn:case ch_12_dyn:
			case ch_13_dyn:case ch_14_dyn:case ch_15_dyn:case ch_16_dyn:
			case ch_17_dyn:case ch_18_dyn:case ch_19_dyn:case ch_20_dyn:
			case ch_21_dyn:case ch_22_dyn:case ch_23_dyn:case ch_24_dyn:
			case ch_25_dyn:case ch_26_dyn:case ch_27_dyn:case ch_28_dyn:
			case ch_29_dyn:case ch_30_dyn:case ch_31_dyn:case ch_32_dyn:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/dyn/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/mode", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "COMP");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/det", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PEAK");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/env", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "UN");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/thr", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -60., 60., 0.5);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/ratio", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xdynratio, Xdynratio_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/knee", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 5., 1.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/mgain", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 24., 0.5);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/attack", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/hold", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log (2000/0.02) = 11.512925465
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/release", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log(4000/5) = 6.684611728
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/keysrc", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/mix", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_percent(buf, k);
				SendDataToX32 // send to X32
				if (X32VER > 209) {	//starting with FW 2.10...
					sprintf(tmp, "/ch/%02d/dyn/auto", ch);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case ch_01_dyn_filter:case ch_02_dyn_filter:case ch_03_dyn_filter:case ch_04_dyn_filter: // /ch/01/dyn/filter
			case ch_05_dyn_filter:case ch_06_dyn_filter:case ch_07_dyn_filter:case ch_08_dyn_filter:
			case ch_09_dyn_filter:case ch_10_dyn_filter:case ch_11_dyn_filter:case ch_12_dyn_filter:
			case ch_13_dyn_filter:case ch_14_dyn_filter:case ch_15_dyn_filter:case ch_16_dyn_filter:
			case ch_17_dyn_filter:case ch_18_dyn_filter:case ch_19_dyn_filter:case ch_20_dyn_filter:
			case ch_21_dyn_filter:case ch_22_dyn_filter:case ch_23_dyn_filter:case ch_24_dyn_filter:
			case ch_25_dyn_filter:case ch_26_dyn_filter:case ch_27_dyn_filter:case ch_28_dyn_filter:
			case ch_29_dyn_filter:case ch_30_dyn_filter:case ch_31_dyn_filter:case ch_32_dyn_filter:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/dyn/filter/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/filter/type", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xgateftype, Xgateftype_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/dyn/filter/f", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				break;
			case ch_01_insert:case ch_02_insert:case ch_03_insert:case ch_04_insert: // /ch/01/insert
			case ch_05_insert:case ch_06_insert:case ch_07_insert:case ch_08_insert:
			case ch_09_insert:case ch_10_insert:case ch_11_insert:case ch_12_insert:
			case ch_13_insert:case ch_14_insert:case ch_15_insert:case ch_16_insert:
			case ch_17_insert:case ch_18_insert:case ch_19_insert:case ch_20_insert:
			case ch_21_insert:case ch_22_insert:case ch_23_insert:case ch_24_insert:
			case ch_25_insert:case ch_26_insert:case ch_27_insert:case ch_28_insert:
			case ch_29_insert:case ch_30_insert:case ch_31_insert:case ch_32_insert:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/insert/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/insert/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/insert/sel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinsel, Xinsel_max);
				SendDataToX32 // send to X32
				break;
			case ch_01_eq:case ch_02_eq:case ch_03_eq:case ch_04_eq: // /ch/01/eq
			case ch_05_eq:case ch_06_eq:case ch_07_eq:case ch_08_eq:
			case ch_09_eq:case ch_10_eq:case ch_11_eq:case ch_12_eq:
			case ch_13_eq:case ch_14_eq:case ch_15_eq:case ch_16_eq:
			case ch_17_eq:case ch_18_eq:case ch_19_eq:case ch_20_eq:
			case ch_21_eq:case ch_22_eq:case ch_23_eq:case ch_24_eq:
			case ch_25_eq:case ch_26_eq:case ch_27_eq:case ch_28_eq:
			case ch_29_eq:case ch_30_eq:case ch_31_eq:case ch_32_eq:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/eq/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case ch_01_eq_1:case ch_02_eq_1:case ch_03_eq_1:case ch_04_eq_1: // /ch/01/eq/1
			case ch_05_eq_1:case ch_06_eq_1:case ch_07_eq_1:case ch_08_eq_1:
			case ch_09_eq_1:case ch_10_eq_1:case ch_11_eq_1:case ch_12_eq_1:
			case ch_13_eq_1:case ch_14_eq_1:case ch_15_eq_1:case ch_16_eq_1:
			case ch_17_eq_1:case ch_18_eq_1:case ch_19_eq_1:case ch_20_eq_1:
			case ch_21_eq_1:case ch_22_eq_1:case ch_23_eq_1:case ch_24_eq_1:
			case ch_25_eq_1:case ch_26_eq_1:case ch_27_eq_1:case ch_28_eq_1:
			case ch_29_eq_1:case ch_30_eq_1:case ch_31_eq_1:case ch_32_eq_1:
			case ch_01_eq_2:case ch_02_eq_2:case ch_03_eq_2:case ch_04_eq_2: // /ch/01/eq/2
			case ch_05_eq_2:case ch_06_eq_2:case ch_07_eq_2:case ch_08_eq_2:
			case ch_09_eq_2:case ch_10_eq_2:case ch_11_eq_2:case ch_12_eq_2:
			case ch_13_eq_2:case ch_14_eq_2:case ch_15_eq_2:case ch_16_eq_2:
			case ch_17_eq_2:case ch_18_eq_2:case ch_19_eq_2:case ch_20_eq_2:
			case ch_21_eq_2:case ch_22_eq_2:case ch_23_eq_2:case ch_24_eq_2:
			case ch_25_eq_2:case ch_26_eq_2:case ch_27_eq_2:case ch_28_eq_2:
			case ch_29_eq_2:case ch_30_eq_2:case ch_31_eq_2:case ch_32_eq_2:
			case ch_01_eq_3:case ch_02_eq_3:case ch_03_eq_3:case ch_04_eq_3: // /ch/01/eq/3
			case ch_05_eq_3:case ch_06_eq_3:case ch_07_eq_3:case ch_08_eq_3:
			case ch_09_eq_3:case ch_10_eq_3:case ch_11_eq_3:case ch_12_eq_3:
			case ch_13_eq_3:case ch_14_eq_3:case ch_15_eq_3:case ch_16_eq_3:
			case ch_17_eq_3:case ch_18_eq_3:case ch_19_eq_3:case ch_20_eq_3:
			case ch_21_eq_3:case ch_22_eq_3:case ch_23_eq_3:case ch_24_eq_3:
			case ch_25_eq_3:case ch_26_eq_3:case ch_27_eq_3:case ch_28_eq_3:
			case ch_29_eq_3:case ch_30_eq_3:case ch_31_eq_3:case ch_32_eq_3:
			case ch_01_eq_4:case ch_02_eq_4:case ch_03_eq_4:case ch_04_eq_4: // /ch/01/eq/4
			case ch_05_eq_4:case ch_06_eq_4:case ch_07_eq_4:case ch_08_eq_4:
			case ch_09_eq_4:case ch_10_eq_4:case ch_11_eq_4:case ch_12_eq_4:
			case ch_13_eq_4:case ch_14_eq_4:case ch_15_eq_4:case ch_16_eq_4:
			case ch_17_eq_4:case ch_18_eq_4:case ch_19_eq_4:case ch_20_eq_4:
			case ch_21_eq_4:case ch_22_eq_4:case ch_23_eq_4:case ch_24_eq_4:
			case ch_25_eq_4:case ch_26_eq_4:case ch_27_eq_4:case ch_28_eq_4:
			case ch_29_eq_4:case ch_30_eq_4:case ch_31_eq_4:case ch_32_eq_4:
				sscanf(l_read + 4, "%d", &ch);
				sscanf(l_read + 10, "%c", &c1);
				sprintf(tmp, "/ch/%02d/eq/%c/type", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqtyp, Xeqtyp_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/eq/%c/f", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/eq/%c/g", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/eq/%c/q", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log(0.3/10) = -3.506557897
				SendDataToX32 // send to X32
				break;
			case ch_01_mix:case ch_02_mix:case ch_03_mix:case ch_04_mix: // /ch/01/mix
			case ch_05_mix:case ch_06_mix:case ch_07_mix:case ch_08_mix:
			case ch_09_mix:case ch_10_mix:case ch_11_mix:case ch_12_mix:
			case ch_13_mix:case ch_14_mix:case ch_15_mix:case ch_16_mix:
			case ch_17_mix:case ch_18_mix:case ch_19_mix:case ch_20_mix:
			case ch_21_mix:case ch_22_mix:case ch_23_mix:case ch_24_mix:
			case ch_25_mix:case ch_26_mix:case ch_27_mix:case ch_28_mix:
			case ch_29_mix:case ch_30_mix:case ch_31_mix:case ch_32_mix:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/mix/st", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/mix/pan", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/mix/mono", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/mix/mlevel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case ch_01_mix_fader:case ch_02_mix_fader:case ch_03_mix_fader:case ch_04_mix_fader: // /ch/01/mix
			case ch_05_mix_fader:case ch_06_mix_fader:case ch_07_mix_fader:case ch_08_mix_fader:
			case ch_09_mix_fader:case ch_10_mix_fader:case ch_11_mix_fader:case ch_12_mix_fader:
			case ch_13_mix_fader:case ch_14_mix_fader:case ch_15_mix_fader:case ch_16_mix_fader:
			case ch_17_mix_fader:case ch_18_mix_fader:case ch_19_mix_fader:case ch_20_mix_fader:
			case ch_21_mix_fader:case ch_22_mix_fader:case ch_23_mix_fader:case ch_24_mix_fader:
			case ch_25_mix_fader:case ch_26_mix_fader:case ch_27_mix_fader:case ch_28_mix_fader:
			case ch_29_mix_fader:case ch_30_mix_fader:case ch_31_mix_fader:case ch_32_mix_fader:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case ch_01_mix_pan:case ch_02_mix_pan:case ch_03_mix_pan:case ch_04_mix_pan: // /ch/01/mix
			case ch_05_mix_pan:case ch_06_mix_pan:case ch_07_mix_pan:case ch_08_mix_pan:
			case ch_09_mix_pan:case ch_10_mix_pan:case ch_11_mix_pan:case ch_12_mix_pan:
			case ch_13_mix_pan:case ch_14_mix_pan:case ch_15_mix_pan:case ch_16_mix_pan:
			case ch_17_mix_pan:case ch_18_mix_pan:case ch_19_mix_pan:case ch_20_mix_pan:
			case ch_21_mix_pan:case ch_22_mix_pan:case ch_23_mix_pan:case ch_24_mix_pan:
			case ch_25_mix_pan:case ch_26_mix_pan:case ch_27_mix_pan:case ch_28_mix_pan:
			case ch_29_mix_pan:case ch_30_mix_pan:case ch_31_mix_pan:case ch_32_mix_pan:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/mix/pan", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				break;
			case ch_01_mix_on:case ch_02_mix_on:case ch_03_mix_on:case ch_04_mix_on: // /ch/01/mix
			case ch_05_mix_on:case ch_06_mix_on:case ch_07_mix_on:case ch_08_mix_on:
			case ch_09_mix_on:case ch_10_mix_on:case ch_11_mix_on:case ch_12_mix_on:
			case ch_13_mix_on:case ch_14_mix_on:case ch_15_mix_on:case ch_16_mix_on:
			case ch_17_mix_on:case ch_18_mix_on:case ch_19_mix_on:case ch_20_mix_on:
			case ch_21_mix_on:case ch_22_mix_on:case ch_23_mix_on:case ch_24_mix_on:
			case ch_25_mix_on:case ch_26_mix_on:case ch_27_mix_on:case ch_28_mix_on:
			case ch_29_mix_on:case ch_30_mix_on:case ch_31_mix_on:case ch_32_mix_on:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case ch_01_mix_mono:case ch_02_mix_mono:case ch_03_mix_mono:case ch_04_mix_mono: // /ch/01/mix
			case ch_05_mix_mono:case ch_06_mix_mono:case ch_07_mix_mono:case ch_08_mix_mono:
			case ch_09_mix_mono:case ch_10_mix_mono:case ch_11_mix_mono:case ch_12_mix_mono:
			case ch_13_mix_mono:case ch_14_mix_mono:case ch_15_mix_mono:case ch_16_mix_mono:
			case ch_17_mix_mono:case ch_18_mix_mono:case ch_19_mix_mono:case ch_20_mix_mono:
			case ch_21_mix_mono:case ch_22_mix_mono:case ch_23_mix_mono:case ch_24_mix_mono:
			case ch_25_mix_mono:case ch_26_mix_mono:case ch_27_mix_mono:case ch_28_mix_mono:
			case ch_29_mix_mono:case ch_30_mix_mono:case ch_31_mix_mono:case ch_32_mix_mono:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/mix/mono", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case ch_01_mix_mlevel:case ch_02_mix_mlevel:case ch_03_mix_mlevel:case ch_04_mix_mlevel: // /ch/01/mix
			case ch_05_mix_mlevel:case ch_06_mix_mlevel:case ch_07_mix_mlevel:case ch_08_mix_mlevel:
			case ch_09_mix_mlevel:case ch_10_mix_mlevel:case ch_11_mix_mlevel:case ch_12_mix_mlevel:
			case ch_13_mix_mlevel:case ch_14_mix_mlevel:case ch_15_mix_mlevel:case ch_16_mix_mlevel:
			case ch_17_mix_mlevel:case ch_18_mix_mlevel:case ch_19_mix_mlevel:case ch_20_mix_mlevel:
			case ch_21_mix_mlevel:case ch_22_mix_mlevel:case ch_23_mix_mlevel:case ch_24_mix_mlevel:
			case ch_25_mix_mlevel:case ch_26_mix_mlevel:case ch_27_mix_mlevel:case ch_28_mix_mlevel:
			case ch_29_mix_mlevel:case ch_30_mix_mlevel:case ch_31_mix_mlevel:case ch_32_mix_mlevel:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/mix/mlevel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case ch_01_mix_01:case ch_02_mix_01:case ch_03_mix_01:case ch_04_mix_01: // /ch/01/mix/01
			case ch_05_mix_01:case ch_06_mix_01:case ch_07_mix_01:case ch_08_mix_01:
			case ch_09_mix_01:case ch_10_mix_01:case ch_11_mix_01:case ch_12_mix_01:
			case ch_13_mix_01:case ch_14_mix_01:case ch_15_mix_01:case ch_16_mix_01:
			case ch_17_mix_01:case ch_18_mix_01:case ch_19_mix_01:case ch_20_mix_01:
			case ch_21_mix_01:case ch_22_mix_01:case ch_23_mix_01:case ch_24_mix_01:
			case ch_25_mix_01:case ch_26_mix_01:case ch_27_mix_01:case ch_28_mix_01:
			case ch_29_mix_01:case ch_30_mix_01:case ch_31_mix_01:case ch_32_mix_01:
			case ch_01_mix_03:case ch_02_mix_03:case ch_03_mix_03:case ch_04_mix_03: // /ch/01/mix/03
			case ch_05_mix_03:case ch_06_mix_03:case ch_07_mix_03:case ch_08_mix_03:
			case ch_09_mix_03:case ch_10_mix_03:case ch_11_mix_03:case ch_12_mix_03:
			case ch_13_mix_03:case ch_14_mix_03:case ch_15_mix_03:case ch_16_mix_03:
			case ch_17_mix_03:case ch_18_mix_03:case ch_19_mix_03:case ch_20_mix_03:
			case ch_21_mix_03:case ch_22_mix_03:case ch_23_mix_03:case ch_24_mix_03:
			case ch_25_mix_03:case ch_26_mix_03:case ch_27_mix_03:case ch_28_mix_03:
			case ch_29_mix_03:case ch_30_mix_03:case ch_31_mix_03:case ch_32_mix_03:
			case ch_01_mix_05:case ch_02_mix_05:case ch_03_mix_05:case ch_04_mix_05: // /ch/01/mix/05
			case ch_05_mix_05:case ch_06_mix_05:case ch_07_mix_05:case ch_08_mix_05:
			case ch_09_mix_05:case ch_10_mix_05:case ch_11_mix_05:case ch_12_mix_05:
			case ch_13_mix_05:case ch_14_mix_05:case ch_15_mix_05:case ch_16_mix_05:
			case ch_17_mix_05:case ch_18_mix_05:case ch_19_mix_05:case ch_20_mix_05:
			case ch_21_mix_05:case ch_22_mix_05:case ch_23_mix_05:case ch_24_mix_05:
			case ch_25_mix_05:case ch_26_mix_05:case ch_27_mix_05:case ch_28_mix_05:
			case ch_29_mix_05:case ch_30_mix_05:case ch_31_mix_05:case ch_32_mix_05:
			case ch_01_mix_07:case ch_02_mix_07:case ch_03_mix_07:case ch_04_mix_07: // /ch/01/mix/07
			case ch_05_mix_07:case ch_06_mix_07:case ch_07_mix_07:case ch_08_mix_07:
			case ch_09_mix_07:case ch_10_mix_07:case ch_11_mix_07:case ch_12_mix_07:
			case ch_13_mix_07:case ch_14_mix_07:case ch_15_mix_07:case ch_16_mix_07:
			case ch_17_mix_07:case ch_18_mix_07:case ch_19_mix_07:case ch_20_mix_07:
			case ch_21_mix_07:case ch_22_mix_07:case ch_23_mix_07:case ch_24_mix_07:
			case ch_25_mix_07:case ch_26_mix_07:case ch_27_mix_07:case ch_28_mix_07:
			case ch_29_mix_07:case ch_30_mix_07:case ch_31_mix_07:case ch_32_mix_07:
			case ch_01_mix_09:case ch_02_mix_09:case ch_03_mix_09:case ch_04_mix_09: // /ch/01/mix/09
			case ch_05_mix_09:case ch_06_mix_09:case ch_07_mix_09:case ch_08_mix_09:
			case ch_09_mix_09:case ch_10_mix_09:case ch_11_mix_09:case ch_12_mix_09:
			case ch_13_mix_09:case ch_14_mix_09:case ch_15_mix_09:case ch_16_mix_09:
			case ch_17_mix_09:case ch_18_mix_09:case ch_19_mix_09:case ch_20_mix_09:
			case ch_21_mix_09:case ch_22_mix_09:case ch_23_mix_09:case ch_24_mix_09:
			case ch_25_mix_09:case ch_26_mix_09:case ch_27_mix_09:case ch_28_mix_09:
			case ch_29_mix_09:case ch_30_mix_09:case ch_31_mix_09:case ch_32_mix_09:
			case ch_01_mix_11:case ch_02_mix_11:case ch_03_mix_11:case ch_04_mix_11: // /ch/01/mix/11
			case ch_05_mix_11:case ch_06_mix_11:case ch_07_mix_11:case ch_08_mix_11:
			case ch_09_mix_11:case ch_10_mix_11:case ch_11_mix_11:case ch_12_mix_11:
			case ch_13_mix_11:case ch_14_mix_11:case ch_15_mix_11:case ch_16_mix_11:
			case ch_17_mix_11:case ch_18_mix_11:case ch_19_mix_11:case ch_20_mix_11:
			case ch_21_mix_11:case ch_22_mix_11:case ch_23_mix_11:case ch_24_mix_11:
			case ch_25_mix_11:case ch_26_mix_11:case ch_27_mix_11:case ch_28_mix_11:
			case ch_29_mix_11:case ch_30_mix_11:case ch_31_mix_11:case ch_32_mix_11:
			case ch_01_mix_13:case ch_02_mix_13:case ch_03_mix_13:case ch_04_mix_13: // /ch/01/mix/13
			case ch_05_mix_13:case ch_06_mix_13:case ch_07_mix_13:case ch_08_mix_13:
			case ch_09_mix_13:case ch_10_mix_13:case ch_11_mix_13:case ch_12_mix_13:
			case ch_13_mix_13:case ch_14_mix_13:case ch_15_mix_13:case ch_16_mix_13:
			case ch_17_mix_13:case ch_18_mix_13:case ch_19_mix_13:case ch_20_mix_13:
			case ch_21_mix_13:case ch_22_mix_13:case ch_23_mix_13:case ch_24_mix_13:
			case ch_25_mix_13:case ch_26_mix_13:case ch_27_mix_13:case ch_28_mix_13:
			case ch_29_mix_13:case ch_30_mix_13:case ch_31_mix_13:case ch_32_mix_13:
			case ch_01_mix_15:case ch_02_mix_15:case ch_03_mix_15:case ch_04_mix_15: // /ch/01/mix/15
			case ch_05_mix_15:case ch_06_mix_15:case ch_07_mix_15:case ch_08_mix_15:
			case ch_09_mix_15:case ch_10_mix_15:case ch_11_mix_15:case ch_12_mix_15:
			case ch_13_mix_15:case ch_14_mix_15:case ch_15_mix_15:case ch_16_mix_15:
			case ch_17_mix_15:case ch_18_mix_15:case ch_19_mix_15:case ch_20_mix_15:
			case ch_21_mix_15:case ch_22_mix_15:case ch_23_mix_15:case ch_24_mix_15:
			case ch_25_mix_15:case ch_26_mix_15:case ch_27_mix_15:case ch_28_mix_15:
			case ch_29_mix_15:case ch_30_mix_15:case ch_31_mix_15:case ch_32_mix_15:
				sscanf(l_read + 4, "%d", &ch);
				sscanf(l_read + 11, "%d", &mx);
				sprintf(tmp, "/ch/%02d/mix/%02d/on", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/mix/%02d/pan", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/mix/%02d/type", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtyp, Xmxtyp_max);
				SendDataToX32 // send to X32
				break;
			case ch_01_mix_02:case ch_02_mix_02:case ch_03_mix_02:case ch_04_mix_02: // /ch/01/mix/02
			case ch_05_mix_02:case ch_06_mix_02:case ch_07_mix_02:case ch_08_mix_02:
			case ch_09_mix_02:case ch_10_mix_02:case ch_11_mix_02:case ch_12_mix_02:
			case ch_13_mix_02:case ch_14_mix_02:case ch_15_mix_02:case ch_16_mix_02:
			case ch_17_mix_02:case ch_18_mix_02:case ch_19_mix_02:case ch_20_mix_02:
			case ch_21_mix_02:case ch_22_mix_02:case ch_23_mix_02:case ch_24_mix_02:
			case ch_25_mix_02:case ch_26_mix_02:case ch_27_mix_02:case ch_28_mix_02:
			case ch_29_mix_02:case ch_30_mix_02:case ch_31_mix_02:case ch_32_mix_02:
			case ch_01_mix_04:case ch_02_mix_04:case ch_03_mix_04:case ch_04_mix_04: // /ch/01/mix/04
			case ch_05_mix_04:case ch_06_mix_04:case ch_07_mix_04:case ch_08_mix_04:
			case ch_09_mix_04:case ch_10_mix_04:case ch_11_mix_04:case ch_12_mix_04:
			case ch_13_mix_04:case ch_14_mix_04:case ch_15_mix_04:case ch_16_mix_04:
			case ch_17_mix_04:case ch_18_mix_04:case ch_19_mix_04:case ch_20_mix_04:
			case ch_21_mix_04:case ch_22_mix_04:case ch_23_mix_04:case ch_24_mix_04:
			case ch_25_mix_04:case ch_26_mix_04:case ch_27_mix_04:case ch_28_mix_04:
			case ch_29_mix_04:case ch_30_mix_04:case ch_31_mix_04:case ch_32_mix_04:
			case ch_01_mix_06:case ch_02_mix_06:case ch_03_mix_06:case ch_04_mix_06: // /ch/01/mix/06
			case ch_05_mix_06:case ch_06_mix_06:case ch_07_mix_06:case ch_08_mix_06:
			case ch_09_mix_06:case ch_10_mix_06:case ch_11_mix_06:case ch_12_mix_06:
			case ch_13_mix_06:case ch_14_mix_06:case ch_15_mix_06:case ch_16_mix_06:
			case ch_17_mix_06:case ch_18_mix_06:case ch_19_mix_06:case ch_20_mix_06:
			case ch_21_mix_06:case ch_22_mix_06:case ch_23_mix_06:case ch_24_mix_06:
			case ch_25_mix_06:case ch_26_mix_06:case ch_27_mix_06:case ch_28_mix_06:
			case ch_29_mix_06:case ch_30_mix_06:case ch_31_mix_06:case ch_32_mix_06:
			case ch_01_mix_08:case ch_02_mix_08:case ch_03_mix_08:case ch_04_mix_08: // /ch/01/mix/08
			case ch_05_mix_08:case ch_06_mix_08:case ch_07_mix_08:case ch_08_mix_08:
			case ch_09_mix_08:case ch_10_mix_08:case ch_11_mix_08:case ch_12_mix_08:
			case ch_13_mix_08:case ch_14_mix_08:case ch_15_mix_08:case ch_16_mix_08:
			case ch_17_mix_08:case ch_18_mix_08:case ch_19_mix_08:case ch_20_mix_08:
			case ch_21_mix_08:case ch_22_mix_08:case ch_23_mix_08:case ch_24_mix_08:
			case ch_25_mix_08:case ch_26_mix_08:case ch_27_mix_08:case ch_28_mix_08:
			case ch_29_mix_08:case ch_30_mix_08:case ch_31_mix_08:case ch_32_mix_08:
			case ch_01_mix_10:case ch_02_mix_10:case ch_03_mix_10:case ch_04_mix_10: // /ch/01/mix/10
			case ch_05_mix_10:case ch_06_mix_10:case ch_07_mix_10:case ch_08_mix_10:
			case ch_09_mix_10:case ch_10_mix_10:case ch_11_mix_10:case ch_12_mix_10:
			case ch_13_mix_10:case ch_14_mix_10:case ch_15_mix_10:case ch_16_mix_10:
			case ch_17_mix_10:case ch_18_mix_10:case ch_19_mix_10:case ch_20_mix_10:
			case ch_21_mix_10:case ch_22_mix_10:case ch_23_mix_10:case ch_24_mix_10:
			case ch_25_mix_10:case ch_26_mix_10:case ch_27_mix_10:case ch_28_mix_10:
			case ch_29_mix_10:case ch_30_mix_10:case ch_31_mix_10:case ch_32_mix_10:
			case ch_01_mix_12:case ch_02_mix_12:case ch_03_mix_12:case ch_04_mix_12: // /ch/01/mix/12
			case ch_05_mix_12:case ch_06_mix_12:case ch_07_mix_12:case ch_08_mix_12:
			case ch_09_mix_12:case ch_10_mix_12:case ch_11_mix_12:case ch_12_mix_12:
			case ch_13_mix_12:case ch_14_mix_12:case ch_15_mix_12:case ch_16_mix_12:
			case ch_17_mix_12:case ch_18_mix_12:case ch_19_mix_12:case ch_20_mix_12:
			case ch_21_mix_12:case ch_22_mix_12:case ch_23_mix_12:case ch_24_mix_12:
			case ch_25_mix_12:case ch_26_mix_12:case ch_27_mix_12:case ch_28_mix_12:
			case ch_29_mix_12:case ch_30_mix_12:case ch_31_mix_12:case ch_32_mix_12:
			case ch_01_mix_14:case ch_02_mix_14:case ch_03_mix_14:case ch_04_mix_14: // /ch/01/mix/14
			case ch_05_mix_14:case ch_06_mix_14:case ch_07_mix_14:case ch_08_mix_14:
			case ch_09_mix_14:case ch_10_mix_14:case ch_11_mix_14:case ch_12_mix_14:
			case ch_13_mix_14:case ch_14_mix_14:case ch_15_mix_14:case ch_16_mix_14:
			case ch_17_mix_14:case ch_18_mix_14:case ch_19_mix_14:case ch_20_mix_14:
			case ch_21_mix_14:case ch_22_mix_14:case ch_23_mix_14:case ch_24_mix_14:
			case ch_25_mix_14:case ch_26_mix_14:case ch_27_mix_14:case ch_28_mix_14:
			case ch_29_mix_14:case ch_30_mix_14:case ch_31_mix_14:case ch_32_mix_14:
			case ch_01_mix_16:case ch_02_mix_16:case ch_03_mix_16:case ch_04_mix_16: // /ch/01/mix/16
			case ch_05_mix_16:case ch_06_mix_16:case ch_07_mix_16:case ch_08_mix_16:
			case ch_09_mix_16:case ch_10_mix_16:case ch_11_mix_16:case ch_12_mix_16:
			case ch_13_mix_16:case ch_14_mix_16:case ch_15_mix_16:case ch_16_mix_16:
			case ch_17_mix_16:case ch_18_mix_16:case ch_19_mix_16:case ch_20_mix_16:
			case ch_21_mix_16:case ch_22_mix_16:case ch_23_mix_16:case ch_24_mix_16:
			case ch_25_mix_16:case ch_26_mix_16:case ch_27_mix_16:case ch_28_mix_16:
			case ch_29_mix_16:case ch_30_mix_16:case ch_31_mix_16:case ch_32_mix_16:
				sscanf(l_read + 4, "%d", &ch);
				sscanf(l_read + 11, "%d", &mx);
				sprintf(tmp, "/ch/%02d/mix/%02d/on", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case ch_01_grp:case ch_02_grp:case ch_03_grp:case ch_04_grp: // /ch/01/grp
			case ch_05_grp:case ch_06_grp:case ch_07_grp:case ch_08_grp:
			case ch_09_grp:case ch_10_grp:case ch_11_grp:case ch_12_grp:
			case ch_13_grp:case ch_14_grp:case ch_15_grp:case ch_16_grp:
			case ch_17_grp:case ch_18_grp:case ch_19_grp:case ch_20_grp:
			case ch_21_grp:case ch_22_grp:case ch_23_grp:case ch_24_grp:
			case ch_25_grp:case ch_26_grp:case ch_27_grp:case ch_28_grp:
			case ch_29_grp:case ch_30_grp:case ch_31_grp:case ch_32_grp:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/grp/dca", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/ch/%02d/grp/mute", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				break;
			case ch_01_automix:case ch_02_automix:case ch_03_automix:case ch_04_automix: // /ch/01/automix
			case ch_05_automix:case ch_06_automix:case ch_07_automix:case ch_08_automix:
			case ch_09_automix:case ch_10_automix:case ch_11_automix:case ch_12_automix:
			case ch_13_automix:case ch_14_automix:case ch_15_automix:case ch_16_automix:
			case ch_17_automix:case ch_18_automix:case ch_19_automix:case ch_20_automix:
			case ch_21_automix:case ch_22_automix:case ch_23_automix:case ch_24_automix:
			case ch_25_automix:case ch_26_automix:case ch_27_automix:case ch_28_automix:
			case ch_29_automix:case ch_30_automix:case ch_31_automix:case ch_32_automix:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/ch/%02d/automix", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xp_linf(buf, k, -12., 24., 0.5);
				SendDataToX32 // send to X32
				break;
			case auxin_01_config: case auxin_02_config: case auxin_03_config: case auxin_04_config: // /auxin/01/config
			case auxin_05_config: case auxin_06_config: case auxin_07_config: case auxin_08_config:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/config/name", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/config/icon", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/config/color", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xcolor, Xcolor_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/config/source", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				break;
			case auxin_01_preamp:case auxin_02_preamp:case auxin_03_preamp:case auxin_04_preamp: // /auxin/01/preamp
			case auxin_05_preamp:case auxin_06_preamp:case auxin_07_preamp:case auxin_08_preamp:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/preamp/trim", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -18., 36., 0.25);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/preamp/invert", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case auxin_01_eq:case auxin_02_eq:case auxin_03_eq:case auxin_04_eq: // /auxin/01/eq
			case auxin_05_eq:case auxin_06_eq:case auxin_07_eq:case auxin_08_eq:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/eq/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case auxin_01_eq_1:case auxin_02_eq_1:case auxin_03_eq_1:case auxin_04_eq_1: // /auxin/01/eq/1
			case auxin_05_eq_1:case auxin_06_eq_1:case auxin_07_eq_1:case auxin_08_eq_1:
			case auxin_01_eq_2:case auxin_02_eq_2:case auxin_03_eq_2:case auxin_04_eq_2: // /auxin/01/eq/2
			case auxin_05_eq_2:case auxin_06_eq_2:case auxin_07_eq_2:case auxin_08_eq_2:
			case auxin_01_eq_3:case auxin_02_eq_3:case auxin_03_eq_3:case auxin_04_eq_3: // /auxin/01/eq/3
			case auxin_05_eq_3:case auxin_06_eq_3:case auxin_07_eq_3:case auxin_08_eq_3:
			case auxin_01_eq_4:case auxin_02_eq_4:case auxin_03_eq_4:case auxin_04_eq_4: // /auxin/01/eq/4
			case auxin_05_eq_4:case auxin_06_eq_4:case auxin_07_eq_4:case auxin_08_eq_4:
				sscanf(l_read + 7, "%d", &ch);
				sscanf(l_read + 13, "%c", &c1);
				sprintf(tmp, "/auxin/%02d/eq/%c/type", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqtyp, Xeqtyp_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/eq/%c/f", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/eq/%c/g", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/eq/%c/q", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log( 0.3/10) = -3.506557897
				SendDataToX32 // send to X32
				break;
			case auxin_01_mix:case auxin_02_mix:case auxin_03_mix:case auxin_04_mix: // /auxin/01/mix
			case auxin_05_mix:case auxin_06_mix:case auxin_07_mix:case auxin_08_mix:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/mix/st", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/mix/pan", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/mix/mono", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/mix/mlevel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case auxin_01_mix_fader:case auxin_02_mix_fader:case auxin_03_mix_fader:case auxin_04_mix_fader: // /auxin/01/mix_fader
			case auxin_05_mix_fader:case auxin_06_mix_fader:case auxin_07_mix_fader:case auxin_08_mix_fader:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case auxin_01_mix_pan:case auxin_02_mix_pan:case auxin_03_mix_pan:case auxin_04_mix_pan: // /auxin/01/mix_pan
			case auxin_05_mix_pan:case auxin_06_mix_pan:case auxin_07_mix_pan:case auxin_08_mix_pan:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/mix/pan", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				break;
			case auxin_01_mix_on:case auxin_02_mix_on:case auxin_03_mix_on:case auxin_04_mix_on: // /auxin/01/mix_on
			case auxin_05_mix_on:case auxin_06_mix_on:case auxin_07_mix_on:case auxin_08_mix_on:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case auxin_01_mix_mono:case auxin_02_mix_mono:case auxin_03_mix_mono:case auxin_04_mix_mono: // /auxin/01/mix_mono
			case auxin_05_mix_mono:case auxin_06_mix_mono:case auxin_07_mix_mono:case auxin_08_mix_mono:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/mix/mono", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case auxin_01_mix_mlevel:case auxin_02_mix_mlevel:case auxin_03_mix_mlevel:case auxin_04_mix_mlevel: // /auxin/01/mix_mlevel
			case auxin_05_mix_mlevel:case auxin_06_mix_mlevel:case auxin_07_mix_mlevel:case auxin_08_mix_mlevel:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/mix/mlevel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case auxin_01_mix_01:case auxin_02_mix_01:case auxin_03_mix_01:case auxin_04_mix_01: // /auxin/01/mix/01
			case auxin_05_mix_01:case auxin_06_mix_01:case auxin_07_mix_01:case auxin_08_mix_01:
			case auxin_01_mix_03:case auxin_02_mix_03:case auxin_03_mix_03:case auxin_04_mix_03: // /auxin/01/mix/03
			case auxin_05_mix_03:case auxin_06_mix_03:case auxin_07_mix_03:case auxin_08_mix_03:
			case auxin_01_mix_05:case auxin_02_mix_05:case auxin_03_mix_05:case auxin_04_mix_05: // /auxin/01/mix/05
			case auxin_05_mix_05:case auxin_06_mix_05:case auxin_07_mix_05:case auxin_08_mix_05:
			case auxin_01_mix_07:case auxin_02_mix_07:case auxin_03_mix_07:case auxin_04_mix_07: // /auxin/01/mix/07
			case auxin_05_mix_07:case auxin_06_mix_07:case auxin_07_mix_07:case auxin_08_mix_07:
			case auxin_01_mix_09:case auxin_02_mix_09:case auxin_03_mix_09:case auxin_04_mix_09: // /auxin/01/mix/09
			case auxin_05_mix_09:case auxin_06_mix_09:case auxin_07_mix_09:case auxin_08_mix_09:
			case auxin_01_mix_11:case auxin_02_mix_11:case auxin_03_mix_11:case auxin_04_mix_11: // /auxin/01/mix/11
			case auxin_05_mix_11:case auxin_06_mix_11:case auxin_07_mix_11:case auxin_08_mix_11:
			case auxin_01_mix_13:case auxin_02_mix_13:case auxin_03_mix_13:case auxin_04_mix_13: // /auxin/01/mix/13
			case auxin_05_mix_13:case auxin_06_mix_13:case auxin_07_mix_13:case auxin_08_mix_13:
			case auxin_01_mix_15:case auxin_02_mix_15:case auxin_03_mix_15:case auxin_04_mix_15: // /auxin/01/mix/15
			case auxin_05_mix_15:case auxin_06_mix_15:case auxin_07_mix_15:case auxin_08_mix_15:
				sscanf(l_read + 7, "%d", &ch);
				sscanf(l_read + 14, "%d", &mx);
				sprintf(tmp, "/auxin/%02d/mix/%02d/on", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/mix/%02d/pan", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/mix/%02d/type", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtyp, Xmxtyp_max);
				SendDataToX32 // send to X32
				break;
			case auxin_01_mix_02:case auxin_02_mix_02:case auxin_03_mix_02:case auxin_04_mix_02: // /auxin/01/mix/02
			case auxin_05_mix_02:case auxin_06_mix_02:case auxin_07_mix_02:case auxin_08_mix_02:
			case auxin_01_mix_04:case auxin_02_mix_04:case auxin_03_mix_04:case auxin_04_mix_04: // /auxin/01/mix/04
			case auxin_05_mix_04:case auxin_06_mix_04:case auxin_07_mix_04:case auxin_08_mix_04:
			case auxin_01_mix_06:case auxin_02_mix_06:case auxin_03_mix_06:case auxin_04_mix_06: // /auxin/01/mix/06
			case auxin_05_mix_06:case auxin_06_mix_06:case auxin_07_mix_06:case auxin_08_mix_06:
			case auxin_01_mix_08:case auxin_02_mix_08:case auxin_03_mix_08:case auxin_04_mix_08: // /auxin/01/mix/08
			case auxin_05_mix_08:case auxin_06_mix_08:case auxin_07_mix_08:case auxin_08_mix_08:
			case auxin_01_mix_10:case auxin_02_mix_10:case auxin_03_mix_10:case auxin_04_mix_10: // /auxin/01/mix/10
			case auxin_05_mix_10:case auxin_06_mix_10:case auxin_07_mix_10:case auxin_08_mix_10:
			case auxin_01_mix_12:case auxin_02_mix_12:case auxin_03_mix_12:case auxin_04_mix_12: // /auxin/01/mix/12
			case auxin_05_mix_12:case auxin_06_mix_12:case auxin_07_mix_12:case auxin_08_mix_12:
			case auxin_01_mix_14:case auxin_02_mix_14:case auxin_03_mix_14:case auxin_04_mix_14: // /auxin/01/mix/14
			case auxin_05_mix_14:case auxin_06_mix_14:case auxin_07_mix_14:case auxin_08_mix_14:
			case auxin_01_mix_16:case auxin_02_mix_16:case auxin_03_mix_16:case auxin_04_mix_16: // /auxin/01/mix/16
			case auxin_05_mix_16:case auxin_06_mix_16:case auxin_07_mix_16:case auxin_08_mix_16:
				sscanf(l_read + 7, "%d", &ch);
				sscanf(l_read + 14, "%d", &mx);
				sprintf(tmp, "/auxin/%02d/mix/%02d/on", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case auxin_01_grp:case auxin_02_grp:case auxin_03_grp:case auxin_04_grp: // /auxin/01/grp
			case auxin_05_grp:case auxin_06_grp:case auxin_07_grp:case auxin_08_grp:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/auxin/%02d/grp/dca", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/auxin/%02d/grp/mute", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_config: case fxrtn_02_config: case fxrtn_03_config: case fxrtn_04_config: // /fxrtn/01/config
			case fxrtn_05_config: case fxrtn_06_config: case fxrtn_07_config: case fxrtn_08_config:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/fxrtn/%02d/config/name", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/config/icon", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/config/color", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xcolor, Xcolor_max);
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_eq:case fxrtn_02_eq:case fxrtn_03_eq:case fxrtn_04_eq: // /fxrtn/01/eq
			case fxrtn_05_eq:case fxrtn_06_eq:case fxrtn_07_eq:case fxrtn_08_eq:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/fxrtn/%02d/eq/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_eq_1:case fxrtn_02_eq_1:case fxrtn_03_eq_1:case fxrtn_04_eq_1: // /fxrtn/01/eq/1
			case fxrtn_05_eq_1:case fxrtn_06_eq_1:case fxrtn_07_eq_1:case fxrtn_08_eq_1:
			case fxrtn_01_eq_2:case fxrtn_02_eq_2:case fxrtn_03_eq_2:case fxrtn_04_eq_2: // /fxrtn/01/eq/2
			case fxrtn_05_eq_2:case fxrtn_06_eq_2:case fxrtn_07_eq_2:case fxrtn_08_eq_2:
			case fxrtn_01_eq_3:case fxrtn_02_eq_3:case fxrtn_03_eq_3:case fxrtn_04_eq_3: // /fxrtn/01/eq/3
			case fxrtn_05_eq_3:case fxrtn_06_eq_3:case fxrtn_07_eq_3:case fxrtn_08_eq_3:
			case fxrtn_01_eq_4:case fxrtn_02_eq_4:case fxrtn_03_eq_4:case fxrtn_04_eq_4: // /fxrtn/01/eq/4
			case fxrtn_05_eq_4:case fxrtn_06_eq_4:case fxrtn_07_eq_4:case fxrtn_08_eq_4:
				sscanf(l_read + 7, "%d", &ch);
				sscanf(l_read + 13, "%c", &c1);
				sprintf(tmp, "/fxrtn/%02d/eq/%c/type", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqtyp, Xeqtyp_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/eq/%c/f", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/eq/%c/g", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/eq/%c/q", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log(0.3 /10) = -3.506557897
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_mix:case fxrtn_02_mix:case fxrtn_03_mix:case fxrtn_04_mix: // /fxrtn/01/mix
			case fxrtn_05_mix:case fxrtn_06_mix:case fxrtn_07_mix:case fxrtn_08_mix:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/fxrtn/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/mix/st", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/mix/pan", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/mix/mono", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/mix/mlevel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_mix_fader:case fxrtn_02_mix_fader:case fxrtn_03_mix_fader:case fxrtn_04_mix_fader: // /fxrtn/01/mix_fader
			case fxrtn_05_mix_fader:case fxrtn_06_mix_fader:case fxrtn_07_mix_fader:case fxrtn_08_mix_fader:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/fxrtn/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_mix_pan:case fxrtn_02_mix_pan:case fxrtn_03_mix_pan:case fxrtn_04_mix_pan: // /fxrtn/01/mix_pan
			case fxrtn_05_mix_pan:case fxrtn_06_mix_pan:case fxrtn_07_mix_pan:case fxrtn_08_mix_pan:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/fxrtn/%02d/mix/pan", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_mix_on:case fxrtn_02_mix_on:case fxrtn_03_mix_on:case fxrtn_04_mix_on: // /fxrtn/01/mix_on
			case fxrtn_05_mix_on:case fxrtn_06_mix_on:case fxrtn_07_mix_on:case fxrtn_08_mix_on:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/fxrtn/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_mix_mono:case fxrtn_02_mix_mono:case fxrtn_03_mix_mono:case fxrtn_04_mix_mono: // /fxrtn/01/mix_mono
			case fxrtn_05_mix_mono:case fxrtn_06_mix_mono:case fxrtn_07_mix_mono:case fxrtn_08_mix_mono:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/fxrtn/%02d/mix/mono", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_mix_mlevel:case fxrtn_02_mix_mlevel:case fxrtn_03_mix_mlevel:case fxrtn_04_mix_mlevel: // /fxrtn/01/mix_mlevel
			case fxrtn_05_mix_mlevel:case fxrtn_06_mix_mlevel:case fxrtn_07_mix_mlevel:case fxrtn_08_mix_mlevel:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/fxrtn/%02d/mix/mlevel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_mix_01:case fxrtn_02_mix_01:case fxrtn_03_mix_01:case fxrtn_04_mix_01: // /fxrtn/01/mix/01
			case fxrtn_05_mix_01:case fxrtn_06_mix_01:case fxrtn_07_mix_01:case fxrtn_08_mix_01:
			case fxrtn_01_mix_03:case fxrtn_02_mix_03:case fxrtn_03_mix_03:case fxrtn_04_mix_03: // /fxrtn/01/mix/03
			case fxrtn_05_mix_03:case fxrtn_06_mix_03:case fxrtn_07_mix_03:case fxrtn_08_mix_03:
			case fxrtn_01_mix_05:case fxrtn_02_mix_05:case fxrtn_03_mix_05:case fxrtn_04_mix_05: // /fxrtn/01/mix/05
			case fxrtn_05_mix_05:case fxrtn_06_mix_05:case fxrtn_07_mix_05:case fxrtn_08_mix_05:
			case fxrtn_01_mix_07:case fxrtn_02_mix_07:case fxrtn_03_mix_07:case fxrtn_04_mix_07: // /fxrtn/01/mix/07
			case fxrtn_05_mix_07:case fxrtn_06_mix_07:case fxrtn_07_mix_07:case fxrtn_08_mix_07:
			case fxrtn_01_mix_09:case fxrtn_02_mix_09:case fxrtn_03_mix_09:case fxrtn_04_mix_09: // /fxrtn/01/mix/09
			case fxrtn_05_mix_09:case fxrtn_06_mix_09:case fxrtn_07_mix_09:case fxrtn_08_mix_09:
			case fxrtn_01_mix_11:case fxrtn_02_mix_11:case fxrtn_03_mix_11:case fxrtn_04_mix_11: // /fxrtn/01/mix/11
			case fxrtn_05_mix_11:case fxrtn_06_mix_11:case fxrtn_07_mix_11:case fxrtn_08_mix_11:
			case fxrtn_01_mix_13:case fxrtn_02_mix_13:case fxrtn_03_mix_13:case fxrtn_04_mix_13: // /fxrtn/01/mix/13
			case fxrtn_05_mix_13:case fxrtn_06_mix_13:case fxrtn_07_mix_13:case fxrtn_08_mix_13:
			case fxrtn_01_mix_15:case fxrtn_02_mix_15:case fxrtn_03_mix_15:case fxrtn_04_mix_15: // /fxrtn/01/mix/15
			case fxrtn_05_mix_15:case fxrtn_06_mix_15:case fxrtn_07_mix_15:case fxrtn_08_mix_15:
				sscanf(l_read + 7, "%d", &ch);
				sscanf(l_read + 14, "%d", &mx);
				sprintf(tmp, "/fxrtn/%02d/mix/%02d/on", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/mix/%02d/pan", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/mix/%02d/type", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtyp, Xmxtyp_max);
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_mix_02:case fxrtn_02_mix_02:case fxrtn_03_mix_02:case fxrtn_04_mix_02: // /fxrtn/01/mix/02
			case fxrtn_05_mix_02:case fxrtn_06_mix_02:case fxrtn_07_mix_02:case fxrtn_08_mix_02:
			case fxrtn_01_mix_04:case fxrtn_02_mix_04:case fxrtn_03_mix_04:case fxrtn_04_mix_04: // /fxrtn/01/mix/04
			case fxrtn_05_mix_04:case fxrtn_06_mix_04:case fxrtn_07_mix_04:case fxrtn_08_mix_04:
			case fxrtn_01_mix_06:case fxrtn_02_mix_06:case fxrtn_03_mix_06:case fxrtn_04_mix_06: // /fxrtn/01/mix/06
			case fxrtn_05_mix_06:case fxrtn_06_mix_06:case fxrtn_07_mix_06:case fxrtn_08_mix_06:
			case fxrtn_01_mix_08:case fxrtn_02_mix_08:case fxrtn_03_mix_08:case fxrtn_04_mix_08: // /fxrtn/01/mix/08
			case fxrtn_05_mix_08:case fxrtn_06_mix_08:case fxrtn_07_mix_08:case fxrtn_08_mix_08:
			case fxrtn_01_mix_10:case fxrtn_02_mix_10:case fxrtn_03_mix_10:case fxrtn_04_mix_10: // /fxrtn/01/mix/10
			case fxrtn_05_mix_10:case fxrtn_06_mix_10:case fxrtn_07_mix_10:case fxrtn_08_mix_10:
			case fxrtn_01_mix_12:case fxrtn_02_mix_12:case fxrtn_03_mix_12:case fxrtn_04_mix_12: // /fxrtn/01/mix/12
			case fxrtn_05_mix_12:case fxrtn_06_mix_12:case fxrtn_07_mix_12:case fxrtn_08_mix_12:
			case fxrtn_01_mix_14:case fxrtn_02_mix_14:case fxrtn_03_mix_14:case fxrtn_04_mix_14: // /fxrtn/01/mix/14
			case fxrtn_05_mix_14:case fxrtn_06_mix_14:case fxrtn_07_mix_14:case fxrtn_08_mix_14:
			case fxrtn_01_mix_16:case fxrtn_02_mix_16:case fxrtn_03_mix_16:case fxrtn_04_mix_16: // /fxrtn/01/mix/16
			case fxrtn_05_mix_16:case fxrtn_06_mix_16:case fxrtn_07_mix_16:case fxrtn_08_mix_16:
				sscanf(l_read + 7, "%d", &ch);
				sscanf(l_read + 14, "%d", &mx);
				sprintf(tmp, "/fxrtn/%02d/mix/%02d/on", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case fxrtn_01_grp:case fxrtn_02_grp:case fxrtn_03_grp:case fxrtn_04_grp: // /fxrtn/01/grp
			case fxrtn_05_grp:case fxrtn_06_grp:case fxrtn_07_grp:case fxrtn_08_grp:
				sscanf(l_read + 7, "%d", &ch);
				sprintf(tmp, "/fxrtn/%02d/grp/dca", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fxrtn/%02d/grp/mute", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				break;
			case bus_01_config: case bus_02_config: case bus_03_config: case bus_04_config: // /bus/01/config
			case bus_05_config: case bus_06_config: case bus_07_config: case bus_08_config:
			case bus_09_config: case bus_10_config: case bus_11_config: case bus_12_config:
			case bus_13_config: case bus_14_config: case bus_15_config: case bus_16_config:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/config/name", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/config/icon", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/config/color", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xcolor, Xcolor_max);
				SendDataToX32 // send to X32
				break;
			case bus_01_dyn:case bus_02_dyn:case bus_03_dyn:case bus_04_dyn: // /bus/01/dyn
			case bus_05_dyn:case bus_06_dyn:case bus_07_dyn:case bus_08_dyn:
			case bus_09_dyn:case bus_10_dyn:case bus_11_dyn:case bus_12_dyn:
			case bus_13_dyn:case bus_14_dyn:case bus_15_dyn:case bus_16_dyn:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/dyn/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/mode", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "COMP");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/det", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PEAK");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/env", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "UN");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/thr", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -60., 60., 0.5);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/ratio", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xdynratio, Xdynratio_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/knee", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 5., 1.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/mgain", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 24., 0.5);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/attack", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/hold", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log(2000 / 0.02) = 11.512925465
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/release", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log(4000/5) = 6.684611728
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/keysrc", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/mix", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_percent(buf, k);
				SendDataToX32 // send to X32
				if (X32VER > 209) {	//starting with FW 2.10...
					sprintf(tmp, "/bus/%02d/dyn/auto", ch);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case bus_01_dyn_filter:case bus_02_dyn_filter:case bus_03_dyn_filter:case bus_04_dyn_filter: // /bus/01/dyn/filter
			case bus_05_dyn_filter:case bus_06_dyn_filter:case bus_07_dyn_filter:case bus_08_dyn_filter:
			case bus_09_dyn_filter:case bus_10_dyn_filter:case bus_11_dyn_filter:case bus_12_dyn_filter:
			case bus_13_dyn_filter:case bus_14_dyn_filter:case bus_15_dyn_filter:case bus_16_dyn_filter:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/dyn/filter/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/filter/type", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xgateftype, Xgateftype_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/dyn/filter/f", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				break;
			case bus_01_insert:case bus_02_insert:case bus_03_insert:case bus_04_insert: // /bus/01/insert
			case bus_05_insert:case bus_06_insert:case bus_07_insert:case bus_08_insert:
			case bus_09_insert:case bus_10_insert:case bus_11_insert:case bus_12_insert:
			case bus_13_insert:case bus_14_insert:case bus_15_insert:case bus_16_insert:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/insert/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/insert/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/insert/sel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinsel, Xinsel_max);
				SendDataToX32 // send to X32
				break;
			case bus_01_eq:case bus_02_eq:case bus_03_eq:case bus_04_eq: // /bus/01/eq
			case bus_05_eq:case bus_06_eq:case bus_07_eq:case bus_08_eq:
			case bus_09_eq:case bus_10_eq:case bus_11_eq:case bus_12_eq:
			case bus_13_eq:case bus_14_eq:case bus_15_eq:case bus_16_eq:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/eq/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case bus_01_eq_1:case bus_02_eq_1:case bus_03_eq_1:case bus_04_eq_1: // /bus/01/eq/1
			case bus_05_eq_1:case bus_06_eq_1:case bus_07_eq_1:case bus_08_eq_1:
			case bus_09_eq_1:case bus_10_eq_1:case bus_11_eq_1:case bus_12_eq_1:
			case bus_13_eq_1:case bus_14_eq_1:case bus_15_eq_1:case bus_16_eq_1:
			case bus_01_eq_2:case bus_02_eq_2:case bus_03_eq_2:case bus_04_eq_2: // /bus/01/eq/2
			case bus_05_eq_2:case bus_06_eq_2:case bus_07_eq_2:case bus_08_eq_2:
			case bus_09_eq_2:case bus_10_eq_2:case bus_11_eq_2:case bus_12_eq_2:
			case bus_13_eq_2:case bus_14_eq_2:case bus_15_eq_2:case bus_16_eq_2:
			case bus_01_eq_3:case bus_02_eq_3:case bus_03_eq_3:case bus_04_eq_3: // /bus/01/eq/3
			case bus_05_eq_3:case bus_06_eq_3:case bus_07_eq_3:case bus_08_eq_3:
			case bus_09_eq_3:case bus_10_eq_3:case bus_11_eq_3:case bus_12_eq_3:
			case bus_13_eq_3:case bus_14_eq_3:case bus_15_eq_3:case bus_16_eq_3:
			case bus_01_eq_4:case bus_02_eq_4:case bus_03_eq_4:case bus_04_eq_4: // /bus/01/eq/4
			case bus_05_eq_4:case bus_06_eq_4:case bus_07_eq_4:case bus_08_eq_4:
			case bus_09_eq_4:case bus_10_eq_4:case bus_11_eq_4:case bus_12_eq_4:
			case bus_13_eq_4:case bus_14_eq_4:case bus_15_eq_4:case bus_16_eq_4:
			case bus_01_eq_5:case bus_02_eq_5:case bus_03_eq_5:case bus_04_eq_5: // /bus/01/eq/5
			case bus_05_eq_5:case bus_06_eq_5:case bus_07_eq_5:case bus_08_eq_5:
			case bus_09_eq_5:case bus_10_eq_5:case bus_11_eq_5:case bus_12_eq_5:
			case bus_13_eq_5:case bus_14_eq_5:case bus_15_eq_5:case bus_16_eq_5:
			case bus_01_eq_6:case bus_02_eq_6:case bus_03_eq_6:case bus_04_eq_6: // /bus/01/eq/6
			case bus_05_eq_6:case bus_06_eq_6:case bus_07_eq_6:case bus_08_eq_6:
			case bus_09_eq_6:case bus_10_eq_6:case bus_11_eq_6:case bus_12_eq_6:
			case bus_13_eq_6:case bus_14_eq_6:case bus_15_eq_6:case bus_16_eq_6:
				sscanf(l_read + 5, "%d", &ch);
				sscanf(l_read + 11, "%c", &c1);
				sprintf(tmp, "/bus/%02d/eq/%c/type", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xeqtyp, Xeqtyp_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/eq/%c/f", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/eq/%c/g", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/eq/%c/q", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log(0.3/10) = -3.506557897
				SendDataToX32 // send to X32
				break;
			case bus_01_mix:case bus_02_mix:case bus_03_mix:case bus_04_mix: // /bus/01/mix
			case bus_05_mix:case bus_06_mix:case bus_07_mix:case bus_08_mix:
			case bus_09_mix:case bus_10_mix:case bus_11_mix:case bus_12_mix:
			case bus_13_mix:case bus_14_mix:case bus_15_mix:case bus_16_mix:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/mix/st", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/mix/pan", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/mix/mono", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/mix/mlevel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case bus_01_mix_fader:case bus_02_mix_fader:case bus_03_mix_fader:case bus_04_mix_fader: // /bus/01/mix_fader
			case bus_05_mix_fader:case bus_06_mix_fader:case bus_07_mix_fader:case bus_08_mix_fader:
			case bus_09_mix_fader:case bus_10_mix_fader:case bus_11_mix_fader:case bus_12_mix_fader:
			case bus_13_mix_fader:case bus_14_mix_fader:case bus_15_mix_fader:case bus_16_mix_fader:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case bus_01_mix_pan:case bus_02_mix_pan:case bus_03_mix_pan:case bus_04_mix_pan: // /bus/01/mix_pan
			case bus_05_mix_pan:case bus_06_mix_pan:case bus_07_mix_pan:case bus_08_mix_pan:
			case bus_09_mix_pan:case bus_10_mix_pan:case bus_11_mix_pan:case bus_12_mix_pan:
			case bus_13_mix_pan:case bus_14_mix_pan:case bus_15_mix_pan:case bus_16_mix_pan:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/mix/pan", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				break;
			case bus_01_mix_on:case bus_02_mix_on:case bus_03_mix_on:case bus_04_mix_on: // /bus/01/mix_on
			case bus_05_mix_on:case bus_06_mix_on:case bus_07_mix_on:case bus_08_mix_on:
			case bus_09_mix_on:case bus_10_mix_on:case bus_11_mix_on:case bus_12_mix_on:
			case bus_13_mix_on:case bus_14_mix_on:case bus_15_mix_on:case bus_16_mix_on:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case bus_01_mix_mono:case bus_02_mix_mono:case bus_03_mix_mono:case bus_04_mix_mono: // /bus/01/mix_mono
			case bus_05_mix_mono:case bus_06_mix_mono:case bus_07_mix_mono:case bus_08_mix_mono:
			case bus_09_mix_mono:case bus_10_mix_mono:case bus_11_mix_mono:case bus_12_mix_mono:
			case bus_13_mix_mono:case bus_14_mix_mono:case bus_15_mix_mono:case bus_16_mix_mono:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/mix/mono", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case bus_01_mix_mlevel:case bus_02_mix_mlevel:case bus_03_mix_mlevel:case bus_04_mix_mlevel: // /bus/01/mix_mlevel
			case bus_05_mix_mlevel:case bus_06_mix_mlevel:case bus_07_mix_mlevel:case bus_08_mix_mlevel:
			case bus_09_mix_mlevel:case bus_10_mix_mlevel:case bus_11_mix_mlevel:case bus_12_mix_mlevel:
			case bus_13_mix_mlevel:case bus_14_mix_mlevel:case bus_15_mix_mlevel:case bus_16_mix_mlevel:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/mix/mlevel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case bus_01_mix_01:case bus_02_mix_01:case bus_03_mix_01:case bus_04_mix_01: // /bus/01/mix/01
			case bus_05_mix_01:case bus_06_mix_01:case bus_07_mix_01:case bus_08_mix_01:
			case bus_09_mix_01:case bus_10_mix_01:case bus_11_mix_01:case bus_12_mix_01:
			case bus_13_mix_01:case bus_14_mix_01:case bus_15_mix_01:case bus_16_mix_01:
			case bus_01_mix_03:case bus_02_mix_03:case bus_03_mix_03:case bus_04_mix_03: // /bus/01/mix/03
			case bus_05_mix_03:case bus_06_mix_03:case bus_07_mix_03:case bus_08_mix_03:
			case bus_09_mix_03:case bus_10_mix_03:case bus_11_mix_03:case bus_12_mix_03:
			case bus_13_mix_03:case bus_14_mix_03:case bus_15_mix_03:case bus_16_mix_03:
			case bus_01_mix_05:case bus_02_mix_05:case bus_03_mix_05:case bus_04_mix_05: // /bus/01/mix/05
			case bus_05_mix_05:case bus_06_mix_05:case bus_07_mix_05:case bus_08_mix_05:
			case bus_09_mix_05:case bus_10_mix_05:case bus_11_mix_05:case bus_12_mix_05:
			case bus_13_mix_05:case bus_14_mix_05:case bus_15_mix_05:case bus_16_mix_05:
				sscanf(l_read + 5, "%d", &ch);
				sscanf(l_read + 12, "%d", &mx);
				sprintf(tmp, "/bus/%02d/mix/%02d/on", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/mix/%02d/pan", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/mix/%02d/type", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtyp, Xmxtyp_max);
				SendDataToX32 // send to X32
				break;
			case bus_01_mix_02:case bus_02_mix_02:case bus_03_mix_02:case bus_04_mix_02: // /bus/01/mix/02
			case bus_05_mix_02:case bus_06_mix_02:case bus_07_mix_02:case bus_08_mix_02:
			case bus_09_mix_02:case bus_10_mix_02:case bus_11_mix_02:case bus_12_mix_02:
			case bus_13_mix_02:case bus_14_mix_02:case bus_15_mix_02:case bus_16_mix_02:
			case bus_01_mix_04:case bus_02_mix_04:case bus_03_mix_04:case bus_04_mix_04: // /bus/01/mix/04
			case bus_05_mix_04:case bus_06_mix_04:case bus_07_mix_04:case bus_08_mix_04:
			case bus_09_mix_04:case bus_10_mix_04:case bus_11_mix_04:case bus_12_mix_04:
			case bus_13_mix_04:case bus_14_mix_04:case bus_15_mix_04:case bus_16_mix_04:
			case bus_01_mix_06:case bus_02_mix_06:case bus_03_mix_06:case bus_04_mix_06: // /bus/01/mix/06
			case bus_05_mix_06:case bus_06_mix_06:case bus_07_mix_06:case bus_08_mix_06:
			case bus_09_mix_06:case bus_10_mix_06:case bus_11_mix_06:case bus_12_mix_06:
			case bus_13_mix_06:case bus_14_mix_06:case bus_15_mix_06:case bus_16_mix_06:
				sscanf(l_read + 5, "%d", &ch);
				sscanf(l_read + 12, "%d", &mx);
				sprintf(tmp, "/bus/%02d/mix/%02d/on", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/mix/%02d/level", ch, mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case bus_01_grp:case bus_02_grp:case bus_03_grp:case bus_04_grp: // /bus/01/grp
			case bus_05_grp:case bus_06_grp:case bus_07_grp:case bus_08_grp:
			case bus_09_grp:case bus_10_grp:case bus_11_grp:case bus_12_grp:
			case bus_13_grp:case bus_14_grp:case bus_15_grp:case bus_16_grp:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/bus/%02d/grp/dca", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/bus/%02d/grp/mute", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_bit(buf, k);
				SendDataToX32 // send to X32
				break;
			case mtx_01_config: case mtx_02_config: case mtx_03_config: // /mtx/01/config
			case mtx_04_config: case mtx_05_config: case mtx_06_config:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/mtx/%02d/config/name", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/config/icon", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/config/color", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xcolor, Xcolor_max);
				SendDataToX32 // send to X32
				break;
			case mtx_01_preamp: case mtx_02_preamp: case mtx_03_preamp: // /mtx/01/preamp
			case mtx_04_preamp: case mtx_05_preamp: case mtx_06_preamp:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/mtx/%02d/preamp/invert", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case mtx_01_dyn:case mtx_02_dyn:case mtx_03_dyn: // /mtx/01/dyn
			case mtx_04_dyn:case mtx_05_dyn:case mtx_06_dyn:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/mtx/%02d/dyn/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/mode", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "COMP");
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/det", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PEAK");
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/env", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "UN");
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/thr", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -60., 60., 0.5);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/ratio", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xdynratio, Xdynratio_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/knee", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 5., 1.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/mgain", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 24., 0.5);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/attack", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/hold", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log (2000 / 0.02) = 11.512925465
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/release", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log (4000/5) = 6.684611728
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/mix", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_percent(buf, k);
				SendDataToX32 // send to X32
				if (X32VER > 209) {	//starting with FW 2.10...
					sprintf(tmp, "/mtx/%02d/dyn/auto", ch);
					k = Xsprint(buf, 0, 's', tmp);
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case mtx_01_dyn_filter:case mtx_02_dyn_filter:case mtx_03_dyn_filter: // /mtx/01/dyn/filter
			case mtx_04_dyn_filter:case mtx_05_dyn_filter:case mtx_06_dyn_filter:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/mtx/%02d/dyn/filter/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/filter/type", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xgateftype, Xgateftype_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/dyn/filter/f", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				break;
			case mtx_01_insert:case mtx_02_insert:case mtx_03_insert: // /mtx/01/insert
			case mtx_04_insert:case mtx_05_insert:case mtx_06_insert:
				sscanf(l_read + 4, "%d", &ch);
				sprintf(tmp, "/mtx/%02d/insert/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/insert/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/insert/sel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xinsel, Xinsel_max);
				SendDataToX32 // send to X32
				break;
			case mtx_01_eq:case mtx_02_eq:case mtx_03_eq: // /mtx/01/eq
			case mtx_04_eq:case mtx_05_eq:case mtx_06_eq:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/mtx/%02d/eq/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case mtx_01_eq_1:case mtx_02_eq_1:case mtx_03_eq_1: // /mtx/01/eq/1
			case mtx_04_eq_1:case mtx_05_eq_1:case mtx_06_eq_1:
			case mtx_01_eq_2:case mtx_02_eq_2:case mtx_03_eq_2: // /mtx/01/eq/2
			case mtx_04_eq_2:case mtx_05_eq_2:case mtx_06_eq_2:
			case mtx_01_eq_3:case mtx_02_eq_3:case mtx_03_eq_3: // /mtx/01/eq/3
			case mtx_04_eq_3:case mtx_05_eq_3:case mtx_06_eq_3:
			case mtx_01_eq_4:case mtx_02_eq_4:case mtx_03_eq_4: // /mtx/01/eq/4
			case mtx_04_eq_4:case mtx_05_eq_4:case mtx_06_eq_4:
			case mtx_01_eq_5:case mtx_02_eq_5:case mtx_03_eq_5: // /mtx/01/eq/5
			case mtx_04_eq_5:case mtx_05_eq_5:case mtx_06_eq_5:
			case mtx_01_eq_6:case mtx_02_eq_6:case mtx_03_eq_6: // /mtx/01/eq/6
			case mtx_04_eq_6:case mtx_05_eq_6:case mtx_06_eq_6:
				sscanf(l_read + 5, "%d", &ch);
				sscanf(l_read + 11, "%c", &c1);
				sprintf(tmp, "/mtx/%02d/eq/%c/type", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmeqtyp, Xmeqtyp_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/eq/%c/f", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/eq/%c/g", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/eq/%c/q", ch, c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log(.3/10) = -3.506557897
				SendDataToX32 // send to X32
				break;
			case mtx_01_mix:case mtx_02_mix:case mtx_03_mix: // /mtx/01/mix
			case mtx_04_mix:case mtx_05_mix:case mtx_06_mix:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/mtx/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/mtx/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case mtx_01_mix_fader:case mtx_02_mix_fader:case mtx_03_mix_fader: // /mtx/01/mix_fader
			case mtx_04_mix_fader:case mtx_05_mix_fader:case mtx_06_mix_fader:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/mtx/%02d/mix/fader", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case mtx_01_mix_on:case mtx_02_mix_on:case mtx_03_mix_on: // /mtx/01/mix_on
			case mtx_04_mix_on:case mtx_05_mix_on:case mtx_06_mix_on:
				sscanf(l_read + 5, "%d", &ch);
				sprintf(tmp, "/mtx/%02d/mix/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case main_st_config: // /main/st/config
				k = Xsprint(buf, 0, 's', "/main/st/config/name");
				k = Xp_str(buf, k);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/config/icon");
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/config/color");
				k = Xp_list(buf, k, Xcolor, Xcolor_max);
				SendDataToX32 // send to X32
				break;
			case main_st_dyn: // /main/st/dyn
				k = Xsprint(buf, 0, 's', "/main/st/dyn/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/mode");
				k = XOff_On(buf, k, "COMP");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/det");
				k = XOff_On(buf, k, "PEAK");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/env");
				k = XOff_On(buf, k, "UN");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/thr");
				k = Xp_linf(buf, k, -60., 60., 0.5);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/ratio");
				k = Xp_list(buf, k, Xdynratio, Xdynratio_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/knee");
				k = Xp_linf(buf, k, 0., 5., 1.);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/mgain");
				k = Xp_linf(buf, k, 0., 24., 0.5);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/attack");
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/hold");
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log(2000/0.02) = 11.512925465
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/release");
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log(4000/5) = 6.684611728
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/pos");
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/mix");
				k = Xp_percent(buf, k);
				SendDataToX32 // send to X32
				if (X32VER > 209) {	//starting with FW 2.10...
					k = Xsprint(buf, 0, 's', "/main/st/dyn/auto");
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case main_st_dyn_filter: // /main/st/dyn/filter
				k = Xsprint(buf, 0, 's', "/main/st/dyn/filter/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/filter/type");
				k = Xp_list(buf, k, Xgateftype, Xgateftype_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/dyn/filter/f");
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				break;
			case main_st_insert: // /main/st/insert
				k = Xsprint(buf, 0, 's', "/main/st/insert/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/insert/pos");
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/insert/sel");
				k = Xp_list(buf, k, Xinsel, Xinsel_max);
				SendDataToX32 // send to X32
				break;
			case main_st_eq: // /main/st/eq
				k = Xsprint(buf, 0, 's', "/main/st/eq/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case main_st_eq_1:case main_st_eq_2:case main_st_eq_3: // /main/st/eq/1
			case main_st_eq_4:case main_st_eq_5:case main_st_eq_6:
				sscanf(l_read + 12, "%c", &c1);
				sprintf(tmp, "/main/st/eq/%c/type", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmeqtyp, Xmeqtyp_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/st/eq/%c/f", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/st/eq/%c/g", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/st/eq/%c/q", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log (0.3/10) = -3.506557897
				SendDataToX32 // send to X32
				break;
			case main_st_mix: // /main/st/mix
				k = Xsprint(buf, 0, 's', "/main/st/mix/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/mix/fader");
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/st/mix/pan");
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				break;
			case main_st_mix_on: // /main/st/mix/on
				k = Xsprint(buf, 0, 's', "/main/st/mix/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case main_st_mix_fader: // /main/st/mix/fader
				k = Xsprint(buf, 0, 's', "/main/st/mix/fader");
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case main_st_mix_pan: // /main/st/mix/pan
				k = Xsprint(buf, 0, 's', "/main/st/mix/pan");
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				break;
			case main_st_mix_01:case main_st_mix_03:case main_st_mix_05: // /main/st/mix/01
				sscanf(l_read + 13, "%d", &mx);
				sprintf(tmp, "/main/st/mix/%02d/on", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/st/mix/%02d/level", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/st/mix/%02d/pan", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/st/mix/%02d/type", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtyp, Xmxtyp_max);
				SendDataToX32 // send to X32
				break;
			case main_st_mix_02:case main_st_mix_04:case main_st_mix_06: // /main/st/mix/02
				sscanf(l_read + 13, "%d", &mx);
				sprintf(tmp, "/main/st/mix/%02d/on", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/st/mix/%02d/level", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case main_m_config: // /main/m/config
				k = Xsprint(buf, 0, 's', "/main/m/config/name");
				k = Xp_str(buf, k);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/config/icon");
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/config/color");
				k = Xp_list(buf, k, Xcolor, Xcolor_max);
				SendDataToX32 // send to X32
				break;
			case main_m_dyn: // /main/m/dyn
				k = Xsprint(buf, 0, 's', "/main/m/dyn/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/mode");
				k = XOff_On(buf, k, "COMP");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/det");
				k = XOff_On(buf, k, "PEAK");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/env");
				k = XOff_On(buf, k, "UN");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/thr");
				k = Xp_linf(buf, k, -60., 60., 0.5);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/ratio");
				k = Xp_list(buf, k, Xdynratio, Xdynratio_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/knee");
				k = Xp_linf(buf, k, 0., 5., 1.);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/mgain");
				k = Xp_linf(buf, k, 0., 24., 0.5);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/attack");
				k = Xp_linf(buf, k, 0., 120., 1.);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/hold");
				k = Xp_logf(buf, k, 0.02, 11.512925465, 100); // log(2000/0.02) = 11.512925465
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/release");
				k = Xp_logf(buf, k, 5., 6.684611728, 100); // log(4000/5) = 6.684611728
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/pos");
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/mix");
				k = Xp_percent(buf, k);
				SendDataToX32 // send to X32
				if (X32VER > 209) {	//starting with FW 2.10...
					k = Xsprint(buf, 0, 's', "/main/m/dyn/auto");
					k = XOff_On(buf, k, "OFF");
					SendDataToX32 // send to X32
				}
				break;
			case main_m_dyn_filter: // /main/m/dyn/filter
				k = Xsprint(buf, 0, 's', "/main/m/dyn/filter/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/filter/type");
				k = Xp_list(buf, k, Xgateftype, Xgateftype_max);
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/dyn/filter/f");
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				break;
			case main_m_insert: // /main/m/insert
				k = Xsprint(buf, 0, 's', "/main/m/insert/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/insert/pos");
				k = XOff_On(buf, k, "PRE");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/insert/sel");
				k = Xp_list(buf, k, Xinsel, Xinsel_max);
				SendDataToX32 // send to X32
				break;
			case main_m_eq: // /main/m/eq
				k = Xsprint(buf, 0, 's', "/main/m/eq/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case main_m_eq_1:case main_m_eq_2:case main_m_eq_3: // /main/m/eq/1
			case main_m_eq_4:case main_m_eq_5:case main_m_eq_6:
				sscanf(l_read + 11, "%c", &c1);
				sprintf(tmp, "/main/m/eq/%c/type", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmeqtyp, Xmeqtyp_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/m/eq/%c/f", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_frequency(buf, k, 200);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/m/eq/%c/g", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -15., 30., 0.25);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/m/eq/%c/q", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_logf(buf, k, 10., -3.506557897, 71); // log (0.3/10) = -3.506557897
				SendDataToX32 // send to X32
				break;
			case main_m_mix: // /main/m/mix
				k = Xsprint(buf, 0, 's', "/main/m/mix/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				k = Xsprint(buf, 0, 's', "/main/m/mix/fader");
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case main_m_mix_on: // /main/m/mix_on
				k = Xsprint(buf, 0, 's', "/main/m/mix/on");
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case main_m_mix_fader: // /main/m/mix_fader
				k = Xsprint(buf, 0, 's', "/main/m/mix/fader");
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case main_m_mix_01:case main_m_mix_03:case main_m_mix_05: // /main/m/mix/01
				sscanf(l_read + 12, "%d", &mx);
				sprintf(tmp, "/main/m/mix/%02d/on", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/m/mix/%02d/level", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/m/mix/%02d/pan", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -100., 200., 2.);
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/m/mix/%02d/type", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xmxtyp, Xmxtyp_max);
				SendDataToX32 // send to X32
				break;
			case main_m_mix_02:case main_m_mix_04:case main_m_mix_06: // /main/m/mix/02
				sscanf(l_read + 12, "%d", &mx);
				sprintf(tmp, "/main/m/mix/%02d/on", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/main/m/mix/%02d/level", mx);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 160);
				SendDataToX32 // send to X32
				break;
			case dca_1:case dca_2:case dca_3:case dca_4: // /dca/1
			case dca_5:case dca_6:case dca_7:case dca_8:
				sscanf(l_read + 5, "%c", &c1);
				sprintf(tmp, "/dca/%c/on", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/dca/%c/fader", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case dca_1_on:case dca_2_on:case dca_3_on:case dca_4_on: // /dca/1/on
			case dca_5_on:case dca_6_on:case dca_7_on:case dca_8_on:
				sscanf(l_read + 5, "%c", &c1);
				sprintf(tmp, "/dca/%c/on", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case dca_1_fader:case dca_2_fader:case dca_3_fader:case dca_4_fader: // /dca/1/fader
			case dca_5_fader:case dca_6_fader:case dca_7_fader:case dca_8_fader:
				sscanf(l_read + 5, "%c", &c1);
				sprintf(tmp, "/dca/%c/fader", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_level(buf, k, 1023);
				SendDataToX32 // send to X32
				break;
			case dca_1_config:case dca_2_config:case dca_3_config:case dca_4_config: // /dca/1/config
			case dca_5_config:case dca_6_config:case dca_7_config:case dca_8_config:
				sscanf(l_read + 5, "%c", &c1);
				sprintf(tmp, "/dca/%c/config/name", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_str(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/dca/%c/config/icon", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/dca/%c/config/color", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xcolor, Xcolor_max);
				SendDataToX32 // send to X32
				break;
			case fx_1:case fx_1_type:case fx_2:case fx_2_type:case fx_3:case fx_3_type:case fx_4:case fx_4_type: // /fx/1
				sscanf(l_read + 4, "%c", &c1);
				sprintf(tmp, "/fx/%c/type", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_fxlist(buf, k, Xfxtyp4, Xfxtyp4_max, &(fx[(int)c1 - (int)'1']));
				SendDataToX32 // send to X32
				break;
			case fx_1_source:case fx_2_source:case fx_3_source:case fx_4_source: // /fx/1/source
				sscanf(l_read + 4, "%c", &c1);
				sprintf(tmp, "/fx/%c/source/l", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xfxsrc4, Xfxsrc4_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/fx/%c/source/r", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xfxsrc4, Xfxsrc4_max);
				SendDataToX32 // send to X32
				break;
			case fx_1_par:case fx_2_par:case fx_3_par:case fx_4_par: // /fx/1/par
				sscanf(l_read + 4, "%c", &c1);
				sprintf(tmp, "/fx/%c/par", c1);
				k = Xsprint(buf, 0, 's', tmp);
				//fx['c1'] is the current FX type
				k = fxparse1(buf, k, fx[(int)c1 - (int)'1']);
				SendDataToX32 // send to X32
				break;
			case fx_5:case fx_5_type:case fx_6:case fx_6_type:case fx_7:case fx_7_type:case fx_8:case fx_8_type: // /fx/5
				sscanf(l_read + 4, "%c", &c1);
				sprintf(tmp, "/fx/%c/type", c1);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_fxlist(buf, k, Xfxtyp5, Xfxtyp5_max, &(fx[(int)c1 - (int)'1']));
				SendDataToX32 // send to X32
				break;
			case fx_5_par:case fx_6_par:case fx_7_par:case fx_8_par: // /fx/5/par
				sscanf(l_read + 4, "%c", &c1);
				sprintf(tmp, "/fx/%c/par", c1);
				k = Xsprint(buf, 0, 's', tmp);
				//fx['c1'] is the current FX type
				k = fxparse5(buf, k, fx[(int)c1 - (int)'1']);
				SendDataToX32 // send to X32
				break;
			case outputs_main_01:case outputs_main_02:case outputs_main_03:case outputs_main_04: // /outputs/main/01
			case outputs_main_05:case outputs_main_06:case outputs_main_07:case outputs_main_08:
			case outputs_main_09:case outputs_main_10:case outputs_main_11:case outputs_main_12:
			case outputs_main_13:case outputs_main_14:case outputs_main_15:case outputs_main_16:
				sscanf(l_read + 14, "%d", &ch);
				sprintf(tmp, "/outputs/main/%02d/src", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/main/%02d/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xoutpos, Xoutpos_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/main/%02d/invert", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case outputs_main_01_delay:case outputs_main_02_delay:case outputs_main_03_delay:case outputs_main_04_delay: // /outputs/main/01
			case outputs_main_05_delay:case outputs_main_06_delay:case outputs_main_07_delay:case outputs_main_08_delay:
			case outputs_main_09_delay:case outputs_main_10_delay:case outputs_main_11_delay:case outputs_main_12_delay:
			case outputs_main_13_delay:case outputs_main_14_delay:case outputs_main_15_delay:case outputs_main_16_delay:
				sscanf(l_read + 14, "%d", &ch);
				sprintf(tmp, "/outputs/main/%02d/delay/on", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/main/%02d/delay/time", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, 0.3, 499.7, 0.1);
				SendDataToX32 // send to X32
				break;
			case outputs_aux_01:case outputs_aux_02:case outputs_aux_03: //  /outputs/aux/01
			case outputs_aux_04:case outputs_aux_05:case outputs_aux_06:
				sscanf(l_read + 13, "%d", &ch);
				sprintf(tmp, "/outputs/aux/%02d/src", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/aux/%02d/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xoutpos, Xoutpos_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/aux/%02d/invert", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case outputs_p16_01:case outputs_p16_02:case outputs_p16_03:case outputs_p16_04: //  /outputs/p16/01
			case outputs_p16_05:case outputs_p16_06:case outputs_p16_07:case outputs_p16_08:
			case outputs_p16_09:case outputs_p16_10:case outputs_p16_11:case outputs_p16_12:
			case outputs_p16_13:case outputs_p16_14:case outputs_p16_15:case outputs_p16_16:
				sscanf(l_read + 13, "%d", &ch);
				sprintf(tmp, "/outputs/p16/%02d/src", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/p16/%02d/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xoutpos, Xoutpos_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/p16/%02d/invert", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
// if (X32VER > 210) { // starting with FW 2.11 (covered by case satement)
			case outputs_p16_01_IQ:case outputs_p16_02_IQ:case outputs_p16_03_IQ:case outputs_p16_04_IQ: //  /outputs/p16/01/iQ
			case outputs_p16_05_IQ:case outputs_p16_06_IQ:case outputs_p16_07_IQ:case outputs_p16_08_IQ:
			case outputs_p16_09_IQ:case outputs_p16_10_IQ:case outputs_p16_11_IQ:case outputs_p16_12_IQ:
			case outputs_p16_13_IQ:case outputs_p16_14_IQ:case outputs_p16_15_IQ:case outputs_p16_16_IQ:
// parsing /outputs/p16/xx/iQ OFF none Linear 0
				sscanf(l_read + 13, "%d", &ch);
				sprintf(tmp, "/outputs/p16/%02d/iQ/group", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, XiQgroup, XiQgroup_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/p16/%02d/iQ/iQmodel", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, XiQtypes, XiQtypes_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/p16/%02d/iQ/eq", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, XiQeq, XiQeq_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/p16/%02d/iQ/model", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);		// typically a value between 0 to 7
				SendDataToX32 // send to X32
				break;
// } // starting with FW 2.11 (covered by case satement)
			case outputs_aes_01:case outputs_aes_02: // /outputs/aes/01
				sscanf(l_read + 13, "%d", &ch);
				sprintf(tmp, "/outputs/aes/%02d/src", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/aes/%02d/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xoutpos, Xoutpos_max);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/aes/%02d/invert", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			case outputs_rec_01:case outputs_rec_02: // /outputs/rec/01
				sscanf(l_read + 13, "%d", &ch);
				sprintf(tmp, "/outputs/rec/%02d/src", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_int(buf, k);
				SendDataToX32 // send to X32
				sprintf(tmp, "/outputs/rec/%02d/pos", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_list(buf, k, Xoutpos, Xoutpos_max);
				SendDataToX32 // send to X32
				break;
			case headamp_000:case headamp_001:case headamp_002:case headamp_003:
			case headamp_004:case headamp_005:case headamp_006:case headamp_007:
			case headamp_008:case headamp_009:case headamp_010:case headamp_011:
			case headamp_012:case headamp_013:case headamp_014:case headamp_015:
			case headamp_016:case headamp_017:case headamp_018:case headamp_019:
			case headamp_020:case headamp_021:case headamp_022:case headamp_023:
			case headamp_024:case headamp_025:case headamp_026:case headamp_027:
			case headamp_028:case headamp_029:case headamp_030:case headamp_031:
			case headamp_032:case headamp_033:case headamp_034:case headamp_035:
			case headamp_036:case headamp_037:case headamp_038:case headamp_039:
			case headamp_040:case headamp_041:case headamp_042:case headamp_043:
			case headamp_044:case headamp_045:case headamp_046:case headamp_047:
			case headamp_048:case headamp_049:case headamp_050:case headamp_051:
			case headamp_052:case headamp_053:case headamp_054:case headamp_055:
			case headamp_056:case headamp_057:case headamp_058:case headamp_059:
			case headamp_060:case headamp_061:case headamp_062:case headamp_063:
			case headamp_064:case headamp_065:case headamp_066:case headamp_067:
			case headamp_068:case headamp_069:case headamp_070:case headamp_071:
			case headamp_072:case headamp_073:case headamp_074:case headamp_075:
			case headamp_076:case headamp_077:case headamp_078:case headamp_079:
			case headamp_080:case headamp_081:case headamp_082:case headamp_083:
			case headamp_084:case headamp_085:case headamp_086:case headamp_087:
			case headamp_088:case headamp_089:case headamp_090:case headamp_091:
			case headamp_092:case headamp_093:case headamp_094:case headamp_095:
			case headamp_096:case headamp_097:case headamp_098:case headamp_099:
			case headamp_100:case headamp_101:case headamp_102:case headamp_103:
			case headamp_104:case headamp_105:case headamp_106:case headamp_107:
			case headamp_108:case headamp_109:case headamp_110:case headamp_111:
			case headamp_112:case headamp_113:case headamp_114:case headamp_115:
			case headamp_116:case headamp_117:case headamp_118:case headamp_119:
			case headamp_120:case headamp_121:case headamp_122:case headamp_123:
			case headamp_124:case headamp_125:case headamp_126:case headamp_127:
				sscanf(l_read +9, "%3d", &ch);
				sprintf(tmp, "/headamp/%03d/gain", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = Xp_linf(buf, k, -12., 72., 0.5);
				SendDataToX32 // send to X32
				sprintf(tmp, "/headamp/%03d/phantom", ch);
				k = Xsprint(buf, 0, 's', tmp);
				k = XOff_On(buf, k, "OFF");
				SendDataToX32 // send to X32
				break;
			default:
				break;
			} // all taken care of, Exit for loop!
			break;
		}
	}
	return(0);
}
/*
/config/routing/IN/1-8 AN1-8
/config/routing/IN/9-16 AN9-16
/config/routing/IN/17-24 AN17-24
/config/routing/IN/25-32 AN25-32
/config/routing/IN/AUX CARD1-2
/config/routing/AES50A/1-8 OUT1-8
/config/routing/AES50A/9-16 OUT9-16
/config/routing/AES50A/17-24 OUT1-8
/config/routing/AES50A/25-32 OUT9-16
/config/routing/AES50A/33-40 P161-8
/config/routing/AES50A/41-48 P169-16
/config/routing/AES50B/1-8 OUT9-16
/config/routing/AES50B/9-16 OUT1-8
/config/routing/AES50B/17-24 OUT9-16
/config/routing/AES50B/25-32 OUT1-8
/config/routing/AES50B/33-40 P169-16
/config/routing/AES50B/41-48 P161-8
/config/routing/CARD/1-8 AN1-8
/config/routing/CARD/9-16 AN9-16
/config/routing/CARD/17-24 AN17-24
/config/routing/CARD/25-32 AN25-32
/config/routing/OUT/1-4 OUT1-4
/config/routing/OUT/5-8 OUT5-8
/config/routing/OUT/9-12 OUT9-12
/config/routing/OUT/13-16 OUT13-16

/ch/01/mix/fader  -9.9
/ch/01/mix/pan +20
/ch/01/mix/on ON
/ch/01/mix/mono OFF
/ch/01/mix/mlevel -41.0

/auxin/01/mix/fader   -oo
/auxin/01/mix/pan +0
/auxin/01/mix/on ON
/auxin/01/mix/mono OFF
/auxin/01/mix/mlevel   -oo

/fxrtn/01/mix/fader  -0.0
/fxrtn/01/mix/pan -100
/fxrtn/01/mix/on ON
/fxrtn/01/mix/mono OFF
/fxrtn/01/mix/mlevel   -oo

/bus/01/mix/fader -48.3
/bus/01/mix/pan +8
/bus/01/mix/on ON
/bus/01/mix/mono ON
/bus/01/mix/mlevel -58.0

/mtx/01/mix/fader   -oo
/mtx/01/mix/on ON

/main/st/mix/fader   -oo
/main/st/mix/pan +0
/main/st/mix/on ON

/main/m/mix/fader   -oo
/main/m/mix/on ON

/dca/1/fader +10.0
/dca/1/on ON
*/
