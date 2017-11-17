//
// X32SetScene.c
//
// Created on: Oct 13, 2014
//      Author: Patrick-Gilles Maillot
//
// This is the symmetric from X32GetScene(). Reading scene or snippet files to send data to X32
// Typical use is to restore a saved [partial] scene to X32, as saved with X32GetScene or from
// an X32 file save.
//
// ver 1.8: support for FW 3.08
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>


#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#define MAXLREAD 512

void 	Xlogf(char *header, char *buf, int len);

extern int SetSceneParse(char *l_read);

int					Xdebug = 0;
int					Xverbose = 0;
int					Xdelay = 1;
int					X32VER = 0212;
int					X32SHOW = 0;		// Not part of the GetShow or SetShow utils.
int					X32PRESET = 1;		// this flags reorients /ch/xx/config to not consider source
int					fx[8]; // saves the FX current type for each of the fx slots


struct sockaddr_in	Xip;
struct sockaddr		*Xip_pt = (struct sockaddr *)&Xip;
char 				Xip_str[20], Xport_str[8];
int 				Xfd;				// our socket
FILE				*Xin, *log_file;	// lof_file only used for X32SetShow
#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
#endif
//
//
//
// X32SetScene: An X32 Scene file interpreter
//
int
main(int argc, char **argv) {
char				l_read[MAXLREAD];
int 				keep_reading, read_status;
char				c1;
//
// initialize communication with X32 server at IP ip and PORT port
//	set a default value for server[]; change to match your X32 desk
	strcpy (Xip_str, "192.168.0.64");
//	Xport_str[] = "10023" //change to use a different port
	strcpy (Xport_str, "10023");
//
	Xdelay = 1;		// default is 1ms between commands
//
// Manage arguments
	while ((c1 = getopt(argc, argv, "i:d:v:s:t:h")) != (char)-1) {
		switch (c1) {
		case 'i':
			strcpy(Xip_str, optarg );
			break;
		case 'd':
			sscanf(optarg, "%d", &Xdebug);
			break;
		case 'v':
			sscanf(optarg, "%d", &Xverbose);
			break;
		case 't':
			sscanf(optarg, "%d", &Xdelay);
			break;
		case 's':
			sscanf(optarg, "%d", &X32VER);
			break;
		default:
		case 'h':
			printf("usage: X32SetScene [-i X32 console ipv4 address]\n");
			printf("                   [-d 0/1 -v 0/1, debug verbose options]\n");
			printf("                   [-s, X32fw ver. 0208, 0210, 0212- default: 0212]\n");
			printf("                   [-t <delay> between commands in ms - default: 1]\n");
			printf("                     default IP is 192.168.0.64    defaults debug: 0 verbose: 0\n");
			printf("reads from <stdin> scene IDs (from *.scn file) and parse scene data\n");
			printf("updates X32 accordingly, thus restoring the scene to the connected X32\n\n");
			return(0);
			break;
		}
	}
#ifdef __WIN32__
//Initialize winsock
	if (Xverbose) printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
#endif
// Load the X32 address we connect to; we're a client to X32, keep it simple.
// Open a UDP socket
	if ((Xfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("failed to create X32 socket");
		return(-1);
	}
//
//Construct the server sockaddr_in structure
	memset(&Xip, 0, sizeof(Xip));				// Clear struct
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
	Xip.sin_port = htons(atoi(Xport_str));		// server port
//
// Open file to interpret from stdin
	printf ("# X32SetScene ver. 1.8 (c)2017 Patrick-Gilles Maillot\n\n");
	Xin = stdin;
	keep_reading = 1;
	fx[0] = fx[1] = fx[2] = fx[3] = fx[4] = fx[5] = fx[6] = fx[7] = -1;
	while(keep_reading) {
		if ((read_status = scanf("%s", l_read)) != EOF) {
			if (l_read[0] == '#') {
				if (strcmp(l_read, "#2.7#") == 0) {
					fgets(l_read, MAXLREAD, Xin); // ignore rest of the line
				} else {
					printf ("Only ver. 2.7 files are accepted at this time\n");
					keep_reading = 0;
				}
			} else if (l_read[0] == '/') {
				if (SetSceneParse(l_read) != 0) keep_reading = 0;
			} else {
				if (strcmp(l_read, "exit") == 0) keep_reading = 0;
				else {
					if (Xverbose) printf("Warning: unknown scene data: %s\n", l_read);
				}
			}
		} else {
			keep_reading = 0;
		}
	}
#ifdef __WIN32__
    WSACleanup();
#endif
	return(0);
}

void 	Xlogf(char *header, char *buf, int len) {} // dummy function for X32SetScene

