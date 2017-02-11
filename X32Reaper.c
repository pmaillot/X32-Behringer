/*
 * X32Reaper.c
 *
 *  Created on: 20 mars 2015
 *      Author: Patrick-Gilles Maillot
 *
 *
 * Ver 1.7: first version being published
 * Ver 1.8: added variable bus_offset and associated code to deal with REAPER increasing the
 *          logical Track Send values when one also assigns a HW output to a REAPER track
 * Ver 1.9: A user correctly reported a pb with several track being simultaneously selected... System
 *          behaves badly. This was due to my code trying to align the REAPER slider values to X32
 *          fader values. I don't send the X32 values back to REAPER anymore (code is commented, for now)
 * Ver 1.91: For Windows, save interactive window size changes
 *           removed useless ioctl() call - use of select() is enough for non-blocking mode
 * Ver 2.0: Introduce REAPER DCA tracks; multiple REAPER tracks can be controlled from X32 DCA
 *          channels
 * Ver 2.1: Added bank selection (up/down) capability on X32 side, only controlling REAPER. For X32 Channels
 *          only, and by blocks of 32. optimized some Windows calls
 * Ver 2.2: Updated with bank selection recall (up/down) capability, reflecting on X32 the corresponding
 *          REAPER bank data and updating REAPER selected channel to reflect X32 select button value (in
 *          Xselected); Xselected is also setup at init by rreading the X32 state.
 *          Additionally, this version enables to set icon type and scribble colors with the track names
 *          by using the following syntax "<name>[ %icon[ %color]]" (spaces are optional.
 * Ver 2.21 & 2.22: bug fixes around memory allocation logic when changing Channel Bank Select state
 * Ver 2.3: removed /action commands 53808 and 53809 which seem to not exist anymore
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifdef __WIN32
#include <winsock2.h>
#define MySleep(a)	Sleep(a)
#define socklen_t	int
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define closesocket(s) 	close(s)
#define MySleep(a)	usleep(a*1000)
#define WSACleanup()
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#endif
#define BSIZE 			512	// Buffer sizes (enough to take into account FX parameters)
//
// X32 to communicate on X32_IP (192.168.0.64) and X32port (10023)
// Reaper to communicate on Rea_IP (192.168.0.64) and Reaport (10025)
//
// Buffers: Xb_r, Xb_s, Xb_lr, Xb_ls - read, send and lengths for X32
// Buffers: Rb_r, Rb_s, Rb_lr, Rb_ls - read, send and lengths for Reaper
// Defines: XBsmax, RBsmax, XBrmax, RBrmax - maximum lengths for buffers
//
#define XBsmax	BSIZE
#define RBsmax	BSIZE
#define XBrmax	BSIZE * 2
#define RBrmax	BSIZE * 4
//
// Private functions
void	X32UsrCtrlC();
int		X32Connect();
void	XRcvClean();
void	Xlogf(char *header, char *buf, int len);
int		X32ParseReaperMessage();
int		X32ParseX32Message();
//
// External calls used
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int Xfprint(char *bd, int index, char* text, char format, void *bs);
//
//
// Communication macros
//
#define SEND_TOX(b, l)											\
	do {														\
		if (Xverbose) Xlogf("->X", b, l);						\
		if (sendto(Xfd, b, l, 0, XX32IP_pt, XX32IP_len) < 0) {	\
			fprintf(log_file, "Error sending data to X32\n");	\
			exit(EXIT_FAILURE);									\
		} 														\
		if (Xdelay > 0) MySleep(Xdelay);						\
	} while (0);
//
//
#define SEND_TOR(b, l)											\
	do {														\
		if (Xverbose) Xlogf("->R", b, l);						\
		if (sendto(Rfd, b, l, 0, RHstIP_pt, RHstIP_len) < 0) {	\
			fprintf(log_file, "Error sending data to REAPER\n");\
			exit(EXIT_FAILURE);									\
		} 														\
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
int Xb_ls;
char Xb_s[XBsmax];
int Xb_lr;
char Xb_r[XBrmax];
int Rb_lr;
char Rb_r[RBrmax];
int Rb_ls;
char Rb_s[RBsmax];
//
int loop_toggle = 0x00; // toggles between 0x00 and 0x7f
//
char	S_SndPort[8], S_RecPort[8], S_X32_IP[20], S_Hst_IP[20];
//char	Xlogpath[LENPATH];
//
int zero = 0;
int one = 1;
int two = 2;
int six4 = 64;
int option = 1;
int play = 0;
int play_1 = 0;
// Misc. flags
int MainLoopOn = 1;		// main loop flag
int Xconnected = 0;		// 1 when communication is running
int Xverbose, Xdelay;	// verbose, List of Action and delay
int Xtransport_on = 1;	// whether transport is enabled or not (bank C)
int Xchbank_on = 0;		// whether we use Channnel bank select and not Loops in bank C
int	Xchbkof = 0;		// channel bank number
int Xselected = 0;		// X32 channel currently selected (if Xchbank_on is on)
int Xmaster_on = 1;		// whether master is enabled or not
int bus_offset = 0;		// offset to manage REAPER track sends logical numbering
//
int Xtrk_min = 0;		// Input min track number for Reaper/X32
int Xtrk_max = 0;		// Input max track number for Reaper/X32
int Xaux_min = 0;		// Auxin min track number for Reaper/X32
int Xaux_max = 0;		// Auxin max track number for Reaper/X32
int Xfxr_min = 0;		// FXrtn min track number for Reaper/X32
int Xfxr_max = 0;		// FXrtn max track number for Reaper/X32
int Xbus_min = 0;		// Bus min track number for Reaper/X32
int Xbus_max = 0;		// Bus max track number for Reaper/X32
int Xdca_min = 0;		// DCA min track number for Reaper/X32
int Xdca_max = 0;		// DCA max track number for Reaper/X32
int Rdca_min[8] = {0, 0, 0, 0, 0, 0, 0, 0};	// REAPER 'dca' mins
int Rdca_max[8] = {0, 0, 0, 0, 0, 0, 0, 0};	// REAPER 'dca' maxs
//
struct ifaddrs *ifa;									// to get our own system's IP address
struct sockaddr_in XX32IP;								// X socket IP we send/receive
struct sockaddr *XX32IP_pt = (struct sockaddr*) &XX32IP;// X socket IP pointer we send/receive
struct sockaddr_in RHstIP;								// R socket IP we send to
struct sockaddr *RHstIP_pt = (struct sockaddr*) &RHstIP;// R socket IP pointer we send to
struct sockaddr_in RFrmIP;								// R socket IP we received from
struct sockaddr *RFrmIP_pt = (struct sockaddr*) &RFrmIP;// R socket IP pointer we received from
int Xfd, Rfd, Mfd;			// X32 and Reaper receive and send sockets; Mfd is max(Xfd, Rfd)+1 for select()

time_t before, now;			// timers for Xremote controls
int poll_status;			// Polling status flag
//
//
int XX32IP_len = sizeof(XX32IP);	// length of X32 address
int RHstIP_len = sizeof(RHstIP);	// length of Reaper send to address
int RFrmIP_len = sizeof(RFrmIP);	// length of Reaper received from address
//
#ifdef __WIN32__
WSADATA wsa;
#endif
fd_set 			fds;
struct timeval	timeout;
//
// structure definition for REAPER data backup (used for REAPER bank switches)
// we'll allocate memory according to the number of channels declared in the
// .X32Reaper.ini resource file
typedef struct bkch {
	char	scribble[16];	// keep only 12 chars for X32
	float	fader;			// volume
	float	pan;			// panoramic
	float	mixbus[16];		// sends values
	float	mute;			// mute
	float	solo;			// solo
	int		color;			// scribble color
	int		icon;			// scribble icon
} S_bkch;
//
S_bkch *XMbanktracks = NULL;
////
// resource and log file data
FILE *res_file;
FILE *log_file;
FILE *Xin;
int r_status;
char Finipath[1024];	// resolved path to .ini file
char **FinilppPart;
int Finiretval;
//-------------------------------------------------------------------------
//

int main(int argc, char **argv) {
	int i, j;
	Xverbose = Xdelay = 0;

	strcpy(S_X32_IP, "");
	strcpy(S_Hst_IP, "");
	printf("X32Reaper - v2.3 - (c)2015 Patrick-Gilles Maillot\n\n");
	// load resource file
	if ((res_file = fopen("./.X32Reaper.ini", "r")) != NULL) { // ignore Width and Height
		fscanf(res_file, "%d %d %d %d %d %d %d\n", &i, &j, &Xverbose, &Xdelay, &Xtransport_on, &Xchbank_on, &Xmaster_on);
		fscanf(res_file, "%d %d %d %d %d %d %d %d %d %d %d\n", &Xtrk_min, &Xtrk_max, &Xaux_min, &Xaux_max, &Xfxr_min, &Xfxr_max, &Xbus_min, &Xbus_max, &Xdca_min, &Xdca_max, &bus_offset);
		fgets(S_X32_IP, sizeof(S_X32_IP), res_file);
		S_X32_IP[strlen(S_X32_IP) - 1] = 0;
		fgets(S_Hst_IP, sizeof(S_Hst_IP), res_file);
		S_Hst_IP[strlen(S_Hst_IP) - 1] = 0;
		fgets(S_SndPort, sizeof(S_SndPort), res_file);
		S_SndPort[strlen(S_SndPort) - 1] = 0;
		fgets(S_RecPort, sizeof(S_RecPort), res_file);
		S_RecPort[strlen(S_RecPort) - 1] = 0;
		for (i = 0; i < 8; i++) fscanf(res_file, "%d %d\n", &Rdca_min[i], &Rdca_max[i]);
		fclose(res_file);
		if (Xchbank_on) {
			//
			// allocate memory for maintaining REAPER data between banks
			// WE make the choice to allocate by blocks of 32, ensuring we cover all input tracks
			// between Xtrk_max and Xtrk_min
			if ((XMbanktracks = (S_bkch*)malloc(((Xtrk_max - Xtrk_min + 1) / 32 + 1) * 32 * sizeof(S_bkch))) == NULL) {
				exit(-1);
			}
			for (i = 0; i < Xtrk_max - Xtrk_min + 1; i++) {
				XMbanktracks[i].fader = 0.0;
				XMbanktracks[i].pan = 0.5;
				XMbanktracks[i].mute = 0.0;
				XMbanktracks[i].solo = 0.0;
				XMbanktracks[i].color = 0;
				XMbanktracks[i].icon = 1;
				for (j = 0; j < 16; j++) {
					XMbanktracks[i].scribble[j] = 0;
					XMbanktracks[i].mixbus[j] = 0.0;
				}
			}
		}
	} else {
		printf("No resource file found\n\n");
		exit (-1);
	}
	printf("X32 at IP %s\n", S_X32_IP);
	printf("REAPER at IP %s\nreceives on port %s\nsends to port %s\n", S_Hst_IP, S_RecPort, S_SndPort);
	printf("Flags: verbose: %1d, delay: %dms, Transport: %1d, CHBank: %1d, Master: %1d\n", Xverbose, Xdelay, Xtransport_on, Xchbank_on, Xmaster_on);
	printf("Map (min/max): Ch %d/%d, Aux %d/%d, FxR %d/%d, Bus %d/%d, DCA %d/%d, Bus Offset %d\n",
			Xtrk_min, Xtrk_max, Xaux_min, Xaux_max, Xfxr_min, Xfxr_max, Xbus_min, Xbus_max, Xdca_min, Xdca_max, bus_offset);
	printf("RDCA Map (min/max):");
	for (i = 0; i < 8; i++) printf(" %d: %d/%d-", i+1, Rdca_min[i], Rdca_max[i]);
	printf("\n");
	fflush(stdout);
	if ((log_file = fopen(".X32Reaper.log", "w")) != NULL) {
		fprintf(log_file, "*\n*\n");
		fprintf(log_file, "*    X32Reaper Log data - ©2015 - Patrick-Gilles Maillot\n");
		fprintf(log_file, "*\n*\n");
		MainLoopOn = 1;
// If connected/running, Consume X32 and REAPER messages
		if ((Xconnected = X32Connect()) != 0) {		//
			// run SW as long as needed
			while (MainLoopOn) {
				now = time(NULL); 			// get time in seconds
				if (now > before + 9) { 	// need to keep xremote alive?
					Xb_ls = Xsprint(Xb_s, 0, 's', "/xremote");
					SEND_TOX(Xb_s, Xb_ls)
					before = now;
				}
//
// Update on the X32 or Reaper?
				FD_ZERO(&fds);
				FD_SET(Rfd, &fds);
				FD_SET(Xfd, &fds);
				Mfd = Rfd + 1;
				if (Xfd > Rfd) Mfd = Xfd + 1;
				if (select(Mfd, &fds, NULL, NULL, &timeout) > 0) {
					if (FD_ISSET(Rfd, &fds) > 0) {
						if ((Rb_lr = recvfrom(Rfd, Rb_r, RBrmax, 0, RFrmIP_pt, &RFrmIP_len)) > 0) {
// Parse Reaper Messages and send corresponding data to X32
// These can be simple or #bundle messages!
// Can result in several/many messages to X32
//							printf("REAPER sent data\n"); fflush(stdout);
							if (Xverbose) Xlogf("R->", Rb_r, Rb_lr);
							Xb_ls = X32ParseReaperMessage();
						}
					}
					if (FD_ISSET(Xfd, &fds) > 0) {
						if ((Xb_lr = recvfrom(Xfd, Xb_r, XBrmax, 0, XX32IP_pt, &XX32IP_len)) > 0) {
// X32 transmitted something
// Parse and send (if applicable) to Reaper
//							printf("X32 sent data\n"); fflush(stdout);
							if (Xverbose) Xlogf("X->", Xb_r, Xb_lr);
							Rb_ls = X32ParseX32Message();
						}
					}
				}
			}
		}
	} else {
		printf("Cannot create log file\n");
		MySleep(10000);
	}
	WSACleanup();
	return 0;
}
//---------------------------------------------------------------------------------
//
//
// Private functions:
//
//
int X32Connect() {
	int i;
//
	poll_status = 0;
//
	if (Xconnected) {
//
// Signing OFF
		Xb_ls = Xsprint(Xb_s, 0, 's', "/unsubscribe");
		if (Xverbose)
			Xlogf("->X", Xb_s, Xb_ls);
		if (sendto(Xfd, Xb_s, Xb_ls, 0, XX32IP_pt, XX32IP_len) < 0)
			fprintf(log_file, "Error sending data to X32\n");
		WSACleanup();
		return 0;
	} else {
//
// Initialize winsock / communication with X32 server at IP ip and PORT port
#ifdef __WIN32__
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			fprintf(log_file, "WSA Startup Error\n");
			exit(EXIT_FAILURE);
		}
#endif
//
// Load the X32 address we connect to; we're a client to X32, keep it simple.
		// Create the UDP socket
		if ((Xfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
			fprintf(log_file, "X32 socket creation error\n");
			WSACleanup();
			return 0; // Make sure we don't considered being connected
		} else {
			// Construct the server sockaddr_in structure
			memset(&XX32IP, 0, sizeof(XX32IP));				// Clear struct
			XX32IP.sin_family = AF_INET;					// Internet/IP
			XX32IP.sin_addr.s_addr = inet_addr(S_X32_IP);	// IP address
			XX32IP.sin_port = htons(atoi("10023"));			// X32 port
//
// Non blocking mode; Check for X32 connectivity
			Xb_ls = Xsprint(Xb_s, 0, 's', "/info");
			if (Xverbose)
				Xlogf("->X", Xb_s, Xb_ls);
			if (sendto(Xfd, Xb_s, Xb_ls, 0, XX32IP_pt, XX32IP_len) < 0) {
				fprintf(log_file, "Error sending data to X32\n");
				WSACleanup();
				return 0; // Make sure we don't considered being connected
			} else {
				timeout.tv_sec = 0;
				timeout.tv_usec = 100000; //Set timeout for non blocking recvfrom(): 100ms
				FD_ZERO(&fds);
				FD_SET(Xfd, &fds);
//				printf("before select\n"); fflush(stdout);
				if ((poll_status = select(Xfd + 1, &fds, NULL, NULL, &timeout)) != -1) {
					if (FD_ISSET(Xfd, &fds) > 0) {
//						printf("after select\n"); fflush(stdout);
						// We have received data - process it!
						Xb_lr = recvfrom(Xfd, Xb_r, BSIZE, 0, 0, 0);
						if (Xverbose)
							Xlogf("X->", Xb_r, Xb_lr);
						if (strcmp(Xb_r, "/info") != 0) {
							fprintf(log_file, "X32Connect: Unexpected answer from X32\n");
							WSACleanup();
							return 0;
						}
					} else {
						// time out... Not connected or Not an X32
						fprintf(log_file, "X32Connect: X32 reception timeout\n");
						WSACleanup();
						return 0; // Make sure we don't considered being connected
					}
				} else {
					fprintf(log_file, "X32Connect: Polling for data failed\n");
					WSACleanup();
					return 0; // Make sure we don't considered being connected
				}
				// Connected! Get the X32 channel ID that's currently selected
				// to init the Xselected global variable.
				// This is likely to be overwritten when loading REAPER template
				Xb_ls = Xsprint(Xb_s, 0, 's', "/-stat/selidx");
				SEND_TOX(Xb_s, Xb_ls);
				// get data back
				FD_ZERO(&fds);
				FD_SET(Xfd, &fds);
				if ((poll_status = select(Xfd + 1, &fds, NULL, NULL, &timeout)) > 0) {
					Xb_lr = recvfrom(Xfd, Xb_r, BSIZE, 0, 0, 0);
					if (strcmp(Xb_r, "/-stat/selidx") == 0) {
						for (i = 0; i < 4; i++) endian.cc[i] = Xb_r[23-i];
						Xselected = endian.ii + 1;
					}
				} // ignore errors or timeout (leave Xselected = 0)
			}
		}
	}
//	printf("X32 Connected 1\n"); fflush(stdout);
//
// X32 connectivity OK. Set Connection with REAPER as Hst (HOST)
// Connect / Bind HOST
	i = 0;
	if ((Rfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0) {
		i = 1;
		if (setsockopt(Rfd, SOL_SOCKET, SO_REUSEADDR, (char*) &option,
				sizeof(option)) >= 0) {

			// Construct the server sockaddr_in structure
			memset(&RHstIP, 0, sizeof(RHstIP)); /* Clear struct */
			memset(&RFrmIP, 0, sizeof(RFrmIP)); /* Clear struct */
			RHstIP.sin_family = RFrmIP.sin_family = AF_INET; /* Internet/IP */
			RHstIP.sin_addr.s_addr = RFrmIP.sin_addr.s_addr = inet_addr(S_Hst_IP); /* Reaper IP address */
			RFrmIP.sin_port = htons(atoi(S_SndPort)); /* The Reaper port we receive from */
			RHstIP.sin_port = htons(atoi(S_RecPort)); /* The Reaper port we send to */
			i = 2;
			if (bind(Rfd, RFrmIP_pt, sizeof(RFrmIP)) != SOCKET_ERROR) {
				timeout.tv_sec = 0;
				timeout.tv_usec = 1000; //Set/Change timeout for non blocking recvfrom(): 1ms for Reaper/X32 comm
//
// Init UserCtrl bank C
				if (Xtransport_on)
					X32UsrCtrlC();
//
// Cleanup X32 buffers if needed
				XRcvClean();
				printf("X32 found and connected!\n"); fflush(stdout);
				return 1; // We are connected!
			}
		}
	}
	// If we're here, there was an error
	fprintf(log_file, "Reaper socket error %d\n", i);
	WSACleanup();
	return 0; // Make sure we don't considered being connected
}
//
// Empty any pending messages from X32 function
//
void XRcvClean() {
//
	if (Xverbose)
		fprintf(log_file, "X32 receive buffer cleanup if needed\n");
	do {
		FD_ZERO(&fds);
		FD_SET(Xfd, &fds);
		if ((poll_status = select(Xfd + 1, &fds, NULL, NULL, &timeout)) > 0) {
			if ((Xb_lr = recvfrom(Xfd, Xb_r, BSIZE, 0, 0, 0)) > 0) {
				if (Xverbose)
					Xlogf("X->", Xb_r, Xb_lr);
			}
		}
	} while (poll_status > 0);	// read and ignore X32 incoming data until
	return;						// first timeout or error
}
//
//
//
void Xlogf(char *header, char *buf, int len) {
	int i, k, n, j, l, comma = 0, data = 0, dtc = 0;
	unsigned char c;

	fprintf(log_file, "%s, %4d B: ", header, len);
	for (i = 0; i < len; i++) {
		c = (unsigned char) buf[i];
		if (c < 32 || c == 127 || c == 255)
			c = '~'; // Manage unprintable chars
		fprintf(log_file, "%c", c);
		if (c == ',') {
			comma = i;
			dtc = 1;
		}
		if (dtc && (buf[i] == 0)) {
			data = (i + 4) & ~3;
			for (dtc = i + 1; dtc < data; dtc++) {
				if (dtc < len) {
					fprintf(log_file, "~");
				}
			}
			dtc = 0;
			l = data;
			while (++comma < l && data < len) {
				switch (buf[comma]) {
				case 's':
					k = (strlen((char*) (buf + data)) + 4) & ~3;
					for (j = 0; j < k; j++) {
						if (data < len) {
							c = (unsigned char) buf[data++];
							if (c < 32 || c == 127 || c == 255)
								c = '~'; // Manage unprintable chars
							fprintf(log_file, "%c", c);
						}
					}
					break;
				case 'i':
					for (k = 4; k > 0; endian.cc[--k] = buf[data++])
						;
					fprintf(log_file, "[%6d]", endian.ii);
					break;
				case 'f':
					for (k = 4; k > 0; endian.cc[--k] = buf[data++])
						;
					if (endian.ff < 10.)
						fprintf(log_file, "[%06.4f]", endian.ff);
					else if (endian.ff < 100.)
						fprintf(log_file, "[%06.3f]", endian.ff);
					else if (endian.ff < 1000.)
						fprintf(log_file, "[%06.2f]", endian.ff);
					else if (endian.ff < 10000.)
						fprintf(log_file, "[%06.1f]", endian.ff);
					break;
				case 'b':
					// Get the number of bytes
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					n = endian.ii;
					// Get the number of data (floats or ints ???) in little-endian format
					for (k = 0; k < 4; endian.cc[k++] = buf[data++]);
					if (n == endian.ii) {
						// Display blob as string
						fprintf(log_file, "%d chrs: ", n);
						for (j = 0; j < n; j++) fprintf(log_file, "%c ", buf[data++]);
					} else {
						// Display blob as floats
						n = endian.ii;
						fprintf(log_file, "%d flts: ", n);
						for (j = 0; j < n; j++) {
							//floats are little-endian format
							for (k = 0; k < 4; endian.cc[k++] = buf[data++]);
							fprintf(log_file, "%06.2f ", endian.ff);
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
	fprintf(log_file, "\n");
	fflush (log_file);
}
//
// XUpdateBkCh():
// This function is called only when switching REAPER channel banks. It maps the values stored
// in the XMbanktracks structure array to the X32 channels 1 to 32 to reflect the values of the
// selected REAPER bank of 32 tracks
void XUpdateBkCh() {
	int i, j, src;
	float fone = 1.0;
	char tmp[32];
	//
	for (i = 1; i < 33; i++) {
		// update the 32 channels of X32 upon REAPER bank change requested from X32
		src = i - 1 + Xchbkof * 32;	// XMbanktracks index start at 0,channel and tracks start at index 1
		sprintf(tmp, "/ch/%02d/mix/fader", i);	// faders
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].fader);
		SEND_TOX(Xb_s, Xb_ls);
		//
		sprintf(tmp, "/ch/%02d/mix/pan", i);	// pan
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].pan);
		SEND_TOX(Xb_s, Xb_ls);
		//
		sprintf(tmp, "/ch/%02d/mix/on", i);		// mute
		j = 1;
		if (XMbanktracks[src].mute != 0.0) j = 0;
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &j);
		SEND_TOX(Xb_s, Xb_ls);
		//
		sprintf(tmp, "/ch/%02d/config/name", i);// scribble names
		Xb_ls = Xfprint(Xb_s, 0, tmp, 's', XMbanktracks[src].scribble);
		SEND_TOX(Xb_s, Xb_ls);
		//
		sprintf(tmp, "/ch/%02d/config/color", i);	// scribble colors
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &XMbanktracks[src].color);
		SEND_TOX(Xb_s, Xb_ls);
		//
		sprintf(tmp, "/ch/%02d/config/icon", i);	// scribble icons
		Xb_ls = Xfprint(Xb_s, 0, tmp, 's', &XMbanktracks[src].icon);
		SEND_TOX(Xb_s, Xb_ls);
		//
		j = 0;
		if (XMbanktracks[src].solo != 0.0) j = 1;
		sprintf(tmp, "/-stat/solosw/%02d", i);	// solo
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &j);
		SEND_TOX(Xb_s, Xb_ls);
		//
		for (j = 1; j < 17; j++) {
			sprintf(tmp, "/ch/%02d/mix/%02d/level", i, j);	// sends
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].mixbus[j]);
			SEND_TOX(Xb_s, Xb_ls);
		}
	}
	// manage channel select
	Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40297");
	SEND_TOR(Rb_s, Rb_ls)
	Rb_ls = Xsprint(Rb_s, 0, 's', "/action/53809");
	SEND_TOR(Rb_s, Rb_ls)
	j = Xselected - 1;
	Xb_ls = Xfprint(Xb_s, 0, "/-stat/selidx", 'i', &j);
	SEND_TOX(Xb_s, Xb_ls);
	sprintf(tmp, "/track/%d/select", Xselected + Xchbkof * 32);
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &fone);
	SEND_TOR(Rb_s, Rb_ls)
}
//
//
void X32UsrCtrlC() {
	int i;
	char* MP[4] = { "MP13000", "MP14000", "MP15000", "MP16000" };
	char* MN[8] = { "MN16000", "MN16001", "MN16002", "MN16003",
					"MN16004", "MN16005", "MN16006", "MN16007" };
	//
	// Encoders
	for (i = 1; i < 5; i++) {
		sprintf(Xb_r, "/config/userctrl/C/enc/%d", i);
		Xb_ls = Xfprint(Xb_s, 0, Xb_r, 's', MP[i - 1]);
		SEND_TOX(Xb_s, Xb_ls)
	}
	//
	// Buttons
	for (i = 5; i < 13; i++) {
		sprintf(Xb_r, "/config/userctrl/C/btn/%d", i);
		Xb_ls = Xfprint(Xb_s, 0, Xb_r, 's', MN[i - 5]);
		SEND_TOX(Xb_s, Xb_ls)
	}
	//
	// Set X32 Bank C Encoders  to center "64" value
	for (i = 33; i < 37; i++) {
		sprintf(Xb_r, "/-stat/userpar/%2d/value", i);
		Xb_ls = Xfprint(Xb_s, 0, Xb_r, 'i', &six4);
		SEND_TOX(Xb_s, Xb_ls)
	}
	//
	// Set X32 Bank C buttons  to "0" value
	for (i = 17; i < 25; i++) {
		sprintf(Xb_r, "/-stat/userpar/%2d/value", i);
		Xb_ls = Xfprint(Xb_s, 0, Xb_r, 'i', &zero);
		SEND_TOX(Xb_s, Xb_ls)
	}
	//
	// Color : black
	Xb_ls = Xfprint(Xb_s, 0, "/config/userctrl/C/color", 'i', &zero);
	SEND_TOX(Xb_s, Xb_ls)
	//
	// Select X32 Bank C
	Xb_ls = Xfprint(Xb_s, 0, "/-stat/userbank", 'i', &two);
	SEND_TOX(Xb_s, Xb_ls)

	return;
}
//--------------------------------------------------------------------
// X32 Messages data parsing
//
//
int X32ParseX32Message() {

	int Xb_i = 0;
	int Rb_ls = 0;
	int Xb_ls = 0;
	float fone = 1.0;
	int six4 = 64;
	int cnum, bus, dca, i;
	char tmp[32];
//
// What is the X32 message made of?
// X32 format is:
//	/ch/%02d/mix/pan......,f..[float]		%02d = 01..32
//	/ch/%02d/mix/fader....,f..[float]		%02d = 01..32
//	/ch/%02d/mix/on.......,i..[0/1]			%02d = 01..32
//	/ch/%02d/config/name..,s..[string\0]	%02d = 01..32
//	/ch/%02d/mix/%02d/level...,f..[float]	%02d = 01..32 / %02d = 01..16
//
// Same applies to /auxin and /fxrtn as for /ch above
//
//	/-stat/selidx.........,i..[%d]
//	/-stat/solosw/%02d....,i..[0/1]
//
//	/main/st/mix/fader....,f..[float]
//	/main/st/mix/pan......,f..[float]
//
//	/bus/%02d/mix/pan......,f..[float]		%02d = 01..32
//	/bus/%02d/mix/fader....,f..[float]		%02d = 01..16
//	/bus/%02d/mix/on.......,i..[0/1]		%02d = 01..16
//	/bus/%02d/config/name..,s..[string\0]	%02d = 01..16
//
//	/dca/1..8/on...........,i..[0/1]
//	/dca/1..8/fader........,f..[0..1.0]
//	/dca/1..8/config/name..,s..[string\0]
//
// Bank C Actuators
//		33		34		35		36
//	   Beat   Measure  Marker  Item
//
// Bank C Buttons
//	/-stat/userpar/%2d/value~,i~~[0 or 127]	%2d = 17..24
//
//		17		18		19		20
//		REW		PLAY	PAUSE	FF
//
//		21		22		23		24
//	 S/S loop  Repeat  STOP    REC
//
//   or:
//
//		21       22       23	24
//	 Bank UP  Bank Down  STOP  REC
//
	Rb_ls = 0;
	if (strncmp(Xb_r, "/ch/", 4) == 0) {
		// /ch/ cases : get channel number
		Xb_i = 4;
		cnum = (int) Xb_r[Xb_i++] - (int) '0';
		cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
		//
		// manage bank offset if the user selected that option
		if (Xchbank_on) {
			// Set actual channel number to match Channel Bank
			cnum = Xchbkof * 32 + cnum;
		}
		if ((Xtrk_max > 0) && (cnum <= (Xtrk_max - Xtrk_min + 1))) {
			Xb_i += 1; // skip '/'
			if (Xb_r[Xb_i] == 'm') {
				Xb_i += 4; //skip "/mix/" string
				if (Xb_r[Xb_i] == 'p') {
					//	/ch/%02d/mix/pan......,f..[float]
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/pan", cnum + Xtrk_min - 1);
					if (Xchbank_on) XMbanktracks[cnum].pan = endian.ff;
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'f') {
					//	/ch/%02d/mix/fader....,f..[float]
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/volume", cnum + Xtrk_min - 1);
					if (Xchbank_on) XMbanktracks[cnum - 1].fader = endian.ff;
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'o') {
					//	/ch/%02d/mix/on.......,i..[0/1]
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					if (endian.ii == 1) endian.ff = 0.0;
					else				endian.ff = 1.0;
					sprintf(tmp, "/track/%d/mute", cnum + Xtrk_min - 1);
					if (Xchbank_on) XMbanktracks[cnum - 1].mute = endian.ff;
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if ((Xb_r[Xb_i] == '0') || (Xb_r[Xb_i] == '1')) {
					// "/mix/" is followed by a Bus send number
					// get bus number
					bus = (int) Xb_r[Xb_i++] - (int) '0';
					bus = bus * 10 + (int) Xb_r[Xb_i++] - (int) '0';
					bus += bus_offset;
					Xb_i += 1; // skip '/'
					if (Xb_r[Xb_i] == 'l') {
						//	/ch/%02d/mix/%02d/level....,f..[float]
						while (Xb_r[Xb_i] != ',') Xb_i += 1;
						Xb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
						sprintf(tmp, "/track/%d/send/%d/volume", cnum + Xtrk_min - 1, bus);
						if (Xchbank_on) XMbanktracks[cnum - 1].mixbus[bus] = endian.ff;
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					}
				}
			} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')){
				Xb_i += 11; //skip "/config/name" string
				//	/ch/%02d/config/name..,s..[string\0]
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				sprintf(tmp, "/track/%d/name", cnum + Xtrk_min - 1);
				if (Xchbank_on) strncpy(XMbanktracks[cnum - 1].scribble, Xb_r + Xb_i, 12);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
			}
		}
	} else if (strncmp(Xb_r, "/auxin/", 7) == 0) {
		// /auxin/ cases : get channel number
		Xb_i = 7;
		cnum = (int) Xb_r[Xb_i++] - (int) '0';
		cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
		if ((Xaux_max > 0) && (cnum <= (Xaux_max - Xaux_min + 1))) {
			Xb_i += 1; // skip '/'
			if (Xb_r[Xb_i] == 'm') {
				Xb_i += 4; //skip "/mix/" string
				if (Xb_r[Xb_i] == 'p') {
					//	/auxin/%02d/mix/pan......,f..[float]
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/pan", cnum + Xaux_min - 1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'f') {
					//	/aunxin/%02d/mix/fader....,f..[float]
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/volume", cnum + Xaux_min - 1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'o') {
					//	/auxin/%02d/mix/on.......,i..[0/1]
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					if (endian.ii == 1) endian.ff = 0.0;
					else				endian.ff = 1.0;
					sprintf(tmp, "/track/%d/mute", cnum + Xaux_min - 1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if ((Xb_r[Xb_i] == '0') || (Xb_r[Xb_i] == '1')) {
					// "/mix/" is followed by a Bus send number
					// get bus number
					bus = (int) Xb_r[Xb_i++] - (int) '0';
					bus = bus * 10 + (int) Xb_r[Xb_i++] - (int) '0';
					bus += bus_offset;
					Xb_i += 1; // skip '/'
					if (Xb_r[Xb_i] == 'l') {
						//	/auxin/%02d/mix/%02d/level....,f..[float]
						while (Xb_r[Xb_i] != ',') Xb_i += 1;
						Xb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
						sprintf(tmp, "/track/%d/send/%d/volume", cnum + Xaux_min - 1, bus);
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					}
				}
			} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')){
				Xb_i += 11; //skip "/config/name" string
				//	/auxin/%02d/config/name..,s..[string\0]
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				sprintf(tmp, "/track/%d/name", cnum + Xaux_min - 1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
			}
		}
	}  else if (strncmp(Xb_r, "/fxrtn/", 7) == 0) {
		// /fxrtn/ cases : get channel number
		Xb_i = 7;
		cnum = (int) Xb_r[Xb_i++] - (int) '0';
		cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
		if ((Xfxr_max > 0) && (cnum <= (Xfxr_max - Xfxr_min + 1))) {
			Xb_i += 1; // skip '/'
			if (Xb_r[Xb_i] == 'm') {
				Xb_i += 4; //skip "/mix/" string
				if (Xb_r[Xb_i] == 'p') {
					//	/fxrtn/%02d/mix/pan......,f..[float]
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/pan", cnum + Xfxr_min - 1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'f') {
					//	/fxrtn/%02d/mix/fader....,f..[float]
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/volume", cnum + Xfxr_min - 1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'o') {
					//	/fxrtn/%02d/mix/on.......,i..[0/1]
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					if (endian.ii == 1) endian.ff = 0.0;
					else				endian.ff = 1.0;
					sprintf(tmp, "/track/%d/mute", cnum + Xfxr_min - 1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if ((Xb_r[Xb_i] == '0') || (Xb_r[Xb_i] == '1')) {
					// "/mix/" is followed by a Bus send number
					// get bus number
					bus = (int) Xb_r[Xb_i++] - (int) '0';
					bus = bus * 10 + (int) Xb_r[Xb_i++] - (int) '0';
					bus += bus_offset;
					Xb_i += 1; // skip '/'
					if (Xb_r[Xb_i] == 'l') {
						//	/fxrtn/%02d/mix/%02d/level....,f..[float]
						while (Xb_r[Xb_i] != ',') Xb_i += 1;
						Xb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
						sprintf(tmp, "/track/%d/send/%d/volume", cnum + Xfxr_min - 1, bus);
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					}
				}
			} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')){
				Xb_i += 11; //skip "/config/name" string
				//	/fxrtn/%02d/config/name..,s..[string\0]
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				sprintf(tmp, "/track/%d/name", cnum + Xfxr_min - 1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
			}
		}
	} else if (strncmp(Xb_r, "/bus/", 5) == 0) {
		//	/bus/%02d/mix/fader....,f..[float]		%02d = 01..16
		//	/bus/%02d/mix/on.......,i..[0/1]		%02d = 01..16
		//	/bus/%02d/config/name..,s..[string\0]	%02d = 01..16
		Xb_i = 5;
		bus = (int) Xb_r[Xb_i++] - (int) '0';
		bus = bus * 10 + (int) Xb_r[Xb_i++] - (int) '0';
		if ((Xbus_max > 0) && (bus <= (Xbus_max - Xbus_min + 1))) {
			Xb_i += 1; // skip '/'
			if (Xb_r[Xb_i] == 'm') {
				Xb_i += 4;	// skip "mix/"
				if (Xb_r[Xb_i] == 'p') {
					//	/bus/%02d/mix/pan
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/pan", bus + Xbus_min - 1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'f') {
					//	/bus/%02d/mix/fader
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/volume", bus + Xbus_min - 1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'o') {
					//	/bus/%02d/mix/on
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					if (endian.ii == 1) endian.ff = 0.0;
					else				endian.ff = 1.0;
					sprintf(tmp, "/track/%d/mute", bus + Xbus_min - 1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				}
			} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')) {
				//	/bus/%02d/config/name
				Xb_i += 11; //skip "/config/name" string
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				sprintf(tmp, "/track/%d/name", bus + Xbus_min - 1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
			}
		}
	} else if (strncmp(Xb_r, "/dca/", 5) == 0) {
		//	/dca/1..8/on...........,i..[0/1]
		//	/dca/1..8/fader........,f..[0..1.0]
		//	/dca/1..8/config/name..,s..[string\0]
		Xb_i = 5;
		dca = (int) Xb_r[Xb_i++] - (int) '0';
		if ((Xdca_max > 0) && (dca <= (Xdca_max - Xdca_min + 1))) {
			Xb_i += 1; // skip '/'
			if (Xb_r[Xb_i] == 'f') {
				//	/dca/1..8/fader
				Xb_i += 5;	// skip "fader"
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				sprintf(tmp, "/track/%d/volume", dca + Xdca_min - 1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				if ((Rdca_min[dca - 1] > 0) && (Rdca_max[dca - 1] >= Rdca_min[dca - 1])) {
					// There are REAPER 'dca' tracks to manage
					for (i = Rdca_min[dca - 1]; i <= Rdca_max[dca - 1]; i++) {
						SEND_TOR(Rb_s, Rb_ls)
						sprintf(tmp, "/track/%d/volume", i);
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					}
				}
			} else if (Xb_r[Xb_i] == 'o') {
				//	/dca/1..8/on
				Xb_i += 2;	// skip "on"
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				if (endian.ii == 1) endian.ff = 0.0;
				else				endian.ff = 1.0;
				sprintf(tmp, "/track/%d/mute", dca + Xdca_min - 1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				if ((Rdca_min[dca - 1] > 0) && (Rdca_max[dca - 1] >= Rdca_min[dca - 1])) {
					// There are REAPER 'dca' tracks to manage
					for (i = Rdca_min[dca - 1]; i <= Rdca_max[dca - 1]; i++) {
						SEND_TOR(Rb_s, Rb_ls)
						sprintf(tmp, "/track/%d/mute", i);
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					}
				}
			} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')) {
				//	/dca/1..8/config/name
				Xb_i += 11; //skip "/config/name" string
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				sprintf(tmp, "/track/%d/name", dca + Xdca_min - 1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
			}
		}
	} else if (strncmp(Xb_r, "/-stat/", 7) == 0) {
		// /-stat/ cases
		Xb_i = 10;
		if (Xb_r[Xb_i] == 'i') { // test on 'i' for selidx
			// unselect all REAPER tracks
			Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40297"); //40297 or 40769
			SEND_TOR(Rb_s, Rb_ls)
//			Rb_ls = Xsprint(Rb_s, 0, 's', "/action/53809"); // seems this does not exist anymore
//			SEND_TOR(Rb_s, Rb_ls)
			Rb_ls = 0;
			//	/-stat/selidx.........,i..[%d]
			while (Xb_r[Xb_i] != ',') Xb_i += 1;
			Xb_i += 4;
			for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]); // get track number
			cnum = -2;
			if ((endian.ii < 32) && (Xtrk_max > 0)) {
				cnum = endian.ii + Xtrk_min;
				if (Xchbank_on) {
					Xselected = cnum;
					// Set actual channel number to match Channel Bank
					cnum = Xchbkof * 32 + cnum;
				}
			}
			else if ((endian.ii < 40) && (Xaux_max > 0))		cnum = endian.ii + Xaux_min - 32;
			else if ((endian.ii < 48) && (Xfxr_max > 0))		cnum = endian.ii + Xfxr_min - 40;
			else if ((endian.ii < 64) && (Xbus_max > 0))		cnum = endian.ii + Xbus_min - 48;
	//		else if (endian.ii == 70) cnum = -1;	// set flag for master track...
			// select requested track
			if (cnum > -2) {
//				if (cnum == -1) {
//					Rb_ls = Xsprint(Rb_s, 0, 's', "/action/53808"); // seems this does not exist anymore
//				} else {
					sprintf(tmp, "/track/%d/select", cnum);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &fone);
//				}
			}
		} else if ((Xb_r[Xb_i] == 'o') && (Xb_r[Xb_i + 1] == 's')) {
			//	/-stat/solosw/%02d....,i..[0/1]
			Xb_i += 4;
			cnum = (int) Xb_r[Xb_i++] - (int) '0';
			cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
			while (Xb_r[Xb_i] != ',') Xb_i += 1;
			Xb_i += 4;
			for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
			if (endian.ii == 1) endian.ff = 1.0;
			else				endian.ff = 0.0;
			i = 0;
			if ((cnum < 33) && (Xtrk_max > 0)) {
				i = cnum + Xtrk_min - 1;
				if (Xchbank_on) {
					// Set actual channel number to match Channel Bank
					i = Xchbkof * 32 + i;
					XMbanktracks[i - 1].solo = endian.ff;
				}
			}
			else if ((cnum < 41) && (Xaux_max > 0)) 				i = cnum + Xaux_min - 33;
			else if ((cnum < 49) && (Xfxr_max > 0)) 				i = cnum + Xfxr_min - 41;
			else if ((cnum < 65) && (Xbus_max > 0)) 				i = cnum + Xbus_min - 49;
			else if ((cnum > 72) && (cnum < 81) && (Xdca_max > 0))	i = cnum + Xdca_min - 73;
			// !! TODO find Master track REAPER numbering
			sprintf(tmp, "/track/%d/solo", i);
			Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
		} else if ((Xb_r[Xb_i] == 'r') && (Xb_r[Xb_i + 1] == 'p')) {
			//	/-stat/userpar/XX/value.,i..[int]
			//	/-stat/userbank.,i..[int]
			//	/-stat/xcardtype....,i..[int]
			//	/-stat/xcardsync....,i..[int]
			Xb_i += 5; // get encoder/button value
			cnum = (int) Xb_r[Xb_i++] - (int) '0';
			cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
			Xb_i += 11; // get value [0...127] in endian;
			for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
			if (Xtransport_on) {
				switch (cnum) { // ignore non-bank C values
				case 17: // bank C button 5
					//user pressed "REW" / Go to Home
					if (endian.ii == 0) { 		// ignore non 0 value
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40042");
					}
					break;
				case 18: // bank C button 6
					//user pressed "PLAY"
					if (endian.ii == 0) { 		// ignore non 0 value
						Rb_ls = Xfprint(Rb_s, 0, "/play", 'f', &fone);
					}
					break;
				case 19: // bank C button 7
					//user pressed "PAUSE" - toggle function
					if (endian.ii == 0) { 		// ignore non 0 value
						Rb_ls = Xfprint(Rb_s, 0, "/pause", 'f', &fone);
					}
					break;
				case 20: // bank C button 8
					//user pressed "FF" / Go to End of Project
					if (endian.ii == 0) { 		// ignore non 0 value
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40043");
					}
					break;
				case 21: // bank C button 9
					if (Xchbank_on) {
						// Channel Bank UP
						if (Xchbkof < (Xtrk_max - 1) / 32) {
							if (endian.ii == 0) {
								Xchbkof += 1; // ignore non zero values
								XUpdateBkCh();
							}
						}
					} else {
						// user pressed "Loop" (start/stop toggle)
						if (endian.ii == 0) { 		// ignore non 0 value
							if (loop_toggle) {
								Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40223"); // end loop
							} else {
								Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40222"); // start loop
							}
							loop_toggle ^= 0x7f; // set Loop start/stop indicator on X32
							Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/21/value", 'i', &loop_toggle);
							SEND_TOX(Xb_s, Xb_ls)
						}
					}
					break;
				case 22: // bank C button 10
					if (Xchbank_on) {
						// Channel Bank DOWN
						if (endian.ii == 0) {
							Xchbkof -= 1; // ignore non zero values
							if (Xchbkof < 0) Xchbkof = 0;
							XUpdateBkCh();
						}
					} else {
						// user pressed "Toggle Repeat"
						if (endian.ii == 0) { 		// ignore non 0 value
							Rb_ls = Xfprint(Rb_s, 0, "/repeat", 'f', &fone);
						}
					}
					break;
				case 23: // bank C button 11
					//user pressed "STOP"
					if (endian.ii == 0) { 		// ignore non 0 value
						Rb_ls = Xfprint(Rb_s, 0, "/stop", 'f', &fone);
					}
					break;
				case 24: // bank C button 12
					//user pressed "REC"
					if (endian.ii == 0) { 		// ignore non 0 value
						Rb_ls = Xfprint(Rb_s, 0, "/record", 'f', &fone);
					}
					break;
				case 33: // bank C encoder 1 - Infinite rotation,
					// Move cursor to previous or next Beat depending on value change
					if (play && !play_1) {
						// stop play so we can move cursor (scrubb)
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 1; // remember we changed the state; "play" may be modified
						SEND_TOR(Rb_s, Rb_ls)
					}
					if (endian.ii > six4) {
						//Move Right
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40841");
					} else {
						//Move Left
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40842");
					}
					SEND_TOR(Rb_s, Rb_ls)
					if (play_1) {
						// restart play after we moved cursor
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 0;
						SEND_TOR(Rb_s, Rb_ls)
					}
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/33/value", 'i', &six4);
					SEND_TOX(Xb_s, Xb_ls)
					Rb_ls = 0;
					break;
				case 34: // bank C encoder 2 - Infinite rotation,
					// Move cursor to previous or next Measure depending on value change
					if (play && !play_1) {
						// stop play so we can move cursor (scrubb)
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 1; // remember we changed the state; "play" may be modified
						SEND_TOR(Rb_s, Rb_ls)
					}
					if (endian.ii > six4) {
						//Next Measure
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40839");
					} else {
						//Previous Measure
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40840");
					}
					SEND_TOR(Rb_s, Rb_ls)
					if (play_1) {
						// restart play after we moved cursor
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 0;
						SEND_TOR(Rb_s, Rb_ls)
					}
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/34/value", 'i', &six4);
					SEND_TOX(Xb_s, Xb_ls)
					Rb_ls = 0;
					break;
				case 35: // bank C encoder 3 - Infinite rotation,
					// Jump to previous or next Marker depending on value change
					if (play && !play_1) {
						// stop play so we can move cursor (scrubb)
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 1; // remember we changed the state; "play" may be modified
						SEND_TOR(Rb_s, Rb_ls)
					}
					if (endian.ii > six4) {
						//Next marker
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40173");
					} else {
						//Previous marker
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40172");
					}
					SEND_TOR(Rb_s, Rb_ls)
					if (play_1) {
						// restart play after we moved cursor
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 0;
						SEND_TOR(Rb_s, Rb_ls)
					}
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/35/value", 'i', &six4);
					SEND_TOX(Xb_s, Xb_ls)
					Rb_ls = 0;
					break;
				case 36: // bank C encoder 4 - Infinite rotation,
					// Move cursor to next Item left or right depending on value change
					if (play && !play_1) {
						// stop play so we can move cursor (scrubb)
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 1; // remember we changed the state; "play" may be modified
						SEND_TOR(Rb_s, Rb_ls)
					}
					if (endian.ii > six4) {
						//Move Right
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40319");
					} else {
						//Move Left
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40318");
					}
					if (play_1) {
						// restart play after we moved cursor
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 0;
						SEND_TOR(Rb_s, Rb_ls)
					}
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/36/value", 'i', &six4);
					SEND_TOX(Xb_s, Xb_ls)
					Rb_ls = 0;
					break;
				}
			}
		}
	} else if (strncmp(Xb_r, "/main/st/mix/", 13) == 0) {
		if(Xmaster_on) {
		//	/main cases
			Xb_i += 13;
			if (Xb_r[Xb_i] == 'f') {
				//	/main/st/mix/fader....,f..[float]
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				Rb_ls = Xfprint(Rb_s, 0, "/master/volume", 'f', &endian.ff);
			} else if (Xb_r[Xb_i] == 'p') {
				//	/main/st/mix/pan......,f..[float]
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				Rb_ls = Xfprint(Rb_s, 0, "/master/pan", 'f', &endian.ff);
			} else if (Xb_r[Xb_i] == 'o') {
				// unselect all REAPER tracks and select Master track
				Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40297");
				SEND_TOR(Rb_s, Rb_ls)
//				Rb_ls = Xsprint(Rb_s, 0, 's', "/action/53808"); // seem this doesn't exist anymore
//				SEND_TOR(Rb_s, Rb_ls)
				//echo Master track selected on X32
				i = 70; // master track on X32
				Xb_ls = Xfprint(Xb_s, 0, "/-stat/selidx", 'i', &i);
				SEND_TOX(Xb_s, Xb_ls)
				//	/main/st/mix/on....,i..0/1
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				if (endian.ii == 1) Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40731");
				else				Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40730");
			}
		}
	}
	if (Rb_ls) {
		SEND_TOR(Rb_s, Rb_ls)
		Rb_ls = 0; // REAPER message has been sent
	}
	return (Rb_ls);
}
//--------------------------------------------------------------------
// REAPER Messages data parsing
//
// X32Reaper should be running when the user launches REAPER; this way, REAPER initial values are loaded
// into the program (twice in fact), ensuring all values are correctly set when the user starts using REAPER
// I noted it is better to have a 2 to 5ms delay in REAPER sending 1KB buffers when managing more than 32
// channels, otherwise, some track names are not transmitted correctly (buffer overflow?)
//
int X32ParseReaperMessage() {

	int Rb_i = 0;
	int Xb_ls = 0;
	int bundle = 0;
	int Rb_nm = 0;
	int mes_len, tnum, bus;
	int i;
	char tmp[32];
//
// is the message a bundle ?
	if (strncmp(Rb_r, "#bundle", 7) == 0) {
		bundle = 1;
		Rb_i = 16;
	}
//	Xlogf("Reaper: ", Rb_r, Rb_lr) ; fflush(log_file);
	do {
		if (bundle) {
			mes_len = (int)Rb_r[Rb_i + 2] * 256 + (int)Rb_r[Rb_i + 3]; // sub-message length fits on 2 bytes max
			Rb_i += 4;
			if ((Rb_nm = (Rb_i + mes_len)) >= Rb_lr) bundle = 0; // Rb_nm is used later!
		}
		// Parse message or message parts
		Xb_ls = 0;
		if (strncmp(Rb_r + Rb_i, "/track/", 7) == 0) {
			Rb_i += 7;
			// build track number...
			tnum = (int) Rb_r[Rb_i++] - (int) '0';
			while (Rb_r[Rb_i] != '/') tnum = tnum * 10 + (int) Rb_r[Rb_i++] - (int) '0';
			Rb_i++; // skip '/'
			// Got track #
			// Known: /pan, /volume, /name, /mute, /select, /solo, /send
			if (Rb_r[Rb_i] == 'p') { // /track/pan
				while (Rb_r[Rb_i] != ',') Rb_i += 1;
				Rb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
//					/xxxx/[01-32]/mix/pan ,f -100 100
				if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
					if (Xchbank_on) {
						XMbanktracks[tnum - Xtrk_min].pan = endian.ff;
						// Set actual channel number to match Channel Bank
						tnum = tnum - Xchbkof * 32;
					}
					sprintf(tmp, "/ch/%02d/mix/pan", tnum  - Xtrk_min + 1);
				}
				else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))	sprintf(tmp, "/auxin/%02d/mix/pan", tnum - Xaux_min + 1);
				else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))	sprintf(tmp, "/fxrtn/%02d/mix/pan", tnum - Xfxr_min + 1);
				else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))	sprintf(tmp, "/bus/%02d/mix/pan", tnum - Xbus_min + 1);
				else tnum = -1;
				if (tnum > 0) Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
			} else if (Rb_r[Rb_i] == 'v') { // /track/volume
				while (Rb_r[Rb_i] != ',') Rb_i += 1;
				Rb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				//
// 				Make REAPER stick to X32 known values to avoid fader kick-backs
				endian.ff = roundf(endian.ff * 1023.) / 1023.;
//					/xxxx/[01-32]/mix/fader ,f 0..1
				if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
					if (Xchbank_on) {
						XMbanktracks[tnum - Xtrk_min].fader = endian.ff;
						// Set actual channel number to match Channel Bank
						tnum = tnum - Xchbkof * 32;
					}
					sprintf(tmp, "/ch/%02d/mix/fader", tnum - Xtrk_min + 1);
				}
				else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))	sprintf(tmp, "/auxin/%02d/mix/fader", tnum - Xaux_min + 1);
				else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))	sprintf(tmp, "/fxrtn/%02d/mix/fader", tnum - Xfxr_min + 1);
				else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))	sprintf(tmp, "/bus/%02d/mix/fader", tnum - Xbus_min + 1);
				else if ((tnum >= Xdca_min) && (tnum <= Xdca_max))	{
					if ((Rdca_min[tnum - Xdca_min] > 0) && (Rdca_max[tnum - Xdca_min] >= Rdca_min[tnum - Xdca_min])) {
						for (i = Rdca_min[tnum - Xdca_min]; i <= Rdca_max[tnum - Xdca_min]; i++) {
							// update all REAPER DCA tracks to same values
							sprintf(tmp, "/track/%d/volume", i);
							Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
							SEND_TOR(Rb_s, Rb_ls)
						}
					}
					sprintf(tmp, "/dca/%1d/fader", tnum - Xdca_min + 1);
				} else {
					// Do we have a REAPER DCA assigned channel?
					for (i = 0; i < 8; i++) {
						if (tnum >= Rdca_min[i] && tnum <= Rdca_max[i]) {
							for (tnum = Rdca_min[i]; tnum <= Rdca_max[i]; tnum++) {
								// update all REAPER DCA tracks to same values
								sprintf(tmp, "/track/%d/volume", tnum);
								Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
								SEND_TOR(Rb_s, Rb_ls)
							}
							tnum = i;
							// also update REAPER DCA fader
							if ((Xdca_max > 0) && (i <= (Xdca_max - Xdca_min + 1))) {
								sprintf(tmp, "/track/%d/volume", Xdca_min + i);
								Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
								SEND_TOR(Rb_s, Rb_ls)
							}
							sprintf(tmp, "/dca/%1d/fader", i + 1);
							break;
						}
					}
					if (i >= 8) tnum = -1;
				}
				if (tnum > 0) Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
			} else if (Rb_r[Rb_i] == 'n') { // /track/name
				while (Rb_r[Rb_i] != ',') Rb_i += 1;
				//
				// Track name starts at Rb_i index
				Rb_i += 4;
				// We can set name, but also icon and color using the following format:
				// <name>[%icon[%color]] (optional spaces; '%' as a delimiter)
				//         icon and color as integers
				int i_icon = 0;
				int i_color = -1;
				int length = strlen(Rb_r + Rb_i);
				for (i = 0; i < length; i++) {
					if (Rb_r[Rb_i + i] == '%') {
						if (i_icon) {
							i_color = i + 1;
						} else {
							i_icon = i + 1;
						}
						Rb_r[Rb_i + i] = 0;
					}
				}
				if (i_icon)       sscanf(Rb_r + Rb_i + i_icon, "%d", &i_icon);
				if (i_color > -1) sscanf(Rb_r + Rb_i + i_color, "%d", &i_color);
//					/xxxx/[01-32]/config/name ,s string
				if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
					if (Xchbank_on) strncpy (XMbanktracks[tnum - Xtrk_min].scribble, Rb_r + Rb_i, 12);
					if (i_icon) {
						sprintf(tmp, "/ch/%02d/config/icon", tnum - Xtrk_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
						SEND_TOX(Xb_s, Xb_ls)
						if (Xchbank_on) XMbanktracks[tnum - Xtrk_min].icon = i_icon;
					}
					if (i_color > -1) {
						sprintf(tmp, "/ch/%02d/config/color", tnum - Xtrk_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
						SEND_TOX(Xb_s, Xb_ls)
						if (Xchbank_on) XMbanktracks[tnum - Xtrk_min].color = i_color;
					}
					if (Xchbank_on) {
						// Set actual channel number to match Channel Bank
						tnum = tnum - Xchbkof * 32;
					}
					sprintf(tmp, "/ch/%02d/config/name", tnum - Xtrk_min + 1);
				} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max)) {
					if (i_icon) {
						sprintf(tmp, "/auxin/%02d/config/icon", tnum - Xaux_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
						SEND_TOX(Xb_s, Xb_ls)
					}
					if (i_color > -1) {
						sprintf(tmp, "/auxin/%02d/config/color", tnum - Xaux_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
						SEND_TOX(Xb_s, Xb_ls)
					}
					sprintf(tmp, "/auxin/%02d/config/name", tnum - Xaux_min + 1);
				} else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max)) {
					if (i_icon) {
						sprintf(tmp, "/fxrtn/%02d/config/icon", tnum - Xfxr_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
						SEND_TOX(Xb_s, Xb_ls)
					}
					if (i_color > -1) {
						sprintf(tmp, "/fxrtn/%02d/config/color", tnum - Xfxr_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
						SEND_TOX(Xb_s, Xb_ls)
					}
					sprintf(tmp, "/fxrtn/%02d/config/name", tnum - Xfxr_min + 1);
				} else if ((tnum >= Xbus_min) && (tnum <= Xbus_max)) {
					if (i_icon) {
						sprintf(tmp, "/bus/%02d/config/icon", tnum - Xbus_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
						SEND_TOX(Xb_s, Xb_ls)
					}
					if (i_color > -1) {
						sprintf(tmp, "/bus/%02d/config/color", tnum - Xbus_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
						SEND_TOX(Xb_s, Xb_ls)
					}
					sprintf(tmp, "/bus/%02d/config/name", tnum - Xbus_min + 1);
				} else if ((tnum >= Xdca_min) && (tnum <= Xdca_max)) {
					if (i_icon) {
						sprintf(tmp, "/dca/%1d/config/icon", tnum - Xdca_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
						SEND_TOX(Xb_s, Xb_ls)
					}
					if (i_color > -1) {
						sprintf(tmp, "/dca/%1d/config/color", tnum - Xdca_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
						SEND_TOX(Xb_s, Xb_ls)
					}
					sprintf(tmp, "/dca/%1d/config/name", tnum - Xdca_min + 1);
				} else tnum = -1;
				if (tnum > 0) {
					Xb_ls = Xfprint(Xb_s, 0, tmp, 's', Rb_r + Rb_i);
				}
			} else if (Rb_r[Rb_i] == 'm') { // /track/mute
				while (Rb_r[Rb_i] != ',') Rb_i += 1;
				Rb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				i = 1 - (int) endian.ff;
//					/xxxx/[01-32]/mix/on ,i 0/1
				if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
					if (Xchbank_on) {
						XMbanktracks[tnum - Xtrk_min].mute = endian.ff;
						// Set actual channel number to match Channel Bank
						tnum = tnum - Xchbkof * 32;
					}
					sprintf(tmp, "/ch/%02d/mix/on", tnum - Xtrk_min + 1);
				}
				else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))	sprintf(tmp, "/auxin/%02d/mix/on", tnum - Xaux_min + 1);
				else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))	sprintf(tmp, "/fxrtn/%02d/mix/on", tnum - Xfxr_min + 1);
				else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))	sprintf(tmp, "/bus/%02d/mix/on", tnum - Xbus_min + 1);
				else if ((tnum >= Xdca_min) && (tnum <= Xdca_max)) {
					if ((Rdca_min[tnum - Xdca_min] > 0) && (Rdca_max[tnum - Xdca_min] >= Rdca_min[tnum - Xdca_min])) {
						for (i = Rdca_min[tnum - Xdca_min]; i <= Rdca_max[tnum - Xdca_min]; i++) {
							// update all REAPER DCA tracks to same values
							sprintf(tmp, "/track/%d/mute", i);
							Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
							SEND_TOR(Rb_s, Rb_ls)
						}
					}
					sprintf(tmp, "/dca/%1d/on", tnum - Xdca_min + 1);
				} else {
					// Do we have a REAPER DCA assigned channel?
					for (i = 0; i < 8; i++) {
						if (tnum >= Rdca_min[i] && tnum <= Rdca_max[i]) {
							for (tnum = Rdca_min[i]; tnum <= Rdca_max[i]; tnum++) {
								// update all REAPER DCA tracks to same values
								sprintf(tmp, "/track/%d/mute", tnum);
								Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
								SEND_TOR(Rb_s, Rb_ls)
							}
							tnum = i;
							// also update REAPER DCA fader
							if ((Xdca_max > 0) && (i <= (Xdca_max - Xdca_min + 1))) {
								sprintf(tmp, "/track/%d/mute", Xdca_min + i);
								Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
								SEND_TOR(Rb_s, Rb_ls)
							}
							sprintf(tmp, "/dca/%1d/on", i + 1);
							break;
						}
					}
					if (i >= 8) tnum = -1;
				}
				i = 1 - (int) endian.ff;
				if (tnum > 0) Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i);
			} else if (Rb_r[Rb_i] == 's') { // /track/select, /track/send or /track/solo
				Rb_i++;
				if ((Rb_r[Rb_i] == 'e') && (Rb_r[Rb_i + 1] == 'l')) { // /track/select
					while (Rb_r[Rb_i] != ',') Rb_i += 1;
					Rb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					// set channel # based on track num.
					if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
						tnum = tnum - Xtrk_min;
						if (Xchbank_on) {
							// Set actual channel number to match Channel Bank
							tnum = tnum - Xchbkof * 32;
							Xselected = tnum + 1;
						}
					}
					else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))	tnum = tnum - Xaux_min + 32;
					else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))	tnum = tnum - Xfxr_min + 40;
					else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))	tnum = tnum - Xbus_min + 48;
					else tnum = -1;	// TODO: How to select Master from REAPER???
					// X32: selecting one track automatically unselects others
					if (((int) endian.ff == 1) && (tnum >= 0)) Xb_ls = Xfprint(Xb_s, 0, "/-stat/selidx", 'i', &tnum);
				} else if ((Rb_r[Rb_i] == 'e') && (Rb_r[Rb_i + 1] == 'n')) { // /track/send
					// example: /track/6/send/2/volume\0\0,f\0\0?7KÇ (track 6 sending to 2nd bus)
					Rb_i += 4;	// skip "....send/"
					// build bus track number
					bus = (int) Rb_r[Rb_i++] - (int) '0';
					while (Rb_r[Rb_i] != '/')
						bus = bus * 10 + (int) Rb_r[Rb_i++] - (int) '0';
					bus -= bus_offset;
					Rb_i++; // skip '/'
					if (Rb_r[Rb_i] == 'v') { // volume <float>
						while (Rb_r[Rb_i] != ',') Rb_i += 1;
						Rb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
						// /xxxx/<tnum>/mix/<bus>/level ,f 0...1.
						if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
							if (Xchbank_on) {
								XMbanktracks[tnum - Xtrk_min].mixbus[bus] = endian.ff;
								// Set actual channel number to match Channel Bank
								tnum = tnum - Xchbkof * 32;
							}
							sprintf(tmp, "/ch/%02d/mix/%02d/level", tnum - Xtrk_min + 1, bus);
						}
						else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))	sprintf(tmp, "/auxin/%02d/mix/%02d/level", tnum - Xaux_min + 1, bus);
						else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))	sprintf(tmp, "/fxrtn/%02d/mix/%02d/level", tnum - Xfxr_min + 1, bus);
						else tnum = -1;
						if (tnum > 0) Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
					}
				} else if (Rb_r[Rb_i] == 'o') { // /track/solo
					while (Rb_r[Rb_i] != ',') Rb_i += 1;
					Rb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					i = (int) endian.ff;
					// set track X32 Channel number based on track
					if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
						tnum = tnum - Xtrk_min + 1;
						if (Xchbank_on) {
							XMbanktracks[tnum - 1].solo = endian.ff;
							// Set actual channel number to match Channel Bank
							tnum = tnum - Xchbkof * 32;
						}
					}
					else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))	tnum = tnum - Xaux_min + 32 + 1;
					else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))	tnum = tnum - Xfxr_min + 40 + 1;
					else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))	tnum = tnum - Xbus_min + 48 + 1;
					else if ((tnum >= Xdca_min) && (tnum <= Xdca_max))	tnum = tnum - Xdca_min + 72 + 1;
					else tnum = -1;
					if (tnum > 0) {// TODO: How to select Master from REAPER???
						sprintf(tmp, "/-stat/solosw/%02d", tnum);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i);
					}
				}
			}
		} 	else if (strncmp(Rb_r + Rb_i, "/master/", 8) == 0) {
			if(Xmaster_on) {
			// MASTER
				Rb_i += 8;
				// Known:/master/pan, /master/volume
				// TODO: Not possible today: Select, Solo, Mute
				if (Rb_r[Rb_i] == 'p') { // pan
					while (Rb_r[Rb_i] != ',') Rb_i += 1;
					Rb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					Xb_ls = Xfprint(Xb_s, 0, "/main/st/mix/pan", 'f', &endian.ff);
				}
				if (Rb_r[Rb_i] == 'v') { // volume
					while (Rb_r[Rb_i] != ',') Rb_i += 1;
					Rb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					Xb_ls = Xfprint(Xb_s, 0, "/main/st/mix/fader", 'f', &endian.ff);
				}
			}
		} else if (Xtransport_on) {
			if (strncmp(Rb_r + Rb_i, "/repeat", 7) == 0) {
					Rb_i += 12;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					if (endian.ff == 1.0) endian.ii = 0x7F;	// otherwise, endian.ii will be all 0
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/22/value", 'i', &endian.ii);
			} else if (strncmp(Rb_r + Rb_i, "/record", 7) == 0) {
					Rb_i += 12;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					if (endian.ff == 1.0) endian.ii = 0x7F;	// otherwise, endian.ii will be all 0
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/24/value", 'i', &endian.ii);
			} else if (strncmp(Rb_r + Rb_i, "/play", 5) == 0) {
					Rb_i += 12;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					if (endian.ff == 1.0) {
						endian.ii = 0x7F;	// otherwise, endian.ii will be all 0
						play = 1;
					} else {
						play = 0;
					}
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/18/value", 'i', &endian.ii);
			} else if (strncmp(Rb_r + Rb_i, "/pause", 6) == 0) {
					Rb_i += 12;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					if (endian.ff == 1.0) endian.ii = 0x7F;	// otherwise, endian.ii will be all 0
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/19/value", 'i', &endian.ii);

			} else if (strncmp(Rb_r + Rb_i, "/stop", 5) == 0) {
				Rb_i += 12;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				if (endian.ff == 1.0) endian.ii = 0x7F;	// otherwise, endian.ii will be all 0
				Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/23/value", 'i', &endian.ii);
			}
		}
		if (Xb_ls) SEND_TOX(Xb_s, Xb_ls)
		Rb_i = Rb_nm; // Set Rb_i pointing to next message (at index Rb_nm) in Reaper bundle
	} while (bundle);
	return (Xb_ls);
}
