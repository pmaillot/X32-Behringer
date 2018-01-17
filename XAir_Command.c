//
// XAir_Command.c
//
//  Created on: Sep 19, 2014
//
//      XAir_Command: a simple udp client for XR12, 16 or 18 sending commands and getting answers
//
//      Author: Patrick-Gilles Maillot
//
// Changelog:
// v 1.27: remove incorrect use of macro FD_ISSET(Xfd, &ufds) in receiving IO.
// v 1.28: remove incorrect use of FD_ISSET() in buffer check.
// v 1.29: Change to X32_cparse.c to accept strings with space chars.
// v 1.30: Change line_size to 512 chars
// v 1.31: added 's' flag to read/send scene/snippets/tidbits/X32node lines from file
// v 1.32: longer timeout when read/send scene/snippets/tidbits/X32node lines from file
// v 1.34: fixed comparison with wrong text
// v 1.35: fixed meters case by cloning xfdump() in this file
// v 1.36: fixed meters data length error
// v 1.37: addresses limitations in certain C compilers wit getopt()
// v 1.38: kb input is now treated as int
// v 1.39: following changes to X32_cparse.c
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#ifdef __WIN32__
#include <winsock2.h>
#include <conio.h>
#define millisleep(a)	Sleep(a)
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#define closesocket(s) 	close(s)
#define millisleep(a)	usleep(a*1000)
#define WSACleanup()
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#endif

// Private calls used

void Xfdump(char *header, char *buf, int len, int debug);
void Xdump(char *buf, int len, int debug);

// External calls used
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int Xcparse(char *buf, char *line);


#ifdef __WIN32__
	#define BACKSPACE	8		// need to use <ctl>h
	#define EOL			13		// enter key
	#define BACKCHARS	2		// 2 chars to remove from line
	#define NO_CHAR		-1		// not a real/printable char
#else
	#define BACKSPACE	8		// backspace key OK
	#define EOL			'\n'	// enter key
	#define BACKCHARS	1		// 1 char to remove from line
	#define NO_CHAR		-1		// not a real/printable char
#endif

//
#define LINEMAX				512		// input line buffer size
#define BSIZE				512		// send/receive buffer sizes
#define XREMOTE_TIMEOUT		9		// timeout set to 9 seconds
//
int X32debug = 0;
int X32verbose = 1;
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
#define RECV																\
do {																		\
	r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);							\
	if (X32verbose) {Xfdump("X->", r_buf, r_len, X32debug); fflush(stdout);}\
} while (0);
//
#define SEND																\
do {																		\
	if (X32verbose) {Xfdump("->X", s_buf, s_len, X32debug); fflush(stdout);}\
	if (s_delay) millisleep (s_delay);										\
	if (sendto (Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {				\
		perror ("Error while sending data");								\
		exit (EXIT_FAILURE);												\
	}																		\
} while(0);
//
#define XREMOTE()															\
do {																		\
	if (xremote_on) {														\
		now = time(NULL); 													\
		if (now > before + XREMOTE_TIMEOUT) { 								\
			if (X32debug) {Xfdump("->X", xremote, 12, 0); fflush(stdout);}	\
			if (sendto (Xfd, xremote, 12, 0, Xip_addr, Xip_len) < 0) {		\
				perror ("coundn't send data to XR18");						\
				exit (EXIT_FAILURE);										\
			}																\
			before = now;													\
		}																	\
	}																		\
} while (0);
//
#define CHECKXR()			\
	do {					\
		RPOLL 				\
		if (p_status > 0) {	\
			RECV			\
		}					\
	} while (p_status > 0);
//
//
#define TESTINPUT()																						\
	else if (strcmp(input_line, "exit") == 0) keep_on = 0;												\
	else if (strcmp(input_line, "quit") == 0) keep_on = 0;												\
	else if (strcmp(input_line, "xremote") == 0) printf(":: xremote is %s\n",((xremote_on)?"on":"off"));\
	else if (strcmp(input_line, "xremote off") == 0) 	xremote_on = 0;									\
	else if (strcmp(input_line, "xremote on") == 0) 	xremote_on = 1;									\
	else if (strcmp(input_line, "verbose") == 0) printf(":: verbose is %s\n",((X32verbose)?"on":"off"));\
	else if (strcmp(input_line, "verbose off") == 0) 	X32verbose = 0;									\
	else if (strcmp(input_line, "verbose on") == 0) 	X32verbose = 1;									\


// specific to XR series: xdump and xfdump are copied/included here as meters are different (more consistent)
// than on X32
void Xdump(char *buf, int len, int debug)
{
	int i, k, n, j, l, comma = 0, data = 0, dtc = 0;
	unsigned char c;
	union littlebig {
		char	c1[4];
		short	si[2];
		int		i1;
		float	f1;
	} endian;

	for (i = 0; i < len; i++) {
		c = (unsigned char)buf[i];
		if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
		if (debug) printf(" %c", c);
		else printf("%c", c);
		if (c == ',') {
			comma = i;
			dtc = 1;
		}
		if (dtc && (buf[i] == 0)) {
			data = (i + 4) & ~3;
			for (dtc = i + 1; dtc < data ; dtc++) {
				if (dtc < len) {
					if (debug) printf(" ~");
					else printf("~");
				}
			}
			dtc = 0;
			l = data;
			while (++comma < l && data < len) {
				switch (buf[comma]) {
				case 's':
					k = (strlen((char*)(buf+data)) + 4) & ~3;
					for (j = 0; j < k; j++) {
						if (data < len) {
							c = (unsigned char)buf[data++];
							if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
							if (debug) printf(" %c", c);
							else printf("%c", c);
						}
					}
					break;
				case 'i':
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					printf("[%6d]", endian.i1);
					break;
				case 'f':
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					if (endian.f1 < 10.) printf("[%06.4f]", endian.f1);
					else if (endian.f1 < 100.) printf("[%06.3f]", endian.f1);
					else if (endian.f1 < 1000.) printf("[%06.2f]", endian.f1);
					else if (endian.f1 < 10000.) printf("[%06.1f]", endian.f1);
					break;
				case 'b':
					// Get the number of bytes
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					n = endian.i1;
					// Get the number of data (floats or ints ???) in little-endian format
					for (k = 0; k < 4; endian.c1[k++] = buf[data++]);
					if (n == endian.i1) {
						// Display blob as string
						printf("%d chrs: ", n);
						for (j = 0; j < n; j++) printf("%c ", buf[data++]);
					} else {
						n = endian.i1;
						// Display blob depending on command
						if(strncmp(buf, "/meters/", 8) == 0) {
							printf("%d rta: \n", n);
							for (j = 0; j < n; j++) {
								// data as short ints, little-endian format
								for (k = 0; k < 2; endian.c1[k++] = buf[data++]);
								endian.f1 = (float)endian.si[0] / 256.0;
								printf("%07.2f ", endian.f1);
							}
						} else {
							printf("%d flts: ", n);
							for (j = 0; j < n; j++) {
								// floats are little-endian format
								for (k = 0; k < 4; endian.c1[k++] = buf[data++]);
								printf("%06.2f ", endian.f1);
							}
						}
					}
					break;
				default:
					break;
				}
			}
			i = data - 1;
		}
	}
	printf("\n");
}
//
//
void Xfdump(char *header, char *buf, int len, int debug) {
int i;

if (debug) {
		printf("%s, %4d B: ", header, len);
		for (i = 0; i < len; i++) {
			printf("%02x", (unsigned char)buf[i]);
		}
		printf("\n");
	}
	if (debug) for (i = 0; i < (strlen(header) + 10); i++) printf(" ");
	else       printf("%s, %4d B: ", header, len);
	Xdump(buf, len, debug);
}
//
//
//
//
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
int					xremote_on;
char				xremote[12] = "/xremote";			// automatic trailing zeroes
int					l_index;
char				input_line[LINEMAX + 4];
int					input_intch;						// addresses limitations in certain C compilers wit getopt()
int					keep_on, do_keyboard, s_delay, filein;
FILE*				fdk = NULL;
time_t				before, now;
//
fd_set 				ufds;
struct timeval		timeout;
//
#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
unsigned long 		mode;
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
#endif
//
// Initialize communication with XR server at IP ip and PORT port
// Set default values to match your XR desk
	strcpy (Xip_str, "192.168.0.64");
	strcpy (Xport_str, "10024");
//
// Manage arguments
	filein = 0;
	do_keyboard = 1;
	s_delay = 10;
// Removed "s" option as it's not support by XAir series
//	while ((input_intch = getopt(argc, argv, "i:d:k:f:s:t:v:h")) != -1) {
	while ((input_intch = getopt(argc, argv, "i:d:k:f:t:v:h")) != -1) {
		switch (input_intch) {
		case 'i':
			strcpy(Xip_str, optarg );
			break;
		case 'd':
			sscanf(optarg, "%d", &X32debug);
			break;
		case 'f':
			filein = 1;
			sscanf(optarg, "%s", input_line);
			break;
		case 'k':
			sscanf(optarg, "%d", &do_keyboard);
			break;
//		case 's':
//			filein = 2;
//			sscanf(optarg, "%s", input_line);
//			break;
		case 't':
			sscanf(optarg, "%d", &s_delay);
			break;
		case 'v':
			sscanf(optarg, "%d", &X32verbose);
			break;
		default:
		case 'h':
			printf("usage: XAir_Command [-i X32 console ipv4 address]\n");
			printf("                    [-d 0/1, [0], debug option]\n");
			printf("                    [-v 0/1  [1], verbose option]\n");
			printf("                    [-k 0/1  [1], keyboard mode on]\n");
			printf("                    [-t int  [10], delay between batch commands in ms]\n");
//			printf("                    [-s file, reads X32node formatted data lines from 'file']\n");
			printf("                    [-f file, sets batch mode on, getting input data from 'file']\n");
			printf("                     default IP is 192.168.0.64\n\n");
//			printf(" If option -s file is used, the program reads data from the provided file \n");
//			printf(" until EOF has been reached, and exits after that.\n\n");
			printf(" If option -f file is used, the program runs in batch mode, taking data from\n");
			printf(" the provided file until EOF has been reached, or 'exit' or 'kill' entered.\n\n");
			printf(" If not killed or no -f option, the program runs in standard mode, taking data\n");
			printf(" from the keyboard or <stdin> on linux systems.\n");
			printf(" While executing, the following commands can be used:\n");
			printf("   '#line of text.....': will print out the input line as a comment line\n");
			printf("   'exit' | 'quit': will quit the current mode\n");
			printf("   'kill': [batch mode only] will exit the progam\n");
			printf("   'time <value>': will change the delay between batch commands to <value>\n");
			printf("   'verbose <on|off>': will change the verbose mode\n");
			printf("   'verbose': will return the current verbose mode\n");
			printf("   'xremote <on|off>': will change the xremote mode\n");
			printf("   'xremote': will return the current xremote mode\n");
			printf("   '' (empty line) [standard mode only]: will repeat the last entered command\n\n");
			printf(" All other commands are parsed and sent to XR18.\n");
			printf(" Typical XR18 command structure:\n");
			printf("   <command> [<format> [<data> [<data> [...]]]], where for example:\n");
			printf("      command: /info, /status, /ch/00/mix/fader, ...\n");
			printf("      format: ',i' ',f' ',s' or a combination: ',siss' ',ffiss' ...\n");
			printf("      data: a list of int, float or string types separated by a space char...\n\n");
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
// Load the XR18 address we connect to; we're a client to XR18, keep it simple.
	// Create UDP socket
	if ((Xfd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror ("failed to create XR18 socket");
		exit (EXIT_FAILURE);
	}
	// Construct  server sockaddr_in structure
	memset (&Xip, 0, sizeof(Xip));				// Clear struct
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
	Xip.sin_port = htons(atoi(Xport_str));		// server port/
//
// Set receiving from XR18 to non blocking mode
// The 500ms timeout is used for delaying the printing of '.' at startup.
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000; //Set timeout for non blocking recvfrom(): 500ms
	FD_ZERO(&ufds);
	FD_SET(Xfd, &ufds);
// make stdin (fd = 0) I/O nonblocking
#ifdef __WIN32__
	mode = 1;
	p_status = ioctlsocket(0, FIONBIO, &mode);

#else
	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);
#endif
//
// All done. Let's send and receive messages
// Establish logical connection with XR18 server
	printf(" XAir_Command - v1.39 - (c)2014-18 Patrick-Gilles Maillot\n\nConnecting to XR18.");
//
	keep_on = 1;
	xremote_on = X32verbose;	// Momentarily save X32verbose
	X32verbose = 0;
	s_len = Xsprint(s_buf, 0, 's', "/xinfo");
	while (keep_on) {
		SEND  				// command /info sent;
		RPOLL 				// read data if available
		if (p_status < 0) {
			printf("Polling for data failed\n");
			return 1;		// exit on receive error
		} else if (p_status > 0) {
			RECV 			// We have received data - process it!
			if (strcmp(r_buf, "/xinfo") == 0)
				break;		// Connected!
		}					// ... else timeout
		printf("."); fflush(stdout);
	}
	printf(" Done!\n");
//
// Set 1ms timeout to get faster response from XR18 (when testing for /xremote data).
	timeout.tv_usec = 1000; 	//Set timeout for non blocking recvfrom(): 1ms
	X32verbose = xremote_on;	// Restore X32verbose
	xremote_on = 0;
	l_index = 0;
	before = 0;
	if (filein) {
		if ((fdk = fopen(input_line, "r")) == NULL) {
			printf ("Cannot read file: %s\n", input_line);
			exit(EXIT_FAILURE);
		}
		if (filein == 1) {
			while (keep_on) {
				XREMOTE()
				if (fgets(input_line, LINEMAX, fdk)) {
					input_line[strlen(input_line) - 1] = 0;
					// Check for program batch mode commands
					if (input_line[0] == '#') printf("---comment: %s\n", input_line);
					TESTINPUT()			// Test for input data checks
					// Additional batch-mode input data checks
					else if (strcmp(input_line, "kill") == 0) {keep_on = 0; do_keyboard = 0;}
					else if (strncmp(input_line, "time", 4) == 0) {sscanf(input_line+5, "%d", &s_delay); printf(":: delay is: %d\n", s_delay);}
					else if (strlen(input_line) > 1) { // XR18 command line
						s_len = Xcparse(s_buf, input_line);
						SEND			// send batch command
					}
					CHECKXR()			// Check if XR sent something back
				} else {
					keep_on = 0;
				}
			}
			printf ("---end of batch mode file\n");
			fflush (stdout);
			if (fdk) fclose(fdk);
		} else {	// filein = 2 ('s' option)
			timeout.tv_usec = 10000; // Set timeout to 10ms
			do_keyboard = 0;	// force exit after end-of-file
			while (fgets(input_line, LINEMAX, fdk) != NULL) {
				// skip comment lines
				if (input_line[0] != '#') {
					input_line[strlen(input_line) - 1] = 0;	// avoid trailing '\n'
					s_len = Xsprint(s_buf, 0, 's', "/");
					s_len = Xsprint(s_buf, s_len, 's', ",s");
					s_len = Xsprint(s_buf, s_len, 's', input_line);
					SEND				// send data to XR
					CHECKXR()		// XR18 will echo back the line
				}
			}
            printf ("---end of file\n");
            fflush (stdout);
            if (fdk) fclose(fdk);
		}
	}
// Done with the file (if there was one)
// revert to keyboard/interactive mode if enabled
	if (do_keyboard) {
		keep_on = 1;
		while (keep_on) {
			XREMOTE()	// process /xremote if needded
		    // build command by reading keyboard characters (from stdin)
#ifdef __WIN32__
			if (kbhit()) {
				input_intch = _getch();
#else
			input_intch = getc(stdin);
			{
#endif
				if (input_intch == EOL) {
					if (l_index) {
#ifdef __WIN32__
						printf("\n");
#endif
						// Check for program interactive mode commands
						input_line[l_index] = 0;
						if (input_line[0] == '#') printf("---comment: %s\n", input_line);
						TESTINPUT()			// Test for input data checks
						else { 				// new line/command
							s_len = Xcparse(s_buf, input_line);
							SEND // send parsed data
						}
						l_index = 0;
					} else {
						printf ("resending\n");
						SEND // empty line: resend previous parsed data
					}
				} else {
					if (l_index < LINEMAX) {
						// parse input_intch values, building new command
						if (input_intch != NO_CHAR) {
#ifdef __WIN32__
							printf("%c", (char)input_intch);
#endif
							input_line[l_index++] = (char)input_intch;
						}
						if (input_intch == BACKSPACE) {
#ifdef __WIN32__
							printf(" \b");
#endif
							l_index -= BACKCHARS; // remove char if backspace entered
							if (l_index < 0) l_index = 0;
						}
					} else {
						printf("!!! line too long - ignored line\n");
						l_index = 0;
					}
				}
			}
			CHECKXR()			// Check if XR sent something back
		}
	}
	close(Xfd);
	return 0;
}
