/*
 * X32port.h
 *
 *  Created on: Sep 19, 2014
 *      Author: Patrick-Gilles Maillot
 */

#ifndef X32PORT_H_
#define X32PORT_H_

#define XDEBUG				0		// 1: be verbose
#define XPORT				10023	// hard-coded port number for X32
#define BUFSIZE				2048
#define XREMOTE_TIMEOUT		9
#define XREMOTE_TIMEOUT10	10

#define COMMA ','
#define SPACE ' '

#define MAXMESS				8
typedef enum {NOOP, INFO, STATUS, XREMOTE, SHOWDUMP, NODE, CH, FX} Xmes;

extern void Xdump(char *buf, int len, int debug);
extern void Xfdump(char *header, char *buf, int len, int debug);
extern int Xparse(char *buf);
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int Xfprint(char *bd, int index, char* text, char format, void *bs);
extern int Xcparse(char *buf, char *input_line);
// Functions used to parse scenes
extern int XOff_On(char *buf, int k, char* Crit_0);
extern float Xr_float();
extern int Xp_percent(char *buf, int k);
extern int Xp_linf(char *buf, int k, float xmin, float lmaxmin, float xstep);
extern int Xp_logf(char *buf, int k, float xmin, float lmaxmin, int n_steps);
extern int Xp_int(char *buf, int k);
extern int Xp_str(char *buf, int k);
extern int Xp_bit(char *buf, int k);
extern int Xp_list(char *buf, int k, char **list, int list_max);
extern int Xp_fxlist(char *buf, int k, char **list, int list_max, int *p_ival);
extern int Xp_frequency(char *buf, int k, int n_steps);
extern int Xp_level(char *buf, int k, int n_steps);




#endif /* X32port.h_ */
