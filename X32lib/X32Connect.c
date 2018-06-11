/*
 * X32Connect.c
 *
 *  Created on: Aug 6, 2016
 *      Author: Patrick-Gilles Maillot
 *
 *
 *  interface:
 *    int X32connect(int Xconnected, char* Xip_str, int btime)
 *    Xconnected	0 if not already connected, 1 if connected
 *    Xip_str		the X32 IP address as a string (e.g. "192.168.1.64")
 *    btime			the final timeout value after connection
 *
 *    returns 1 if connection to X32 is successful
 *            0 if the connection to X32 could not be established
 *
 */
#ifdef __WIN32__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>



#define BSIZE		64		// communication buffer size

#define RPOLL													\
	do {														\
		FD_ZERO (&ufds);										\
		FD_SET (Xfd, &ufds);									\
		p_status = select(Xfd+1, &ufds, NULL, NULL, &timeout);	\
	} while (0);
//
#define RECV											\
do {													\
	r_len = recvfrom(Xfd, buf, BSIZE, 0, 0, 0);			\
} while (0);
//
#define SEND(b, l)											\
do {														\
	if (sendto (Xfd, b, l, 0, Xip_addr, Xip_len) < 0) {		\
		perror ("Error while sending data");				\
		exit (EXIT_FAILURE);								\
	}														\
} while(0);

extern int 			Xsprint(char *bd, int index, char format, void *bs);

// external references
//
extern int					Xfd;				// our socket
extern struct sockaddr_in	Xip;
extern struct sockaddr*		Xip_addr;

extern struct timeval		timeout;
extern fd_set 				ufds;
extern int					p_status;

#ifdef __WIN32__
extern WSADATA 				wsa;
extern int					Xip_len;	// length of addresses
#else
extern socklen_t			Xip_len;	// length of addresses
#endif
//
int X32Connect(int Xconnected, char* Xip_str, int btime) {
	char				buf[BSIZE];
	int					len;
//
	if (Xconnected) {
		close (Xfd);
		return 0;
	} else {
//
// initialize communication with X32 server at IP ip and PORT port
#ifdef __WIN32__
//Initialize winsock
		if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
			perror("WSA Startup Error");
			exit(EXIT_FAILURE);
		}
#endif
		// Create the UDP socket
		if ((Xfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
			perror("X32 socket creation error");
			return 0; // Make sure we don't considered being connected
		} else {
			// Construct the server sockaddr_in structure
			memset(&Xip, 0, sizeof(Xip));				// Clear struct
			Xip.sin_family = AF_INET;					// Internet/IP
			Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
			Xip.sin_port = htons(atoi("10023"));		// server port = "10023"
			// Non blocking mode
			timeout.tv_sec = 0;
			timeout.tv_usec = 100000; //Set timeout for non blocking recvfrom(): 100ms
			//
			len = Xsprint(buf, 0, 's', "/info");
			SEND(buf, len)
			RPOLL		// poll for data in
			if (p_status < 0) {
				perror("Polling for data failed");
				return 0; // Make sure we don't considered being connected
			} else if (p_status > 0) {
			// We have received data - process it!
				len = recvfrom(Xfd, buf, BSIZE, 0, 0, 0); //(struct sockaddr *)&Xip, &Xip_len);
				if (strcmp(buf, "/info") != 0) {
					perror ("Unexpected answer from X32");
					return 0;
				}
			} else {
			// time out... Not connected or Not an X32
				perror("X32 reception timeout");
					return 0; // Make sure we don't considered being connected
			}
		}
	}
	timeout.tv_usec = btime; //Set timeout for non blocking recvfrom(): 5ms
	return 1; // We are connected !
}
