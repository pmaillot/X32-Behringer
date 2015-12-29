/*
 * X32.c
 *
 *  Created on: 16 janv. 2015
 *      Author: patrick
 */
#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#else
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SOCKET_ERROR -1
#define WSAGetLastError() -1
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>



#define BSIZE				512			// Buffer sizes
#define X32DEBUG			0			// default debug mode
#define X32VERBOSE			1			// default verbose mode
#define VERB_REMOTE			0			// if verbose: default /xremote verbose
#define VERB_BATCH			0			// if verbose: default /batch... verbose
#define VERB_FORMAT			0			// if verbose: default /format... verbose
#define VERB_RENEW			0			// if verbose: default /renew verbose
#define VERB_METER			0			// if verbose: default /meters verbose
#define MAX_SIM_CLIENTS		4
#define XREMOTE_TIME		11

#define F_GET		0x0001				// Get parameter(s) command
#define F_SET		0x0002				// Set parameter(s) command
#define	F_NPR		0x0004				// Do not propagate to other clients
#define F_XET		F_GET | F_SET
#define F_FND		0x0008				// first of a series or node header

#define S_SND		0x0001				// Update current client needed
#define S_REM		0x0002				// Update remote clients needed

#define C_HA		0x0001				// copy mask : Head amps
#define C_CONFIG	0x0002				// copy mask : Config
#define C_GATE		0x0004				// copy mask : Gate
#define C_DYN		0x0008				// copy mask : Dynamics
#define C_EQ		0x0010				// copy mask : EQs
#define C_SEND		0x0020				// copy mask : Sends

enum		types {NIL, I32, F32, S32, B32, A32, RES1, RES2, RES3, RES4, FX32,
				OFFON, CMONO, CSOLO, CTALK, CTALKAB, COSC, CROUTIN, CROUTAC,
				CROUTOT, CCTRL, CENC, CTAPE, CHCO, CHDE, CHPR, CHGA, CHGF, CHDY,
				CHDF,CHIN, CHEQ, CHMX, CHMO, CHME, CHGRP, AXPR, BSCO, MXPR,
				MXDY, MSMX, FXTYP1, FXSRC, FXPAR1, FXTYP2, FXPAR2, OMAIN, OMAIND,
				HAMP,
				};


typedef struct X32header {			// The Header structure is used to quickly scan through
	union {							// blocks of commands (/config, /ch/xxx, etc) with
		char	ccom[4];			// a single test on the first four characters.
		int		icom;				//
	} header;						// A union enables doing this on a single int test
	int (*fptr)();					// The result points to a parsing function, via a
} X32header;						// dedicated function pointer call

typedef struct X32command {			// The Command structure describes ALL X32 commands
	char* command;					// - Their syntax (/ch/01/config/icon (for example))
	union {							// - Their format type (no data, int, float, string, blob, special type...)
		int		typ;				//     see enum types above
		char*	str;				//     address of a string if applicable
	} format;						//
	int flags;						// - a command flag: (simple command, get/set, node entry, etc.)
	union {							// - The value associated to the format type above
		int		ii;					//     can be int, float, string address or general data address
		float	ff;					//
		char*	str;				//
		void*	dta;				//
	} value;						//
} X32command;						//

typedef struct X32node {			// The Node header structure is used to directly
	char*		command;			// parse a node command on a limited number of characters and
	int			nchars;				// "jump" to the associated node entry function to manage
	X32command*	cmd_ptr;			// the "node ,s" appropriate answer
	int			cmd_max;			//
} X32node;							//



struct {							// The Client structure
	int					vlid;		// - Valid entry
	struct sockaddr		sock;		// - Client identification data (based on IP/Port)
	time_t				xrem;		// - /xremote time when initiated
} X32Client[MAX_SIM_CLIENTS];		//


//
// Public/External functions
extern void		Xfdump(char *header, char *buf, int len, int debug);
extern int		Xsprint(char *bd, int index, char format, void *bs);
extern int 		Xfprint(char *bd, int index, char* text, char format, void *bs);

//
// Private functions
void	getmyIP();
void 	Xsend(int who_to);
void	X32Print(struct X32command* command);
int		X32Shutdown();
int		X32Init();
int		FXc_lookup(int i);
int 	funct_params(X32command* command, int i);
int 	function_shutdown();
int 	function_info();
int 	function_status();
int 	function_xremote();
int 	function_node();
int 	function_config();
int 	function_main();
int 	function_prefs();
int 	function_stat();
int 	function_channel();
int 	function_auxin();
int 	function_fxrtn();
int 	function_bus();
int 	function_mtx();
int 	function_dca();
int 	function_fx();
int 	function_output();
int 	function_headamp();
int 	function_misc();
int 	function_show();
int 	function_copy();
int 	function_add();
int 	function_save();
int 	function_delete();
int		function_unsubscribe();
int 	function_misc();
int 	function();


char	snode_str[32];				// used to temporarely save a string in node and FX commands

#include "X32Channel.h"				//
#include "X32CfgMain.h"				//
#include "X32PrefStat.h"			// These are the actual files describing
#include "X32Auxin.h"				// X32 Commands by blocks of the same
#include "X32Fxrtn.h"				// type of commands... More than 10k commands!
#include "X32Bus.h"					//
#include "X32Mtx.h"					//
#include "X32Dca.h"					//
#include "X32Fx.h"					//
#include "X32Output.h"				//
#include "X32Headamp.h"				//
#include "X32Show.h"				//
#include "X32Misc.h"				//

X32header Xheader[] = {						// X32 Headers, the data used for testing and the
		{{"/shu"},	&function_shutdown},	// associated function call
		{{"/inf"},	&function_info},
		{{"/sta"},	&function_status},
		{{"/xre"},	&function_xremote},
		{{"/nod"},	&function_node},
		{{"/con"},	&function_config},
		{{"/mai"},	&function_main},
		{{"/-pr"},	&function_prefs},
		{{"/-st"},	&function_stat},
		{{"/ch/"},	&function_channel},
		{{"/aux"},	&function_auxin},
		{{"/fxr"},	&function_fxrtn},
		{{"/bus"},	&function_bus},
		{{"/mtx"},	&function_mtx},
		{{"/dca"},	&function_dca},
		{{"/fx/"},	&function_fx},
		{{"/out"},	&function_output},
		{{"/hea"},	&function_headamp},
		{{"/-ha"},	&function_misc},
		{{"/ins"},	&function_misc},
		{{"/-sh"},	&function_show},
		{{"/ren"},	&function_misc},
		{{"/cop"},	&function_copy},
		{{"/add"},	&function_add},
		{{"/sav"},	&function_save},
		{{"/del"},	&function_delete},
		{{"/uns"},	&function_unsubscribe},

		{{"/-us"},	&function_misc},
		{{"/und"},	&function},

		{{"/-ac"},	&function},
};
int Xheader_max = sizeof(Xheader) / sizeof(X32header);

X32command Xdummy[] = {};
int 	Xdummy_max = sizeof(Xdummy) / sizeof(X32command);

X32node Xnode[] = {			// /node Command Headers (see structure definition above
		{"conf", 4, Xconfig, sizeof(Xconfig)	/ sizeof(X32command)},
		{"main", 4, Xmain, 	sizeof(Xmain)	/ sizeof(X32command)},
		{"-pre", 4, Xdummy,	sizeof(Xdummy)	/ sizeof(X32command)},
		{"-sta", 4, Xdummy,	sizeof(Xdummy)	/ sizeof(X32command)},
		{"ch/01", 5, Xchannel01, sizeof(Xchannel01)	/ sizeof(X32command)},
		{"ch/02", 5, Xchannel02, sizeof(Xchannel02)	/ sizeof(X32command)},
		{"ch/03", 5, Xchannel03, sizeof(Xchannel03)	/ sizeof(X32command)},
		{"ch/04", 5, Xchannel04, sizeof(Xchannel04)	/ sizeof(X32command)},
		{"ch/05", 5, Xchannel05, sizeof(Xchannel05)	/ sizeof(X32command)},
		{"ch/06", 5, Xchannel06, sizeof(Xchannel06)	/ sizeof(X32command)},
		{"ch/07", 5, Xchannel07, sizeof(Xchannel07)	/ sizeof(X32command)},
		{"ch/08", 5, Xchannel08, sizeof(Xchannel08)	/ sizeof(X32command)},
		{"ch/09", 5, Xchannel09, sizeof(Xchannel09)	/ sizeof(X32command)},
		{"ch/10", 5, Xchannel10, sizeof(Xchannel10)	/ sizeof(X32command)},
		{"ch/11", 5, Xchannel11, sizeof(Xchannel11)	/ sizeof(X32command)},
		{"ch/12", 5, Xchannel12, sizeof(Xchannel12)	/ sizeof(X32command)},
		{"ch/13", 5, Xchannel13, sizeof(Xchannel13)	/ sizeof(X32command)},
		{"ch/14", 5, Xchannel14, sizeof(Xchannel14)	/ sizeof(X32command)},
		{"ch/15", 5, Xchannel15, sizeof(Xchannel15)	/ sizeof(X32command)},
		{"ch/16", 5, Xchannel16, sizeof(Xchannel16)	/ sizeof(X32command)},
		{"ch/17", 5, Xchannel17, sizeof(Xchannel17)	/ sizeof(X32command)},
		{"ch/18", 5, Xchannel18, sizeof(Xchannel18)	/ sizeof(X32command)},
		{"ch/19", 5, Xchannel19, sizeof(Xchannel19)	/ sizeof(X32command)},
		{"ch/20", 5, Xchannel20, sizeof(Xchannel20)	/ sizeof(X32command)},
		{"ch/21", 5, Xchannel21, sizeof(Xchannel21)	/ sizeof(X32command)},
		{"ch/22", 5, Xchannel22, sizeof(Xchannel22)	/ sizeof(X32command)},
		{"ch/23", 5, Xchannel23, sizeof(Xchannel23)	/ sizeof(X32command)},
		{"ch/24", 5, Xchannel24, sizeof(Xchannel24)	/ sizeof(X32command)},
		{"ch/25", 5, Xchannel25, sizeof(Xchannel25)	/ sizeof(X32command)},
		{"ch/26", 5, Xchannel26, sizeof(Xchannel26)	/ sizeof(X32command)},
		{"ch/27", 5, Xchannel27, sizeof(Xchannel27)	/ sizeof(X32command)},
		{"ch/28", 5, Xchannel28, sizeof(Xchannel28)	/ sizeof(X32command)},
		{"ch/29", 5, Xchannel29, sizeof(Xchannel29)	/ sizeof(X32command)},
		{"ch/30", 5, Xchannel30, sizeof(Xchannel30)	/ sizeof(X32command)},
		{"ch/31", 5, Xchannel31, sizeof(Xchannel31)	/ sizeof(X32command)},
		{"ch/32", 5, Xchannel32, sizeof(Xchannel32)	/ sizeof(X32command)},
		{"auxin/01", 8, Xauxin01, sizeof(Xauxin01)	/ sizeof(X32command)},
		{"auxin/02", 8, Xauxin02, sizeof(Xauxin02)	/ sizeof(X32command)},
		{"auxin/03", 8, Xauxin03, sizeof(Xauxin03)	/ sizeof(X32command)},
		{"auxin/04", 8, Xauxin04, sizeof(Xauxin04)	/ sizeof(X32command)},
		{"auxin/05", 8, Xauxin05, sizeof(Xauxin05)	/ sizeof(X32command)},
		{"auxin/06", 8, Xauxin06, sizeof(Xauxin06)	/ sizeof(X32command)},
		{"auxin/07", 8, Xauxin07, sizeof(Xauxin07)	/ sizeof(X32command)},
		{"auxin/08", 8, Xauxin08, sizeof(Xauxin08)	/ sizeof(X32command)},
		{"fxrtn/01", 8, Xfxrtn01, sizeof(Xfxrtn01)	/ sizeof(X32command)},
		{"fxrtn/02", 8, Xfxrtn02, sizeof(Xfxrtn02)	/ sizeof(X32command)},
		{"fxrtn/03", 8, Xfxrtn03, sizeof(Xfxrtn03)	/ sizeof(X32command)},
		{"fxrtn/04", 8, Xfxrtn04, sizeof(Xfxrtn04)	/ sizeof(X32command)},
		{"fxrtn/05", 8, Xfxrtn05, sizeof(Xfxrtn05)	/ sizeof(X32command)},
		{"fxrtn/06", 8, Xfxrtn06, sizeof(Xfxrtn06)	/ sizeof(X32command)},
		{"fxrtn/07", 8, Xfxrtn07, sizeof(Xfxrtn07)	/ sizeof(X32command)},
		{"fxrtn/08", 8, Xfxrtn08, sizeof(Xfxrtn08)	/ sizeof(X32command)},
		{"bus/01", 6, Xbus01, sizeof(Xbus01)	/ sizeof(X32command)},
		{"bus/02", 6, Xbus02, sizeof(Xbus02)	/ sizeof(X32command)},
		{"bus/03", 6, Xbus03, sizeof(Xbus03)	/ sizeof(X32command)},
		{"bus/04", 6, Xbus04, sizeof(Xbus04)	/ sizeof(X32command)},
		{"bus/05", 6, Xbus05, sizeof(Xbus05)	/ sizeof(X32command)},
		{"bus/06", 6, Xbus06, sizeof(Xbus06)	/ sizeof(X32command)},
		{"bus/07", 6, Xbus07, sizeof(Xbus07)	/ sizeof(X32command)},
		{"bus/08", 6, Xbus08, sizeof(Xbus08)	/ sizeof(X32command)},
		{"bus/09", 6, Xbus09, sizeof(Xbus09)	/ sizeof(X32command)},
		{"bus/10", 6, Xbus10, sizeof(Xbus10)	/ sizeof(X32command)},
		{"bus/11", 6, Xbus11, sizeof(Xbus11)	/ sizeof(X32command)},
		{"bus/12", 6, Xbus12, sizeof(Xbus12)	/ sizeof(X32command)},
		{"bus/13", 6, Xbus13, sizeof(Xbus13)	/ sizeof(X32command)},
		{"bus/14", 6, Xbus14, sizeof(Xbus14)	/ sizeof(X32command)},
		{"bus/15", 6, Xbus15, sizeof(Xbus15)	/ sizeof(X32command)},
		{"bus/16", 6, Xbus16, sizeof(Xbus16)	/ sizeof(X32command)},
		{"mtx/01", 6, Xmtx01, sizeof(Xmtx01)	/ sizeof(X32command)},
		{"mtx/02", 6, Xmtx02, sizeof(Xmtx02)	/ sizeof(X32command)},
		{"mtx/03", 6, Xmtx03, sizeof(Xmtx03)	/ sizeof(X32command)},
		{"mtx/04", 6, Xmtx04, sizeof(Xmtx04)	/ sizeof(X32command)},
		{"mtx/05", 6, Xmtx05, sizeof(Xmtx05)	/ sizeof(X32command)},
		{"mtx/06", 6, Xmtx06, sizeof(Xmtx06)	/ sizeof(X32command)},
		{"dca/", 4, Xdca, sizeof(Xdca)		/ sizeof(X32command)},
		{"fx/",  3, Xfx, sizeof(Xfx)			/ sizeof(X32command)},
		{"outp", 4, Xoutput, sizeof(Xoutput)	/ sizeof(X32command)},
		{"head", 4, Xheadamp, sizeof(Xheadamp)/ sizeof(X32command)},
		{"-ha/", 4, Xmisc, sizeof(Xmisc)		/ sizeof(X32command)},
		{"inse", 4, Xmisc, sizeof(Xmisc)		/ sizeof(X32command)},
		{"-sho", 4, Xmisc, sizeof(Xmisc)		/ sizeof(X32command)},
		{"rene", 4, Xmisc, sizeof(Xmisc)		/ sizeof(X32command)},
		{"-usb", 4, Xdummy, sizeof(Xdummy)	/ sizeof(X32command)},

		{"/und", 4, Xdummy, sizeof(Xdummy)	/ sizeof(X32command)},
		{"/sav", 4, Xdummy, sizeof(Xdummy)	/ sizeof(X32command)},
		{"/cop", 4, Xdummy, sizeof(Xdummy)	/ sizeof(X32command)},
		{"/del", 4, Xdummy, sizeof(Xdummy)	/ sizeof(X32command)},
		{"/-ac", 4, Xdummy, sizeof(Xdummy)	/ sizeof(X32command)},
};
int Xnode_max = sizeof(Xnode) / sizeof(X32node);

union littlebig {				//
	int		ii;					// A small union to manage
	float	ff;					// Endian type conversions
	char	cc[4];				//
} endian;						//

char 	r_buf[BSIZE];			// Receiving buffer
int		r_len;					// receiving buffer number of bytes
void*	v_buf = &(r_buf[0]);	// void ptr to avoid gcc complaining about non-strict aliasing
char	s_buf[BSIZE];			// Send buffer
int		s_len;					// send buffer number of bytes
int		Xverbose			= X32VERBOSE;
int		Xverb_remote		= VERB_REMOTE;
int		Xverb_batch			= VERB_BATCH;
int		Xverb_format		= VERB_FORMAT;
int		Xverb_renew			= VERB_RENEW;
int		Xverb_meter			= VERB_METER;
int		Xdebug				= X32DEBUG;
int		zero				= 0;
int		one					= 1;
int		keep_on = 1;			// main loop flag
time_t	xremote_time;			// time value for /xremote command
int		Xxremote_on = 0;
char	tmp_str[48];


int 	Xfd, p_status;
struct	sockaddr_in Client_ip, Server_ip;
struct	sockaddr *Client_ip_pt = (struct sockaddr*) &Client_ip;
struct	sockaddr *Server_ip_pt = (struct sockaddr*) &Server_ip;
char	Xport_str[8];

fd_set 				readfds;
struct timeval		timeout;

#ifdef __WIN32__
WSADATA 			wsa;
int					Client_ip_len = sizeof(Client_ip);	// length of addresses
#else
socklen_t			Client_ip_len = sizeof(Client_ip);	// length of addresses
#endif



int
main(int argc, char **argv)
{
	int		i;
	char	input_ch;
// Manage arguments
	while ((input_ch = getopt(argc, argv, "i:d:v:x:b:f:r:m:h")) != -1) {
		switch (input_ch) {
		case 'd':
			sscanf(optarg, "%d", &Xdebug);
			break;
		case 'v':
			sscanf(optarg, "%d", &Xverbose);
			break;
		case 'x':
			sscanf(optarg, "%d", &Xverb_remote);
			break;
		case 'b':
			sscanf(optarg, "%d", &Xverb_batch);
			break;
		case 'f':
			sscanf(optarg, "%d", &Xverb_format);
			break;
		case 'r':
			sscanf(optarg, "%d", &Xverb_renew);
			break;
		case 'm':
			sscanf(optarg, "%d", &Xverb_meter);
			break;
		default:
		case 'h':
			printf("usage: X32 [-d 0/1, debug option] -default: 0\n");
			printf("           [-v 0/1, verbose option] -default: 1\n");
			printf("       The options below apply in conjunction with -v 1\n");
			printf("           [-x 0/1, echoes incoming verbose for /xremote] -default: 0\n");
			printf("           [-b 0/1, echoes incoming verbose for /batchsubscribe] -default: 0\n");
			printf("           [-f 0/1, echoes incoming verbose for /formatsubscribe] -default: 0\n");
			printf("           [-r 0/1, echoes incoming verbose for /renew] -default: 0\n");
			printf("           [-m 0/1, echoes incoming verbose for /meters] -default: 0\n\n");
			printf("   The (non-Behringer) command \"/shutdown\" will save data and quit\n");
			return(0);
			break;
		}
	}
// Initiate timers
	xremote_time = time(NULL);
//	port[] = "10023" 			// 10023: X32 desk, 10024: XAir18
	strcpy (Xport_str, "10023");
//
	for (i = 0; i < MAX_SIM_CLIENTS; i++) {
		X32Client[i].vlid = 0;
		X32Client[i].xrem = 0;
	}
// read initial data (if  exists)
	i = X32Init();
//
#ifdef __WIN32__
//Initialize winsock
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
		perror("WSA Startup Error");
		exit(EXIT_FAILURE);
	}
#endif
// Create a UDP socket
	if ((Xfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("X32 socket creation error");
        exit(EXIT_FAILURE);
	}
// Load up address structures:
	memset(&Client_ip, 0, sizeof(Client_ip));		// Clear struct
	memset(&Server_ip, 0, sizeof(Server_ip));		// Clear struct
	Server_ip.sin_family = AF_INET;					// IP4
	Server_ip.sin_addr.s_addr = INADDR_ANY;			// any address in
	Server_ip.sin_port = htons(atoi(Xport_str));	// server port
//
// Bind IP & port information
    if( bind(Xfd ,Server_ip_pt , sizeof(Server_ip)) == SOCKET_ERROR)
    {
    	perror("Bind failed");
        exit(EXIT_FAILURE);
    }
//
// make socket non blocking
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000; //Set timeout for non blocking recvfrom(): 10ms
//
// Wait for messages from client
	i = 0;
	r_len = 0;
	printf("X32 - v0.26 - An X32 Emulator - (c)2014-2015 Patrick-Gilles Maillot\n");
	getmyIP(); // Try to get our IP...
	if (Xverbose) printf("Listening to port %s, X32 IP = %s\n", Xport_str, r_buf);
	while(keep_on) {			// Main, receiving loop (active as long as keep_on is 1)
		FD_ZERO (&readfds);
		FD_SET (Xfd, &readfds);
		p_status = select(Xfd+1,&readfds,NULL,NULL,&timeout);
		if ((p_status = FD_ISSET(Xfd, &readfds)) < 0) {
			printf("Error while receiving\n");
			exit(1);
		}
		if (p_status > 0) {
		// We have received data - process it!
		// and if appropriate, reply to the address received into Client addr (ip_pt)
			r_len = recvfrom(Xfd, r_buf, BSIZE, 0, Client_ip_pt, &Client_ip_len);
			r_buf[r_len] = 0;
			if (Xverbose) {
				if (strncmp(r_buf, "/xre", 4) == 0) {
					if (Xverb_remote) {
						Xfdump("->X", r_buf, r_len, Xdebug);
						fflush(stdout);
					}
				} else if (strncmp(r_buf, "/bat", 4) == 0) {
					if (Xverb_batch) {
						Xfdump("->X", r_buf, r_len, Xdebug);
						fflush(stdout);
					}
				} else if (strncmp(r_buf, "/for", 4) == 0) {
					if (Xverb_format) {
						Xfdump("->X", r_buf, r_len, Xdebug);
						fflush(stdout);
					}
				} else if (strncmp(r_buf, "/ren", 4) == 0) {
					if (Xverb_renew) {
						Xfdump("->X", r_buf, r_len, Xdebug);
						fflush(stdout);
					}
				} else if (strncmp(r_buf, "/met", 4) == 0) {
					if (Xverb_meter) {
						Xfdump("->X", r_buf, r_len, Xdebug);
						fflush(stdout);
					}
				} else {
					Xfdump("->X", r_buf, r_len, Xdebug);
					fflush(stdout);
				}
			}
			// We have data coming in - Parse!
			i = s_len = p_status = 0;
			// Parse the command; this will update the Send buffer (and send buffer number of bytes)
			// and the parsing status in p_status
			while (i < Xheader_max) {
				if (Xheader[i].header.icom == (int)*((int*)v_buf)) {	// single int test!
					p_status = Xheader[i].fptr();					// call associated parsing function
					break;									// Done parsing, exit parsing while loop
				}
				i += 1;
			}
			// Done receiving/managing command parameters;
			// Update current client with data to be sent?
			Xsend(p_status);
		}
	}
	return 0;
}



#ifdef __WIN32__
void getmyIP() {
	char **pp = NULL;
	struct hostent *host = NULL;

	if(!gethostname(r_buf, 256) && (host = gethostbyname(r_buf)) != NULL) {
		for(pp = host->h_addr_list ; *pp != NULL ; pp++) {
			strcpy (r_buf, (inet_ntoa(*(struct in_addr *)*pp))); // copy IP (string) address to r_buf
			return;
		}
	}
	return;
}
#else
void getmyIP() {

    struct ifaddrs *ifaddr, *ifa;
    int s;

	r_buf[0] = 0;

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        return;
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr != NULL) {
        	if ((s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in), r_buf, NI_MAXHOST, NULL, 0, NI_NUMERICHOST)) == 0) {
// you typically have to replace "en0" by "wlan0", "eth0",... depending on your physical interface support
        		if ((strcmp(ifa->ifa_name,"en0") == 0) && (ifa->ifa_addr->sa_family == AF_INET))
        		{
//					printf("\tInterface : <%s>\n",ifa->ifa_name );
//					printf("\t  Address : <%s>\n", r_buf);
					break;
				}
        	} else {
                perror("getnameinfo");
        		break;
        	}
        }
    }
    freeifaddrs(ifaddr);
	return;
}
#endif



void
X32Print(struct X32command* command) {
	printf ("X32-Command: %s data: ", command->command);
	if (command->format.typ == 1) {
		printf ("[%6d]\n", command->value.ii);
	} else if (command->format.typ == 2) {
		if (command->value.ff < 10.) printf ("[%6.4f]\n", command->value.ff);
		else if (command->value.ff < 100.) printf ("[%6.3f]\n", command->value.ff);
		else if (command->value.ff < 1000.) printf ("[%6.2f]\n", command->value.ff);
		else if (command->value.ff < 10000.) printf ("[%6.1f]\n", command->value.ff);
		else printf ("[%6f]\n", command->value.ff);
	} else if (command->format.typ == 3) {
		printf ("%s\n", command->value.str);
	} else if (command->format.typ == 4) {
		printf ("blob at address %08x\n", (unsigned int)command->value.ii);		// casts .dta to int
	} else {
		printf ("composed type at address %08x\n", (unsigned int)command->format.typ);	// casts .str to int
	}
}


void Xsend(int who_to) {
	int j;

	if ((who_to & S_SND) && s_len) {
		if (Xverbose) {Xfdump("X->", s_buf, s_len, Xdebug); fflush(stdout);}
		if (sendto(Xfd, s_buf, s_len, 0, Client_ip_pt, Client_ip_len) < 0) {
			perror("Error while sending data");
			return;
		}
	}
	// Other clients to update based on their xremote status?
	if ((who_to & S_REM) && s_len) {
		xremote_time = time(NULL);
		for (j = 0; j < MAX_SIM_CLIENTS; j++) {
			if ((X32Client[j].vlid) && (strcmp(X32Client[j].sock.sa_data, Client_ip_pt->sa_data) != 0)) {
				if (X32Client[j].xrem > xremote_time) {
					if (Xverbose) {Xfdump("X->", s_buf, s_len, Xdebug); fflush(stdout);}
					if (sendto(Xfd, s_buf, s_len, 0, &(X32Client[j].sock), Client_ip_len) < 0) {
						perror("Error while sending data");
						return;
					}
				}
			}
		}
	}
}





int
FXc_lookup(int index) {
	int	ipar, ityp;
	char	ctyp;
// lookup function to find the parameter type of an FX parameter for command at index
// the function returns I32, F32, S32, B32, or A32, and NIL if an error is detected
//
// Command at index is like: /fx/<n>/par/<pp>
// we get the FX type at index -<pp> - 5
	ipar = (Xfx[index].command[10] - 48) * 10 + Xfx[index].command[11] - 48 - 1;
	if ((ipar < 0) || (ipar > 63)) return NIL;
	ityp = Xfx[index - ipar - 5].value.ii;
// 2 cases to consider depending on <n>
	if (Xfx[index].command[4] < 53) {
		ctyp = *(Sflookup[ityp] + ipar);	// /fx/1/... to /fx/4/
	} else {
		ctyp = *(Sflookup2[ityp] + ipar);	// /fx/5/... to /fx/8/
	}
	switch (ctyp) {
	case 'i':
		return I32;
		break;
	case 'f':
		return F32;
		break;
	case 's':
		return S32;
		break;
	default:
		return F32;
		break;
	}
	return NIL;
}

char*
Slevel(float fin) {

	if (fin == 0.)  		sprintf(snode_str, " -oo");
	else if (fin <= 0.0625)	sprintf(snode_str, " %+.1f", 30. / 0.0625 * fin - 90.);
	else if (fin <= 0.25)	sprintf(snode_str, " %+.1f", 30. / (0.25 - 0.0625) * (fin - 0.0625) - 60.);
	else if (fin < 0.5)		sprintf(snode_str, " %+.1f", 20. / (0.5 - 0.25) * (fin - 0.25) - 30.);
	else					sprintf(snode_str, " %+.1f", 20. / (1. - 0.5) * (fin - 0.5) - 10.);
	return snode_str;
}

char*
Slinf(float fin, float fmin, float fmax, int pre) {
	float fl;

	fl = fmin + (fmax - fmin) * fin;
	if (pre == 0)		sprintf(snode_str, " %.0f", fl);
	else if (pre == 1)	sprintf(snode_str, " %.1f", fl);
	else if (pre == 2)	sprintf(snode_str, " %.2f", fl);
	else if (pre == 3)	sprintf(snode_str, " %.3f", fl);
	return snode_str;
}

char*
Slinfs(float fin, float fmin, float fmax, int pre) {
	float fl;

	fl = fmin + (fmax - fmin) * fin;
	if (pre == 0)		sprintf(snode_str, " %+.0f", fl);
	else if (pre == 1)	sprintf(snode_str, " %+.1f", fl);
	else if (pre == 2)	sprintf(snode_str, " %+.2f", fl);
	else if (pre == 3)	sprintf(snode_str, " %+.3f", fl);
	return snode_str;
}

char*
Slogf(float fin, float fmin, float fmax, int pre) {
	float fl;
	fl = exp(fin * log(fmax/fmin) + log(fmin));
	if (pre == 0)		sprintf(snode_str, " %.0f", fl);
	else if (pre == 1)	sprintf(snode_str, " %.1f", fl);
	else if (pre == 2)	sprintf(snode_str, " %.2f", fl);
	else if (pre == 3)	sprintf(snode_str, " %.3f", fl);
	return snode_str;
}


char*
Sbitmp(int iin, int len) {

	int i, j;

	j = 0;
	snode_str[j++] = ' ';
	snode_str[j++] = '%';
	for (i = len - 1;  i > -1; i--) {
		snode_str[j++] = ((iin & (1 << i))? '1': '0');
	}
	snode_str[j] = 0;
	return snode_str;
}

char*
Sint(int iin) {
	sprintf(snode_str, " %d", iin);
	return snode_str;
}

void GetFxPar1(char* buf, int ipar, int type) {
	int i;

	switch (type) {
	case HALL:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.2, 5., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 2., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 250., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case AMBI:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.2, 7.3, 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case RPLT:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.3, 29., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 4., 39., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.25, 4., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 1200., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 1200., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		break;
	case ROOM:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.3, 29., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 4., 72., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.25, 4., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 250., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 1200., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 1200., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		break;
	case CHAM:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.3, 29., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 4., 72., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.25, 4., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 250., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 500., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 500., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case PLAT:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.2, 10., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 2., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case VREV:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 120., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.3, 4.5, 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " REAR": " FRONT");
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 2., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.25, 1., 0));
		break;
	case VRM:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 20., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.1, 20., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 190., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.1, 10., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.1, 10., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 00., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case GATE:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 140, 1000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -30., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		break;
	case RVRS:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 140, 1000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -30., 0., 0));
		break;
	case DLY:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 3000., 0));
		strcat(buf, Smode[Xfx[ipar++].value.ii]);
		strcat(buf, Sfactor[Xfx[ipar++].value.ii]);
		strcat(buf, Sfactor[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., +100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200, 20000., 1));
		break;
	case _3TAP:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 3000., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Sfactor[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Sfactor[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case _4TAP:
		strcat(buf, Slinf(Xfx[ipar].value.ff, 0., 3000., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 6., 0));
		strcat(buf, Sfactor[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Sfactor[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Sfactor[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case CRS:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 5., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case FLNG:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 5., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -90., 90., 0));
		break;
	case PHAS: case _5_PHAS:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 5., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 80., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 2., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 1000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1., 2000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 1000., 1));
		break;
	case DIMC:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ST": " M");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case FILT: case _5_FILT:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 20., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 15000., 1));
		strcat(buf, Sfmode[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Sfwave[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 250., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case ROTA:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.1, 4., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 2., 10., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case PAN: case _5_PAN:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 1000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1., 2000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 1000., 1));
		break;
	case SUB: case _5_SUB:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Sfrange[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Sfrange[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case D_RV:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Sfpattern[Xfx[ipar++].value.ii]);
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.1, 5., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000, 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case CR_R:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 50., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.1, 5., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case FL_R:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -90., 90., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.1, 5., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case D_CR:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Sfpattern[Xfx[ipar++].value.ii]);
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 50., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case D_FL:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Sfpattern[Xfx[ipar++].value.ii]);
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -90., 90., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case MODD:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Sfddelay[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 10., 1));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " SER": " PAR");
		strcat(buf, Sfdtype[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 10., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case GEQ2: case TEQ2: case _5_GEQ2: case _5_TEQ2:
		for (i = 0; i < 64; i++) {
			strcat(buf, Slinf(Xfx[ipar++].value.ff, -15., 15., 0));
		}
		break;
	case GEQ: case TEQ: case _5_GEQ: case _5_TEQ:
		for (i = 0; i < 32; i++) {
			strcat(buf, Slinf(Xfx[ipar++].value.ff, -15., 15., 0));
		}
		break;
	case DES2: case _5_DES2:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " MALE": " FEM");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " MALE": " FEM");
		break;
	case DES: case _5_DES:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " MALE": " FEM");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " M/S": " ST");
		break;
	case P1A: case _5_P1A:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfplfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfpmfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfphfreq[Xfx[ipar++].value.ii]);
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case P1A2: case _5_P1A2:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfplfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfpmfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfphfreq[Xfx[ipar++].value.ii]);
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfplfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfpmfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfphfreq[Xfx[ipar++].value.ii]);
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case PQ5: case _5_PQ5:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Sfqlfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqmfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqhfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case PQ5S: case _5_PQ5S:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Sfqlfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqmfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqhfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Sfqlfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqmfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqhfreq[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case WAVD: case _5_WAVD:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -24., 24., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -24., 24., 0));
		break;
	case LIM: case _5_LIM:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -18., 18., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 0.05, 1., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 20., 2000., 1));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case CMB:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Sflcmb[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 19., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 20., 3000., 1));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 48": " 12");
		strcat(buf, Sfrcmb[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -40., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Sfmcmb[Xfx[ipar++].value.ii]);
		break;
	case CMB2:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Sflcmb[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 19., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 20., 3000., 1));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 48": " 12");
		strcat(buf, Sfrcmb[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -40., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Sfmcmb[Xfx[ipar++].value.ii]);
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Sflcmb[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 19., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 20., 3000., 1));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 48": " 12");
		strcat(buf, Sfrcmb[Xfx[ipar++].value.ii]);
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -40., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " 1": " 0");
		strcat(buf, Sfmcmb[Xfx[ipar++].value.ii]);
		break;
	case FAC: case _5_FAC:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -20., 20., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 6., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -18., 6., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		break;
	case FAC1M: case FAC2: case _5_FAC1M: case _5_FAC2:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -20., 20., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 6., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -18., 6., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -20., 20., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 6., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -18., 6., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		break;
	case LEC: case _5_LEC:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " LIM": " COMP");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -18., 6., 0));
		break;
	case LEC2: case _5_LEC2:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " LIM": " COMP");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -18., 6., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " LIM": " COMP");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -18., 6., 0));
		break;
	case ULC: case _5_ULC:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Sfrulc[Xfx[ipar++].value.ii]);
		break;
	case ULC2: case _5_ULC2:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Sfrulc[Xfx[ipar++].value.ii]);
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Sfrulc[Xfx[ipar++].value.ii]);
		break;
	case ENH2: case _5_ENH2:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case ENH: case _5_ENH:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 1., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case EXC2: case _5_EXC2:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 10000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 10000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case EXC: case _5_EXC:
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 10000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case IMG: case _5_IMG:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 100., 1000., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1., 10., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		break;
	case EDI: case _5_EDI:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " M/S": " ST");
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " M/S": " ST");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		break;
	case SON: case _5_SON:
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		break;
	case AMP2: case _5_AMP2:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case AMP: case _5_AMP:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, Xfx[ipar++].value.ii? " ON": " OFF");
		break;
	case DRV2: case _5_DRV2:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 20., 200., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 4000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 50., 400., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 10000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 20., 200., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 4000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 50., 400., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 10000., 1));
		break;
	case DRV: case _5_DRV:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 20., 200., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 4000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 50., 400., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1000., 10000., 1));
		break;
	case PIT2:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1., 100., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 2000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1., 100., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 2000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	case PIT:
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 1., 100., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(Xfx[ipar++].value.ff, 2000., 20000., 1));
		strcat(buf, Slinf(Xfx[ipar++].value.ff, 0., 100., 0));
		break;
	default:
		break;
	}
	return;
}


int funct_params(X32command *command, int i) {

int		j, c_len, f_len, f_num, c_type;
char*	s_adr;
char*	s_fmt;


	f_len = f_num = c_type = 0;
	c_len = strlen(command[i].command);
	if (r_len - 4 > c_len) {
		// First of a list command gives the first of next data
		if (command[i].flags & F_FND) ++i;
		// command has parameter(s) (set)
		if (command[i].flags & F_SET) {
			f_len = c_len = (((c_len + 4) & ~3) + 1);	// now pointing at first format char after ','
			f_num = 0;
			while (r_buf[c_len++]) ++f_num;	// count number of format characters
			c_len = (c_len + 4) & ~3;	// now pointing at first value
			while (f_num--) {
				switch (r_buf[f_len++]) {
				case 'i':
					j = 4;
					while (j) endian.cc[--j] = r_buf[c_len++];
					//save value to respective field - index i
					if (command[i].flags & F_SET) command[i].value.ii = endian.ii;
					break;
				case 'f':
					j = 4;
					while (j) endian.cc[--j] = r_buf[c_len++];
					//save value to respective field - index i
					if (command[i].flags & F_SET) command[i].value.ff = endian.ff;
					break;
				case 's':
					j = strlen(r_buf + c_len); // actual need can be up to 4 more \0 bytes; add 8 by security
					if (command[i].flags & F_SET) {
						if (command[i].value.str) free(command[i].value.str);
						if (j > 0) {
							command[i].value.str = malloc((j + 8) * sizeof(char));
							strncpy(command[i].value.str, r_buf + c_len, j);
							command[i].value.str[j] = 0;
						} else {
							command[i].value.str = NULL;
						}
					}
					c_len = ((c_len + j + 4) & ~3);
					break;
				case 'b':
					printf ("parameter: blob\n");
					break;
				}
				i += 1;
			}
			memcpy(s_buf, r_buf, r_len);
			s_len = r_len;
			// no need for sending to local client
			// authorize remote clients to receive info
			p_status = S_REM;
		} // done parsing
	} else {
		// First of a list command gives the first of next data
		if (command[i].flags & F_FND) ++i;
		if (command[i].flags & F_GET) { // if the command is part of the GET family
		// command without parameters: (get)
			s_len = Xsprint(s_buf, 0, 's', command[i].command);
			c_type = command[i].format.typ;
			if (c_type == FX32) {
			// special case of FX parameters. Need to validate if the requested parameter
			// is an int of a float. Decision based on a lookup table. Once found,
			// we process normally
				c_type = FXc_lookup(i); // the function returns I32, F32, S32,...
			}
			if (c_type == I32) {
				s_len = Xsprint(s_buf, s_len, 's', ",i");
				s_len = Xsprint(s_buf, s_len, 'i', &command[i].value.ii);
			} else if (c_type == F32) {
				s_len = Xsprint(s_buf, s_len, 's', ",f");
				s_len = Xsprint(s_buf, s_len, 'i', &command[i].value.ff);
			} else if (c_type == S32) {
				s_len = Xsprint(s_buf, s_len, 's', ",s");
				if (command[i].value.str) s_len = Xsprint(s_buf, s_len, 's', command[i].value.str);
				else s_len = Xsprint(s_buf, s_len, 's', &zero); // return nil chars if no string
			} else if (c_type == B32) {
				if ((s_adr = command[i].value.dta) != NULL) {
					s_len = Xsprint(s_buf, s_len, 's', ",b"); //todo - incorrect
					s_len = Xsprint(s_buf, s_len, 'b', command[i].value.dta);
				}
			} else {
				if ((s_adr = command[i].value.str) != NULL) {
					s_len = Xsprint(s_buf, s_len, 's', command[i].format.str);
					s_fmt = command[i].format.str + 1;
					while (*s_fmt) {
						if (*s_fmt == 'i') {
							s_len = Xsprint(s_buf, s_len, 'i', (int*)s_adr);
							s_adr += 4;
						} else if (*s_fmt == 'f') {
							s_len = Xsprint(s_buf, s_len, 'f', (float*)s_adr);
							s_adr += 4;
						} else if (*s_fmt == 's') {
							if (s_adr) s_len = Xsprint(s_buf, s_len, 's', s_adr);
							s_adr += ((strlen(s_adr) + 4) & ~3);
						} else if (*s_fmt == 'b') {
							s_len = Xsprint(s_buf, s_len, 'b', (void*)s_adr);
							s_adr += ((strlen(s_adr) + 4) & ~3); //todo - incorrect
						}
						s_fmt++;
					}
				}
			}
			// need to send to local client
			// no need to send to remote clients
			p_status = S_SND;
		}
	}
	return (p_status);
}


int function_shutdown() {
	char*	info = "osc-server\000\000";

	if (Xverbose) {Xfdump("Shutting down", info, 12, Xdebug); fflush(stdout);}
	return X32Shutdown();
}

int function_info() {
	s_len = Xsprint(s_buf, 0, 's', "/info");
	s_len = Xsprint(s_buf, s_len, 's', ",ssss");
	s_len = Xsprint(s_buf, s_len, 's', "V2.05");
	s_len = Xsprint(s_buf, s_len, 's', "osc-server");
	s_len = Xsprint(s_buf, s_len, 's', "X32");
	s_len = Xsprint(s_buf, s_len, 's', "2.10");
	return S_SND;		// send reply only to requesting client
}

int function_status() {
	getmyIP();	// get my IP in r_buf

	s_len = Xsprint(s_buf, 0, 's', "/status");
	s_len = Xsprint(s_buf, s_len, 's', ",sss");
	s_len = Xsprint(s_buf, s_len, 's', "active");
	s_len = Xsprint(s_buf, s_len, 's', r_buf);
	s_len = Xsprint(s_buf, s_len, 's', "osc-server");
	return S_SND;		// send reply only to requesting client
}

int function_unsubscribe() {
	int		k;

	//
	// For now, only simple/single unsubscribe command is recognized to remove
	// possible xremote client

	for (k = 0; k < MAX_SIM_CLIENTS; k++) {
		if (X32Client[k].vlid) {
			if (strcmp(X32Client[k].sock.sa_data, Client_ip_pt->sa_data) == 0) {
				X32Client[k].vlid = 0;	//No longer a valid client
				return 0;
			}
		}
	}
	return 0;
}
//
//
int function_xremote() {
	int		k;

	// command is xremote (set remote time for requesting client)
	// Need to update existing client or register requesting client as new client?
	for (k = 0; k < MAX_SIM_CLIENTS; k++) {
		if (X32Client[k].vlid) {
			if (strcmp(X32Client[k].sock.sa_data, Client_ip_pt->sa_data) == 0) {
				X32Client[k].xrem = time(NULL) + XREMOTE_TIME;	//update existing client
				return 0;
			}
		}
	}
	//	attempt to register a new client... if room available
	for (k = 0; k < MAX_SIM_CLIENTS; k++) {
		if (X32Client[k].vlid == 0) {					// create new record
			memcpy(&(X32Client[k].sock), Client_ip_pt, Client_ip_len);
			X32Client[k].vlid = 1;
			X32Client[k].xrem = time(NULL) + XREMOTE_TIME;
			return 0;
		} else if (X32Client[k].xrem < time(NULL)) {	// replace outdated record
			memcpy(&(X32Client[k].sock), Client_ip_pt, Client_ip_len);
			X32Client[k].xrem = time(NULL) + XREMOTE_TIME;
			return 0;
		}
	}
	return 0; // no room for new clients! (todo; another return status?)
}

int function_node() {
char*		str_pt_in;
int			i, j, cmd_max;
X32command*	command;
	// received a /node~~~,s~~[string] command
	// parse [string]
	// reply with node~~~~,s~~/[string] <data>...\n
	cmd_max = 0;
	str_pt_in = r_buf + 12;
	for (i = 0; i < Xnode_max; i++) {
		if (strncmp(Xnode[i].command, str_pt_in, Xnode[i].nchars) == 0) {
			cmd_max = Xnode[i].cmd_max;
			command = Xnode[i].cmd_ptr;
			break;
		}
	}
	if (i == Xnode_max) return 0;
	for (i = 0; i < cmd_max; i++) {
		if (command[i].flags == F_FND) {
			if (strcmp(str_pt_in, command[i].command + 1) == 0) {
				s_len = Xsprint(s_buf, 0, 's', "node");
				s_len = Xsprint(s_buf, s_len, 's',",s");
				s_buf[s_len] = 0;
				strcat (s_buf + s_len, command[i].command);
				switch (command[i].format.typ) {
				case OFFON:
					for (j = 1; j < command[i].value.ii + 1; j++) {
						strcat(s_buf + s_len, command[i + j].value.ii? " ON": " OFF");
					}
					break;
				case CMONO:
					strcat(s_buf + s_len, command[i + 1].value.ii? " LCR": " LR+M");
					strcat(s_buf + s_len, command[i + 2].value.ii? " ON": " OFF");
					break;
				case CSOLO:
					strcat(s_buf + s_len, Slevel(command[i + 1].value.ff));
					strcat(s_buf + s_len, Ssource[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 3].value.ff, -18., +18., 1));
					strcat(s_buf + s_len, command[i + 4].value.ii? " AFL": " PFL");
					strcat(s_buf + s_len, command[i + 5].value.ii? " AFL": " PFL");
					strcat(s_buf + s_len, command[i + 6].value.ii? " AFL": " PFL");
					strcat(s_buf + s_len, command[i + 7].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, command[i + 8].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, command[i + 9].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slinf(command[i + 10].value.ff, -40., 0., 0));
					strcat(s_buf + s_len, command[i + 11].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, command[i + 12].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, command[i + 13].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slinf(command[i + 14].value.ff, 0.3, 500., 1));
					strcat(s_buf + s_len, command[i + 15].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, command[i + 16].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, command[i + 17].value.ii? " ON": " OFF");
					break;
				case CTALK:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, command[i + 2].value.ii? " EXT": " INT");
					break;
				case CTALKAB:
					strcat(s_buf + s_len, Slevel(command[i + 1].value.ff));
					strcat(s_buf + s_len, command[i + 2].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, command[i + 3].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Sbitmp(command[i + 4].value.ii, 18));
					break;
				case COSC:
					strcat(s_buf + s_len, Slevel(command[i + 1].value.ff));
					strcat(s_buf + s_len, f121[(int)(120 * command[i + 2].value.ff + 0.5)]);
					strcat(s_buf + s_len, f121[(int)(120 * command[i + 3].value.ff + 0.5)]);
					strcat(s_buf + s_len, command[i + 4].value.ii? " F2": " F1");
					strcat(s_buf + s_len, Sosct[command[i + 5].value.ii]);
					strcat(s_buf + s_len, Sint(command[i + 6].value.ii));
					break;
				case CROUTIN:
					strcat(s_buf + s_len, Sroutin[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Sroutin[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Sroutin[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Sroutin[command[i + 4].value.ii]);
					strcat(s_buf + s_len, Sroutax[command[i + 5].value.ii]);
					break;
				case CROUTAC:
					for (j = 1; j < command[i].value.ii + 1; j++) {
						strcat(s_buf + s_len, Sroutac[command[i + j].value.ii]);
					}
					break;
				case CROUTOT:
					strcat(s_buf + s_len, Srouto1[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Srouto1[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Srouto2[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Srouto2[command[i + 4].value.ii]);
					break;
				case CCTRL:
					strcat(s_buf + s_len, Scolor[command[i + 1].value.ii]);
					break;
				case CENC:
					for (j = 1; j < command[i].value.ii + 1; j++) {
						if (command[i + j].value.str) {
							strcat(s_buf + s_len, " \"");
							strcat(s_buf + s_len, command[i + j].value.str);
							strcat(s_buf + s_len, "\"");
						}
						else strcat(s_buf + s_len, " \"-\"");
					}
					break;
				case CTAPE:
					strcat(s_buf + s_len, Slinf(command[i + 1].value.ff, -6., +24., 1));
					strcat(s_buf + s_len, Slinf(command[i + 2].value.ff, -6., +24., 1));
					strcat(s_buf + s_len, command[i + 3].value.ii? " ON": " OFF");
					break;
				case CHCO:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 1].value.str);
						strcat(s_buf + s_len, "\"");
					}
					else strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Scolor[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					break;
				case CHDE:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slinf(command[i + 2].value.ff, 0.3, 500., 1));
					break;
				case CHPR:
					strcat(s_buf + s_len, Slinfs(command[i + 1].value.ff, -18., +18., 1));
					strcat(s_buf + s_len, command[i + 2].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, command[i + 3].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Sfslope[command[i + 4].value.ii]);
					strcat(s_buf + s_len, f101[(int)(100 * command[i + 5].value.ff + 0.5)]);
					break;
				case CHGA:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Sgmode[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 3].value.ff, -80., 0., 1));
					strcat(s_buf + s_len, Slinf(command[i + 4].value.ff, 3., 60., 1));
					strcat(s_buf + s_len, Slinf(command[i + 5].value.ff, 0., 120., 0));
					strcat(s_buf + s_len, Slogf(command[i + 6].value.ff, 0.02, 2000., 2));
					strcat(s_buf + s_len, Slogf(command[i + 7].value.ff, 5., 4000., 0));
					strcat(s_buf + s_len, Sint(command[i + 8].value.ii));
					break;
				case CHGF: case CHDF:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Sgftype[command[i + 2].value.ii]);
					strcat(s_buf + s_len, f201[(int)(200 * command[i + 3].value.ff + 0.5)]);
					break;
				case CHDY:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Sdmode[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Sddet[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Sdenv[command[i + 4].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 5].value.ff, -60., 0., 1));
					strcat(s_buf + s_len, Sdratio[command[i + 6].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 7].value.ff, 0., 5., 0));
					strcat(s_buf + s_len, Slinf(command[i + 8].value.ff, 0., 24., 1));
					strcat(s_buf + s_len, Slinf(command[i + 9].value.ff, 0., 120., 0));
					strcat(s_buf + s_len, Slogf(command[i + 10].value.ff, 0.02, 2000., 2));
					strcat(s_buf + s_len, Slogf(command[i + 11].value.ff, 5., 4000., 0));
					strcat(s_buf + s_len, Sdpos[command[i + 12].value.ii]);
					strcat(s_buf + s_len, Sint(command[i + 13].value.ii));
					strcat(s_buf + s_len, Slinf(command[i + 14].value.ff, 0., 100., 0));
					strcat(s_buf + s_len, command[i + 15].value.ii? " ON": " OFF");
					break;
				case CHIN:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Sdpos[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Sinsel[command[i + 3].value.ii]);
					break;
				case CHEQ:
					strcat(s_buf + s_len, Setype[command[i + 1].value.ii]);
					strcat(s_buf + s_len, f201[(int)(200 * command[i + 2].value.ff + 0.5)]);
					strcat(s_buf + s_len, Slinfs(command[i + 3].value.ff, -15., +15., 2));
					strcat(s_buf + s_len, Slogf(command[i + 4].value.ff, 10., 0.315, 1));
					break;
				case CHMX:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 2].value.ff));
					strcat(s_buf + s_len, command[i + 3].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slinfs(command[i + 4].value.ff, -100., +100., 0));
					strcat(s_buf + s_len, command[i + 5].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 6].value.ff));
					break;
				case CHMO:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 2].value.ff));
					strcat(s_buf + s_len, Slinfs(command[i + 3].value.ff, -100., +100., 0));
					strcat(s_buf + s_len, Sctype[command[i + 4].value.ii]);
					break;
				case CHME:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 2].value.ff));
					break;
				case CHGRP:
					strcat(s_buf + s_len, Sbitmp(command[i + 1].value.ii, 8));
					strcat(s_buf + s_len, Sbitmp(command[i + 2].value.ii, 6));
					break;
				case AXPR:
					strcat(s_buf + s_len, Slinf(command[i + 1].value.ff, -18., +18., 1));
					strcat(s_buf + s_len, command[i + 2].value.ii? " ON": " OFF");
					break;
				case BSCO:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " ");
						strcat(s_buf + s_len, command[i + 1].value.str);
					}
					else strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Scolor[command[i + 3].value.ii]);
					break;
				case MXPR:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					break;
				case MXDY:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Sdmode[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Sddet[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Sdenv[command[i + 4].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 5].value.ff, -60., 0., 1));
					strcat(s_buf + s_len, Sdratio[command[i + 6].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 7].value.ff, 0., 5., 0));
					strcat(s_buf + s_len, Slinf(command[i + 8].value.ff, 0., 24., 1));
					strcat(s_buf + s_len, Slinf(command[i + 9].value.ff, 0., 120., 0));
					strcat(s_buf + s_len, Slogf(command[i + 10].value.ff, 0.02, 2000., 2));
					strcat(s_buf + s_len, Slogf(command[i + 11].value.ff, 5., 4000., 0));
					strcat(s_buf + s_len, Sdpos[command[i + 12].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 14].value.ff, 0., 100., 0));
					strcat(s_buf + s_len, command[i + 15].value.ii? " ON": " OFF");
					break;
				case MSMX:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 2].value.ff));
					strcat(s_buf + s_len, Slinfs(command[i + 4].value.ff, -100., +100., 0));
					break;
				case FXTYP1:
					strcat(s_buf + s_len, Sfxtyp1[command[i + 1].value.ii]);
					break;
				case FXSRC:
					strcat(s_buf + s_len, Sfxsrc[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Sfxsrc[command[i + 2].value.ii]);
					break;
				case FXPAR1:
					GetFxPar1(s_buf + s_len, i + 1, command[i - 4].value.ii);
					break;
				case FXTYP2:
					strcat(s_buf + s_len, Sfxtyp1[command[i + 1].value.ii + _FX5_8 + 1]);
					break;
				case FXPAR2:
					GetFxPar1(s_buf + s_len, i + 1, command[i - 4].value.ii + _FX5_8 + 1);
					break;
				case OMAIN:
					strcat(s_buf + s_len, Sint(command[i + 1].value.ii));
					strcat(s_buf + s_len, Smpos[command[i + 2].value.ii]);
					break;
				case OMAIND:
					strcat(s_buf + s_len, command[i + 1].value.ii? " ON": " OFF");
					strcat(s_buf + s_len, Slinf(command[i + 2].value.ff, 0.3, 500., 1));
					break;
				case HAMP:
					strcat(s_buf + s_len, Slinf(command[i + 1].value.ff, -12., 60., 1));
					strcat(s_buf + s_len, command[i + 2].value.ii? " ON": " OFF");
					break;
				default:
					return 0;
					break;
				}
				s_len += strlen(s_buf + s_len);
				s_buf[s_len++] = '\n';
				s_buf[s_len++] = 0;
				while (s_len & 3) s_buf[s_len++] = 0;
				return S_SND;		// send reply only to requesting client
			}
		}
	}
	return 0;  // no reply if error detected
}


int function_config() {
int		i;
//
// check for actual command
	i = 0;
	while (i < Xconfig_max) {
		if (strcmp(r_buf, Xconfig[i].command) == 0) {
			// found command at index i
			return(funct_params(Xconfig, i));
		}
		i += 1;
	}
	return 0;
}

int function_main() {
int		i;
//
// check for actual command
	i = 0;
	while (i < Xmain_max) {
		if (strcmp(r_buf, Xmain[i].command) == 0) {
			// found command at index i
			return(funct_params(Xmain, i));
		}
		i += 1;
	}
	return 0;
}

int function_prefs() {
int		i;
//
// check for actual command
	i = 0;
	while (i < Xprefs_max) {
		if (strcmp(r_buf, Xprefs[i].command) == 0) {
			// found command at index i
			return(funct_params(Xprefs, i));
		}
		i += 1;
	}
	return 0;
}

int function_stat() {
int		i;
//
// check for actual command
	i = 0;
	while (i < Xstat_max) {
		if (strcmp(r_buf, Xstat[i].command) == 0) {
			// found command at index i
			return(funct_params(Xstat, i));
		}
		i += 1;
	}
	return 0;
}

int function_channel() {
int				i;
X32command		*Xchannel;
//
// check for actual command
// manage 32 channels 01 to 32 - /ch/xx/yyy
	Xchannel = Xchannelset[(r_buf[4] - 48) * 10 + r_buf[5] - 48 - 1];
	i = 0;
	while (i < Xchannel01_max) {
		if (strcmp(r_buf, Xchannel[i].command) == 0) {
			// found command at index i
			return(funct_params(Xchannel, i));
		}
		i += 1;
	}
	return 0;
}


int function_auxin() {
int				i;
X32command		*Xauxin;
//
// check for actual command
// manage 8 auxin 01 to 08 - /auxin/xx/yyy
	Xauxin = Xauxinset[(r_buf[7] - 48) * 10 + r_buf[8] - 48 - 1];
	i = 0;
	while (i < Xauxin01_max) {
		if (strcmp(r_buf, Xauxin[i].command) == 0) {
			// found command at index i
			return(funct_params(Xauxin, i));
		}
		i += 1;
	}
	return 0;
}

int function_fxrtn() {
int				i;
X32command		*Xfxrtn;
//
// check for actual command
// manage 8 fxrtn 01 to 08 - /fxrtn/xx/yyy
	Xfxrtn = Xfxrtnset[(r_buf[7] - 48) * 10 + r_buf[8] - 48 - 1];
	i = 0;
	while (i < Xfxrtn01_max) {
		if (strcmp(r_buf, Xfxrtn[i].command) == 0) {
			// found command at index i
			return(funct_params(Xfxrtn, i));
		}
		i += 1;
	}
	return 0;
}


int function_bus() {
int				i;
X32command		*Xbus;
//
// check for actual command
// manage 16 bus 01 to 16 - /bus/xx/yyy
	Xbus = Xbusset[(r_buf[5] - 48) * 10 + r_buf[6] - 48 - 1];
	i = 0;
	while (i < Xbus01_max) {
		if (strcmp(r_buf, Xbus[i].command) == 0) {
			// found command at index i
			return(funct_params(Xbus, i));
		}
		i += 1;
	}
	return 0;
}


int function_mtx() {
int				i;
X32command		*Xmtx;
//
// check for actual command
// manage 6 mtx 01 to 06 - /mtx/xx/yyy
	Xmtx = Xmtxset[(r_buf[5] - 48) * 10 + r_buf[6] - 48 - 1];
	i = 0;
	while (i < Xmtx01_max) {
		if (strcmp(r_buf, Xmtx[i].command) == 0) {
			// found command at index i
			return(funct_params(Xmtx, i));
		}
		i += 1;
	}
	return 0;
}


int function_dca() {
int		i;
//
// check for actual command
	i = 0;
	while (i < Xdca_max) {
		if (strcmp(r_buf, Xdca[i].command) == 0) {
			// found command at index i
			return(funct_params(Xdca, i));
		}
		i += 1;
	}
	return 0;
}


int function_fx() {
int		i;
//
// check for actual command
	i = 0;
	while (i < Xfx_max) {
		if (strcmp(r_buf, Xfx[i].command) == 0) {
			// found command at index i
			return(funct_params(Xfx, i));
		}
		i += 1;
	}
	return 0;
}


int function_output() {
int		i;
//
// check for actual command
	i = 0;
	while (i < Xoutput_max) {
		if (strcmp(r_buf, Xoutput[i].command) == 0) {
			// found command at index i
			return(funct_params(Xoutput, i));
		}
		i += 1;
	}
	return 0;
}


int function_headamp() {
int		i;
//
// check for actual command
	i = 0;
	while (i < Xheadamp_max) {
		if (strcmp(r_buf, Xheadamp[i].command) == 0) {
			// found command at index i
			return(funct_params(Xheadamp, i));
		}
		i += 1;
	}
	return 0;
}



int function_misc() {
int		i;
//
// check for actual command
	i = 0;
	while (i < Xmisc_max) {
		if (strcmp(r_buf, Xmisc[i].command) == 0) {
			// found command at index i
			return(funct_params(Xmisc, i));
		}
		i += 1;
	}
	return 0;
}


int function_show() {
int		i;
//
// check for actual command
	i = 0;
	if (strncmp(r_buf + 16, "scene", 5) == 0) {
		while (i < Xscene_max) {
			if (strcmp(r_buf, Xscene[i].command) == 0) {
				// found command at index i
				return(funct_params(Xscene, i));
			}
			i += 1;
		}
		return 0;
	}
	if (strncmp(r_buf + 16, "snippet", 7) == 0) {
		while (i < Xsnippet_max) {
			if (strcmp(r_buf, Xsnippet[i].command) == 0) {
				// found command at index i
				return(funct_params(Xsnippet, i));
			}
			i += 1;
		}
		return 0;
	}
	while (i < Xshow_max) {
		if (strcmp(r_buf, Xshow[i].command) == 0) {
			// found command at index i
			return(funct_params(Xshow, i));
		}
		i += 1;
	}
	return 0;
}


int function_copy() {
int			i, j, srce, dest, mask;
X32command	*ch_src, *ch_dst;
//
// Copy function - format is /copy~~~,siii~~~type source destination mask
	i = 0;
	if (strcmp(r_buf + 16, "channel") == 0) {
		i = 24;
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		srce = endian.ii;
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		dest = endian.ii;
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		mask = endian.ii;
		//
		if ((srce >= 0) && (srce < 32) && (dest >= 0) && (dest < 32)) {
			// copy channel <srce> to channel <dest>, for bits in <mask>
			ch_src = Xchannelset[srce];
			ch_dst = Xchannelset[dest];

			if (mask & C_CONFIG) {
				//config: i= 1-7
				for (i = 0; i < 8; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_HA) {
				//preamp: i= 8-19
				for (i = 8; i < 20; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_GATE) {
				//gate: i= 20-32
				for (i = 20; i < 33; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_DYN) {
				//dyn: i= 33-52
				for (i = 33; i < 53; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_EQ) {
				//eq: i= 53-74
				for (i = 53; i < 75; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_SEND) {
				//sends: i= 75-145
				for (i = 75; i < 146; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
		}
	}
	return 0;
}



int function_add() {
//
// add function - format is /add~~~~,sis~~~~type number name
	printf("Nothing actually added\n"); fflush(stdout);
	if (strcmp(r_buf + 16, "cue") == 0) {
		// test... do nothing, just return OK status
		s_len = Xsprint(s_buf, 0, 's', "/add");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "cue");
		s_len = Xsprint(s_buf, s_len, 'i', &one);
	}
	return S_SND;
}

int function_save() {
int			i, j;
//
// add function - format is /save~~~,sissi~~type number name note value
	i = 0;
	if (strcmp(r_buf + 16, "scene") == 0) {
		i = 24;
		//get sscene index
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		sprintf (tmp_str, "/-show/showfile/scene/%03d/name", endian.ii);
		for (j = 0; j < Xscene_max; j++) {
			if ((strcmp(tmp_str, Xscene[j].command)) == 0) {
				// save name
				if (Xscene[j].value.str) free(Xscene[j].value.str);
				if ((s_len = strlen(r_buf + i)) > 0) {
					Xscene[j].value.str = malloc((s_len + 8) * sizeof(char));
					strncpy (Xscene[j].value.str, r_buf + i, s_len);
					Xscene[j].value.str[s_len] = 0;
				} else {
					Xscene[j].value.str = NULL;
				}
				// save note in j+1, note data is at i + s_len % 4
				i = (i + s_len + 4) & 0xfffffffc;
				if (Xscene[j + 1].value.str) free(Xscene[j + 1].value.str);
				if ((s_len = strlen(r_buf + i)) > 0) {
					Xscene[j + 1].value.str = malloc((s_len + 8) * sizeof(char));
					strncpy (Xscene[j + 1].value.str, r_buf + i, s_len);
					Xscene[j + 1].value.str[s_len] = 0;
				} else {
					Xscene[j + 1].value.str = NULL;
				}
				//
				// TODO At this time the last <int> parameter is ignored
				//
				// update remote clients
				s_len = Xfprint(s_buf, 0, Xscene[j].command, 's', Xscene[j].value.str);
				Xsend(S_REM); // update xremote clients
				s_len = Xfprint(s_buf, 0, Xscene[j + 1].command, 's', Xscene[j + 1].value.str);
				Xsend(S_REM); // update xremote clients
				//
				// collect and save (locally) scene data
				// save corresponding data as nodes in "hasdata" indicator (at j+2)
				// todo: this is not actually done here!
				// hasdata should point to .data and not to .ii and a set of node commands should be saved
				Xscene[j+2].value.ii = 1;
				s_len = Xfprint(s_buf, 0, Xscene[j+2].command, 'i', &one);
				Xsend(S_REM); // update xremote clients
				//
				s_len = Xsprint(s_buf, 0, 's', "/save");
				s_len = Xsprint(s_buf, s_len, 's', ",si");
				s_len = Xsprint(s_buf, s_len, 's', "scene");
				s_len = Xsprint(s_buf, s_len, 'i', &one);
				return S_SND;
			}
		}
		//Scene not found
		s_len = Xsprint(s_buf, 0, 's', "/save");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "scene");
		s_len = Xsprint(s_buf, s_len, 'i', &zero);
		return S_SND;
	}
	if (strcmp(r_buf + 16, "snippet") == 0) {
		i = 24;
		//get snippet index
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		sprintf (tmp_str, "/-show/showfile/snippet/%03d/name", endian.ii);
		for (j = 0; j < Xsnippet_max; j++) {
			if ((strcmp(tmp_str, Xsnippet[j].command)) == 0) {
				if (Xsnippet[j].value.str) free(Xsnippet[j].value.str);
				if ((s_len = strlen(r_buf + i)) > 0) {
					Xsnippet[j].value.str = malloc((s_len + 8) * sizeof(char));
					strncpy (Xsnippet[j].value.str, r_buf + i, s_len);
					Xsnippet[j].value.str[s_len] = 0;
				} else {
					Xsnippet[j].value.str = NULL;
				}
				//
				// TODO At this time the last two <string><int> parameters are ignored
				//
				// update remote clients
				s_len = Xfprint(s_buf, 0, tmp_str, 's', Xsnippet[j].value.str);
				Xsend(S_REM); // update xremote clients
				//
				// collect and save (locally) snippet data
				// save corresponding data as nodes in "hasdata" indicator (at j+1)
				// todo: this is not actually done here!
				// hasdata should point to .data and not to .ii and a set of node commands should be saved
				Xsnippet[j+1].value.ii = 1;
				s_len = Xfprint(s_buf, 0, Xsnippet[j+1].command, 'i', &one);
				Xsend(S_REM); // update xremote clients
				//
				s_len = Xsprint(s_buf, 0, 's', "/save");
				s_len = Xsprint(s_buf, s_len, 's', ",si");
				s_len = Xsprint(s_buf, s_len, 's', "snippet");
				s_len = Xsprint(s_buf, s_len, 'i', &one);
				// update requesting client
				return S_SND;
			}
		}
		// Snippet not found
		s_len = Xsprint(s_buf, 0, 's', "/save");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "snippet");
		s_len = Xsprint(s_buf, s_len, 'i', &zero);
		return S_SND;
	}
	if (strcmp(r_buf + 16, "libch") == 0) {
		printf("Nothing actually saved\n"); fflush(stdout);
		i = 20;
		// test... do nothing, just return OK status
		s_len = Xsprint(s_buf, 0, 's', "/save");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "libch");
		s_len = Xsprint(s_buf, s_len, 'i', &one);
		return S_SND;
	}
	return 0;
}

int function_delete() {
int			i, j;

//
// delete function - format is /delete~,si~~~~type number
	i = 0;
	if (strcmp(r_buf + 12, "scene") == 0) {
		i = 20;
		//get scene index
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		sprintf (tmp_str, "/-show/showfile/scene/%03d/name", endian.ii);
		for (j = 0; j < Xscene_max; j++) {
			if ((strcmp(tmp_str, Xscene[j].command)) == 0) {
				// found name
				if (Xscene[j].value.str) free(Xscene[j].value.str);
				Xscene[j].value.str = NULL;
				if (Xscene[j + 1].value.str) free(Xscene[j + 1].value.str);
				Xscene[j + 1].value.str = NULL;
				Xscene[j + 2].value.ii = 0; // No data
				// update remote clients
				s_len = Xfprint(s_buf, 0, Xscene[j].command, 's', Xscene[j].value.str);
				Xsend(S_REM); // update xremote clients
				s_len = Xfprint(s_buf, 0, Xscene[j + 1].command, 's', Xscene[j + 1].value.str);
				Xsend(S_REM); // update xremote clients
				// todo: this is not actually done here!
				// hasdata should point to .data and not to .ii and a set of node commands should be saved
				s_len = Xfprint(s_buf, 0, Xscene[j + 2].command, 'i', &zero);
				Xsend(S_REM); // update xremote clients
				// return OK status
				s_len = Xsprint(s_buf, 0, 's', "/delete");
				s_len = Xsprint(s_buf, s_len, 's', ",si");
				s_len = Xsprint(s_buf, s_len, 's', "scene");
				s_len = Xsprint(s_buf, s_len, 'i', &one);
				return S_SND;
			}
		}
		// Not found.. just return NOK status
		s_len = Xsprint(s_buf, 0, 's', "/delete");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "scene");
		s_len = Xsprint(s_buf, s_len, 'i', &zero);
		return S_SND;
	}
	if (strcmp(r_buf + 12, "snippet") == 0) {
		i = 20;
		//get snippet index
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		sprintf (tmp_str, "/-show/showfile/snippet/%03d/name", endian.ii);
		for (j = 0; j < Xsnippet_max; j++) {
			if ((strcmp(tmp_str, Xsnippet[j].command)) == 0) {
				// found name
				if (Xsnippet[j].value.str) free(Xsnippet[j].value.str);
				Xsnippet[j].value.str = NULL;
				Xsnippet[j + 1].value.ii = 0; // No data
				// update remote clients
				s_len = Xfprint(s_buf, 0, Xsnippet[j].command, 's', Xsnippet[j].value.str);
				Xsend(S_REM); // update xremote clients
				// todo: this is not actually done here!
				// hasdata should point to .data and not to .ii and a set of node commands should be saved
				s_len = Xfprint(s_buf, 0, Xsnippet[j + 1].command, 'i', &zero);
				Xsend(S_REM); // update xremote clients
				// return OK status
				s_len = Xsprint(s_buf, 0, 's', "/delete");
				s_len = Xsprint(s_buf, s_len, 's', ",si");
				s_len = Xsprint(s_buf, s_len, 's', "snippet");
				s_len = Xsprint(s_buf, s_len, 'i', &one);
				return S_SND;
			}
		}
		// Not found, return NOK status
		s_len = Xsprint(s_buf, 0, 's', "/delete");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "snippet");
		s_len = Xsprint(s_buf, s_len, 'i', &zero);
		return S_SND;
	}
	if (strcmp(r_buf + 16, "libch") == 0) {
		i = 20;
		// test... do nothing, just return OK status
		s_len = Xsprint(s_buf, 0, 's', "/delete");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "libch");
		s_len = Xsprint(s_buf, s_len, 'i', &one);
		return S_SND;
	}
	return 0;
}


int function() {
	printf("dummy function\n"); fflush(stdout);
	return 0;
}



#define save(xx) \
		fprintf(X32File, "%d ", xx[i].format.typ);			\
		if (xx[i].format.typ == S32) {						\
			if (xx[i].value.str != NULL) {					\
				fprintf(X32File, "%d %s\n", (int)strlen(xx[i].value.str), xx[i].value.str);	\
			} else {										\
				fprintf(X32File, "%d\n", 0);				\
			}												\
		} else {											\
			fprintf(X32File, "%d\n", xx[i].value.ii);		\
		}

int X32Shutdown() {
int		i, j;
X32command* Xarray;
FILE	*X32File;
//
	if ((X32File = fopen(".X32res.rc", "w")) == NULL) return 0;
	printf("saving init file..."); fflush(stdout);
// read file init values for all data
	for (i = 0; i < Xconfig_max; i++) {
		save(Xconfig)
	}
	for (i = 0; i < Xmain_max; i++) {
		save(Xmain);
	}
	for (i = 0; i < Xprefs_max; i++) {
		save(Xprefs);
	}
	for (i = 0; i < Xstat_max; i++) {
		save(Xstat);
	}
	for (j = 0; j < 32; j++) {
		Xarray = Xchannelset[j];
		for (i = 0; i < Xchannel01_max; i++) {
			save(Xarray);
		}
	}
	for (j = 0; j < 8; j++) {
		Xarray = Xauxinset[j];
		for (i = 0; i < Xauxin01_max; i++) {
			save(Xarray);
		}
	}
	for (j = 0; j < 8; j++) {
		Xarray = Xfxrtnset[j];
		for (i = 0; i < Xfxrtn01_max; i++) {
			save(Xarray);
		}
	}
	for (j = 0; j < 16; j++) {
		Xarray = Xbusset[j];
		for (i = 0; i < Xbus01_max; i++) {
			save(Xarray);
		}
	}
	for (j = 0; j < 6; j++) {
		Xarray = Xmtxset[j];
		for (i = 0; i < Xmtx01_max; i++) {
			save(Xarray);
		}
	}
	for (i = 0; i < Xdca_max; i++) {
		save(Xdca);
	}
	for (i = 0; i < Xfx_max; i++) {
		save(Xfx);
	}
	for (i = 0; i < Xoutput_max; i++) {
		save(Xoutput);
	}
	for (i = 0; i < Xheadamp_max; i++) {
		save(Xheadamp);
	}
	for (i = 0; i < Xmisc_max; i++) {
		save(Xmisc);
	}
	fclose (X32File);
	printf(" Done\n"); fflush(stdout);
	keep_on = 0;
	return 0;
}


#define restore(xx)												\
	f_stat = fscanf(X32File, "%d " , &type);					\
	if (type == S32) {											\
		f_stat = fscanf(X32File, "%d ", &r_len);				\
		if (r_len > 0) {										\
			for (k = 0; k < r_len; k++)							\
				f_stat = fscanf(X32File, "%c", r_buf + k);		\
			if (xx[i].value.str) free(xx[i].value.str);			\
			xx[i].value.str = malloc(r_len + 8);				\
			strncpy(xx[i].value.str, r_buf, r_len);				\
			xx[i].value.str[r_len] = 0;							\
		} else {												\
			xx[i].value.str = NULL;								\
		}														\
	} else {													\
		f_stat = fscanf(X32File, "%d ", &xx[i].value.ii);		\
	}

int X32Init() {
int		i, j, k, type, f_stat;
X32command* Xarray;
FILE	*X32File;
//
	if ((X32File = fopen(".X32res.rc", "r")) == NULL) return 0;
	printf("Reading init file..."); fflush(stdout);
// read file init values for all data
	for (i = 0; i < Xconfig_max; i++) {
		restore(Xconfig)
	}
	for (i = 0; i < Xmain_max; i++) {
		restore(Xmain);
	}
	for (i = 0; i < Xprefs_max; i++) {
		restore(Xprefs);
	}
	for (i = 0; i < Xstat_max; i++) {
		restore(Xstat);
	}
	for (j = 0; j < 32; j++) {
		Xarray = Xchannelset[j];
		for (i = 0; i < Xchannel01_max; i++) {
			restore(Xarray);
		}
	}
	for (j = 0; j < 8; j++) {
		Xarray = Xauxinset[j];
		for (i = 0; i < Xauxin01_max; i++) {
			restore(Xarray);
		}
	}
	for (j = 0; j < 8; j++) {
		Xarray = Xfxrtnset[j];
		for (i = 0; i < Xfxrtn01_max; i++) {
			restore(Xarray);
		}
	}
	for (j = 0; j < 16; j++) {
		Xarray = Xbusset[j];
		for (i = 0; i < Xbus01_max; i++) {
			restore(Xarray);
		}
	}
	for (j = 0; j < 6; j++) {
		Xarray = Xmtxset[j];
		for (i = 0; i < Xmtx01_max; i++) {
			restore(Xarray);
		}
	}
	for (i = 0; i < Xdca_max; i++) {
		restore(Xdca);
	}
	for (i = 0; i < Xfx_max; i++) {
		restore(Xfx);
	}
	for (i = 0; i < Xoutput_max; i++) {
		restore(Xoutput);
	}
	for (i = 0; i < Xheadamp_max; i++) {
		restore(Xheadamp);
	}
	for (i = 0; i < Xmisc_max; i++) {
		restore(Xmisc);
	}
	i = f_stat; // to avoid gcc warning;
	fclose (X32File);
	printf(" Done\n"); fflush(stdout);
	return 0;
}

