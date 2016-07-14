//
// XAirGetScene.c
//
//  Created on: Oct 13, 2014 (X32GetScene.c)
//      Author: Patrick-Gilles Maillot
//
// Modified to Support XAir Series on: Jul 10, 2016
//      Author: Ken Mitchell
//
// The program connects to XAir (considers connection is always possible, no specific validation)
//
// It then asks for scene data to populate the header for the scene it will create,
// unless -s and -n parameters are provide.
//
// Scene-like data is then read from stdin to extract requests for XAir which will reply
// with /node formatted commands used to fill the newly created scene file.
//
// The created scene file's data is sent to stdout.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


#ifdef __WIN32__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

//
// XAirGetScene: An XAir Scene file generator
// Reads stdin getting an XAir Scene file to request the same data from the XAir
// Writes the resulting Scene file to stdout
//
#define BSIZE 512
extern int Xsprint(char *bd, int index, char format, void *bs);


int
main(int argc, char **argv)
{

int					i, k, read_status;
char				l_read[128], c;
struct sockaddr_in	Xip;
int fd;				// our socket
int					recvlen, sendlen;
char				b_rec[BSIZE], Xip_str[20], Xport_str[8];
char				b_snd[BSIZE];
//
#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
#endif
struct timeval		timeout;
fd_set 				ufds;
//
// initialize communication with XAir server at IP ip and PORT port
//	set a default value for server[]; change to match your XAir desk
	strcpy (Xip_str, "192.168.0.64");
//	port[] = "10023" //change to use a different port
	strcpy (Xport_str, "10024");

//
// Manage arguments
	recvlen = sendlen = 0;
	while ((c = getopt(argc, argv, "i:s:n:h")) != (char)-1) {
		switch (c) {
		case 'i':
			strcpy(Xip_str, optarg );
			break;
		case 's':
			strcpy(b_rec, optarg );
			recvlen = 1;
			break;
		case 'n':
			strcpy(b_snd, optarg );
			sendlen = 1;
			break;
		default:
		case 'h':
			printf("usage: XAirGetScene [-i XAir console ipv4 address] [-s scene name][-n note]\n");
			printf("                     default IP is 192.168.0.64\n");
			printf("reads from <stdin> scene IDs to request to XAir\n");
			printf("writes to <stdout> scene IDs and parameters in readable format (identical to *.scn)\n\n");
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
// Load the XAir address we connect to; we're a client to XAir, keep it simple.
// Open a UDP socket
	if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("failed to create XAir socket");
		return(-1);
	}
//
// Construct the server sockaddr_in structure
	memset(&Xip, 0, sizeof(Xip));				// Clear struct
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
	Xip.sin_port = htons(atoi(Xport_str));		// server port
//
// Non blocking mode
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000; //Set timeout for non blocking recvfrom(): 100ms
//
// All done. Let's send and receive messages
// Establish logical connection with XAir server
//
// Print header
	if (!recvlen) {
		printf ("Please enter scene name: ");
		scanf ("%s", b_rec);
	}
	if (!sendlen) {
		printf ("Please enter note data: ");
		scanf ("%s", b_snd);
	}
	printf ("#2.1# \"%s\" \"%s\" %%000000000 1 XAirGetScene V1.4 (c)2014 Patrick-Gilles Maillot\n\n", b_rec, b_snd);
// Open file to interpret from stdin
	k = Xsprint (b_snd, 0, 's', "/node");
	k = Xsprint (b_snd, k, 's', ",s");
	while ((read_status = scanf("%s", l_read)) != EOF) {
		if (l_read[0] == '/') {
			sendlen = Xsprint(b_snd, k, 's', l_read + 1 );
			if (sendto(fd, b_snd, sendlen, 0, (struct sockaddr *)&Xip, Xip_len) < 0) {
				perror("coundn't send data with sendto");
				return (-1);
			}
			FD_ZERO (&ufds);
			FD_SET (fd, &ufds);
			i = select(fd+1, &ufds, NULL, NULL, &timeout);
			if (i > 0) {
				recvlen = recvfrom(fd, b_rec, BSIZE, 0, (struct sockaddr *)&Xip, &Xip_len);
				if (recvlen > 0) {
					i = 0;
					while((b_rec[i] != '/') && (i < recvlen)) i++;
					// print reply to stdout (or attached file); comes with trailing \a and no \0 chars
					printf("%s", b_rec + i);
				    fflush (stdout); // make sure all data is printed out
				}
			}
		} else {
// Decide here if we want to do anything with other command (comments for example)
// Normal commands parameters should be discarded
			if (strcmp(l_read, "exit") == 0) break;
		}
	}
    fflush (stdout); // make sure all data is printed out
#ifdef __WIN32__
    WSACleanup();
#endif
	return(0);
}
