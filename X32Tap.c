//
// X32Tap.c
//
//  Created on: Apr 10, 2015
//
//      Author: Patrick-Gilles Maillot
//
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#ifdef __WIN32__
#include <winsock2.h>
#include <conio.h>
#else
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif


// External calls used
extern int	Xsprint(char *bd, int index, char format, void *bs);
extern int	Xfprint(char *bd, int index, char* text, char format, void *bs);

#ifdef __WIN32__
	#define BACKSPACE	8		// need to use <ctl>h
	#define EOL			13		// enter key
	#define BACKCHARS	2		// 2 chars to remove from line
	#define NO_CHAR		-1		// not a real/printable char
	#define TE			116		// 't'
#else
	#define BACKSPACE	8		// backspace key OK
	#define EOL			'\n'	// enter key
	#define BACKCHARS	1		// 1 char to remove from line
	#define NO_CHAR		-1		// not a real/printable char
	#define TE			116		// 't'
#endif


//
#define LINEMAX				32		// input line buffer size
#define BSIZE 				512		// Buffer sizes (enough to take into account FX parameters)
#define DLY					10 		// Stereo delay FX number
#define XREMOTE_TIMEOUT		9		// timeout set to 9 seconds
//
int X32debug = 0;
int X32verbose = 1;
//
//
struct timeval		t_1, t_2, t_dlta;
//
// Macros:
//
#ifndef timersub
#	define timersub(a, b, result)							\
	do {													\
		(result)->tv_sec = (a)->tv_sec - (b)->tv_sec;		\
		(result)->tv_usec = (a)->tv_usec - (b)->tv_usec;	\
		if ((result)->tv_usec < 0) { 						\
			  --(result)->tv_sec;							\
			  (result)->tv_usec += 1000000; 				\
		} 													\
	} while (0)
#endif
//

#define RECV										\
do {												\
	r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);	\
} while (0);

#define SEND													\
do {															\
	if (sendto (Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {	\
		perror ("Error while sending data");					\
		exit (EXIT_FAILURE);									\
	}															\
} while(0);

#define RPOLL													\
	do {														\
		FD_ZERO (&ufds);										\
		FD_SET (Xfd, &ufds);									\
		p_status = select(Xfd+1, &ufds, NULL, NULL, &timeout);	\
	} while (0);


int		i, Xdel_found, Xdel_slot;
char	tmpstr[32];
float	f;
//
// type cast union
union littlebig {
	char	cc[4];
	int		ii;
	float	ff;
} endian;


int
main(int argc, char **argv)
{
struct sockaddr_in	Xip;
struct sockaddr*	Xip_addr = (struct sockaddr *)&Xip;
int 				Xfd;		// our socket
char				Xip_str[20], Xport_str[8];
int					r_len, s_len, p_status;
char				r_buf[BSIZE];
char				s_buf[BSIZE];
//
int					l_index;
char				input_line[LINEMAX + 4], input_ch;
int					keep_on;
//
fd_set 				ufds;
struct timeval		timeout;
//
#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
#endif

//
// Initialize communication with X32 server at IP ip and PORT port
// Set default values to match your X32 desk
	strcpy (Xip_str, "192.168.0.64");
	strcpy (Xport_str, "10023");
//
// Manage arguments
	while ((input_ch = getopt(argc, argv, "i:h")) != (char)-1) {
		switch (input_ch) {
		case 'i':
			strcpy(Xip_str, optarg );
			break;
		default:
		case 'h':
			printf("usage: X32Tap [-i X32 console ipv4 address]\n");
			printf(" then:\n");
			printf(" '1'...'4' <cr> to select FX slot with DLY,\n");
			printf(" 'q' <cr> to exit,\n");
			printf(" <cr> to set tempo\n");
			return(0);
			break;
		}
	}
#ifdef __WIN32__
//Initialize winsock
	if (WSAStartup (MAKEWORD( 2, 2), &wsa) != 0) {
		printf ("Failed. Error Code : %d", WSAGetLastError());
		exit (EXIT_FAILURE);
	}
#endif
//
// Load the X32 address we connect to; we're a client to X32, keep it simple.
	// Create UDP socket
	if ((Xfd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror ("failed to create X32 socket");
		exit (EXIT_FAILURE);
	}
	// Construct  server sockaddr_in structure
	memset (&Xip, 0, sizeof(Xip));			// Clear struct
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
	Xip.sin_port = htons(atoi(Xport_str));	// server port/
//
//register for receiving X32 console updates
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000; //Set timeout for non blocking recvfrom(): 500ms
//
// All done. Let's send and receive messages
// Establish logical connection with X32 server
	printf(" X32Tap - v1.2 - (c)2015 Patrick-Gilles Maillot\n\n");
	printf(" '1'...'4' <cr> to select FX slot with DLY,\n");
	printf(" 'q' <cr> to exit,\n");
	printf(" <cr> to set tempo\n");
//
	keep_on = 1;
	printf("Connecting to X32.");
	s_len = Xsprint(s_buf, 0, 's', "/info");
	while (keep_on) {
		SEND  // command /info sent; read data if available
		RPOLL // poll for data in
		if ((p_status = FD_ISSET(Xfd, &ufds)) < 0) {
			printf("Polling for data failed\n");
			return 1;		// exit on receive error
		} else if (p_status > 0) {
			RECV // We have received data - process it!
			if (r_len && (strcmp(r_buf, "/info") == 0))
				break;	// Connected!
		}				// ... else timeout (~500ms)
		printf(".");
		fflush(stdout);
	}
	printf(" Done!\n"); fflush (stdout);
//
	l_index = 0;
	keep_on = 1;
	while (keep_on) {
//
#ifdef __WIN32__
		if (kbhit()) {
			input_ch = _getch();
#else
		input_ch = getc(stdin);
		{
#endif
			if (input_ch == EOL) {
				if (l_index) {
#ifdef __WIN32__
					printf("\n");
#endif
					// Check for commands
					input_line[l_index] = 0;
					if ((input_line[0] == '1') || (input_line[0] == '2') ||
						(input_line[0] == '3') || (input_line[0] == '4')) {
						Xdel_found = 0;
						Xdel_slot = (int)input_line[0] - 48;
						// interrogate X32 on what FX is there
						printf("check effect slot %d\n", Xdel_slot);
						sprintf(tmpstr, "/fx/%1d/type", Xdel_slot);
						s_len = Xsprint(s_buf, 0, 's', tmpstr);
						SEND		// send command
						RPOLL		// read UDP status
						if ((p_status = FD_ISSET(Xfd, &ufds)) > 0) {
							RECV
							if ((strcmp(r_buf, tmpstr)) == 0) {
								// FX type number starts at index 16
								for (i = 0 ; i < 4; i++) endian.cc[3 - i] = r_buf[16 + i];
								if (endian.ii == DLY) {
									printf ("Found FX!\n");
									Xdel_found = 1;
								} else {
									printf ("No DLY effect at FX#%d!\n", Xdel_slot);
								}
							}
						}
						fflush (stdout);

					} else if ((input_line[0] == 'q') || (input_line[0] == 'Q')) {
						keep_on = 0;
					}
				} else {
					// We got an empty line, meaning tap request.
					if (Xdel_found) {
						gettimeofday(&t_2, NULL);			// get precise time
						timersub(&t_2, &t_1, &t_dlta);		// t_dlta = t2 - t1
						t_1 = t_2;
						i = t_dlta.tv_sec * 1000 + t_dlta.tv_usec / 1000;
						f = (float)i / 3000.0;
						if (f < 0.) f = 0.;
						if (f > 1.) f = 1.;
						sprintf(tmpstr, "/fx/%1d/par/02", Xdel_slot);
						s_len = Xfprint(s_buf, 0, tmpstr, 'f', &f);
						SEND
						i = (int)(f * 3000.0);
						printf ("Tempo: %dms\n", i); fflush (stdout);
					}
				}
				l_index = 0;
			} else {
				if (l_index < LINEMAX) {
// parse input_ch values, check for 't' or 'T', as well as managing backspace
					if (input_ch != NO_CHAR) {
#ifdef __WIN32__
						printf("%c", input_ch);
#endif
						input_line[l_index++] = input_ch;
					}
					if (input_ch == BACKSPACE) {
#ifdef __WIN32__
						printf(" \b");
#endif
						l_index -= BACKCHARS; // remove char if backspace entered
						if (l_index < 0) l_index = 0;
					}
				} else {
					printf("!!! line too long - ignored\n");
					l_index = 0;
				}
			}
		}
	}
	close(Xfd);
	return 0;
}


