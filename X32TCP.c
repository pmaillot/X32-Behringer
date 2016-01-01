//
// X32TCP.c
//
//  Created on: Oct 3, 2015
//      Author: Patrick-Gilles Maillot
//
// Driving an X32 from TCP:
//
// The program acts as a daemon/server, reading its config from command line params
//		it opens a UDP connection with X32
//		it sets a server, binding to a TCP socket and waiting for client connections
//		upon client connection, the client data is parsed and sent to the X32 UDP connection
//		data back from the X32 is expected/available is sent as a reply towards the client
//
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#ifdef __WIN32__
#include <winsock2.h>
#else
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h> //for threading , link with lpthread
#define closesocket(s) 	close(s)
#define MySleep(a)	usleep(a*1000)
#define WSACleanup()
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#endif

#define PORT 10041
#define BACKLOG 10
#define TIMEOUT 10

#define BSIZE				512		// send/receive buffer sizes
#define XREMOTE_TIMEOUT		9		// timeout set to 9 seconds
//
int X32debug, X32verbose;
//
// Macros:
//
#ifdef __WIN32__
#define millisleep(x)	Sleep(x)
#define skt_close(s)	closesocket(s)
#else
#define millisleep(x)	usleep(x*1000)
#define skt_close(s)	close(s)
#endif

// External calls used
extern void Xfdump(char *header, char *buf, int len, int debug);
extern void Xsdump(char *out, char *buf, int len);
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int Xcparse(char *buf, char *line);

//
//
void *serve_client_handler(int client_sock);
void XRcvClean(int X_fd);
int X32_init();

//
// X32 data in global dataset

struct sockaddr_in Xip;					// X32 IP
struct sockaddr* Xip_addr = (struct sockaddr *) &Xip;
char Xip_str[20], Xport_str[8];
//
//
#ifdef __WIN32__
int Xip_len = sizeof(Xip);	// length of addresses
struct timeval timeout;
#else
socklen_t Xip_len = sizeof(Xip);	// length of addresses
#endif
//
//
//
int main(int argc, char **argv) {
	struct sockaddr_in Sip;					// Server IP
	struct sockaddr* Sip_addr = (struct sockaddr *) &Sip;
	char Sport_str[8];
	struct sockaddr_in Cip;					// Client IP
	struct sockaddr* Cip_addr = (struct sockaddr *) &Cip;
	int S_fd, C_fd;				// Server, Client sockets
#ifdef __WIN32__
	WSADATA wsa;
	int Sip_len = sizeof(Sip);	// length of addresses
	int Cip_len = sizeof(Cip);	// length of addresses
	HANDLE thread;
#else
	socklen_t Sip_len = sizeof(Sip);	// length of addresses
	socklen_t Cip_len = sizeof(Cip);    // length of addresses
	pthread_t thread;
#endif
//
	int backlog, reuse_addr = 1;
	char input_ch;
//
//  Set (temporary) default value for X32 IP
	strcpy(Xip_str, "127.0.0.1");
//  Set (temporary) default value for server port
	strcpy(Sport_str, "10041");
	X32debug = 0;
	X32verbose = 0;
	backlog = BACKLOG;
//
// manage command line arguments
	while ((input_ch = getopt(argc, argv, "b:i:d:p:v:h")) != (char) -1) {
		switch (input_ch) {
		case 'b':
			sscanf(optarg, "%d", &backlog);
			break;
		case 'i':
			strcpy(Xip_str, optarg);
			break;
		case 'd':
			sscanf(optarg, "%d", &X32debug);
			break;
		case 'p':
			strcpy(Sport_str, optarg);
			break;
		case 'v':
			sscanf(optarg, "%d", &X32verbose);
			break;
		default:
		case 'h':
			printf("usage: X32TCP [-b [10] server max connections backlog]\n");
			printf("              [-i X32 console ipv4 address]\n");
			printf("              [-d 0/1, [0], debug option]\n");
			printf("              [-v 0/1  [1], verbose option]\n");
			printf("              [-p [10041] server port]\n\n");
			printf("   After starting, the server waits for clients to connect and send X32_command\n");
			printf("   format like commands, sent as character strings to the TCP server.\n");
			printf("   All commands from connected clients are parsed and formatted to X32 OSC standard\n");
			printf("   before being sent to X32. If X32 answers, the X32 OSC data is formatted to readable\n");
			printf("   format before being returned to the connected client as a string (null characters\n");
			printf("   are replaced with '~' to ease printing or parsing).\n");
			printf("   If no answer after a timeout of 10ms from X32, the string 'no data' is returned.\n\n");
			printf("   The command 'exit' from a connected client input closes the respective client stream.\n");
			return 0;
			break;
		}
	}
//
#ifdef __WIN32__
//Initialize winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(1);
	}
#endif
//
	printf(" X32TCP - v0.90 - (c)2015 Patrick-Gilles Maillot\n\n");
	fflush(stdout);
//
	if ((S_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket failure!!\n");
		exit(1);
	}
	if (setsockopt(S_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse_addr,
			sizeof(reuse_addr)) < 0) {
		printf("Call to setsockopt() error");
		exit(1);
	}
	memset(&Sip, 0, sizeof(Sip));
	memset(&Cip, 0, sizeof(Cip));
	Sip.sin_family = AF_INET;
	Sip.sin_port = htons(PORT);
	Sip.sin_addr.s_addr = INADDR_ANY;
	if ((bind(S_fd, Sip_addr, Sip_len)) < 0) { //sizeof(struct sockaddr)
		printf("Binding Failure\n");
		exit(1);
	}
	if ((listen(S_fd, backlog)) < 0) {
		printf("Listening Failure\n");
		exit(1);
	}
	if (X32verbose) {
		printf("Server now accepting Client connections\n");
		fflush(stdout);
	}
	while (1) {
		if ((C_fd = accept(S_fd, Cip_addr, &Cip_len)) < 0) {
			perror("accept");
			exit(1);
		}
		if (X32debug) {
			printf("Server got connection from client %s\n",
					inet_ntoa(Cip.sin_addr));
			fflush(stdout);
		}
#ifdef __WIN32__
		if ((thread = CreateThread(NULL, 0,
				(LPTHREAD_START_ROUTINE) serve_client_handler, (LPVOID) C_fd, 0, NULL)) < 0) {
#else
		if ((pthread_create(&thread, NULL, serve_client_handler, C_fd)) < 0) {
#endif
			printf("Could not create thread\n");
		}
		if (X32debug) {
			printf("Thread 0x%x created\n", (unsigned int) thread);
			fflush(stdout);
		}
//		serve_client_handler(C_fd);
	} // While serving clients
//
	skt_close(S_fd);
	WSACleanup();
	return 0;
} //End of main

void *serve_client_handler(int ssock) {

	char c_buf[BSIZE], r_buf[BSIZE], s_buf[BSIZE], o_buf[BSIZE];
	int c_len, r_len, s_len;
	int p_status;
	int X_fd;	// X32 socket returned by X32_init()

	fd_set ufds1;

	int i, keep_on = 1;

//
// Initialize X32 connection for the client to obtain an X32 socket descriptor
	if ((X_fd = X32_init()) <= 0)
		exit(1);
//
// Free X32 buffer from any remaining data
	XRcvClean(X_fd);
	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUT * 1000; //Set timeout in ms
	if (X32verbose) {
		printf("New client connected!\n");
		fflush(stdout);
	}
	if (X32debug) {
		printf("Thread using socket: %d\n", ssock);
		fflush(stdout);
	}
	while (keep_on) {
//
// Read from TCP client
		if ((c_len = recv(ssock, c_buf, BSIZE, 0)) < 0) {
			printf("Error reading Client data\n");
			keep_on = 0;
			break;
		} else if (c_len > 0) {
//
// Send to X32, after data interpretation
			c_buf[--c_len] = '\0';
			if (c_len) {
//				printf("nb: %d,", c_len);
//				for (i = 0; i< c_len; i++) printf("%2x ", c_buf[i]); printf("\n"); fflush(stdout);
				if (strncmp(c_buf, "exit", 4) == 0)
					break; // exit while() and close socket.
				if (X32debug) {
					printf("server received from TCP client %d bytes:\n", c_len);
					for (i = 0; i < c_len; i++)
						printf("%02x ", c_buf[i]);
					printf("\n");
					fflush(stdout);
				}
				s_len = Xcparse(s_buf, c_buf);
				if (X32debug) {
					printf("server sends to UDP server: %s %d\n", s_buf, s_len);
					fflush(stdout);
				}
				if (sendto(X_fd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {
					perror("Error while sending data");
					keep_on = 0;
					break;
				}
			}
			FD_ZERO(&ufds1);
			FD_SET(X_fd, &ufds1);
			if ((p_status = select(X_fd + 1, &ufds1, NULL, NULL, &timeout)) != -1) {
				if (FD_ISSET(X_fd, &ufds1) > 0) {
					if (X32debug) {
						printf("Server reading from UDP: p_status = %d\n", p_status);
						fflush(stdout);
					}
					r_len = recvfrom(X_fd, r_buf, BSIZE, 0, 0, 0);
					if (X32debug) {
						Xfdump("X->", r_buf, r_len, 0);
						fflush(stdout);
					}
					r_buf[r_len] = '\0';
					if (r_len > 0) {
					//
					// Make UDP message readable for TCP client
						Xsdump(o_buf, r_buf, r_len);
						if (X32debug) {
							printf(
									"Server received: %s, %d from UDP; sending to TCP client\n",
									o_buf, strlen(o_buf));
							fflush(stdout);
						}
						if ((send(ssock, o_buf, strlen(o_buf), 0)) < 0) {
							printf("Failure Sending Message back to Client\n");
							keep_on = 0;
							break;
						}
						if (X32debug) {
							printf("Server sent: %s; Number of bytes sent: %d\n",
									r_buf, r_len);
							fflush(stdout);
						}
					} else {
						if (X32debug) {
							printf("Server received no data from UDP\n");
							fflush(stdout);
						}
					}

				} else {
					if (X32debug) {
						printf("No data after timeout\n");
						fflush(stdout);
					}
					if ((send(ssock, "no data", 7, 0)) < 0) {
						printf("Failure Sending Message back to Client\n");
						keep_on = 0;
						break;
					}
				}
			} else {
				printf("Reading for X32 data failed\n");
				keep_on = 0;
			}
		}
	} //  While keep_on...
//
//Close Connection Socket
	skt_close(ssock);
	if (X32verbose) {
		printf("Client socket closed\n");
		fflush(stdout);
	}
	return (0);
}

int X32_init() {

	int x_len, p_status;
	char x_buf[BSIZE];
	int X_fd;					// X32 socket

	fd_set ufds0;
//
// Load the X32 address we connect to; we're a client to X32, keep it simple.
	// Initialize communication with X32 server at IP ip and PORT port
	// Set default values to match your X32 desk
	strcpy(Xport_str, "10023");
	// Create UDP socket
	if ((X_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("failed to create X32 socket");
		return (0);
	}
	// Construct  server sockaddr_in structure
	memset(&Xip, 0, sizeof(Xip));			// Clear struct
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
	Xip.sin_port = htons(atoi(Xport_str));	// server port/
//
//register for receiving X32 console updates
// Non blocking mode
	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUT * 1000; //Set timeout in ms
//
// All done. Let's send and receive messages
// Establish logical connection with X32 server
	if (X32verbose) {
		printf("Connecting to X32.");
		fflush(stdout);
	}
	while (1) {
		if (sendto(X_fd, "/info\0\0\0", 8, 0, Xip_addr, Xip_len) < 0) {
			perror("Error while sending data");
			return (0);
		}
		FD_ZERO(&ufds0);
		FD_SET(X_fd, &ufds0);
		if((p_status = select(X_fd + 1, &ufds0, NULL, NULL, &timeout)) != -1) {
			if (FD_ISSET(X_fd, &ufds0) > 0) {
				x_len = recvfrom(X_fd, x_buf, BSIZE, 0, 0, 0);
				if (X32debug) {
					Xfdump("X->", x_buf, x_len, 0);
					fflush(stdout);
				}
				if (strcmp(x_buf, "/info") == 0)
					break;	// Connected!
			}				// ... else timeout
			if (X32verbose) {
				printf(".");
				fflush(stdout);
			}
			millisleep(500); // wait a little before asking again
		} else {
			perror("Error while reading data");
			return (0);
		}
	}
	if (X32verbose) {
		printf(" Done!\n");
		fflush(stdout);
	}
	return (X_fd);
}

void XRcvClean(int X_fd) {
//
// Empty reception buffer while data is available
//
	int x_len, p_status;
	char x_buf[BSIZE];
	fd_set ufds2;

	do {
		FD_ZERO(&ufds2);
		FD_SET(X_fd, &ufds2);
		if ((p_status = select(X_fd + 1, &ufds2, NULL, NULL, &timeout)) != -1) {
			x_len = 0;
			if (FD_ISSET(X_fd, &ufds2) > 0) {
				x_len = recvfrom(X_fd, x_buf, BSIZE, 0, 0, 0);
			}
			if (X32debug) {
				printf("XrcvClean: p_status = %d, x_len = %d\n", p_status, x_len);
				fflush(stdout);
			}
		}
	} while (p_status > 0);
	return;
}

