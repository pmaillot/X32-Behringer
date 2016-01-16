//
// x32GEQ2cpy.c
//
//  Created on: Oct 24, 2014
//      Author: Patrick-Gilles Maillot
//
//
// X32GEQ2cpy: An X32 utility to copy EQ settings one side to the other
// Reads X32 and validate FX presence at given slot.
// Copies A->B (or vice-versa)
// Copies A->B (or vice-versa)
// options R enable reset of EQ data, C copy of one EQ to the other
// including master level(s) or not in the copy operation
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#define microsleep(a) 			Sleep (a/100)
#define X32closesocket(a)	    closesocket(a)
#else
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define microsleep(a) 			usleep(a)
#define X32closesocket(a)	    close(a)
#endif

extern int Xfprint(char *bd, int index, char* text, char format, void *bs);
extern int Xsprint(char *bd, int index, char format, void *bs);
extern void Xfdump(char *header, char *buf, int len, int debug);

#define BSIZE 512

int X32verbose = 0;
int X32debug = 0;

#define SEND2X32(b, l)												\
do {																\
	if (Xverbose) {Xfdump("->X", b, l, Xdebug); fflush(stdout);}	\
	if (sendto(fd, b, l, 0, Xip_pt, Xip_len) < 0) {					\
		perror("Coundn't send data to client");						\
		exit(EXIT_FAILURE);											\
	} 																\
} while (0);


int main(int argc, char **argv)
{

int 				Xverbose = 0;
int 				Xdebug = 0;
int					i, k;
char				l_read[128], c;
int					cpydir, fxslot, gxslot, cpymaster;
struct sockaddr_in	Xip;
struct sockaddr*	Xip_pt = (struct sockaddr*)&Xip;
int 				fd;				// our socket
int					recvlen;
char				b_rec[BSIZE], Xip_str[20], Xport_str[8];
char				b_snd[BSIZE];
float				dot5 = 0.5;

#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
#endif

//
// initialize communication with X32 server at IP ip and PORT port
//	set a default value for server[]; change to match your X32 desk
	strcpy (Xip_str, "192.168.0.64");
//	server_port[] = "10023" //change to use a different port
	strcpy (Xport_str, "10023");
// set defaults
	cpydir = 0;
	cpymaster = 1;
	fxslot = gxslot = 1;
//
// Manage arguments
	while ((c = getopt(argc, argv, "i:d:m:f:g:v:D:h")) != (char)-1) {
		switch (c) {
		case 'i':
			strcpy(Xip_str, optarg );
			break;
		case 'd':
			if(optarg[0] == 'f' || optarg[0] == 'a') cpydir = 0;
			else if (optarg[0] == 'B' || optarg[0] == 'b') cpydir = 1;
			else if (optarg[0] == 'R' || optarg[0] == 'r') cpydir = 2;
			else if (optarg[0] == 'C' || optarg[0] == 'c') cpydir = 3;
			else printf("Wrong value for argument -d, copying A->B\n");
			break;
		case 'm':
			if(optarg[0] == '0' || optarg[0] == 'n') cpymaster = 0;
			else if(optarg[0] == '1' || optarg[0] == 'y') cpymaster = 1;
			else printf("Wrong value for argument -l, copying master level\n");
			break;
		case 'f':
			if(optarg[0] > '0' && optarg[0] < '9') fxslot = (int)(optarg[0] - '0');
			else printf("Wrong value for argument -f, checking with fx slot 1\n");
			break;
		case 'g':
			if(optarg[0] > '0' && optarg[0] < '9') gxslot = (int)(optarg[0] - '0');
			else printf("Wrong value for argument -g, copying to fx slot 1\n");
			break;
		case 'D':
			sscanf(optarg, "%d", &X32debug);
			break;
		case 'v':
			sscanf(optarg, "%d", &X32verbose);
			break;
		default:
		case 'h':
			printf("usage: X32GEQ2cpy [-i X32 console ipv4 address] default 192.168.0.64\n");
			printf("                  [-f FX slot#] default: 1\n");
			printf("                  [-g FX slot#] default: 1\n");
			printf("                  [-d A>B | B>A | R | C] default: A>B\n");
			printf("                      A>B, B>A: copy FX# f sides\n");
			printf("                      R: Reset FX# f\n");
			printf("                      C: copy FX# f to FX# g\n");
			printf("                  [-m 0/1 for copying master] default: 1/yes\n");
			printf("                  [-v 0/1 verbose] default: 0\n\n");
			return(0);
			break;
		}
	}
//
#ifdef __WIN32__
//Initialize winsock
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
#endif
// Load the X32 address we connect to; we're a client to X32, keep it simple.
	if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("failed to create X32 socket");
		return(-1);
	}
//
// Construct the server sockaddr_in structure
	memset(&Xip, 0, sizeof(Xip));				// Clear struct
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
	Xip.sin_port = htons(atoi(Xport_str));		// server port
//
// All done. Let's send and receive messages
// Establish logical connection with X32 server
//
// Print header
	printf ("# X32GEQ2cpy V1.3 (c)2014 Patrick-Gilles Maillot\n\n");
// Check for FX at the right slot #
	sprintf(l_read, "fx/%d", fxslot);
	k = Xfprint (b_snd, 0, "/node", 's', l_read);
	SEND2X32(b_snd, k);
	recvlen = recvfrom(fd, b_rec, BSIZE, 0, (struct sockaddr *)&Xip, &Xip_len);
    for (i= 0; i < recvlen; i++) {
    	if (b_rec[i] == 'G' || b_rec[i] == 'T') {
    		if (b_rec[i+1] == 'E' && b_rec[i+2] == 'Q' && b_rec[i+3] == '2') break;
    	}
    }
    if (i < recvlen) {
    	sprintf(l_read, "fx/%d", gxslot);
    	k = Xfprint (b_snd, 0, "/node", 's', l_read);
    	SEND2X32(b_snd, k);
    	recvlen = recvfrom(fd, b_rec, BSIZE, 0, (struct sockaddr *)&Xip, &Xip_len);
        for (i= 0; i < recvlen; i++) {
        	if (b_rec[i] == 'G' || b_rec[i] == 'T') {
        		if (b_rec[i+1] == 'E' && b_rec[i+2] == 'Q' && b_rec[i+3] == '2') break;
        	}
        }
        if (i < recvlen) {
// found "GEQ2" or "TEQ2", all right!
// Copy all 31 sliders from A>B or B>A
			if (cpydir == 0) {				// copy A->B
				for (i = 1; i < 32; i++) {
					sprintf(l_read, "/fx/%d/par/%02d", fxslot, i);
					k = Xsprint (b_snd, 0, 's', l_read);
					SEND2X32(b_snd, k);
					recvlen = recvfrom(fd, b_rec, BSIZE, 0, (struct sockaddr *)&Xip, &Xip_len);
					if (X32verbose) Xfdump("X->", b_rec, recvlen, X32debug);
					sprintf(b_rec+10, "%02d", i+32);
					if (X32verbose) Xfdump("->X", b_rec, recvlen, X32debug);
					SEND2X32(b_rec, recvlen);
				}
			} else if (cpydir == 1) {		// copy B->A
				for (i = 33; i < 64; i++) {
					sprintf(l_read, "/fx/%d/par/%02d", fxslot, i);
					k = Xsprint (b_snd, 0, 's', l_read);
					SEND2X32(b_snd, k);
					recvlen = recvfrom(fd, b_rec, BSIZE, 0, (struct sockaddr *)&Xip, &Xip_len);
					if (X32verbose) Xfdump("X->", b_rec, recvlen, X32debug);
					sprintf(b_rec+10, "%02d", i-32);
					if (X32verbose) Xfdump("->X", b_rec, recvlen, X32debug);
					SEND2X32(b_rec, recvlen);
				}
			} else if (cpydir == 2) {		// reset
				for (i = 0; i < 32; i++) {
					sprintf(l_read, "/fx/%d/par/%02d", fxslot, i);
					k = Xfprint (b_snd, 0, l_read, 'f', &dot5);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					microsleep(100);
					sprintf(l_read, "/fx/%d/par/%02d", fxslot, i+32);
					k = Xfprint (b_snd, 0, l_read, 'f', &dot5);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					microsleep(100);
				}
			} else if (cpydir == 3) {		// copy fxslot -> gxslot
				for (i = 1; i < 32; i++) {
					sprintf(l_read, "/fx/%d/par/%02d", fxslot, i);
					k = Xsprint (b_snd, 0, 's', l_read);
					SEND2X32(b_snd, k);
					recvlen = recvfrom(fd, b_rec, BSIZE, 0, (struct sockaddr *)&Xip, &Xip_len);
					if (X32verbose) Xfdump("X->", b_rec, recvlen, X32debug);
					b_rec[4] = (char)gxslot + '0';
					if (X32verbose) Xfdump("->X", b_rec, recvlen, X32debug);
					SEND2X32(b_rec, recvlen);
				}
				for (i = 33; i < 64; i++) {
					sprintf(l_read, "/fx/%d/par/%02d", fxslot, i);
					k = Xsprint (b_snd, 0, 's', l_read);
					SEND2X32(b_snd, k);
					recvlen = recvfrom(fd, b_rec, BSIZE, 0, (struct sockaddr *)&Xip, &Xip_len);
					if (X32verbose) Xfdump("X->", b_rec, recvlen, X32debug);
					b_rec[4] = (char)gxslot + '0';
					if (X32verbose) Xfdump("->X", b_rec, recvlen, X32debug);
					SEND2X32(b_rec, recvlen);
				}
			}
			if (cpymaster) {
				if (cpydir == 0) {			// copy A->B
					sprintf(l_read, "/fx/%d/par/32", fxslot);
					k = Xsprint (b_snd, 0, 's', l_read);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					recvlen = recvfrom(fd, b_rec, BSIZE, 0, (struct sockaddr *)&Xip, &Xip_len);
					if (X32verbose) Xfdump("X<-", b_rec, recvlen, X32debug);
					sprintf(b_rec+10, "%02d", 64);
					SEND2X32(b_rec,recvlen);
					if (X32verbose) Xfdump("->X", b_rec, recvlen, X32debug);
				} else if (cpydir == 1) {	// copy B->A
					sprintf(l_read, "/fx/%d/par/64", fxslot);
					k = Xsprint (b_snd, 0, 's', l_read);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					recvlen = recvfrom(fd, b_rec, BSIZE, 0, (struct sockaddr *)&Xip, &Xip_len);
					if (X32verbose) Xfdump("X<-", b_rec, recvlen, X32debug);
					sprintf(b_rec+10, "%02d", 32);
					SEND2X32(b_rec,recvlen);
					if (X32verbose) Xfdump("->X", b_rec, recvlen, X32debug);
				} else if (cpydir == 2) {	// reset
					sprintf(l_read, "/fx/%d/par/32", fxslot);
					k = Xfprint (b_snd, 0, l_read, 'f', &dot5);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					microsleep(100);
					sprintf(l_read, "/fx/%d/par/64", fxslot);
					k = Xfprint (b_snd, 0, l_read, 'f', &dot5);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					microsleep(100);
				} else if (cpydir == 3) {	// copy fxslot->gxslot
					sprintf(l_read, "/fx/%d/par/32", fxslot);
					k = Xfprint (b_snd, 0, l_read, 'f', &dot5);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					microsleep(100);
					sprintf(l_read, "/fx/%d/par/32", gxslot);
					k = Xfprint (b_snd, 0, l_read, 'f', &dot5);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					microsleep(100);
					sprintf(l_read, "/fx/%d/par/64", fxslot);
					k = Xfprint (b_snd, 0, l_read, 'f', &dot5);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					microsleep(100);
					sprintf(l_read, "/fx/%d/par/64", gxslot);
					k = Xfprint (b_snd, 0, l_read, 'f', &dot5);
					SEND2X32(b_snd,k);
					if (X32verbose) Xfdump("->X", b_snd, k, X32debug);
					microsleep(100);
				}
			}
        } else {
        	printf ("--!!-- No GEQ2/TEQ2 effect at FX slot #%d\n", gxslot);
        }
    } else {
    	printf ("--!!-- No GEQ2/TEQ2 effect at FX slot #%d\n", fxslot);
    }
    X32closesocket(fd);
#ifdef __WIN32__
    WSACleanup();
#endif
	return(0);
}
