/*
 * X32Midi2OSC.c
 *
 *  Created on: Jun 11, 2018
 *      ©2018 Patrick-Gilles Maillot
 *
 *
 * This program take MIDI IN data and afer possible modification of MIDI data sends an
 * OSC message to X32, depending on the received MIDI command. MIDI data can be used as
 * float or int in the sent OSC message.
 *
 * At this time, only short MIDI messages are considered. Only short SYSEX messages are
 * considered. Long SYSEX is ignored.
 *
 * Accepted MIDI messages:
 *
 *    Message				  Status	  Data 1			  Data 2
 *    Note Off					8n		Note Number			Velocity
 *    Note On					9n		Note Number			Velocity
 *    Polyphonic Aftertouch		An		Note Number			Pressure
 *    Control Change			Bn		Controller Number	Data
 *    Program Change			Cn		Program Number		from file
 *    Channel Aftertouch		Dn		Pressure			from file
 *    Pitch Wheel				En		LSB					MSB
 *    System Message			Fn		depends on message	depends on message
 *
 * Key
 * n is the MIDI Channel Number (0-F)
 * LSB is the Least Significant Byte
 * MSB is the Least Significant Byte
 *
 * To be able to use/adapt MIDI data that comes in before being use in an OSC
 * message, we use a reverse polish notation calculator with a complete set of operators.
 *
 * File History
 * 	ver 0.10: initial release
 * 	ver 0.11: midi in and out devices in a memory allocated array rather than fixed size
 * 	ver 0.12: midi in and out combobox displaying 1 midi device less than found
 * 	ver 0.13: changed calculator code for an RPN system
 * 	ver 0.14: fixed error in handling end of lines in command file parsing
 * 	ver 0.15: small changes in rpn calculator
 * 	ver 0.16: rpn calculator pushed to X32_lib -> X32RpnCalc
 */
#include <winsock2.h>	// Windows functions for std GUI & sockets
#include <commdlg.h>	// Windows widgets functions (file handling..)
#include <mmsystem.h>	// Multimedia functions (such as MIDI) for Windows
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <io.h>
#include <sys/stat.h>
//
//
//#define printdata
//
//
#define MESSAGE(s1,s2)	\
		MessageBox(NULL, s1, s2, MB_OK);
#define zeromem(a1, a2) \
		ZeroMemory(a1, a2);
#define BSIZE		512		// X32 dialog buffer size
#define MAXFN		256		// File name max length
#define MAXPARAM	3		// Max midi parameters in commands
#define XBMP		105		//
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
extern int    X32Connect(int Xconnected, char* Xip_str, int btime);
extern double X32RpnCalc(char **s, char *t);
extern int    validateIP4Dotted(const char *s);
extern int    FileParse(char* fname, Param* par_tab);
extern void   FreeParse(Param* par_tab);
//
//
//
// Windows Declarations
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE		hInstance = 0;
HWND			hwndipaddr, hwndconx, hwndfile, hwndrfile, hwndmport;
HWND			hwndmincombo, hwndmoutcombo, hwnddebug;
HWND			Ghwnd;
HFONT			hfont, htmp;
HDC				hdc;
PAINTSTRUCT		ps;
MSG				wMsg;
OPENFILENAME	ofn;       			// common dialog box structure
//
HDC hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP hBmp;
BITMAP bmp;
//
// Parameter (resource file) handling
// (see testparams.c)
Param MyParameters[] = {
		{"Xip_str", 			P_STR,	{ 0 }},
		{"Xmidiinport",			P_INT,	{ 0 }},
		{"Xmidioutport", 		P_INT,	{ 0 }},
		{}
};
//
#define Xip_str			MyParameters[0].param_data.s1	// X32 IP
#define Xmidiinport		MyParameters[1].param_data.i1	// MIDI in port #
#define Xmidioutport	MyParameters[2].param_data.i1	// MIDI out port #
//
// end of Parameter (resource file) handling
//
// Midi data
char				Xmidiinbuffer[BSIZE / 2];	// MIDI IN buffer
HMIDIOUT			MidiOutDevice;    			// MIDI device interface for sending MIDI output
MIDIHDR				MidiOutHdr = {0};
LPMIDIHDR			lpMidiOutHdr = &MidiOutHdr;
UINT				cbMidiOutHdr = sizeof(MidiOutHdr);
//
HMIDIIN				MidiInDevice;    			// MIDI device interface for reading MIDI input
MIDIHDR				MidiInHdr = {0};
LPMIDIHDR			lpMidiIntHdr = &MidiInHdr;
UINT				cbMidiInHdr = sizeof(MidiInHdr);
int					Mflag;						// Midi flag
int					Midi_debug;					// display MIDI in data flag
//
int					keep_running = 1;
int					wWidth = 360;				// Default window size
int					wHeight = 160;				//
int					Xmidihr, Xmidimn, Xmidiss, Xmidifr, Xfrrate;	// Midi TC data
int					Xoldmidihr, Xoldmidimn, Xoldmidiss, Xoldmidifr;	// Midi TC "old" data
//
char				Xpathsave[MAXFN], Xpath[MAXFN];					// file paths
char				Xfile_r_str[32] = "No file selected";
// flags
int					Xconnected = 0;				// X32 is connected (1)
int					XMconnected = 0;			// MIDI is connected (1)
char				**XInMidiNames;				// List of Midi IN devices (limit 32 chars names)
int					XnumInMidi;					// Number of IN devices
char				**XOutMidiNames;			// List of Midi OUT devices (limit 32 chars names)
int					XnumOutMidi;				// Number of OUT devices
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
struct timeval		t_now, t_before;			// time structures
char				xremote[12] = "/xremote";	// extra chars are set to \0
//
typedef union {
    char    cc[4];
    int     ii;
    float   ff;
} endian;
//
typedef struct {
    unsigned int XMCommand;
    int		Mmc;
    int     Mch;
    int     Md1;
    int     Md2;
    char*   OSC;
} XM2O;
//
extern endian		mparam[];					// midi or OSC parameters
//
XM2O    *XM2O_pt;
//
endian	Xdian;					// used for conversions
int		num_lines = 0;			// number of active lines
int		ccc;					// global intermediate value for the calculator
//
// Private functions
void CALLBACK XReceiveMidi(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
void XFreeAllocatedResources();
int  XMidiConnect();
void XSendMidi(char *XMidi_str);
void XCloseMidiDevices();
int  XListMidiInDevices();
int  XListMidiOutDevices();
int  XLoadFile(char *Xpathsave);
void XParseAndSendOSC(int dwParam1);
//
// Macros
//
#define SEND_TO(b, l)													\
	do {																\
		if (sendto(Xfd, b, l, 0, Xip_addr, Xip_len) < 0) {				\
			MessageBox(NULL, "Can't send data to X32", NULL, MB_OK);	\
			exit(EXIT_FAILURE);											\
		} 																\
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
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdFile) {
//
	WNDCLASSW wc = {0};
	wc.lpszClassName = L"X32Midi2OSC";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
		L"X32Midi2OSC - translates MIDI commands to OSC",
		WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU,
		100, 220, XBMP + wWidth, wHeight, 0, 0, hInstance, 0);
//
// some initializations needed:
	XMconnected = Xconnected = 0;
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
			//
			// Do things in mainloop?
			Sleep(1); // avoid high CPU by waiting 1ms as there's nothing to do
			//
		} else {
			Sleep(10); // avoid high CPU by waiting 10ms if there's nothing to do
		}
	}
	return (int) wMsg.wParam;
}
//
// Windows Callbacks
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int i;
	//
	Ghwnd = hwnd;		// Ghwnd: global window hwnd
	switch (msg) {
	case WM_CREATE:
		//
		hwndipaddr = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, XBMP + 70, 25, 120, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwndconx = CreateWindow("button", "Connect", WS_VISIBLE | WS_CHILD,
				XBMP + 195, 25, 85, 20, hwnd, (HMENU )1, NULL, NULL);
		hwnddebug = CreateWindow("button", "Dbg OFF", WS_VISIBLE | WS_CHILD,
				XBMP + 285, 25, 60, 20, hwnd, (HMENU )2, NULL, NULL);
		//
		XnumInMidi = XListMidiInDevices();
		hwndmincombo = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				XBMP + 70, 50, 185, (XnumInMidi + 1) * 20, hwnd, (HMENU)0, NULL, NULL);
			// Load dropdown item list
		for (i = 0; i < XnumInMidi; i++) {
			SendMessage(hwndmincombo, CB_ADDSTRING, (WPARAM)0, (LPARAM)XInMidiNames[i]);
		}
		// display initial item 0 / off in the selection field
		SendMessage(hwndmincombo, CB_SETCURSEL, (WPARAM)Xmidiinport, (LPARAM)0);

		XnumOutMidi = XListMidiOutDevices();
		hwndmoutcombo = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				XBMP + 70, 75, 185, (XnumOutMidi + 1) * 20, hwnd, (HMENU)0, NULL, NULL);
		// Load dropdown item list
		for (i = 0; i < XnumOutMidi; i++) {
			SendMessage(hwndmoutcombo, CB_ADDSTRING, (WPARAM)0, (LPARAM)XOutMidiNames[i]);
		}
		// display initial item 0 / off in the selection field
		SendMessage(hwndmoutcombo, CB_SETCURSEL, (WPARAM)Xmidioutport, (LPARAM)0);
		//
		hwndmport = CreateWindow("button", "Midi: OFF", WS_VISIBLE | WS_CHILD | BS_MULTILINE,
				XBMP + 260, 50, 40 + 45, 48, hwnd, (HMENU )3, NULL, NULL);

		hwndfile = CreateWindow("button", "Select File",
				WS_VISIBLE | WS_CHILD, XBMP + 5, 105, 85, 20, hwnd, (HMENU )4, NULL, NULL);
		hwndrfile = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, XBMP + 95, 105, 250, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./.X32Midi2OSC.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_SHARED);
		if(hBmp==NULL) {
			perror("Pixel bitmap file no found");
		}
		break;
//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		//
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 5, 20, 100, 105, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
		//
		// update user information
		SetWindowText(hwndrfile, (LPSTR) Xfile_r_str);
		SetWindowText(hwndipaddr, (LPSTR) Xip_str);
		//
		// update diverse texts
		SetBkMode(hdc, TRANSPARENT);
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, FIXED_PITCH, NULL);//TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, XBMP + 5, 25, s_buf, wsprintf(s_buf, "X32 IP:"));
		TextOut(hdc, XBMP + 5, 52, s_buf, wsprintf(s_buf, "MIDIin:"));
		TextOut(hdc, XBMP + 5, 77, s_buf, wsprintf(s_buf, "MIDIthru:"));
		TextOut(hdc, 282, 0, s_buf, wsprintf(s_buf, "ver. 0.16"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 5, 0, s_buf, wsprintf(s_buf, "X32Midi2OSC - ©2018 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		EndPaint(hwnd, &ps);
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
					}
				} else {
					MessageBox(NULL, "Incorrect IP string form!", NULL, MB_OK);
				}
				break;
			case 2:
				// Debug
				if (Midi_debug ^= 1) SetWindowText(hwnddebug, "Dbg ON");
				else                 SetWindowText(hwnddebug, "Dbg OFF");
				break;
			case 3:
				// MIDI connect
				XMconnected = XMidiConnect();
				SetWindowText(hwndmport, XMconnected ? "Midi: ON" : "Midi: OFF");
				break;
			case 4:	// Merge to file name obtained from dialog
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
				ofn.lpstrFilter = "X32Midi2OSC\0*.m2o\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrTitle = (LPCTSTR) "Select a File name containing MIDI to OSC commands\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				//
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
					strcpy(Xpathsave, Xpath);
					// remove filename from returned path so we can either save to
					// existing directory or newly created one
					i = strlen(Xpath);
					while ((i > 0) && (Xpath[i] != '\\')) --i;
					Xpath[i] = 0;
					strcpy(Xfile_r_str, Xpath + i + 1);
					// test if an extension is already present; if not add one
					i = strlen(Xfile_r_str);
					while ((i > 0) && (Xfile_r_str[i] != '.')) --i;
					if (i == 0) {
						// add an extension
						strcat(Xfile_r_str, ".m2o");
						strcat(Xpathsave, ".m2o");
					}
					if (XLoadFile(Xpathsave) < 0) {
						MessageBox(NULL, "Error in loading file!", NULL, MB_OK);
					} else {
						SetWindowText(hwndrfile, (LPSTR) Xfile_r_str);
					}
				}
				break;
			}
		}
		break;
	case WM_DESTROY:
		if (Xconnected) close(Xfd);
		keep_running = 0;
		XFreeAllocatedResources();
		XCloseMidiDevices();
		// free memory allocated for Midi device names
		for (i = 0; i < XnumInMidi; i++) {
			if (XInMidiNames[i]) free(XInMidiNames[i]);
		}
		if (XInMidiNames) free(XInMidiNames);
		for (i = 0; i < XnumOutMidi; i++) {
			if (XOutMidiNames[i]) free(XOutMidiNames[i]);
		}
		if (XOutMidiNames) free(XOutMidiNames);
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
//	LPMIDIHDR	hmidiheader;
	//
	switch(wMsg) {
	case MIM_DATA:				// receiving small (2 or 3bytes) messages
		if (dwParam1 & 0x80) {	//detect MIDI command
			if (Midi_debug) {
				sprintf(s_buf, "MIDI in: %8X", (unsigned int)dwParam1);
				MessageBox(NULL, s_buf, "Midi IN", MB_OK  | MB_ICONINFORMATION);
			}
			switch ((dwParam1 & 0x70) >> 4) {
			case 0x00:	// Note OFF sent when a note is released (ended):	1000nnnn	0kkkkkkk 0vvvvvvv
			case 0x01:	// Note ON sent when a note is depressed (start):	1001nnnn	0kkkkkkk 0vvvvvvv
			case 0x02:	// Polyphonic Aftertouch sent by pressing down on the key after it "bottoms out":
						//													1010nnnn	0kkkkkkk 0ppppppp
			case 0x03:	// Control Change sent when a controller value changes:
						//													1011nnnn	0ccccccc 0vvvvvvv
			case 0x06:	// Pitch Wheel  sent to indicate a change in the pitch bender (wheel or lever, typically).
						// The pitch bender is measured by a fourteen bit value. Center (no pitch change)
						// is 2000H
						//													1110nnnn 	0lllllll 0mmmmmmm
						// (kkkkkkk) is the key (note) number.
						// (vvvvvvv) is the velocity.
						// (ccccccc) is the control change
						// (ppppppp) is the pressure value.
						// (lllllll) are the least significant 7 bits, (mmmmmmm) are the most significant 7 bits
			case 0x07:	// System Common Message (MTC, etc...), System Real Time Messages
						// only considering here the short messages, data1 or data2 can be undefined
				mparam[2].ii = (dwParam1 >> 16) & 0x7F;
				// search within existing codes and send respective OSC
				XParseAndSendOSC(dwParam1);
				break;
			case 0x04:		// Program Change sent when the patch number changes
							// 												1100nnnn	0ppppppp
			case 0x05:		// Channel Aftertouch sent by pressing down on the key after it "bottoms out"
							//												1101nnnn	0vvvvvvv
							// (ppppppp) is the program change
							// (vvvvvvv) is the pressure value
				mparam[2].ii = 0x80;
				// search within existing codes and send respective OSC
				XParseAndSendOSC(dwParam1);
				break;
			default:
				break;
			}
		}
		break;
//	case MIM_LONGDATA:		// Receiving long (SYSEX...) messages
//		hmidiheader = (LPMIDIHDR)dwParam1;
//		if (hmidiheader->dwBytesRecorded == 0) break;
//		// looking for full frame MTC message
//		if ((unsigned char)hmidiheader->lpData[0] == 0xf0) {
//			if ((unsigned char)hmidiheader->lpData[1] == 0x7f) {
//				if ((unsigned char)hmidiheader->lpData[3] == 0x01) {	// SYSEX MTC signature
//					if ((unsigned char)hmidiheader->lpData[4] == 0x01) {
////						Xfrrate = XfRate[(hmidiheader->lpData[5] >> 5) & 0x03];
//						Xmidihr = hmidiheader->lpData[5] & 0x1f;		// get time data
//						Xmidimn = hmidiheader->lpData[6];				// hr:mn:ss:fr
//						Xmidiss = hmidiheader->lpData[7];				// on 4
//						Xmidifr = hmidiheader->lpData[8];				// bytes
//						//
//					}
//				}
//			}
//		}
//		Mflag = midiInPrepareHeader(hMidiIn, &MidiInHdr, sizeof(MidiInHdr));
//		if (Mflag) fprintf(stderr, "Error preparing MIDI in: %d", Mflag);
//		Mflag = midiInAddBuffer(hMidiIn, &MidiInHdr, sizeof(MidiInHdr));
//		if (Mflag) fprintf(stderr, "Error adding buffer to MIDI in: %d", Mflag);
//		break;
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
	r_len = FileParse("./.X32Midi2OSC.ini", MyParameters);
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
void XFreeAllocatedResources() {
	int	i;
	XM2O	*XM2O_pt1;
	//
	for (i = 0, XM2O_pt1 = XM2O_pt; i < num_lines; i++, XM2O_pt1++) {
		if (XM2O_pt1->OSC) free(XM2O_pt1->OSC);
	}
	free (XM2O_pt);
	return;
}
//
//
// XMidiConnect()
// MIDI connect. Or disconnect. Try to open and connect the selected MIDI devices
// MIDI in is used to get MTC from DAW
// MIDI out is used to send to DAW the different requests for play, stop, pause, etc.
// returns 0 if not connected, and 1 if connected
int  XMidiConnect() {
	int i;
	//
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
			XCloseMidiDevices();
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
			XCloseMidiDevices();
			return 0;
		} else {
			if (Xmidiinport) {
				if ((Mflag = midiConnect((HMIDI)MidiInDevice, MidiOutDevice, NULL)) != MMSYSERR_NOERROR) {
					sprintf(s_buf, "connecting MIDI Output returned %d\n", Mflag);
					MessageBox(NULL, s_buf, NULL, MB_OK);
					return 0;
				}
			}
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
	if (XMconnected) {
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
//	if (XMconnected) {
		Mflag = midiOutUnprepareHeader(MidiOutDevice, &MidiOutHdr, sizeof(MidiInHdr));
//		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiOutUnprepareHeader %d\n", Mflag);
		Mflag = midiOutClose(MidiOutDevice);
//		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiOutClose %d\n", Mflag);
		Mflag = midiInStop(MidiInDevice);
//		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiInStop %d\n", Mflag);
		Mflag = midiInReset(MidiInDevice);
//		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiInReset %d\n", Mflag);
		Mflag = midiInUnprepareHeader(MidiInDevice, &MidiInHdr, sizeof(MidiInHdr));
//		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiInUnprepareHeader %d\n", Mflag);
		Mflag = midiInClose(MidiInDevice);
//		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiInClose %d\n", Mflag);
		MidiOutDevice = 0;
		MidiInDevice = 0;
//	}
	return;
}
//
// XListMidiOutDevices()
// List the available MIDI out devices
int XListMidiOutDevices() {
	int 			i, NumDevs;
	MIDIOUTCAPS		moc;
	//
	NumDevs = midiOutGetNumDevs() + 1;  	// Get the number of MIDI Out devices in this computer
	// Go through all of those devices, displaying their names
	XOutMidiNames = malloc((NumDevs) * sizeof(char*));
	XOutMidiNames[0] = malloc(32);
	strcpy(XOutMidiNames[0], "Off");
	for (i = 1; i < NumDevs; i++) {
		// Get info about the next device
		if (!midiOutGetDevCaps(i - 1, &moc, sizeof(MIDIOUTCAPS))) {
			// Display its Device ID and name
			XOutMidiNames[i] = malloc(32);
			strncpy(XOutMidiNames[i], moc.szPname, 31);
			XOutMidiNames[i][31] = 0;
		}
	}
	return NumDevs;
}
//
// XListMidiInDevices()
// List the available MIDI in devices
int XListMidiInDevices() {
	int 			i, NumDevs;
	MIDIINCAPS		mic;
	//
	NumDevs = midiInGetNumDevs() + 1;  	// Get the number of MIDI In devices in this computer
	// Go through all of those devices, displaying their names
	XInMidiNames = malloc((NumDevs) * sizeof(char*));
	XInMidiNames[0] = malloc(32);
	strcpy(XInMidiNames[0], "Off");
	for (i = 1; i < NumDevs; i++) {
		// Get info about the next device
		if (!midiInGetDevCaps(i - 1, &mic, sizeof(MIDIINCAPS))) {
			// Display its Device ID and name
			XInMidiNames[i] = malloc(32);
			strncpy(XInMidiNames[i], mic.szPname, 31);
			XInMidiNames[i][31] = 0;
		}
	}
	return NumDevs;
}
//
//
// XLoadFilei(Xpathsave)
// OPen and parse file for translation strings
int XLoadFile(char *Xpathsave) {
	FILE	*fp;
    int		i, j, k, lines;
    char	*read_status;
    XM2O	*XM2O_pt1;
    //
	if((fp = fopen(Xpathsave, "r")) != NULL) {
		lines = 0;										// count the max number of lines
		if (fp) {										// of the file
			char buf[4096] = { 0 };
			while (fgets(buf, sizeof(buf), fp)) {
				for (const char* ptr = buf; *ptr != '\0'; ++ptr) {
					if (*ptr == '\n' || *ptr == '\r') {
						++lines;
					}
				}
			}
		}
		// Rewind file; the file is 'lines' long
		if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */}
		XFreeAllocatedResources();
		num_lines = 0;								// Allocate memory to save parsed data
		if ((XM2O_pt = (XM2O*) malloc(sizeof(XM2O) * lines)) == NULL) {
			MessageBox(NULL, "File parsing - memory allocation error", NULL, MB_OK);
			return(EXIT_FAILURE);
		}
		// Parse file and save parsed data
		for (i = 0, XM2O_pt1 = XM2O_pt; i < lines; i++, XM2O_pt1++) {
			do {									// skip comment lines
				if ((read_status = fgets(r_buf, BSIZE - 1, fp)) == NULL) break;
			} while (r_buf[0] == '#');
			if (read_status == NULL) break;
			XM2O_pt1->Mmc = 0;
			sscanf(r_buf, "%2x %i %i %i", &XM2O_pt1->Mmc, &(XM2O_pt1->Mch), &(XM2O_pt1->Md1), &(XM2O_pt1->Md2));
			XM2O_pt1->XMCommand = (XM2O_pt1->Md1 << 8) | (XM2O_pt1->Mmc | ((XM2O_pt1->Mch - 1) & 0xF));
			j = 0;								// set pointer to OSC (allocate memory for it)
			while (r_buf[j] != '|') j++;
			j++;								// ignore spaces or tabs
			while ((r_buf[j] == ' ') || (r_buf[j] == '\t')) j++;
			k = j;								// OSC string is from j to eol
			while (r_buf[k] != '\n') k++;
			// length is k - j; copy OSC string affected to MIDI command
			if ((XM2O_pt1->OSC = malloc(sizeof(char) * (k - j + 8))) != NULL) {
				memcpy((void *)(XM2O_pt1->OSC), (void *)(r_buf + j), (k - j) * sizeof(char));
				XM2O_pt1->OSC[k - j] = 0;		// finalize buffer string
			} else {
				XM2O_pt1->OSC = NULL;
				MessageBox(NULL, "OSC copying - memory allocation error", NULL, MB_OK);
			}
			num_lines += 1;
		}
	    fclose(fp);
	}
    return 0;
}

//
// XParseAndSendOSC(char* cmd)
// Parse OSC command after MIDI command received to send data to X32
void XParseAndSendOSC(int dwParam1) {
	char	*parstr_pt, *cmd;					// pointers in XM2O_pt1->OSC
	int		i;
    XM2O	*XM2O_pt1;
//    char	cmd1[128];

	//
	// search for match in command and store commands in XM2O_pt structure array
	for (i = 0, XM2O_pt1 = XM2O_pt; i < num_lines; i++, XM2O_pt1++) {
		if ((dwParam1 & 0xFFFF) == XM2O_pt1->XMCommand) {
			cmd = XM2O_pt1->OSC;
			// set parameters; mparam[2] set by caller depending on incoming MIDI command
			mparam[0].ii = (dwParam1 & 0x0F) + 1;	// channel
			mparam[1].ii = (dwParam1 >> 8) & 0xFF;// data 1
			if (mparam[2].ii & 0x80) mparam[2].ii = XM2O_pt1->Md2 & 0x7F;
			// parse and send OSC command
			s_len = 0;
			parstr_pt = 0;
			while ((*cmd == ' ') || (*cmd == '\t')) cmd++;	// ignore leading tabs or spaces
			while (*cmd) {									// parse full line
				if (*cmd == '#') break;						// ignore the rest of the line
				if (*cmd == ' ') {							// a space in a command line means
					s_buf[s_len++] = 0;						// end of command or end of type tags
					while (s_len & 3) s_buf[s_len++] = 0;	// add a \0 and ensure alignment to 4 bytes
					cmd += 1;
				} else  if (*cmd == ',') {					// mark the beginning of type tags
					s_buf[s_len++] = *cmd;					// save the ','
					parstr_pt = cmd + 1;					// save pointer to first type tag
					cmd += 1;
				} else if (*cmd == '[') {					// evaluate expression within '['']'
					if (*parstr_pt == 'i') {
						Xdian.ii = (int)X32RpnCalc(&cmd, NULL);		// target type is int
					} else {
						Xdian.ff = (float)X32RpnCalc(&cmd, NULL);	// target type is float
					}
					s_buf[s_len++] = Xdian.cc[3];
					s_buf[s_len++] = Xdian.cc[2];
					s_buf[s_len++] = Xdian.cc[1];
					s_buf[s_len++] = Xdian.cc[0];			// value stored in output buffer
					parstr_pt += 1;							// manage next type if applicable
					while (*cmd != ']') cmd++;
					cmd += 1;								// skip trailing ']'
					while ((*cmd == ' ') || (*cmd == '\t')) cmd++;	// ignore tabs or spaces between expressions
				} else {
					s_buf[s_len++] = *cmd;					// copy byte
					cmd += 1;
				}
			}
			if (s_len) SEND_TO(s_buf, s_len)				// send to X32
			return;
		}
	}
	return;
}
