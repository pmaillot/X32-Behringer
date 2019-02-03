//
// XX32Replay.c
//
//  Created on: Nov, 2014
//
//      X32Replay: a simple record & playback service for X32
//
//      Patrick-Gilles Maillot
//
//  History log:
//    v 0.2 - Initial version
//    v 0.3 - Use select() rather than poll()
//            Enable recording user tags (text lines) that are printed during play mode
//            A better help. Added stop and pause commands (same as record pause)
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#ifdef __WIN32__
#include <windows.h>
#include <conio.h>
#define millisleep(a)	Sleep((a))
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#define closesocket(s) 	close((s))
#define millisleep(a)	usleep((a)*1000)
#define WSACleanup()
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#endif

#define RPOLL													\
	do {														\
		FD_ZERO (&Xfs);											\
		FD_SET (Xfd, &Xfs);										\
		p_status = select(Xfd+1, &Xfs, NULL, NULL, &timeout);	\
	} while (0);
//
#ifndef timersub
#define timersub(a, b, result)								\
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
#ifndef timercmp
#define timercmp(a, b, CMP)									\
	do {													\
		(((a)->tv_sec != (b)->tv_sec) ?						\
		((a)->tv_sec CMP (b)->tv_sec)) :					\
		((a)->tv_usec CMP (b)->tv_usec)						\
	} while (0)
#endif
//
// External calls used
extern void Xfdump(char *header, char *buf, int len, int debug);
extern int Xsprint(char *bd, int index, char format, void *bs);
//
#define VERBOSE	0
#define DEBUG	0
//
#ifdef __WIN32__
	#define BACKSPACE	8		// need to use <ctl>h
	#define EOL			13		// enter key
	#define BACKCHARS	2		// 2 chars to remove from line
#else
	#define BACKSPACE	8		// backspace key OK
	#define EOL			'\n'	// enter key
	#define BACKCHARS	1		// 1 char to remove from line
#endif
//
#define XBSMAX	512
#define XBRMAX	512
//
int					X32verbose = VERBOSE;
//
//
int
main(int argc, char **argv)
{
struct sockaddr_in	Xip;								// X socket IP we send/receive
struct sockaddr		*Xip_pt = (struct sockaddr*)&Xip;	// X socket IP pointer we send/receive
int					Xb_lr, Xb_ls;
char				Xb_r[XBRMAX], Xb_s[XBSMAX];
char				X32ip_ch[20], X32port_ch[8];	// X32 IP and port
char				X32file_ch[128]; 				// File Name for recording/playing
char				inputline[32], input_ch;		// stdio buffer and single char input
int					l_index, p_status;				// input buffer index, poll status
int					u_data, keep_on, play_on, record_on, record_pause;
struct timeval		t_rec, t_play, t_dlta, t_dlta1;
time_t				before, now;
int					Xfd;							// X32 socket
fd_set 				Xfs;							// X32 file descriptor set
struct timeval		timeout;						// used for select()
FILE				*X32file_pt;
//
#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
unsigned long 		mode;
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
#endif
//
// initialize communication with X32 server at IP ip and PORT port
//	set a default value for server[]; change to match your X32 desk
	strcpy (X32ip_ch, "192.168.0.64");			// default for X32
	strcpy (X32port_ch, "10023"); 				// This is the send & receive port for X32 (on our system and X32)
	strcpy (X32file_ch, "X32ReplayFile.txt");	// This is the default filename for recording/playing data
//
//
// Manage arguments
	while ((input_ch = getopt(argc, argv, "i:v:f:h")) != (char)-1) {
		switch (input_ch) {
		case 'i':
			strcpy(X32ip_ch, optarg );
			break;
		case 'v':
			sscanf(optarg, "%d", &X32verbose);
			break;
		case 'f':
			strcpy(X32file_ch, optarg );
			break;
		default:
		case 'h':
			printf("usage: X32Replay [-i X32 console ipv4 address] -default: 192.168.0.64\n");
			printf("                 [-v 0/1, verbose option] -default: 0\n");
			printf("                 [-f file name] -default: X32ReplayFile.txt\n\n");
			printf("  known commands:\n");
			printf("    stop:         stops recording or playing and closes file\n");
			printf("    record:       reports recording state\n");
			printf("    record off:   stops recording\n");
			printf("    record on:    starts or resumes recording\n");
			printf("    record pause: pauses recording and keeps file opened\n");
			printf("    pause:        pauses recording and keeps file opened\n");
			printf("    play:         reports playing state\n");
			printf("    play off:     stops playing\n");
			printf("    play on:      starts playing\n");
			printf("    # typed line: during recording, records the typed line as a user tag\n");
			printf("    exit:         exits the program\n\n");
			fflush(stdout);
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
// Construct server sockaddr_in structure
	memset(&Xip, 0, sizeof(Xip));				// Clear struct
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(X32ip_ch);	// X32 IP address
	Xip.sin_port = htons(atoi(X32port_ch));		// X32 port
//
// Set receiving from X32 to non blocking mode
// The 500ms timeout is used for delaying the printing of '.' at startup.
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000; //Set timeout for non blocking recvfrom(): 500ms
	FD_ZERO(&Xfs);
	FD_SET(Xfd, &Xfs);
// make stdin (fd = 0) I/O nonblocking
#ifdef __WIN32__
	mode = 1;
	p_status = ioctlsocket(0, FIONBIO, &mode);

#else
	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);
#endif
//
// All done. Let's send and receive messages
// Establish logical connection with X32 server
	printf("X32Replay - v0.3 - (c)2014 Patrick-Gilles Maillot\n\n");
//
	keep_on = 1;
	printf("Connecting to X32.");
	Xb_ls = Xsprint(Xb_s, 0, 's', "/info");
	while (keep_on) {
		if (sendto(Xfd, Xb_s, Xb_ls, 0, Xip_pt, Xip_len) < 0) {
			perror("couldn't send data to X32");
			return 1;
		}
		RPOLL 				// read data if available
		if (p_status < 0) {
			printf("Polling for data failed\n");
			return 1;		// exit on receive error
		} else if (p_status > 0) {	// We have received data - process it!
			Xb_lr = recvfrom(Xfd, Xb_r, XBRMAX, 0, Xip_pt, &Xip_len);
			if (strcmp(Xb_r, "/info") == 0)
				break;		// Connected!
		}					// ... else timeout
		printf("."); fflush(stdout);
	}
	printf(" Done!\n");
//
//Connection established
	X32file_pt = NULL;
	l_index = 0;
	u_data = 0;
	play_on = 0;
	record_on = 0;
	record_pause = 0;
	before = 0;
	t_dlta.tv_sec = 0;
	t_dlta.tv_usec = 0; // our reference delta time
	timeout.tv_usec = 10000; //Set timeout for subsequent recvfrom(): 10ms
//
	while (keep_on) {
		if (record_on) {
// RECORD mode
			now = time(NULL); // get time in seconds
			if (now > before + 9) { // need to keep xremote alive?
				Xb_ls = Xsprint(Xb_s, 0, 's', "/xremote");
				if (X32verbose) {Xfdump("->X", Xb_s, Xb_ls, DEBUG); fflush(stdout);}
				if (sendto(Xfd, Xb_s, Xb_ls, 0, Xip_pt, Xip_len) < 0) {
					perror("coundn't send data to X32");
					keep_on = 0;
				}
				before = now;
			}
// X32 sent something?
			RPOLL
			// Dialog with the X32 as long as there's no need to send /xremote again
			// for now, read and echo the desk
			if (p_status > 0) {
				if ((Xb_lr = recvfrom(Xfd, Xb_r, XBRMAX, 0, Xip_pt, &Xip_len)) > 0) {
					gettimeofday(&t_rec, NULL);						// get precise time
					fwrite(&t_rec, sizeof(t_rec), 1, X32file_pt);	// write record time
					fwrite(&Xb_lr, sizeof(int), 1, X32file_pt);		// write # of record bytes
					fwrite(Xb_r, sizeof(char), Xb_lr, X32file_pt);	// write record buffer
					fflush(X32file_pt);
					if (X32verbose) {
						printf("Time: %ld.%ld\n", t_rec.tv_sec, t_rec.tv_usec);
						Xfdump("X->", Xb_r, Xb_lr, DEBUG);
						fflush(stdout);
					}
				}
			}
			//
			// Take into account user input in order to record
			// comments to add to the file - Say comment will all start with # so
			// they are easy to ignore during reading for automation playback
			//
			if (u_data) {
				gettimeofday(&t_rec, NULL);						// get precise time
				fwrite(&t_rec, sizeof(t_rec), 1, X32file_pt);	// write record time
				fwrite(&l_index, sizeof(int), 1, X32file_pt);			// write # of record bytes
				fwrite(inputline, sizeof(char), l_index, X32file_pt);	// write record buffer
				fflush(X32file_pt);
				if (X32verbose) {
					printf("Time: %ld.%ld\n", t_rec.tv_sec, t_rec.tv_usec);
					Xfdump(">>>", inputline, l_index, DEBUG);
					fflush(stdout);
				}
				printf("\n");
				fflush(stdout);
				u_data = 0;
				l_index = 0;
			}
		} else if (play_on) {
//
// PLAY mode
			gettimeofday(&t_rec, NULL);						// get precise time
			timersub(&t_rec, &t_play, &t_dlta1);
			if(!(timercmp(&t_dlta1, &t_dlta, <))) {
				Xb_lr = fread(&Xb_ls, sizeof(int), 1, X32file_pt);		// read # of record bytes
				Xb_lr = fread(Xb_s, sizeof(char), Xb_ls, X32file_pt);	// read record buffer
				//
				// Take into account user input in order print back comments
				// for automation playback
				//
				if (*Xb_s != '#') { // the command is aimed at X32
					if (sendto(Xfd, Xb_s, Xb_ls, 0, Xip_pt, Xip_len) < 0) {	//update X32
						perror("coundn't send data to X32");
						fclose (X32file_pt);
						X32file_pt = NULL;
						play_on = 0;
					}
				} else {
					Xb_s[Xb_lr] = 0;
					printf ("%s\n", Xb_s); fflush(stdout);
				}
				if (X32verbose) {Xfdump("->X", Xb_s, Xb_ls, DEBUG); fflush(stdout);}
				if ((Xb_lr = fread(&t_play, sizeof(t_play), 1, X32file_pt)) == 0) {	// read next record time
					printf ("-->End of replay file\n"); fflush(stdout);
					fclose (X32file_pt);
					X32file_pt = NULL;
					play_on = 0;
				}
			}
		}
// CHECK USER input
// build command by reading keyboard characters (from stdin)
#ifdef __WIN32__
		if (kbhit()) {
			input_ch = _getch();
#else
		input_ch = getc(stdin);
		{
#endif
			if (input_ch == EOL) {
// Check for program exit command ("exit")
				if (l_index) {
					if (inputline[0] == '#') {
						u_data = 1; // the data is meant as a comment for recording
					} else {
#ifdef __WIN32__
						printf("\n");
#endif
						inputline[l_index] = 0;
						if (strcmp(inputline, "exit") == 0) keep_on = 0;
						else if (strcmp(inputline, "stop") == 0) {
							if (play_on || record_on) {
								// Close file
								if (X32file_pt) {
									fclose(X32file_pt);
									X32file_pt = NULL;
								}
							}
							printf("activity stopped\n"); fflush(stdout);
							play_on = 0;
							record_on = 0;
							record_pause = 0;
						} else if ((strcmp(inputline, "record pause") == 0) ||
								   (strcmp(inputline, "pause") == 0)){
							if (record_on) {
								record_pause = 1;
								record_on = 0;
								printf("record paused\n"); fflush(stdout);
							} else {
								printf("record pause: no change\n"); fflush(stdout);
							}
						} else if (strcmp(inputline, "record") == 0) {
							printf("record is %s\n",((record_on)?"on":"off"));
							if (record_pause) printf("recording is paused\n");
							fflush(stdout);
						} else if (strcmp(inputline, "record off") == 0) 	{
							if (record_on) {
								// Close file
								if (X32file_pt) {
									fclose(X32file_pt);
									X32file_pt = NULL;
								}
							}
							record_on = 0;
						} else if (strcmp(inputline, "record on") == 0) {
							if (play_on) {
								printf("Use stop or play off first\n");
							} else {
								if (record_pause) {
									record_pause = 0;
									record_on = 1;
								} else {
									// Open file for writing
									if ((X32file_pt = fopen(X32file_ch, "wb")) == NULL) {
										printf ("cannot open file for recording\n");
									} else {
										gettimeofday(&t_rec, NULL);						// get precise time
										if (X32verbose) printf("t_rec: %ld.%ld\n", t_rec.tv_sec, t_rec.tv_usec);
										fwrite(&t_rec, sizeof(t_rec), 1, X32file_pt);	// write start record time
										record_on = 1;
									}
								}
							}
						} else if (strcmp(inputline, "play") == 0) {
							printf("play is %s\n",((play_on)?"on":"off"));
							fflush(stdout);
						} else if (strcmp(inputline, "play off") == 0) {
							if (play_on) {
								// Close file
								if (X32file_pt) {
									fclose(X32file_pt);
									X32file_pt = NULL;
								}
							}
							play_on = 0;
						} else if (strcmp(inputline, "play on") == 0) {
							if (record_on) {
								printf("Use stop or record off first\n");
							} else {
								// Open file for reading
								if ((X32file_pt = fopen(X32file_ch, "rb")) == NULL) {
									printf ("cannot open file for playing\n");
								} else {
									play_on = 1;
									gettimeofday(&t_rec, NULL);	// get precise start play time
									Xb_lr = fread(&t_play, sizeof(t_play), 1, X32file_pt);	// read start record time
									timersub(&t_rec, &t_play, &t_dlta);
									Xb_lr = fread(&t_play, sizeof(t_play), 1, X32file_pt);	// read first record time
									if (X32verbose) printf("t_dlta: %ld.%ld\n", t_dlta.tv_sec, t_dlta.tv_usec);
								}
							}
						}
						l_index = 0;
					}
				}
			} else {
//
// parse input_ch values for setting flags
				if (input_ch != -1) {
#ifdef __WIN32__
					printf("%c", input_ch);
#endif
					inputline[l_index++] = input_ch;
				}
				if (input_ch == BACKSPACE) {
#ifdef __WIN32__
					printf(" \b");
#endif
					l_index -= BACKCHARS; // remove char if backspace entered
					if (l_index < 0) l_index = 0;
				}
			}
		}
	}
#ifdef __WIN32__
	if (Xfd) closesocket(Xfd);
	WSACleanup();
#else
	if (Xfd) close(Xfd);
#endif
	if (X32file_pt) {
		fclose(X32file_pt);
		X32file_pt = NULL;
	}
	return 0;
}







