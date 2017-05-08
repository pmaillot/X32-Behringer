/*
 * X32PunchControl.c
 *
 *  Created on: Aug 25, 2016
 *
 * ©2016 - Patrick-Gilles Maillot
 *
 * X32PunchControl - a windows app for managing DAW punch IN/OUT down mixing updates
 *
 * Change log
 *    0.01: Creation
 *    0.02: Added button & bank capability - code cleanup
 *    0.03: Midi out & Midi in OK - enabled MTC to display
 *    0.04: Midi selection through dropboxes - fixed reset functions behavior
 *    0.05: Better logic around file handling
 *    0.06: MTC now functional for play/punch - code cleaning
 *    0.07: Get Frame rate from MTC to recalculate more accurate time
 *    0.08: Use 8 buttons.
 *    0.09: Use a different .ini import mechanism / Enable merging off only faders
 *    0.10: fixed bug in the Bank selection; added a Record imply Play selection option
 *    0.11: got rid of "reset temp file"; added a default select for MTC vs. LTC
 *    0.12: forgot Xrecord flag in XExecuteReadMerge(); changed text Midi: ON/OFF & MERGE: ON/OFF
 *    0.13: code cleanup; better handling of catchback when REW button/GUI is used
 *    0.14: code cleanup; handling of catchup when FF button/GUI is used
 *    0.15: missing exit-loop condition in XCatchBackProc()
 *    0.16: new algorithm in catch-up XCatchBackProc()
 *    0.17: Better handling of early stop, ensuring work will not be lost
 */
#include <winsock2.h>	// Windows functions for std GUI & sockets
#include <commdlg.h>	// Windows widgets functions (file handling..)
#include <mmsystem.h>	// Multimedia functions (such as MIDI) for Windows
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <io.h>
//
//
#define BSIZE 512		// X32 dialog buffer size
#define MAXFN 256		// File name max length
//
// Parameter (resource file) handling
typedef enum {
	P_CHR,
	P_INT,
	P_FLT,
	P_STR,
	P_HSTR,
	P_YN
} Ptype;
//
typedef union  {
	char	c1;
	char*	s1;
	int		i1;
	float	f1;
} Pdata;
//
typedef struct param {
	char*	param_name;
	Ptype   param_type;
	Pdata	param_data;
} Param;
//
// External calls used
extern int  Xsprint(char *buffer, int index, char format, void *data);
extern int  Xfprint(char *buffer, int index, char *header, char format, void *data);
extern int  X32Connect(int Xconnected, char* Xip_str, int btime);
extern int  validateIP4Dotted(const char *s);
extern int  FileParse(char* fname, Param* par_tab);
extern void FreeParse(Param* par_tab);
//
// Private functions
void CALLBACK XReceiveMidi(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
void XClearDisplay();
void XDisplayMTC();
void XDisplayLocalTimer();
void XDisplayMidiTimer();
void XWriteAndSend(int send);
void XExecuteReadMerge();
int  XParseBank();
void XInitUsrCtrl();
void XPlayGUI();
void XPunchGUI();
void XMergeGUI();
void XPauseGUI();
void XFfGUI();
void XRewGUI();
void XStopGUI();
void XRecordGUI();
int  XMidiConnect();
void XSendMidi(char *XMidi_str);
void XCloseMidiDevices();
void XLoadScene();
int  XListMidiInDevices();
int  XListMidiOutDevices();
void XCatchBackProc();
void XCatchUpProc();


//
//
// Windows Declarations
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE		hInstance = 0;
HWND			hwndipaddr, hwndconx, hwndfile, hwndrfile, hwndscene, hwndrscene;
HWND			hwndPLAY, hwndPUNCH, hwndMERGE, hwndPAUSE, hwndtcode, hwndmport;
HWND			hwndREW, hwndFF, hwndSTOP, hwndREC, hwndMFdrs;
HWND			hwndmincombo, hwndmoutcombo, hwndreset, hwnddfile, hwndTop;
HWND			Ghwnd;
RECT			rc1 = {300,  5, 540, 60};
RECT			rc2 = {305, 10, 535, 55};
HFONT			hfont, htmp;
HDC				hdc;
PAINTSTRUCT		ps;
MSG				wMsg;
OPENFILENAME	ofn;       			// common dialog box structure
//
// Parameter (resource file) handling
// (see testparams.c)
Param MyParameters[] = {
		{"Xip_str", 			P_STR,	{ 0 }},
		{"Xscene_str", 			P_STR,	{ 0 }},
		{"Xusemidi", 			P_YN,	{ 0 }},
		{"Xmidiinport",			P_INT,	{ 0 }},
		{"Xmidioutport", 		P_INT,	{ 0 }},
		{"Xmidi_play_str",		P_HSTR,	{ 0 }},
		{"Xmidi_pchin_str",		P_HSTR,	{ 0 }},
		{"Xmidi_pchout_str",	P_HSTR,	{ 0 }},
		{"Xmidi_stop_str",		P_HSTR,	{ 0 }},
		{"Xmidi_pause_str",		P_HSTR,	{ 0 }},
		{"Xmidi_resume_str",	P_HSTR,	{ 0 }},
		{"Xmidi_reset_str",		P_HSTR,	{ 0 }},
		{"Xmidi_bot_str",		P_HSTR,	{ 0 }},
		{"Xmidi_eot_str",		P_HSTR,	{ 0 }},
		{"Xmidi_record_str",	P_HSTR,	{ 0 }},
		{"Xusebank",			P_YN,	{ 0 }},
		{"Xbank",				P_CHR,	{ 0 }},
		{"Xbbcolor",			P_INT,	{ 0 }},
		{"Xkeepontop", 			P_YN,	{ 0 }},
		{"Xrecordplay", 		P_YN,	{ 0 }},
		{"XMTCon", 				P_YN,	{ 0 }},
		{"Xcatchdelay",			P_INT,	{ 0 }},
		{}
};
//
#define Xip_str				MyParameters[0].param_data.s1	// X32 IP
#define Xscene_str			MyParameters[1].param_data.s1	// Scene number to load
#define Xusemidi			MyParameters[2].param_data.i1	// Whether to use MIDI or not
#define Xmidiinport			MyParameters[3].param_data.i1	// MIDI in port #
#define Xmidioutport		MyParameters[4].param_data.i1	// MIDI out port #
#define Xmidi_play_str		MyParameters[5].param_data.s1	// MIDI "play" string
#define Xmidi_pchin_str		MyParameters[6].param_data.s1	// MIDI "punch in" string
#define Xmidi_pchout_str	MyParameters[7].param_data.s1	// MIDI "Punch out" string
#define Xmidi_stop_str		MyParameters[8].param_data.s1	// MIDI "stop" string
#define Xmidi_pause_str		MyParameters[9].param_data.s1	// MIDI "pause" string
#define Xmidi_resume_str	MyParameters[10].param_data.s1	// MIDI "resume" string
#define Xmidi_reset_str		MyParameters[11].param_data.s1	// MIDI "reset" string
#define Xmidi_bot_str		MyParameters[12].param_data.s1	// MIDI "rew/bot" string
#define Xmidi_eot_str		MyParameters[13].param_data.s1	// MIDI "ff/eot" string
#define Xmidi_record_str	MyParameters[14].param_data.s1	// MIDI "record" string
#define Xusebank			MyParameters[15].param_data.i1	// Whether to use X32 userbank or not
#define Xbank				MyParameters[16].param_data.c1	// User bank number
#define Xbbcolor			MyParameters[17].param_data.i1	// User bank scribbles color
#define Xkeepontop			MyParameters[18].param_data.i1	// App window always on top
#define Xrecordplay			MyParameters[19].param_data.i1	// Record implies Play
#define XMTCon				MyParameters[20].param_data.i1	// start with MTC enable
#define Xcatchdelay			MyParameters[21].param_data.i1	// delay in ms used during catchup
//
// end of Parameter (resource file) handling
//
// Global variables
FILE				*res_file;
FILE				*X32file_r_pt;
FILE				*X32file_w_pt;
//
// Midi data
char				Xmidiinbuffer[BSIZE / 2];	// MIDI IN buffer
HMIDIOUT			MidiOutDevice;    // MIDI device interface for sending MIDI output
MIDIHDR				MidiOutHdr = {0};
LPMIDIHDR			lpMidiOutHdr = &MidiOutHdr;
UINT				cbMidiOutHdr = sizeof(MidiOutHdr);
//
HMIDIIN				MidiInDevice;    // MIDI device interface for reading MIDI input
MIDIHDR				MidiInHdr = {0};
LPMIDIHDR			lpMidiIntHdr = &MidiInHdr;
UINT				cbMidiInHdr = sizeof(MidiInHdr);
int					Mflag;						// Midi flag
int					XCtachBack = 0;				// CatchBack flag
int					XCatchUp = 0;				// CatchUp flag
//
int 				X0127[] = {0, 127};
//
int					keep_running = 1;
int					wWidth = 560;				// Default window size
int					wHeight = 245;				//
int					Xmergefaders = 0;			// if 1 will only protect only from fader changes
int					Xbid[8];					// button id associated to button
int					Xmidihr, Xmidimn, Xmidiss, Xmidifr, Xfrrate;	// Midi TC data
int					Xoldmidihr, Xoldmidimn, Xoldmidiss, Xoldmidifr;	// Midi TC "old" data
//
int					XRm, YRm, XRM, YRM;
char				str1[64];
//
char				Xpathsave[MAXFN], Xpath[MAXFN];				// file paths
char				Xfile_r_str[32] = "No file selected";
char				Xfile_w_str[40];
// flags
int					Xconnected = 0;				// X32 is connected (1)
int					XMconnected = 0;			// MIDI is connected (1)
int					Xreadfile = 0;				// file info available (valid)
int					Xplay = 0;					// button PLAY state
int					Xpunch = 0;					// button PUNCH state
int					Xmerge = 0;					// button MERGE: ON state
int					Xpause = 0;					// button PAUSE state
int					Xrecord = 0;				// button RECORD state
int					Xfiledataready = 0;			// indicates if a record should be treated
int					XfRate[] = {41666, 40000, 33366, 33333}; // 1/24, 1/25, 1/30 drop, 1/30 fps
char				XInMidiNames[16][32];		// List of Midi IN devices (limit 16)
char				XOutMidiNames[16][32];		// List of Midi OUT devices (limit 16)
//
char 				r_buf[BSIZE], s_buf[BSIZE];	// X32 receive and send buffers
int 				r_len, s_len;				// associated data lengths
int 				p_status;					// "polling" status
//
WSADATA				wsa;						// Windows Sockets
int 				Xfd;						// X32 socket
struct sockaddr_in	Xip;						// IP structure
struct sockaddr		*Xip_addr = (struct sockaddr*) &Xip;
struct timeval		timeout;					// timeout for non blocking udp comm
fd_set 				ufds;						// file descriptor
int 				Xip_len = sizeof(Xip);		// length of addresses
//
struct timeval		t_1sec = {1, 0};			// 1 second timer
struct timeval		t_now, t_before, t_play, t_pause, t_rew, t_ff;	// time structures
struct timeval		dt_read, dt_play;			// dt_read: from the file, dt_play: t_now - t_play
char				xremote[12] = "/xremote";	// extra chars are set to \0
//
// Macros
//
#define SEND_TO(b, l)										\
	do {													\
		if (sendto(Xfd, b, l, 0, Xip_addr, Xip_len) < 0) {	\
			perror ("Can't send data to X32\n");			\
			exit(EXIT_FAILURE);								\
		} 													\
	} while (0);
//
//
#define RECV_FR(b, l)									\
	do {												\
		l = recvfrom(Xfd, b, BSIZE, 0, 0, 0);			\
	} while (0);
//
//
#define RPOLL											\
	do {												\
		FD_ZERO (&ufds);								\
		FD_SET (Xfd, &ufds);							\
		p_status = select(Xfd+1, &ufds, 0, 0, &timeout);\
	} while (0);
//
//
#ifndef timeradd
#define	timeradd(a, b, result)								\
	do {													\
		(result)->tv_sec = (a)->tv_sec + (b)->tv_sec;		\
		(result)->tv_usec = (a)->tv_usec + (b)->tv_usec;	\
		if ((result)->tv_usec >= 1000000) {					\
			++(result)->tv_sec;								\
			(result)->tv_usec -= 1000000;					\
		}													\
	} while (0)
#endif
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
//
#ifndef timercmp
#define timercmp(a, b, CMP)									\
	do {													\
		  (((a)->tv_sec == (b)->tv_sec) ?					\
		   ((a)->tv_usec CMP (b)->tv_usec) :				\
		   ((a)->tv_sec CMP (b)->tv_sec))					\
	} while (0)
#endif
//
//
//
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdFile) {
//
	WNDCLASSW wc = {0};
	wc.lpszClassName = L"X32PunchControl";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
		L"X32PunchControl - Manage DAW punch IN/OUT down mixing updates",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 220, wWidth, wHeight, 0, 0, hInstance, 0);
//
// some initializations needed:
	t_pause.tv_sec = t_pause.tv_usec = t_before.tv_sec = t_rew.tv_sec = 0;
	t_ff.tv_sec  = 0;
	XMconnected = Xconnected = Xplay = Xpunch = Xrecord = Xmerge = Xpause = 0;
//
// Main loop
	while (keep_running) {
		gettimeofday(&t_now, NULL);		// current timestamp
		if (PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		}
		if (Xconnected) {
			// Manage the need for sending /xremote requests (every 10s)
			if (t_now.tv_sec > t_before.tv_sec + 9) {
				SEND_TO(xremote, 12)
				t_before = t_now;
			}
			// t_rew.tv_sec is typically = to 0, except when REW has been hit.
			// a timeout of 1 second enables to ensure there's been enough time after a REW or FF
			// command to set catchback mode, enabling the tool to play as quickly as possible
			// already recorded commands to attain the state it should be at for the new cursor
			// position
			if (t_rew.tv_sec) {
				if (timercmp(&t_now, &t_rew, >)) {
					XCtachBack = 1;
				}
			}
			// XCtachBack can be set either if REW has been hit (1 second after the last hit)
			// or if the DAW cursor was moved back in time, getting MTC to be reset using a MIDI
			// SYSEX set time command
			if (XCtachBack) {
				XCatchBackProc();
				XCtachBack = 0;
				t_rew.tv_sec = 0;
			}
// below an attempt to manage FF / forward jumps... not working as expected :-(
// DAW (REAPER) is not sending SYSEX updates as it does for backward jumps.
// There seems also to be some kind of a race condition on FF clicks, programmed
// the same way as REW clicks but does not behave similarly... TODO therefore
//
//			// t_ff.tv_sec is typically = to 0, except when FF has been hit.
//			// a timeout of 1 second enables to ensure there's been enough time after a FF
//			// command to set catchup mode, enabling the tool to play as quickly as possible
//			// already recorded commands to attain the state it should be at for the new cursor
//			// position
//			if (t_ff.tv_sec) {
//				if (timercmp(&t_now, &t_ff, >)) {
//					XCatchUp = 1;
//				}
//			}
//			// XCatchUp can be set either if FF has been hit (1 second after the last hit)
//			// or if the DAW cursor was moved forward in time, getting MTC to be reset using a MIDI
//			// SYSEX set time command
//			if (XCatchUp) {
//				XCatchUpProc();
//				XCatchUp = 0;
//				t_ff.tv_sec = 0;
//			}
			if (Xplay) {
				if (!Xpause) {
					//
					// Manage Local and Midi TC & display
//					if (XMTCon) {
//						// if MTC is selected the actual update of the MTC panel is
//						// made within callback mechanism (see XReceiveMidi()) as
//						// there's no need to do it more often than per frame.
//						// set dt_play accordingly (convert MTC to LTC)
//						dt_play.tv_sec = Xmidihr * 3600 + Xmidimn * 60 + Xmidiss;
//						dt_play.tv_usec = Xmidifr * Xfrrate ;	// that's the number of µs using frames/s
//					} else {
					if (!XMTCon) {
					// local TC handling
						timersub(&t_now, &t_play, &dt_play);	// dt_play = t_now - t_play
						XDisplayLocalTimer();					// display dt_play = Local TC
					}
				}
				// Any thing to do with current data file record?
				// or did we receive an OSC command from X32?
				Xfiledataready = 0;
				if (Xreadfile) {
					if(timercmp(&dt_play, &dt_read, >)) {
						r_len = fread(&s_len, sizeof(int), 1, X32file_r_pt);		// read # of record bytes
						r_len = fread(s_buf, sizeof(char), s_len, X32file_r_pt);	// read record buffer
						Xfiledataready = 1;
					}
				}
				RPOLL		// X32 data in?
				if ((p_status > 0) || Xfiledataready) XExecuteReadMerge();    // manage data
			} else {
				// connected but not started: accept button play and punch (at minimum)
				RPOLL		// X32 data in?
				if (p_status > 0) {
					RECV_FR(r_buf, r_len)
					r_len = XParseBank();	// Parse buttons if use bank enabled (ignore returned value)
				}
			}
		} else {
			Sleep(5); // avoid high CPU by waiting 5ms if there's nothing to do
		}
	}
	return (int) wMsg.wParam;
}
//
// Windows Callbacks
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int i;
	int XMidiDevNum;
//
	Ghwnd = hwnd;		// Ghwnd: global window hwnd
	switch (msg) {
	case WM_CREATE:
		//
		hwndipaddr = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 40, 120, 20, hwnd,
				(HMENU )0, NULL, NULL);

		hwndconx = CreateWindow("button", "Connect", WS_VISIBLE | WS_CHILD,
				130, 40, 85, 20, hwnd, (HMENU )1, NULL, NULL);

		hwndrfile = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 345, 115, 195, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndfile = CreateWindow("button", "Choose File",
				WS_VISIBLE | WS_CHILD, 255, 115, 85, 20, hwnd, (HMENU )2, NULL, NULL);

		hwndrscene = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 115, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndscene = CreateWindow("button", "Load Scene",
				WS_VISIBLE | WS_CHILD, 105, 115, 100, 20, hwnd, (HMENU )3, NULL, NULL);

		hwndtcode = CreateWindow("BUTTON", "MTC/LTC",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
				rc1.left + 45, rc1.bottom + 5, 80, 20, hwnd, (HMENU)4, NULL, NULL);

		hwndTop = CreateWindow("BUTTON", "KeepOnTop",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
				400 + 45, rc1.bottom + 5, 130, 20, hwnd, (HMENU)9, NULL, NULL);

		hwndMFdrs = CreateWindow("BUTTON", "MergeOFF: Faders Only",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
				rc1.left + 45, rc1.bottom + 30, 200, 20, hwnd, (HMENU)8, NULL, NULL);

		hwndmincombo = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				70, 63, 180, 140, hwnd, (HMENU)0, NULL, NULL);
		if (Xusemidi) {
			// Load dropdown item list
			XMidiDevNum = XListMidiInDevices();
			for (i = 0; i < XMidiDevNum; i++) {
				SendMessage(hwndmincombo, CB_ADDSTRING, (WPARAM)0, (LPARAM)XInMidiNames[i]);
			}
			// display initial item 0 / off in the selection field
			SendMessage(hwndmincombo, CB_SETCURSEL, (WPARAM)Xmidiinport, (LPARAM)0);
		} else {
			SendMessage(hwndmincombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		}
		// display initial item 0 / off in the selection field
		SendMessage(hwndmincombo, CB_SETCURSEL, (WPARAM)Xmidiinport, (LPARAM)0);
		hwndmoutcombo = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				70, 88, 180, 140, hwnd, (HMENU)0, NULL, NULL);
		if (Xusemidi) {
		// Load dropdown item list
			XMidiDevNum = XListMidiOutDevices();
			for (i = 0; i < XMidiDevNum; i++) {
				SendMessage(hwndmoutcombo, CB_ADDSTRING, (WPARAM)0, (LPARAM)XOutMidiNames[i]);
			}
			// display initial item 0 / off in the selection field
			SendMessage(hwndmoutcombo, CB_SETCURSEL, (WPARAM)Xmidioutport, (LPARAM)0);
		} else {
			SendMessage(hwndmoutcombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		}

		hwndmport = CreateWindow("button", "Midi: OFF", WS_VISIBLE | WS_CHILD | BS_MULTILINE,
				255, 63, 40 + 45, 48, hwnd, (HMENU )5, NULL, NULL);

		hwndreset = CreateWindow("button", "RESET", WS_VISIBLE | WS_CHILD,
				220, 40, 75, 20, hwnd, (HMENU )6, NULL, NULL);

		hwndREW = CreateWindow("button", "|<<",
				WS_VISIBLE | WS_CHILD, 5, 145, 130, 25, hwnd, (HMENU )17, NULL, NULL);

		hwndPLAY = CreateWindow("button", "PLAY",
				WS_VISIBLE | WS_CHILD, 140, 145, 130, 25, hwnd, (HMENU )18, NULL, NULL);

		hwndPAUSE = CreateWindow("button", "||",
				WS_VISIBLE | WS_CHILD, 275, 145, 130, 25, hwnd, (HMENU )19, NULL, NULL);

		hwndFF = CreateWindow("button", ">>|",
				WS_VISIBLE | WS_CHILD, 410, 145, 130, 25, hwnd, (HMENU )20, NULL, NULL);

		hwndPUNCH = CreateWindow("button", "PUNCH IN",
				WS_VISIBLE | WS_CHILD, 5, 180, 130, 25, hwnd, (HMENU )21, NULL, NULL);

		hwndMERGE = CreateWindow("button", "MERGE: ON",
				WS_VISIBLE | WS_CHILD, 140, 180, 130, 25, hwnd, (HMENU )22, NULL, NULL);

		hwndSTOP = CreateWindow("button", "STOP",
				WS_VISIBLE | WS_CHILD, 275, 180, 130, 25, hwnd, (HMENU )23, NULL, NULL);

		hwndREC = CreateWindow("button", "RECORD",
				WS_VISIBLE | WS_CHILD, 410, 180, 130, 25, hwnd, (HMENU )24, NULL, NULL);

		break;
//
	case WM_PAINT:
		//
		hdc = BeginPaint(hwnd, &ps);
		//
		// update user information
		SetWindowText(hwndrfile, (LPSTR) Xfile_r_str);
		SetWindowText(hwndipaddr, (LPSTR) Xip_str);
		SetWindowText(hwndrscene, (LPSTR) Xscene_str);
		SendMessage(hwndTop, BM_SETCHECK, Xkeepontop, 0);
		SendMessage(hwndtcode, BM_SETCHECK, XMTCon, 0);
		SetWindowPos(hwnd, Xkeepontop?HWND_TOPMOST:HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		//
		// prepare LTC/MTC window
		SetBkMode(hdc, TRANSPARENT);
		Rectangle(hdc, rc1.left, rc1.top, rc1.right, rc1.bottom);
		Rectangle(hdc, rc2.left, rc2.top, rc2.right, rc2.bottom);
		MoveToEx(hdc, 102, 65, NULL);
		LineTo(hdc, 102, 110);
		//
		// update diverse texts
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, FIXED_PITCH, NULL);//TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 5, 25, str1, wsprintf(str1, "Set X32 IP below:"));
		TextOut(hdc, 5, 67, str1, wsprintf(str1, "MIDIin:"));
		TextOut(hdc, 5, 92, str1, wsprintf(str1, "MIDIout:"));
		TextOut(hdc, 5, 117, str1, wsprintf(str1, "X32Scene:"));
		TextOut(hdc, 223, 15, str1, wsprintf(str1, "ver. 0.17"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 5, 0, str1, wsprintf(str1, "X32PunchControl - ©2016 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		EndPaint(hwnd, &ps);
		XClearDisplay();

		break;
//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			switch (LOWORD(wParam)) {
			case 1: // connect X32
				GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32Connect(Xconnected, Xip_str, 5000);
					if (Xconnected)	{
						SetWindowText(hwndconx, "Connected");
						// init user bank if needed
						XInitUsrCtrl();
					} else {
						Xplay = Xpunch = Xrecord = Xmerge = Xpause = 0;
						SetWindowText(hwndconx, "Connect");
						SetWindowText(hwndPLAY, "PLAY");
						SetWindowText(hwndPUNCH, "PUNCH IN");
						SetWindowText(hwndMERGE, "MERGE: ON");
						SetWindowText(hwndPAUSE, "||");
					}
				} else {
					printf("Incorrect IP string form\n");
				}
				break;
			case 2:	// Merge to file name obtained from dialog
				Xreadfile = 0;
				strcpy(Xpath, Xfile_r_str);
				SetWindowText(hwndrfile, (LPSTR) Xfile_r_str);
				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = Xpath;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				//ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(Xpath);
				ofn.lpstrFilter = "X32 Punch Control\0*.xpc\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrTitle = (LPCTSTR) "Select a File name to merge your X32 data from\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST;
				//| OFN_FILEMUSTEXIST;
				//
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
					strcpy(Xpathsave, Xpath);
					// remove filename from returned path so we can either save to
					// existing directory or newly created one
					i = strlen(Xpath);
					while (i && (Xpath[i] != '\\')) --i;
					Xpath[i] = 0;
					strcpy(Xfile_r_str, Xpath + i + 1);
					// test if an extension is already present; if not add one
					i = strlen(Xfile_r_str);
					while (i && (Xfile_r_str[i] != '.')) --i;
					if (i == 0) {
						// add an extension
						strcat(Xfile_r_str, ".xpc");
						strcat(Xpathsave, ".xpc");
					}
					sprintf(s_buf, "Directory: %s\nFile Name: %s", Xpath, Xfile_r_str);
					if (MessageBox(NULL, s_buf, "Source directory & File name: ", MB_OKCANCEL) == IDOK) {
						SetWindowText(hwndrfile, (LPSTR) Xfile_r_str);
						// Open files for reading and writing
						if ((X32file_r_pt = fopen(Xfile_r_str, "rb+"))) {
							Xreadfile = 1;
							// X32file_r_pt can be null... means there's nothing to read from, that's OK
							if ((r_len = fread(&dt_read, sizeof(dt_read), 1, X32file_r_pt)) == 0) {
								Xreadfile = 0;
								Xfiledataready = 0;
							}
						}
						strcpy(Xfile_w_str, Xfile_r_str);
						strcat(Xfile_w_str, "_xpc");
						if ((X32file_w_pt = fopen(Xfile_w_str, "wb")) == NULL) {
							MessageBox(NULL, "cannot open file for recording", NULL, MB_OKCANCEL);
						}
					}
					strcat(Xpathsave, ".old");
					remove(Xpathsave);
					Xpathsave[strlen(Xpathsave) - 4] = 0;
				}
				break;
			case 3:
				// Scene number
				GetWindowText(hwndrscene, Xscene_str, GetWindowTextLength(hwndrscene) + 1);
				XLoadScene();
				break;
			case 4:
				// Time Code Display
				if (Xusemidi) {
					XMTCon ^= 1;
					SendMessage(hwndtcode, BM_SETCHECK, XMTCon, 0);
					XClearDisplay();
				}
				break;
			case 5:
				// MIDI connect
				if (Xusemidi) {
					XMconnected = XMidiConnect();
					SetWindowText(hwndmport, XMconnected ? "Midi: ON" : "Midi: OFF");
				}
				break;
			case 6:
				// RESET
				if (MessageBox(NULL, "RESET?\nAre you sure?", "Warning!", MB_YESNO | MB_ICONWARNING) == IDYES) {
					SetWindowText(hwndrfile, "No file selected");
					SetWindowText(hwndconx, "Connect");
					SetWindowText(hwndmport, "Midi: OFF");
					SetWindowText(hwndPLAY, "PLAY");
					SetWindowText(hwndPUNCH, "PUNCH IN");
					SetWindowText(hwndMERGE, "MERGE: ON");
					SetWindowText(hwndPAUSE, "||");
					XClearDisplay();
					if (X32file_w_pt) {
						fclose(X32file_w_pt);
						X32file_w_pt = 0;
					}
					if (X32file_r_pt) {
						fclose(X32file_r_pt);
						X32file_r_pt = 0;
					}
					Xfile_r_str[0] = 0;
					XSendMidi(Xmidi_reset_str);
					XCloseMidiDevices();
					if (Xconnected) {
						XInitUsrCtrl();
						close(Xfd);
					}
					t_pause.tv_sec = t_pause.tv_usec = t_before.tv_sec = t_rew.tv_sec = 0;
					t_ff.tv_sec = 0;
					XMconnected = Xconnected = Xplay = Xpunch = Xrecord = Xmerge = Xpause = 0;
				}
				break;
			case 7:	// Reset the working/temp file
				if (!Xplay) {
					if (*Xfile_w_str) {
						sprintf(s_buf, "reset \"%s\" temp file?", Xfile_w_str);
						if (MessageBox(NULL, s_buf, "Warning!", MB_YESNO | MB_ICONWARNING) == IDYES) {
							// re-open the file (truncates it)
							if ((X32file_w_pt = fopen(Xfile_w_str, "wb")) == NULL) {
								MessageBox(NULL, "could not reset temp file", NULL, MB_OK);
							}
						}
					}
				} else {
					MessageBox(NULL, "set STOP state first", NULL, MB_OK);
				}
				break;
			case 8:
				Xmergefaders ^= 1;
				SendMessage(hwndMFdrs, BM_SETCHECK, Xmergefaders, 0);
				break;
			case 9:
				Xkeepontop ^= 1;
				SendMessage(hwndTop, BM_SETCHECK, Xkeepontop, 0);
				SetWindowPos(hwnd, Xkeepontop?HWND_TOPMOST:HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				break;
			case 17:
				// REW/BOT clicked
				if (Xconnected && X32file_r_pt) XRewGUI();
				break;
			case 18:
				// PLAY clicked
				if (Xconnected && X32file_r_pt) XPlayGUI();
				break;
			case 19:
				// PAUSE clicked
				if (Xconnected) XPauseGUI();
				break;
			case 20:
				// FF/EOT clicked
				if (Xconnected && X32file_r_pt) XFfGUI();
				break;
			case 21:
				// PUNCH clicked
				if (Xconnected && X32file_w_pt) XPunchGUI();
				break;
			case 22:
				// MERGE: clicked
				if (Xconnected) XMergeGUI();
				break;
			case 23:
				// STOP clicked
				if (Xconnected) XStopGUI();
				break;
			case 24:
				// Record clicked
				if (Xconnected && X32file_w_pt) XRecordGUI();
				break;
			}
		}
		break;
	case WM_DESTROY:
		if (Xconnected) close(Xfd);
		keep_running = 0;
		// Is there additional data to copy to the write file?
		if (X32file_r_pt) {
			Xreadfile = 1;
			if (Xmerge) {
				// MERGE is OFF... do we really want to skip write file update?
				if (MessageBox(NULL, "Do you really want to \npossibly truncate your new .xpc file?", "Warning!",
						MB_YESNO | MB_ICONWARNING) == IDYES) {
					Xreadfile = 0;
				}
			}
			do {
				r_len = fread(&s_len, sizeof(int), 1, X32file_r_pt);		// read # of record bytes
				if (r_len) {
					r_len = fread(s_buf, sizeof(char), s_len, X32file_r_pt);	// read record buffer
					XWriteAndSend(0);
				}
				if ((r_len = fread(&dt_read, sizeof(dt_read), 1, X32file_r_pt)) == 0) {
					Xreadfile = 0;
				}
			} while (Xreadfile == 1);
		}
		if (X32file_r_pt) {
			fclose(X32file_r_pt);
			strcpy(Xpath, Xpathsave);
			if(rename(Xpathsave, strcat(Xpath, ".old")) != 0) {
				sprintf(s_buf, "rename to *.old failed: %s\n",strerror(errno));
				MessageBox(NULL, s_buf, NULL, MB_OK);
			}
		}
		if (X32file_w_pt) {
			fclose(X32file_w_pt);
			strcpy(Xpath, Xpathsave);
			if(rename(strcat(Xpath, "_xpc"), Xpathsave) != 0) {
				sprintf(s_buf, "rename to *.xpc failed: %s\n",strerror(errno));
				MessageBox(NULL, s_buf, NULL, MB_OK);
			}
		}
		XCloseMidiDevices();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
//
// MIDI Callback function
//
void CALLBACK XReceiveMidi(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance,
                           DWORD dwParam1, DWORD dwParam2) {
	int i;
	LPMIDIHDR	hmidiheader;
//
	switch(wMsg) {		// We're only interested in certain MIDI meesages and specific SYSEX
	case MIM_DATA:
		// Quarter Frame update from MIDI
		if (LOBYTE(dwParam1) == 0xf1) {	//MTC Quarter frame signature
			i = HIBYTE(dwParam1);
			switch (i & 0x70) {
			case 0x00:
				Xmidifr = (Xmidifr & 0xf0) | (i &  0x0f);
				break;
			case 0x10:
				Xmidifr = (Xmidifr & 0x0f) | ((i << 4) & 0xf0);
				break;
			case 0x20:
				Xmidiss = (Xmidiss & 0xf0) | (i &  0x0f);
				break;
			case 0x30:
				Xmidiss = (Xmidiss & 0x0f) | ((i << 4) & 0xf0);
				break;
			case 0x40:
				Xmidimn = (Xmidimn & 0xf0) | (i &  0x0f);
				break;
			case 0x50:
				Xmidimn = (Xmidimn & 0x0f) | ((i << 4) & 0xf0);
				break;
			case 0x60:
				Xmidihr = (Xmidihr & 0xf0) | (i &  0x0f);
				break;
			case 0x70:
				Xmidihr = (Xmidihr & 0x0f) | ((i & 1) << 4);
				Xfrrate = XfRate[(i >> 1) & 0x03];
				break;
			}
			// set dt_play accordingly (convert MTC to LTC)
//			dt_play.tv_sec = Xmidihr * 3600 + Xmidimn * 60 + Xmidiss;
//			dt_play.tv_usec = Xmidifr * Xfrrate ;	// that's the number of µs using frames/s
			//
			XDisplayMidiTimer();
		}
		break;
	case MIM_LONGDATA:
		hmidiheader = (LPMIDIHDR)dwParam1;
		if (hmidiheader->dwBytesRecorded == 0) break;
		// looking for full frame MTC message
		if ((unsigned char)hmidiheader->lpData[0] == 0xf0) {
			if ((unsigned char)hmidiheader->lpData[1] == 0x7f) {
				if ((unsigned char)hmidiheader->lpData[3] == 0x01) {	// SYSEX MTC signature
					if ((unsigned char)hmidiheader->lpData[4] == 0x01) {
						Xfrrate = XfRate[(hmidiheader->lpData[5] >> 5) & 0x03];
						Xmidihr = hmidiheader->lpData[5] & 0x1f;		// get time data
						Xmidimn = hmidiheader->lpData[6];				// hr:mn:ss:fr
						Xmidiss = hmidiheader->lpData[7];				// on 4
						Xmidifr = hmidiheader->lpData[8];				// bytes, and
						//
						// Detecting an MTC backward jumps; Test if we need to catch-back events,
						// by reading/storing events. If the new MTC is less (a little or a lot) than the
						// expected MTC, then we'll have to play all changes up to the new cursor position.
						//
						// We do this only if a REW command is not already registered
						if (t_rew.tv_sec == 0) {
							if (Xmidihr < Xoldmidihr) {
								XCtachBack = 1;
							} else if (Xmidihr == Xoldmidihr) {
								if (Xmidimn < Xoldmidimn) {
									XCtachBack = 1;
								} else if (Xmidimn == Xoldmidimn) {
									if (Xmidiss < Xoldmidiss) {
										XCtachBack = 1;
									} else if (Xmidiss == Xoldmidiss) {
										if (Xmidifr < Xoldmidifr) {
											XCtachBack = 1;
										}
									}
								}
							}
						}
//						// Detecting MTC forward jumps; Test if we need to catch-up events,
//						// by reading/storing events. If the new MTC is more (a little or a lot) than the
//						// expected MTC, then we'll have to play all changes up to the new cursor position.
//						//
//						// We do this only if a FF command is not already registered
//						if (t_ff.tv_sec == 0) {
//							if (Xmidifr > Xoldmidifr + 4) {
//								XCatchUp = 1;
//							} else if (Xmidiss > Xoldmidiss) {
//								XCatchUp = 1;
//							} else if (Xmidimn > Xoldmidimn) {
//								XCatchUp = 1;
//							}  else if (Xmidihr > Xoldmidihr) {
//								XCatchUp = 1;
//							}
//						}
						// we are [back] in a normal situation:
						if (XCtachBack == 0 && XCatchUp == 0) {
							XDisplayMidiTimer();	// update midi time if appropriate
						}
					}
				}
			}
		}
		Mflag = midiInPrepareHeader(hMidiIn, &MidiInHdr, sizeof(MidiInHdr));
		if (Mflag) fprintf(stderr, "Error preparing MIDI in: %d", Mflag);
		Mflag = midiInAddBuffer(hMidiIn, &MidiInHdr, sizeof(MidiInHdr));
		if (Mflag) fprintf(stderr, "Error adding buffer to MIDI in: %d", Mflag);
		break;
//	case MIM_OPEN:
//		break;
//	case MIM_CLOSE:
//		break;
//	case MIM_ERROR:
//		break;
//	case MIM_LONGERROR:
//		break;
//	case MIM_MOREDATA:
//		break;
	default:
		break;
	}
	return;
}
//
//
// main program
//
int main(int argc, char **argv) {
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmdFile = 0;
	//
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	//
	// load resource file data
	r_len = FileParse("./.X32PunchControl.ini", MyParameters);
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
	//
	FreeParse(MyParameters);
	return 0;
}
//
//
//
// Private functions:
//
//
// XCatchUpProc()
void XCatchUpProc() {
	// Starting from current position, copy all events from reading file to writing
	// file (updating X32) until new time, and keep going from standard reading file
	dt_read.tv_sec = dt_read.tv_usec = 0;
	if ((r_len = fread(&dt_read, sizeof(dt_read), 1, X32file_r_pt)) > 0) {
		Xreadfile = 1;
		while (Xreadfile  && (timercmp(&dt_play, &dt_read, >))) {
			r_len = fread(&s_len, sizeof(int), 1, X32file_r_pt);		// read # of record bytes
			r_len = fread(s_buf, sizeof(char), s_len, X32file_r_pt);	// read record buffer
			XWriteAndSend(1);
			Sleep(Xcatchdelay);	 // introduce a small delay to help fader moves
			if ((r_len = fread(&dt_read, sizeof(dt_read), 1, X32file_r_pt)) == 0) {
				Xreadfile = 0;
			}
		}
	} else {
		MessageBox(NULL, "Cannot manage intermediary file", NULL, MB_OKCANCEL);
	}
	return;
}
//
//
// XCatchBackProc()
void XCatchBackProc() {;
FILE	*X32file_r_pt1;
	// close reading file; close writing file
	// re-open ex-reading and ex-writing file
	// copy all events from ex-writing file to new writing file until new time
	// then close ex-writing file, and keep going from standard reading file
	fclose (X32file_r_pt);								// close reading file (is there always one here?)
	fclose (X32file_w_pt);								// close writing file (writes all pending data too)
	X32file_r_pt = X32file_r_pt1 = X32file_w_pt = 0;
	rename (Xfile_w_str, "X32pctl.xpc");				// rename writing file as ex-writing file
//	Sleep(100);											// need to wait?
	// re-open ex-writing file as read file
	if ((X32file_r_pt1 = fopen("X32pctl.xpc", "rb"))) {
		// create/open a new writing file
		if ((X32file_w_pt = fopen(Xfile_w_str, "wb")) == NULL) {
			MessageBox(NULL, "Cannot open file for recording", NULL, MB_OKCANCEL);
		} else {
			// ex-write (for reading) and new write files ok
			// If needed/OK, re-open the read file to continue project update
			// Note: the read file may not exist (if we write a file for the first time)
			// shouldn't prevent from continuing, X32file_r_pt will be NULL
			X32file_r_pt = fopen(Xfile_r_str, "rb");
			// ex-write file opened for reading; new write file OK
			dt_read.tv_sec = dt_read.tv_usec = 0;
			if (X32file_r_pt) r_len = fread(&dt_read, sizeof(dt_read), 1, X32file_r_pt);
			if ((r_len = fread(&dt_read, sizeof(dt_read), 1, X32file_r_pt1)) > 0) {
				// The ex-write file data will be copied into the newly created write file.
				Xreadfile = 1;
				while (Xreadfile  && (timercmp(&dt_play, &dt_read, >))) {
					// Get the read file to progress
					if (X32file_r_pt) {
						r_len = fread(&s_len, sizeof(int), 1, X32file_r_pt);		// read # of record bytes
						r_len = fread(s_buf, sizeof(char), s_len, X32file_r_pt);	// read record buffer
					}
					// the ex-write file progresses equally
					r_len = fread(&s_len, sizeof(int), 1, X32file_r_pt1);		// read # of record bytes
					r_len = fread(s_buf, sizeof(char), s_len, X32file_r_pt1);	// read record buffer
					// update the new write file
					XWriteAndSend(1);
					// Avoiding UDP packets drop; if a fader takes place
					// can do that with strstr() as the term "fader" will always appear in the first
					// part of the buffer just sent, before any null byte
					if (strstr(s_buf, "fader")) Sleep(Xcatchdelay);	 // introduce a small delay
					// read and test next dt_read time value
					// make sure we progress equally on both files
					if (X32file_r_pt) r_len = fread(&dt_read, sizeof(dt_read), 1, X32file_r_pt);
					// the stop test in the while loop is made using the ex-writing file
					if ((r_len = fread(&dt_read, sizeof(dt_read), 1, X32file_r_pt1)) == 0) {
						Xreadfile = 0; // nothing more to read from the ex-write file
					}
				}
				fclose(X32file_r_pt1);
				r_len = remove("X32pctl.xpc"); // don't need the ex-write file anymore
				// done catching up:
				// dt_read time at the right position
				// read file if it exists, too
				// write file updated up to the dt_read position
			}
		}
	} else {
		MessageBox(NULL, "Cannot manage intermediary file", NULL, MB_OKCANCEL);
	}
	return;
}
//
// XDisplayMTC()
// Does what it says :)
void XDisplayMTC() {
	hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	htmp = (HFONT) SelectObject(hdc, hfont);
	TextOut(hdc, 517, 14, "M", 1);
	TextOut(hdc, 518, 25, "T", 1);
	TextOut(hdc, 518, 36, "C", 1);
	DeleteObject(htmp);
	DeleteObject(hfont);
}
//
// XClearDisplay()
// Clears the LTC/MTC window area
// This is done by displaying a series of spaces
// We also display a logo to indicate if the clock is MTC
void XClearDisplay() {
	hdc = GetDC(Ghwnd);
	//
	// select fon and display  main text (i.e. spaces)
	hfont = CreateFont(42, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	htmp = (HFONT) SelectObject(hdc, hfont);
	TextOut(hdc, 310, 11, str1, wsprintf(str1, "                      "));
	DeleteObject(htmp);
	DeleteObject(hfont);
	//
	// if needded, display MTC logo using the appropriate font
	if (XMTCon) {
		XDisplayMTC();
	}
	ReleaseDC(Ghwnd, hdc);
	return;
}
//
// XDisplayMidiTimer()
// Displays the hours, minute, seconds and frames of MTC
//
void XDisplayMidiTimer() {
	if (XMconnected && XMTCon) {
		hdc = GetDC(Ghwnd);
		//
		// select font and display time, from the MIDI timer
		hfont = CreateFont(42, 0, 0, 0, FW_LIGHT, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 310, 11, str1, wsprintf(str1, "%02d:%02d:%02d:%02d",
			Xmidihr, Xmidimn, Xmidiss, Xmidifr));
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		// display MTC logo using appropriate font
		XDisplayMTC();
		ReleaseDC(Ghwnd, hdc);
		//
		// set dt_play accordingly (convert MTC to LTC)
		dt_play.tv_sec = Xmidihr * 3600 + Xmidimn * 60 + Xmidiss;
		dt_play.tv_usec = Xmidifr * Xfrrate ;	// that's the number of µs using frames/s
		//
		// keep current time in "old" values
		Xoldmidifr = Xmidifr;
		Xoldmidiss = Xmidiss;
		Xoldmidimn = Xmidimn;
		Xoldmidihr = Xmidihr;
	}
	return;
}
//
// XDisplayLocalTimer()
// Display local time counter
void XDisplayLocalTimer() {
	hdc = GetDC(Ghwnd);
	//
	// select font and display time, directly from dt_play value
	hfont = CreateFont(42, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	htmp = (HFONT) SelectObject(hdc, hfont);
	TextOut(hdc, 310, 11, str1, wsprintf(str1, "%02d:%02d:%02d:%03d",
			dt_play.tv_sec / 3600,
			dt_play.tv_sec / 60,
			dt_play.tv_sec % 60,
			dt_play.tv_usec / 1000));
	DeleteObject(htmp);
	DeleteObject(hfont);
	ReleaseDC(Ghwnd, hdc);
	return;
}
//
// XWriteAndSend()
// write data record to the witing file (time, length, data) and
// send the actual data payload to X32
void XWriteAndSend(int send) {
	// add data file records to merge file
	fwrite(&dt_read, sizeof(dt_read), 1, X32file_w_pt);	// write record time
	fwrite(&s_len, sizeof(int), 1, X32file_w_pt);		// write # of record bytes
	fwrite(s_buf, sizeof(char), s_len, X32file_w_pt);	// write record buffer
	// update X32
	if (send) {
		SEND_TO(s_buf, s_len)
	}
}
//
// XExecuteReadMerge()
// Manage the data just read (need to be written to the writing file?) and/or
// the data punched IN by the user, avoiding conflicting Read data if needed/requested by the user
void XExecuteReadMerge() {
	//
	if (Xfiledataready) {
		// by default and in normal operation, Merging is enabled (Xmerge = 0),
		// if Xmerge = 1, we try to "protect" X32 faders from mutally exclusive requests
		// from a file order and the user actions on faders. As a result, we decide to
		// either protect faders only by preventing fader changes coming from the file data
		// if Xmergefaders = 1; or prevent *all* writing from the file if Xmergefaders = 0;
		// The above logic also applies to the file data that would normally be sent to X32.
		//
		if (Xmerge) {
			// here, we would normally prevent writing data to the X32
			// do we want to filter out only fader moves or everything?
			if (Xmergefaders) {
				// filter out only faders... so we'll test for fader moves
				// the data to be written and sent to X32 is in s_buf[]/s_len
				if (strstr(s_buf, "fader") == 0) {
					// s_buf[] does not contain "fader", so we can write and send data
					XWriteAndSend(1);
				}
			}
		} else {
			// add data file records to merge file, and send data to X32
			XWriteAndSend(1);
		}
		if ((r_len = fread(&dt_read, sizeof(dt_read), 1, X32file_r_pt)) == 0) {
			Xreadfile = 0;
			Xfiledataready = 0;
		}
	}
	if (p_status > 0) {
		RECV_FR(r_buf, r_len)
		if (XParseBank()) {  	// if bank control is enabled, we need to parse for bank commands
			if (Xpunch || Xrecord) {		// record mode on?
				// add new data from X32 to merge file
				fwrite(&dt_play, sizeof(dt_play), 1, X32file_w_pt);	// write record time
				fwrite(&r_len, sizeof(int), 1, X32file_w_pt);		// write # of record bytes
				fwrite(r_buf, sizeof(char), r_len, X32file_w_pt);	// write record buffer
			}
		}
	}
}
//
// XParseBank()
// Manage information coming from X32; parsing user bank data
// Depending on the received code, call appropriate functions
// returns 0 if the X32 data has been "consumed" or 1 if the user data is left untouched
int XParseBank() {
	int bnum;
	if (strncmp(r_buf, "/-stat/userpar/", 15) == 0) {
		//
		// Do we need to parse the data (ony do that if Xusebank is set
		if (Xusebank) {  // parse for bank commands
			sscanf(r_buf + 15, "%2d", &bnum);
			if (bnum >= Xbid[0] || bnum <= Xbid[7]) {
				// a user control parameter was received
				if (r_buf[31] == 0) {		// ignore press in, only consider press out
					if (bnum == Xbid[0]) {
						if (X32file_w_pt) XRewGUI();
					} else if (bnum == Xbid[1]) {
						if (X32file_w_pt) XPlayGUI();
					} else if (bnum == Xbid[2]) {
						XPauseGUI();
					} else if (bnum == Xbid[3]) {
						if (X32file_w_pt) XFfGUI();
					} else if (bnum == Xbid[4]) {
						if (X32file_w_pt) XPunchGUI();
					} else if (bnum == Xbid[5]) {
						XMergeGUI();
					} else if (bnum == Xbid[6]) {
						XStopGUI();
					} else if (bnum == Xbid[7]) {
						if (X32file_w_pt) XRecordGUI();
					}
				}
				return 0; // the data has been consumed
			}
		}
	}
	return 1; // the data must still be used
}
//
// XInitUsrCtrl()
// Initialize the X32 user control buttons to be able to manage play, punch, etc.
// functions directly from the X32
// This applies only if Xusebank is set
void XInitUsrCtrl() {
	int i;

	if (Xusebank) {
//
// Initialize the user selected bank and 4 user selected buttons with push/fugitive behavior
// the 4 buttons will receive Midi Toggle on ch 16, notes 0 to 7
		char* MN[] = { "MN16000", "MN16001", "MN16002", "MN16003",
					   "MN16004", "MN16005", "MN16006", "MN16007" };
		//
		// 8 Buttons: for the user selected bank (A, B, or C)
		// Set X32 Bank buttons and their value to "0"
		for (i = 0; i < 8; i++) {
			Xbid[i] = ((int)Xbank - 65) * 8 + i + 1;
			sprintf(r_buf, "/config/userctrl/%c/btn/%d", Xbank, 5+i);
			s_len = Xfprint(s_buf, 0, r_buf, 's', MN[i]);
			SEND_TO(s_buf, s_len)
			sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[i]);
			s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[0]);
			SEND_TO(s_buf, s_len)
		}
		// init scribbles' color
		sprintf(r_buf, "/config/userctrl/%c/color", Xbank);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &Xbbcolor);
		SEND_TO(s_buf, s_len)
		// select X32 Bank as per user request (A, B, or C)
		i = (int)Xbank - 65;
		s_len = Xfprint(s_buf, 0, "/-stat/userbank", 'i', &i);
		SEND_TO(s_buf, s_len)
		//
		// finally set STOP button to '1'
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[6]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[1]);
		SEND_TO(s_buf, s_len)
	}
	return;
}
//
// XPlayGUI()
// Manage the logic behind pressing the "play" button (GUI or X32)
void XPlayGUI() {
	Xplay = 1;
	SetWindowText(hwndPLAY, "PLAYING...");
	if (Xusebank) {
		// set "play"
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[1]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xplay]);
		SEND_TO(s_buf, s_len)
		// clear "stop"
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[6]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[0]);
		SEND_TO(s_buf, s_len)
	}
	// if in "pause" mode, stay until "pause" is hit
	// record time at which "play" was requested
	if (Xpause) {
		t_play = t_pause;
	} else {
		gettimeofday(&t_play, NULL);
	}
	// if appropriate, send MIDI command to DAW
	XSendMidi(Xmidi_play_str);
//	// Set PAUSE to 'off'
//	Xpause = 0;
//	SetWindowText(hwndPAUSE, "||");
//	sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[3]);
//	s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xpause]);
//	SEND_TO(s_buf, s_len)
	return;
}

void XPunchGUI() {
	Xpunch ^= 1;
	SetWindowText(hwndPUNCH, ((Xpunch)? "PUNCH OUT": "PUNCH IN"));
	if (Xusebank) {
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[4]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xpunch]);
		SEND_TO(s_buf, s_len)
	}
	if (Xpunch) {
		if (!Xplay) {
			XPlayGUI();
		}
		XSendMidi(Xmidi_pchin_str);
	} else {
		XSendMidi(Xmidi_pchout_str);
	}
	return;
}
//
// XMergeGUI()
// Manage the GUI/X32 User bank aspects of pressing "Merge ON/OFF"
// The actual Merge processing is made in the mainloop, depending on
// different messages coming in, and within the destination file
// write/update process
void XMergeGUI() {
	Xmerge ^= 1;
	SetWindowText(hwndMERGE, ((Xmerge)? "MERGE: OFF": "MERGE: ON"));
	if (Xusebank) {
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[5]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xmerge]);
		SEND_TO(s_buf, s_len)
	}
	return;
}
//
// XPauseGUI()
// Manage the "pause"/"resume" button
// Pause can be activated prior to hitting "play" or "record"
void XPauseGUI() {
	Xpause ^= 1;
	SetWindowText(hwndPAUSE, ((Xpause)? "RESUME": "||"));
	if (Xusebank) {
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[2]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xpause]);
		SEND_TO(s_buf, s_len)
	}
	//
	// Manage "play" or "resume"
	if (Xpause) {
		gettimeofday(&t_pause, NULL);
		XSendMidi(Xmidi_pause_str);
	} else {
		timersub(&t_now, &t_pause, &t_pause); // t_pause = t_now - t_pause;
		timeradd(&t_play, &t_pause, &t_play); // t_play += t_pause;
		XSendMidi(Xmidi_resume_str);
	}
	return;
}
//
// XFfGUI()
// fast Forward or End Of Track.
void XFfGUI() {
	// get time for this FF request
	timeradd(&t_now, &t_1sec, &t_ff);
	// send FF or EOT command
	XSendMidi(Xmidi_eot_str);
	return;
}
//
// XRewGUI()
// rewind or Beginning of track.
void XRewGUI() {
	// get time for this REW request
	timeradd(&t_now, &t_1sec, &t_rew);
	// send REW or BOT command
	XSendMidi(Xmidi_bot_str);
	return;
}
//
// XStopGUI()
// "stop" GUI/X32 buttons; set the SW to a "stop" state
void XStopGUI() {
	Xplay = 0;
	SetWindowText(hwndPLAY, "PLAY");
	Xpause = 0;
	SetWindowText(hwndPAUSE, "||");
	Xpunch = 0;
	SetWindowText(hwndPUNCH, "PUNCH IN");
	Xrecord = 0;
	SetWindowText(hwndREC, "RECORD");
	if (Xusebank) {
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[1]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xplay]);
		SEND_TO(s_buf, s_len)
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[2]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xpause]);
		SEND_TO(s_buf, s_len)
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[4]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xpunch]);
		SEND_TO(s_buf, s_len)
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[7]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xrecord]);
		SEND_TO(s_buf, s_len)
		// finally set STOP to '1'
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[6]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[1]);
		SEND_TO(s_buf, s_len)
	}
	//
	XSendMidi(Xmidi_stop_str);
	return;
}
//
// XRecordGUI()
// Manage "record" GUI & X32 interface
void XRecordGUI() {
	Xrecord = 1;
	SetWindowText(hwndREC, "Recording");
	if (Xusebank) {
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[7]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[Xrecord]);
		SEND_TO(s_buf, s_len)
		// Set STOP to '0'
		sprintf(r_buf, "/-stat/userpar/%02d/value", Xbid[6]);
		s_len = Xfprint(s_buf, 0, r_buf, 'i', &X0127[0]);
		SEND_TO(s_buf, s_len)
	}
	if (Xrecordplay) {
		if (!Xplay) {
			XPlayGUI();
		}
	}
	XSendMidi(Xmidi_record_str);
	return;
}
//
// XMidiConnect()
// MIDI connect. Or disconnect. Try to open and connect the selected MIDI devices
// MIDI in is used to get MTC from DAW
// MIDI out is used to send to DAW the different requests for play, stop, pause, etc.
// returns 0 if not connected, and 1 if connected
int  XMidiConnect() {
	int i;

	if (XMconnected) {
		XCloseMidiDevices();
		XMconnected = 0;
		return 0;
	}
	MidiInDevice = NULL;
	MidiOutDevice = NULL;
	Xmidiinport = SendMessage(hwndmincombo, CB_GETCURSEL,(WPARAM) 0, (LPARAM) 0);
	// MIDI in port
	if (Xmidiinport) {
		i = Xmidiinport - 1;;
		if ((Mflag = midiInOpen(&MidiInDevice, i, (DWORD)(void*)XReceiveMidi, 0, CALLBACK_FUNCTION))!= MMSYSERR_NOERROR) {
			sprintf(s_buf, "opening MIDI Intput returned #%d\n",Mflag);
			MessageBox(NULL, s_buf, NULL, MB_OK);
			return 0;
		}
		MidiInHdr.lpData = Xmidiinbuffer;
		MidiInHdr.dwBufferLength  = BSIZE / 2;
		if ((Mflag = midiInPrepareHeader(MidiInDevice, &MidiInHdr, sizeof(MidiInHdr))) != MMSYSERR_NOERROR) {
			sprintf(s_buf, "Error preparing MIDI in: %d", Mflag);
			MessageBox(NULL, s_buf, NULL, MB_OK);
			return 0;
		}
		if ((Mflag = midiInAddBuffer(MidiInDevice, &MidiInHdr, sizeof(MidiInHdr))) != MMSYSERR_NOERROR) {
			sprintf(s_buf, "Error adding buffer to MIDI in: %d", Mflag);
			MessageBox(NULL, s_buf, NULL, MB_OK);
			return 0;
		}
		if ((Mflag = midiInStart(MidiInDevice)) != MMSYSERR_NOERROR) {
			sprintf(s_buf, "Error starting MIDI in: %d", Mflag);
			MessageBox(NULL, s_buf, NULL, MB_OK);
			return 0;
		}
	}
	//
	// MIDI out port
	Xmidioutport = SendMessage(hwndmoutcombo, CB_GETCURSEL,(WPARAM) 0, (LPARAM) 0);
	if (Xmidioutport) {
		i = Xmidioutport - 1;
		if ((Mflag = midiOutOpen(&MidiOutDevice, i, 0, 0, CALLBACK_NULL)) != MMSYSERR_NOERROR) {
			sprintf(s_buf, "opening MIDI Output returned %d\n", Mflag);
			MessageBox(NULL, s_buf, NULL, MB_OK);
			return 0;
		}
	}
	// All OK, Midi connected
	if (Xmidiinport || Xmidioutport ) return 1;
	return 0;
}
//
// XSendMidi()
// sending MIDI data to MIDI out
void XSendMidi(char *XMidi_str) {
	if (XMconnected && Xusemidi) {
		MidiOutHdr.lpData = XMidi_str + 1;
		MidiOutHdr.dwBufferLength = (int)XMidi_str[0];
		MidiOutHdr.dwFlags = 0;
		Mflag = midiOutPrepareHeader(MidiOutDevice, &MidiOutHdr, sizeof(MidiOutHdr));
		if (Mflag == MMSYSERR_NOERROR) {
			Mflag = midiOutLongMsg(MidiOutDevice, &MidiOutHdr, sizeof(MidiOutHdr));
			if (Mflag != MMSYSERR_NOERROR) {
				sprintf(s_buf, "Error sending MIDI Output: %d\n", Mflag);
				MessageBox(NULL, s_buf, NULL, MB_OK);
			}
		} else {
			sprintf(s_buf, "Error preparing MIDI Output: %d\n", Mflag);
			MessageBox(NULL, s_buf, NULL, MB_OK);
		}
	}
	return;
}
//
// XCloseMidiDevices()
// closing MIDI devices
void XCloseMidiDevices() {
	if (XMconnected) {
		Mflag = midiOutUnprepareHeader(MidiOutDevice, &MidiOutHdr, sizeof(MidiInHdr));
		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiOutUnprepareHeader %d\n", Mflag);
		Mflag = midiOutClose(MidiOutDevice);
		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiOutClose %d\n", Mflag);
		Mflag = midiInStop(MidiInDevice);
		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiInStop %d\n", Mflag);
		Mflag = midiInReset(MidiInDevice);
		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiInReset %d\n", Mflag);
		Mflag = midiInUnprepareHeader(MidiInDevice, &MidiInHdr, sizeof(MidiInHdr));
		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiInUnprepareHeader %d\n", Mflag);
		Mflag = midiInClose(MidiInDevice);
		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiInClose %d\n", Mflag);
		MidiOutDevice = 0;
		MidiInDevice = 0;
	}
	return;
}
//
// XLoadScene()
// Attempt to load an X32 scene, per request from the user. The scene number is in Xscene.
void XLoadScene() {
	int	Xscene, i, XLock;
	union littlebig {
		char	cc[4];
		int		ii;
	} endian;
//
	if (Xconnected) {
		sscanf(Xscene_str, "%d\n", &Xscene);
		s_len = Xsprint(s_buf, 0, 's', "/load");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "scene");
		s_len = Xsprint(s_buf, s_len, 'i', &Xscene);
		SEND_TO(s_buf, s_len)
		XLock = 1;
		while (XLock) {
			RPOLL; // read the desk for answer to /load
			if (p_status > 0) 	{		// ignore responses that do not correspond to what is
				RECV_FR(r_buf, r_len);	// expected: /load~~~,si~scene~~~[     1]
				if (strncmp(r_buf, "/load", 5) == 0) {
					for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
					if (endian.ii != 1) {
						sprintf(s_buf, "Error: Could not load scene #%d\n",Xscene);
						MessageBox(NULL, s_buf, NULL, MB_OK);
					}
				} else {
					sprintf(s_buf, "Waiting /load... received:\n%s\n",r_buf);
					MessageBox(NULL, s_buf, NULL, MB_OK);
				}
				XLock = 0;
			}
		}
	}
	return;
}
//
// XListMidiOutDevices()
// List the available MIDI out devices
int XListMidiOutDevices() {
	int 			i, NumDevs;
	MIDIOUTCAPS		moc;
//
	NumDevs = midiOutGetNumDevs();  	// Get the number of MIDI Out devices in this computer
	// Go through all of those devices, displaying their names
	strcpy(XOutMidiNames[0], "Off");
	for (i = 0; i < NumDevs; i++) {
		// Get info about the next device
		if (!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS))) {
			// Display its Device ID and name
			strncpy(XOutMidiNames[i + 1], moc.szPname, 31);
			XOutMidiNames[i + 1][31] = 0;
		}
	}
	return NumDevs + 1;
}
//
// XListMidiInDevices()
// List the available MIDI in devices
int XListMidiInDevices() {
	int 			i, NumDevs;
	MIDIINCAPS		mic;
//
	NumDevs = midiInGetNumDevs();  	// Get the number of MIDI In devices in this computer
	// Go through all of those devices, displaying their names
	strcpy(XInMidiNames[0], "Off");
	for (i = 0; i < NumDevs; i++) {
		// Get info about the next device
		if (!midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS))) {
			// Display its Device ID and name
			strncpy(XInMidiNames[i + 1], mic.szPname, 31);
			XInMidiNames[i + 1][31] = 0;
		}
	}
	return NumDevs + 1;
}
