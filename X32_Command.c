//
// X32_Command.c
//
//  Created on: Sep 19, 2014
//
//      X32_Command: a simple udp client for X32 sending commands and getting answers
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
// v 1.33: added netinet/in.h include (freeBSD support)
// v 1.34: addresses limitations in certain C compilers with getopt()
// v 1.35: kb input is now treated as int
// v 1.36: following changes to X32_cparse.c
// v 1.37: code cleaning
// v 1.38: finally got rid of call to kbhit() which was causing issues with ",`, %, even
//         for non-international US keyboard
// v 1.39: corrected handling of non-printable characters, and ctrl-V
// v 1.40: enable autoconnect if no IP data is provided
// v 1.41: correct handling of file names with space (option -f and -s)
// v 1.42: correctly re-init keep_on in the case of file1==1
// v 1.43: Better handling of ctrl-V
// v 1.44: added a page-up command to restore last line sent
// v 1.45: Using keyboard thread function to better manage user inputs
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#ifdef __WIN32__
#include <windows.h>
char					broadcast = 1;
#define millisleep(a)	Sleep(a)
HANDLE 					clip;
HANDLE					thread;
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/fcntl.h>
int						broadcast = 1;
#define closesocket(s) 	close((s))
#define millisleep(a)	usleep((a)*1000)
#define WSACleanup()
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
pthread_t thread;
#endif



// External calls used
extern void Xfdump(char *header, char *buf, int len, int debug);
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int Xcparse(char *buf, char *line);


//
#define LINEMAX				512		// input line buffer size
#define BSIZE				512		// send/receive buffer sizes
#define XREMOTE_TIMEOUT		9		// timeout set to 9 seconds
//
int 				InputReady = 0;
int 				X32debug = 0;
int 				X32verbose = 1;
struct sockaddr_in	Xip;
struct sockaddr*	Xip_addr = (struct sockaddr *)&Xip;
int 				Xfd;		// our socket
char				Xip_str[20], Xport_str[8];
int					r_len, s_len, p_status;
char				r_buf[BSIZE];
char				s_buf[BSIZE];
char				InputLine[LINEMAX + 4];
size_t		 		s_InputLine = sizeof(InputLine);
char*        		pt_InputLine = InputLine;
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
	if (X32verbose) {Xfdump("X->", r_buf, r_len, X32debug); fflush(stdout);}	\
} while (0);
//
#define SEND																	\
do {																			\
	if (X32verbose) {Xfdump("->X", s_buf, s_len, X32debug); fflush(stdout);}	\
	if (s_delay) millisleep (s_delay);											\
	if (sendto (Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {					\
		perror ("Error while sending data");									\
		exit (EXIT_FAILURE);													\
	}																			\
} while(0);
//
#define XREMOTE()																\
do {																			\
	if (xremote_on) {															\
		now = time(NULL); 														\
		if (now > before + XREMOTE_TIMEOUT) { 									\
			if (X32debug) {Xfdump("->X", xremote, 12, 0); fflush(stdout);}		\
			if (sendto (Xfd, xremote, 12, 0, Xip_addr, Xip_len) < 0) {			\
				perror ("coundn't send data to X32");							\
				exit (EXIT_FAILURE);											\
			}																	\
			before = now;														\
		}																		\
	}																			\
} while (0);
//
#define CHECKX32()				\
do {							\
	RPOLL 						\
	if (p_status > 0) {			\
		RECV					\
	}							\
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
//
//
//
// Thread routine to read input line
void *KeyboardInput() {
	InputReady = getline(&pt_InputLine, &s_InputLine, stdin);
	InputLine[--InputReady] = 0;
	return 0;
}
//
// manage error cases
void Die(char* message) {
	perror(message);
	exit(-1);
}
//
//
unsigned long	getmyIP();

#ifdef __WIN32__
// getmyIP(): A function to gather the IP of the network to which we are attached
unsigned long getmyIP() {
	char **pp = NULL;
	struct hostent *host = NULL;

	if (!gethostname(r_buf, 256) && (host = gethostbyname(r_buf)) != NULL) {
		for (pp = host->h_addr_list; *pp != NULL; pp++) {
			return (*(struct in_addr *)*pp).S_un.S_addr;
		}
	}
	return 0;
}
#else
//
unsigned long getmyIP() {
	struct ifaddrs *ifaddr, *ifa;
	int s;
	unsigned long k = 0;
	//
	r_buf[0] = 0;
	//
	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return k;
	}
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr != NULL) {
			// use r_buf as we may need a large string
			if ((s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in), r_buf, NI_MAXHOST, NULL, 0, NI_NUMERICHOST)) == 0) {
				// you typically have to replace "en0" by "wlan0", "eth0",... depending on your physical interface support
				if ((strcmp(ifa->ifa_name,"en0") == 0) && (ifa->ifa_addr->sa_family == AF_INET)) {
					//printf("\tInterface : <%s>\n",ifa->ifa_name );
					//printf("\t Address : <%s>\n", r_buf);
					//strcpy(Wip_str, r_buf); // update Xip_str
					k = inet_addr(r_buf);
					break;
				}
			}
		}
	}
	freeifaddrs(ifaddr);
	return k;
}
#endif
//
//
//
//
int main(int argc, char **argv) {
//
int					xremote_on;
char				xremote[12] = "/xremote"; // automatic trailing zeroes
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
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
#endif
//
// Initialize communication with X32 server at IP ip and PORT port
// Set default values to match your X32 desk
	strcpy (Xip_str, "");
	strcpy (Xport_str, "10023");
//
// Manage arguments
	filein = 0;
	do_keyboard = 1;
	s_delay = 10;
	while ((input_intch = getopt(argc, argv, "i:d:k:f:s:t:v:h")) != -1) {
		switch ((char)input_intch) {
		case 'i':
			strcpy(Xip_str, optarg);
			break;
		case 'd':
			sscanf(optarg, "%d", &X32debug);
			break;
		case 'f':
			filein = 1;
			strcpy(input_line, optarg);
			break;
		case 'k':
			sscanf(optarg, "%d", &do_keyboard);
			break;
		case 's':
			filein = 2;
			strcpy(input_line, optarg);
			break;
		case 't':
			sscanf(optarg, "%d", &s_delay);
			break;
		case 'v':
			sscanf(optarg, "%d", &X32verbose);
			break;
		default:
		case 'h':
			printf("usage: X32_command [-i X32 console ipv4 address]\n");
			printf("                   [-d 0/1, [0], debug option]\n");
			printf("                   [-v 0/1  [1], verbose option]\n");
			printf("                   [-k 0/1  [1], keyboard mode on]\n");
			printf("                   [-t int  [10], delay between batch commands in ms]\n");
			printf("                   [-s file, reads X32node formatted data lines from 'file']\n");
			printf("                   [-f file, sets batch mode on, getting input data from 'file']\n");
			printf("                     default IP is 192.168.1.62\n\n");
			printf(" If option -s file is used, the program reads data from the provided file \n");
			printf(" until EOF has been reached, and exits after that.\n\n");
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
			printf(" All other commands are parsed and sent to X32.\n");
			printf(" Typical X32 command structure:\n");
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
// Load the X32 address we connect to; we're a client to X32, keep it simple.
	// Create UDP socket
	if ((Xfd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror ("failed to create X32 socket");
		exit (EXIT_FAILURE);
	}
	// Construct  server sockaddr_in structure
	memset (&Xip, 0, sizeof(Xip));					// Clear struct
	Xip.sin_family = AF_INET;						// Internet/IP
	if (Xip_str[0] == 0) {
#ifdef __WIN32
		if ((Xip.sin_addr.S_un.S_addr = getmyIP()) != 0) {
			Xip.sin_addr.S_un.S_un_b.s_b4 = 0xff; 	// Local [/24] broadcast address
#else
		if ((Xip.sin_addr.s_addr =  getmyIP()) != 0) {
			Xip.sin_addr.s_addr |= 0xff000000;
#endif
			if (setsockopt(Xfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
				printf("setsockopt (SO_BROADCAST) failed\n");
				exit(EXIT_FAILURE);
			}
		}
	} else {
		Xip.sin_addr.s_addr = inet_addr(Xip_str);	// set IP address
	}
	Xip.sin_port = htons(atoi(Xport_str));			// server port/
//
// Set receiving from X32 to non blocking mode
// The 500ms timeout is used for delaying the printing of '.' at startup.
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000; //Set timeout for non blocking recvfrom(): 500ms
	FD_ZERO(&ufds);
	FD_SET(Xfd, &ufds);
// make stdin (fd = 0) I/O nonblocking
#ifndef __WIN32__
	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);
#endif
//
// All done. Let's send and receive messages
// Establish logical connection with X32 server
	printf(" X32_Command - v1.45 - (c)2014-20 Patrick-Gilles Maillot\n\nConnecting to X32.");
//
	xremote_on = X32verbose;	// Momentarily save X32verbose
	X32verbose = 0;
	s_len = Xsprint(s_buf, 0, 's', "/xinfo");
	for (keep_on = 0; keep_on < 5; keep_on++) {
		SEND  				// command /info sent;
		RPOLL 				// read data if available
		if (p_status < 0) {
			printf("Polling for data failed\n");
			return 1;		// exit on receive error
		} else if (p_status > 0) {
			RECV 			// We have received data - process it!
			if (strcmp(r_buf, "/xinfo") == 0) {
				strcpy(Xip_str, r_buf+16);
				break;		// Connected!
			}
		}					// ... else timeout
		printf("."); fflush(stdout);
	}
	if (keep_on < 5) {
		printf("X32 replied at %s!\n", Xip_str);
	} else {
		printf("Could not connect to X32!\n");
		sleep(10);
		exit(0);
	}
//
// Set 1ms timeout to get faster response from X32 (when testing for /xremote data).
	timeout.tv_usec = 1000; //Set timeout for non blocking recvfrom(): 1ms
	X32verbose = xremote_on;	// Restore X32verbose
	xremote_on = 0;
	before = 0;
	if (filein) {
		if ((fdk = fopen(input_line, "r")) == NULL) {
			printf ("Cannot read file: %s\n", input_line);
			exit(EXIT_FAILURE);
		}
		if (filein == 1) {
			keep_on = 1;
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
					else if (strlen(input_line) > 1) { // X32 command line
						s_len = Xcparse(s_buf, input_line);
						SEND			// send batch command
					}
					CHECKX32()			// Check if X32 sent something back
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
					SEND			// send data to X32
					CHECKX32()		// X32 will echo back the line
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
			if (thread == 0) {
#ifdef __WIN32__
				if ((thread = CreateThread(NULL, 0,
					(LPTHREAD_START_ROUTINE) KeyboardInput, 0, 0, 0)) < 0) Die("Could not create thread\n");
#else
				if ((pthread_create(&thread, NULL, KeyboardInput, 0)) < 0) Die("Could not create thread\n");
#endif
			}
			if (InputReady) {
				InputReady = 0;
#ifdef __WIN32__
				if (TerminateThread(thread, 0)) thread = 0;
#else
				if (pthread_cancel(thread) == 0) thread = 0;
#endif
				if (InputLine[0] == '#') printf("---comment: %s\n", InputLine);
				TESTINPUT()			// Test for input data checks
				s_len = Xcparse(s_buf, InputLine);
				SEND
			}
			CHECKX32()			// Check if X32 sent something back
		}
	}
	close(Xfd);
	return 0;
}
