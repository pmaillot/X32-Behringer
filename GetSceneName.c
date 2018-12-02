//
// GetSceneName.c
//
//  Created on: Nov 25, 2018
//
//      GetSceneName: a command line utility to get scene names when a scene change takes place
//
//     ©Patrick-Gilles Maillot
//
// Changelog:
// v 0.1:	Creation of the tool
// v 0.2:	Added information to the data returned to caller (scene ID)
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#ifdef __WIN32__
#include <windows.h>
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define closesocket(s) 	close(s)
#define WSACleanup()
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#endif
//
// External calls used
extern int Xsprint(char *bd, int index, char format, void *bs);
//
//
#define BSIZE				256		// send/receive buffer sizes
#define XREMOTE_TIMEOUT		9		// timeout set to 9 seconds
//
// Macros:
//
#define RPOLL													\
	do {														\
		FD_ZERO (&ufds);										\
		FD_SET (Xfd, &ufds);									\
		p_status = select(Xfd+1, &ufds, NULL, NULL, &timeout);	\
	} while (0);
//
#define RECV																	\
do {																			\
	r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);								\
} while (0);
//
#define SEND																	\
do {																			\
	if (sendto (Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {					\
		perror ("Error while sending data");									\
		exit (EXIT_FAILURE);													\
	}																			\
} while(0);
//
#define XREMOTE()																\
do {																			\
	now = time(NULL); 															\
	if (now > before + XREMOTE_TIMEOUT) { 										\
		if (sendto (Xfd, xremote, 12, 0, Xip_addr, Xip_len) < 0) {				\
			perror ("coundn't send data to X32");								\
			exit (EXIT_FAILURE);												\
		}																		\
		before = now;															\
		if (sendto (Xfd, xshwctl, 32, 0, Xip_addr, Xip_len) < 0) {				\
			perror ("coundn't send data to X32");								\
			exit (EXIT_FAILURE);												\
		}																		\
	}																			\
} while (0);
//
//
// type cast union
union littlebig {
	char cc[4];
	int ii;
	float ff;
} endian;
//
//
int
main(int argc, char **argv) {

struct sockaddr_in	Xip;
struct sockaddr*	Xip_addr = (struct sockaddr *)&Xip;
int 				Xfd;		// our socket
char				Xip_str[20], Xport_str[8];
int					r_len, s_len, p_status;
char				r_buf[BSIZE];
char				s_buf[BSIZE];
//
char				xremote[12] = "/xremote"; // automatic trailing zeroes
char				xshwctl[32] = "/-prefs/show_control\0\0\0\0,i\0\0\0\0\0\1";
int					input_intch, keep_on, i, verbose, onetime;
time_t				before, now;
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
	strcpy (Xip_str, "192.168.1.62");
	strcpy (Xport_str, "10023");
	verbose = 1;
	onetime = 1;
//
// Manage arguments
	while ((input_intch = getopt(argc, argv, "i:v:o:h")) != -1) {
		switch ((char)input_intch) {
		case 'i':
			strcpy(Xip_str, optarg );
			break;
		case 'v':
			sscanf(optarg, "%d", &verbose);
			break;
		case 'o':
			sscanf(optarg, "%d", &onetime);
			break;
			default:
		case 'h':
			printf("usage: GetSceneName [-i X32 console ipv4 address]\n");
			printf("                     default IP is 192.168.1.62\n\n");
			printf("                    [-v 0|1 prints welcome and connection status messages [default: 1]]\n");
			printf("                    [-o 0|1 exits at first occurrence [default: 1]]\n");
			printf("       Connects to X32 and scans for new scenes being loaded; when this happens,\n");
			printf("       the newly loaded scene name will be echoed to <stdout>\n\n");
			return(0);
			break;
		}
	}
#ifdef __WIN32__
//Initialize winsock
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
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
	memset (&Xip, 0, sizeof(Xip));				// Clear struct
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
	Xip.sin_port = htons(atoi(Xport_str));		// server port/
//
// Set receiving from X32 to non blocking mode
// The 500ms timeout is used for delaying the printing of '.' at startup.
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000; //Set timeout for non blocking recvfrom(): 500ms
	FD_ZERO(&ufds);
	FD_SET(Xfd, &ufds);
//
// All done. Let's send and receive messages
// Establish logical connection with X32 server
	if (verbose) printf(" GetSceneName - v0.2 - (c)2018 Patrick-Gilles Maillot\n\nConnecting to X32.");
//
	keep_on = 1;
	s_len = Xsprint(s_buf, 0, 's', "/info");
	while (keep_on) {
		SEND  				// command /info sent;
		RPOLL 				// read data if available
		if (p_status < 0) {
			printf("Polling for data failed\n");
			return 1;		// exit on receive error
		} else if (p_status > 0) {
			RECV 			// We have received data - process it!
			if (strcmp(r_buf, "/info") == 0)
				break;		// Connected!
		}					// ... else timeout
		if (verbose) {
			printf(".");
			fflush(stdout);
		}
	}
	if (verbose) printf(" Done!\n");
//
// Set 1ms timeout to get faster response from X32 (when testing for /xremote data).
	timeout.tv_usec = 1000;

	before = 0;
	keep_on = 1;
	while (keep_on) {
		XREMOTE()	// process /xremote & send show_control command for scenes
		RPOLL
		if (p_status > 0) {
			RECV				// Check if X32 sent something back
			if (r_len > 24) {	// avoids a warning at compile time and we're interested in > 24 bytes replies
				// only interested in "/-show/prepos/current" commands
				if (strcmp(r_buf, "/-show/prepos/current") == 0) {
					// get the scene index
					for (i = 0; i < 4; i++) endian.cc[i] = r_buf[31-i];
					sprintf(s_buf, "/-show/showfile/scene/%03d", endian.ii);
					s_len = Xsprint(s_buf, 0, 's', s_buf);
					SEND
				}
				// ...or in "/-show/showfile/scene" commands
				if (strncmp(r_buf, "/-show/showfile/scene", 21) == 0) {
					printf("%02d - %s\n", endian.ii, r_buf+36);
					fflush(stdout);
					if (onetime) keep_on = 0;
				}
			}
		}
	}
	close(Xfd);
	return 0;
}
