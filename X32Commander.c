/*
 * X32Commander.c
 *
 *  Created on: Oct 26, 2016
 *      Author: Patrick-Gilles Maillot
 *
 * Initial calculator code from:
 * http://blog.brush.co.nz/2007/11/recursive-decent/
 *
 * Modifications to include $n like parameters as ints or floats in
 * the accepted notation and parsing; also added bitwise and shift
 * operators (for ints) and the possibility to have spaces within formula
 * expressions
 *
 * Simple expression parser and calculator
 *
 * The grammar in BNF-ish notation
 * * ---for ints / MIDI-------------------
 * expression:  ['+'|'-'] shifter ['&'|'|'|'^' shifter]*
 * shifter:		pminus ['>'|'<' pminus]*
 * pminus:		dmulti ['+'|'-' dmulti]*
 * dmulti:		factor ['*'|'/' factor]*
 * factor:		'(' expression ')' | operand
 * operand:		number | '$'number
 * number:		digit [digit]*
 * ---for ints & floats / OSC-------------------
 * oxpression:  ['+'|'-'] odmulti ['+'|'-' odmulti]*
 * odmulti:		ofactor ['*'|'/' ofactor]*
 * ofactor:		'(' oxpression ')' | ooperand
 * ooperand:	number | '$'number
 * number:		digit [digit]*
 *
 *	ver 1.00:	Initial release; single, no-operation, MIDI only,  integer parameters
 *	ver 1.01:	Introduced calculator for integer parameters, isolated within '[' and ']'
 *	ver 1.02:	float parameters accepted, isolated within '[' and ']', transformed to int
 *				[0..127] before use. Accepting multiple parameters (limit = MAXPARAM)
 *	ver 1.03:	added OSC capability for both int and float type tags.
 *	ver 1.04:	fixed bugs! .
 *	ver 1.05:	fixed index error in file reader&parser.
 */
#include <winsock2.h>	// Windows functions for std GUI & sockets
#include <mmsystem.h>	// Multimedia functions (such as MIDI) for Windows
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
//
#define XREMOTE_TIMEOUT			9
#define BSIZE					512
#define MAXPARAM				16
#define C_I						105 // 'i' in decimal
#define C_F						102 // 'f' in decimal
#define C_S						115 // 's' in decimal
//
typedef	union {
	int		c_i;
	float	c_f;
	char	c_s[32];
} Param;
//
union littlebig {
    char    c1[4];
    int     i1;
    float   f1;
} endian;
//
// Global definitions
char				**sindex_pt = NULL;
char				*source_pt = NULL;
char				*s_pt;
int					num_lines = 0;
Param				param[MAXPARAM];			// accept MAXPARAM parameters max
int					keep_on = 1;
int					Xconnected = 0;				// X32 Connected
int					XMconnected = 0;			// MIDI Connected
int					XOconnected = 0;			// OSC out Connected
int					XScanOne = 1;				// stop at first match
//
char 				r_buf[BSIZE], s_buf[BSIZE];	// X32 receive and send buffers
int 				r_len, s_len;				// associated data lengths
int 				p_status;					// "polling" status
//
HMIDIOUT			MidiOutDevice;    // MIDI device interface for sending MIDI output
MIDIHDR				MidiOutHdr = {0};
LPMIDIHDR			lpMidiOutHdr = &MidiOutHdr;
UINT				cbMidiOutHdr = sizeof(MidiOutHdr);
char				XOutMidiNames[16][32];		// List of Midi OUT devices (limit 16)
int					Xmidioutport;
//
WSADATA				wsa;						// Windows Sockets
int 				Xfd;						// X32 socket
struct sockaddr_in	Xip;						// IP structure
struct sockaddr		*Xip_addr = (struct sockaddr*) &Xip;
struct timeval		timeout;					// timeout for non blocking udp comm
fd_set 				ufds;						// file descriptor
int 				Xip_len = sizeof(Xip);		// length of addresses
char				xremote[12] = "/xremote";	// automatic trailing zeroes
char				Xip_str[32];				// X32 IP address as a string
time_t				before, now;				// timers for /xremote
char				str1[64];
char				*cmd;
int					ccc;						// global intermediate value for the calculator
//
int 				Xofd;						// OSC Out socket
struct sockaddr_in	Xop;						// OSC Out IP structure
struct sockaddr		*Xop_addr = (struct sockaddr*) &Xop;
struct timeval		otimeout;					// OSC Out timeout for non blocking udp comm
fd_set 				oufds;						// OSC Out file descriptor
int 				Xop_len = sizeof(Xop);		// OSC Out length of addresses
char				Xop_str[32];				// OSC Out address as a string
//
char 				o_buf[BSIZE];				// OSC Out send buffer
int 				o_len;						// OSC Out associated data length
//
// Windows Declarations
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE		hInstance = 0;
HWND			hwndipaddr, hwndmoutcombo, hwndmport, hwndconx, hwndrun, hwndopaddr, hwndoconx;
HFONT			hfont, htmp;
HDC				hdc;
PAINTSTRUCT		ps;
MSG				wMsg;
//
//
// Macros:
//
#define RPOLL													\
do {															\
	FD_ZERO (&ufds);											\
	FD_SET (Xfd, &ufds);										\
	p_status = select(Xfd+1, &ufds, NULL, NULL, &timeout);		\
} while (0);
//
#define RECV													\
do {															\
	r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);				\
} while (0);
//
#define SEND													\
do {															\
	if (sendto (Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {	\
	perror ("Error while sending data");						\
	exit (EXIT_FAILURE);										\
	}															\
} while(0);
//
#define XREMOTE()													\
do {																\
	now = time(NULL);												\
	if (now > before + XREMOTE_TIMEOUT) {							\
		if (sendto (Xfd, xremote, 12, 0, Xip_addr, Xip_len) < 0) {	\
			perror ("coundn't send data to X32");					\
			exit (EXIT_FAILURE);									\
		}															\
		before = now;												\
	}																\
} while (0);
//
#define SENDO														\
do {																\
	if (sendto (Xofd, o_buf, o_len, 0, Xop_addr, Xop_len) < 0) {	\
	perror ("Error while sending data");							\
	exit (EXIT_FAILURE);											\
	}																\
} while(0);
//
// External calls used
//extern int  Xsprint(char *buffer, int index, char format, void *data);
//extern int  Xfprint(char *buffer, int index, char *header, char format, void *data);
extern int  X32Connect(int Xconnected, char* Xip_str, int btime);
extern int  validateIP4Dotted(const char *s);
//
// Private functions
int Xinit();
void XCommand();
void XParseAndSendOSC(char* cmd);
void XsendSysex(char* cmd);
int  XListMidiOutDevices();
int  XMidiConnect();
int  XOConnect();
void XCloseMidiDevices();
//
//
// Expression Calculator code
//
void next(char** cmd);
int	 expression(char** cmd);
int	 number(char** cmd);
int	 operand(char** cmd);
int	 factor(char** cmd);
int	 dmulti(char** cmd);
int  shifter(char** cmd);
int  pminus(char** cmd);
void error(char *msg);
float oxpression(char** cmd, char* parstr_pt);
float ooperand(char** cmd);
float ofactor(char** cmd, char* parstr_pt);
float odmulti(char** cmd, char* parstr_pt);
//
// Int Calculator code
void error(char *msg) {
    puts(msg);
    exit(1);
}
//
void next(char** cmd) {
    do {
    	(*cmd) += 1;
    } while ((ccc = **cmd) == ' ');
    if (ccc == ']') ccc = 0;;
}
//
int number(char** cmd) {
    int n;

    if (!isdigit(ccc)) error("digit expected");
    n = 0;
    do {
        n = n * 10 + ccc - '0';
        next(cmd);
    } while (isdigit(ccc));
    return n;
}
//
int operand(char** cmd) {

    if (ccc == '$') {
        next(cmd);
        return param[number(cmd)].c_i;
    }
    return number(cmd);
}
//
int factor(char** cmd) {
    int n;

    if (ccc == '(') {
        n = expression(cmd);
        if (ccc != ')') error(") expected");
        next(cmd);
    } else
        n = operand(cmd);
    return n;
}
//
int dmulti(char** cmd) {
    int op, n, m;

    n = factor(cmd);
    while ((op = ccc) == '*' || op == '/') {
        next(cmd);
        m = factor(cmd);
        n = (op == '*') ? n * m : n / m;
    }
    return n;
}
//
int pminus(char** cmd) {
    int op, n, m;

    n = dmulti(cmd);
    while ((op = ccc) == '+' || op == '-') {
        next(cmd);
        m = dmulti(cmd);
        n = op=='+' ? n+m : n-m;
    }
    return n;
}
//
int shifter(char** cmd) {
    int op, n, m;

    n = pminus(cmd);
    while ((op = ccc) == '>' || op == '<') {
        next(cmd);
        m = pminus(cmd);
        n = op=='>' ? n>>m : n<<m;
    }
    return n;
}
//
int expression(char** cmd) {
    int sign, op, n, m;

    next(cmd);
    if ((sign = (ccc == '-')) || ccc == '+') next(cmd);
    n = shifter(cmd);
    if (sign) n = -n;
    while ((op = ccc) == '&' || op == '|' || op == '^') {
        next(cmd);
        m = shifter(cmd);
        n = (op == '&' )? n & m : ((op == '|' )? n | m : n ^ m);
    }
    return n;
}
//
// Float Calculator code
float ooperand(char** cmd) {

    if (ccc == '$') {
        next(cmd);
        return param[number(cmd)].c_f;
    }
    return (float)number(cmd);
}
//
float ofactor(char** cmd, char* parstr_pt) {
    float f;

    if (ccc == '(') {
        f = oxpression(cmd, parstr_pt);
        if (ccc != ')') error(") expected");
        next(cmd);
    } else
        f = ooperand(cmd);
    return f;
}
//
float odmulti(char** cmd, char* parstr_pt) {
    int op;
    float f, g;

    f = ofactor(cmd, parstr_pt);
    while ((op = ccc) == '*' || op == '/') {
        next(cmd);
        g = ofactor(cmd, parstr_pt);
        f = (op == '*') ? f * g : f / g;
    }
    return f;
}
// oxpression(&cmd, parstr)
float oxpression(char **cmd, char* parstr_pt) {
    int sign, op;
    float f, g;

	// int case
	next(cmd);
	if ((sign = (ccc == '-')) || ccc == '+') next(cmd);
	f = odmulti(cmd, parstr_pt);
	if (sign) f = -f;
	while ((op = ccc) == '+' || op == '-') {
		next(cmd);
		g = odmulti(cmd, parstr_pt);
		f = (op == '+' )? f + g : f - g;
	}
	if (*parstr_pt == 'i') {
		endian.i1 = (int)f;
    } else {
		endian.f1 = f;
    }
	o_buf[o_len++] = endian.c1[3];
	o_buf[o_len++] = endian.c1[2];
	o_buf[o_len++] = endian.c1[1];
	o_buf[o_len++] = endian.c1[0];
    return f;
}
//
// End of Expression Calculator code
//
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdFile) {
//
	WNDCLASSW wc = {0};
	wc.lpszClassName = L"X32Commander";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
		L"X32Commander",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 220, 297, 187, 0, 0, hInstance, 0);
//
	XMconnected = Xconnected = 0;
// Main loop
	while (keep_on) {
		if (PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		}
		if (Xconnected) {
			XREMOTE()
			// check for data
        	RPOLL
			if (p_status > 0) {
				RECV
				// Check if an action is needed for that command
				XCommand();
				// Done
			} else if (p_status < 0) {
				keep_on = 0; // stop loop on error
			}
			// Ignore timeouts and errors
		} else {
			Sleep(5); // avoid high CPU by waiting 5ms if there's nothing to do
		}
	}
	return (int) wMsg.wParam;
}
//
//
// Windows Callbacks
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int i;
	int XMidiDevNum;
//
	switch (msg) {
	case WM_CREATE:
		//
		hwndipaddr = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 40, 120, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndconx = CreateWindow("button", "Connect & Run", WS_VISIBLE | WS_CHILD,
				130, 40, 145, 20, hwnd, (HMENU )1, NULL, NULL);
		// display initial item 0 / off in the selection field
		hwndmoutcombo = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				5, 78, 180, 140, hwnd, (HMENU)0, NULL, NULL);
		// Load dropdown item list
		XMidiDevNum = XListMidiOutDevices();
		for (i = 0; i < XMidiDevNum; i++) {
			SendMessage(hwndmoutcombo, CB_ADDSTRING, (WPARAM)0, (LPARAM)XOutMidiNames[i]);
		}
		// display initial item 0 / off in the selection field
		SendMessage(hwndmoutcombo, CB_SETCURSEL, (WPARAM)Xmidioutport, (LPARAM)0);
		hwndmport = CreateWindow("button", "Midi: OFF", WS_VISIBLE | WS_CHILD | BS_MULTILINE,
				190, 80, 85, 20, hwnd, (HMENU )2, NULL, NULL);
		hwndopaddr = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 120, 175, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndoconx = CreateWindow("button", "Connect", WS_VISIBLE | WS_CHILD,
				190, 120, 85, 20, hwnd, (HMENU )3, NULL, NULL);
		break;
//
	case WM_PAINT:
		//
		hdc = BeginPaint(hwnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		// update user information
		SetWindowText(hwndipaddr, (LPSTR) Xip_str);
		//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 5, 0, str1, wsprintf(str1, "X32Commander - ©2016 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		// update diverse texts
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, FIXED_PITCH, NULL);//TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 235, 15, str1, wsprintf(str1, "v 1.05"));
		TextOut(hdc, 5, 25, str1, wsprintf(str1, "Set X32 IP below:"));
		TextOut(hdc, 5, 63, str1, wsprintf(str1, "MIDIout:"));
		TextOut(hdc, 5, 105, str1, wsprintf(str1, "OSC out IP, Port below:"));
		DeleteObject(htmp);
		DeleteObject(hfont);
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
						SetWindowText(hwndconx, "Running");
					} else {
						SetWindowText(hwndconx, "Connect & Run");
					}
				} else {
					printf("Incorrect IP string form\n");
				}
				break;
			case 2:
				// MIDI connect
				XMconnected = XMidiConnect();
				SetWindowText(hwndmport, XMconnected ? "Midi: ON" : "Midi: OFF");
				break;
			case 3:
				// Optional OSC out
				GetWindowText(hwndopaddr, Xop_str, GetWindowTextLength(hwndopaddr) + 1);
				XOconnected = XOConnect();
				SetWindowText(hwndoconx, XOconnected ? "Connected" : "Connect");
				break;
			}
		}
		break;
	case WM_DESTROY:
		if (Xconnected) close(Xfd);
		if (XOconnected)  close (Xofd);
		Xconnected = XOconnected = 0;
		XCloseMidiDevices();
		keep_on = 0;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
// XOConnect()
// Establish USD connection to OSC out. No test is done on the IP and port besides maybe a simple
// IP syntax check. Can't test the actual connectiona s we cannot know what's on the other side.
int XOConnect() {
	int i;
//
	if (XOconnected) {
		close (Xofd);
		return 0;
	} else {
		// Create the UDP socket
		if ((Xofd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
			perror("OSC Out socket creation error");
			return 0; // Make sure we don't considered being connected
		} else {
			// isolate IP from Port
			i = 0;
			while (Xop_str[i]) {
				if (Xop_str[i] != ',') i++;
				else {
					Xop_str[i] = 0;
					i++;
					break;
				}
			}
			if (validateIP4Dotted(Xop_str)) {
				// Construct the server sockaddr_in structure
				memset(&Xop, 0, sizeof(Xop));				// Clear struct
				Xop.sin_family = AF_INET;					// Internet/IP
				Xop.sin_addr.s_addr = inet_addr(Xop_str);	// IP address
				Xop.sin_port = htons(atoi(Xop_str + i));	// Port
			} else {
				return 0; // Make sure we don't considered being connected
			}
		}
	}
	return 1; // done !
}
//
//
// Xinit: Open file for commands to scan for,
// Parse file to create a source in memory and an index of line start pointers
// <r_buf> is expected to contain the name/path of the file to read and parse.
int Xinit() {
//
	struct stat sb;
	FILE *fp;
	size_t newLen;
	int i;
	//
	//get file descriptor
	fp = fopen(r_buf, "r");
	if (fp != NULL) {
		// Get file stats
		if (stat(r_buf, &sb) == -1) {
			fclose (fp);
			perror("error: stat");
			return (EXIT_FAILURE);
		}
		// The size of the file is now in stat.st_size
		// Allocate our buffer to that size, and read/load the entire file into memory.
		source_pt = malloc(sizeof(char) * sb.st_size + 1);
		if (source_pt) {
			newLen = fread(source_pt, sizeof(char), sb.st_size, fp);
			if (ferror(fp) != 0) {
				fclose (fp);
				perror("Error reading file");
				if (source_pt) free(source_pt);
				return (EXIT_FAILURE);
			} else {
				source_pt[newLen] = '\0'; /* Just to be safe. */
			}
		} else {
			fclose (fp);
			perror("Error: memory source_pt");
			return (EXIT_FAILURE);
		}
		// Done loading the file.
		fclose(fp);
		// Build list of lines (count the number of \n)
		num_lines = 0;
		for (i = 0, s_pt = source_pt; i < newLen; i++, s_pt++) {
			if (*s_pt == '\n') {
				num_lines++;
				*s_pt = 0;
			}
		}
		// allocate index table and fill with indexes to lines
		if (num_lines > 0) {
			sindex_pt = malloc(sizeof(char*) * (num_lines + 1));
			if (sindex_pt) {
				num_lines = 0;
				sindex_pt[num_lines] = source_pt;
				for (i = 0, s_pt = source_pt; i < newLen; i++) {
					if (*s_pt++ == '\0') {
						if (*s_pt != '#') {		// keep only valid lines
							if (strncmp(s_pt, "scan all", 8) == 0) {
								XScanOne = 0;	// all or only one command match?
							} else {
								sindex_pt[num_lines++] = s_pt;
							}
						}
					}
				}
			} else {
				perror("Error: memory index");
				if (source_pt)
					free(source_pt);
				return (EXIT_FAILURE);
			}
		}
	} else {
		perror("Error: file open");
		return (EXIT_FAILURE);
	}
	return 0;
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
// XMidiConnect()
// MIDI connect. Or disconnect. Try to open and connect the selected MIDI devices
// MIDI in is used to get MTC from DAW
// MIDI out is used to send to DAW the different requests for play, stop, pause, etc.
// returns 0 if not connected, and 1 if connected
int  XMidiConnect() {
	int i;
	int Mflag;

	if (XMconnected) {
		XCloseMidiDevices();
		XMconnected = 0;
		return 0;
	}
	MidiOutDevice = NULL;
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
	if (Xmidioutport) return 1;
	return 0;
}
//
// XSendMidi()
// sending MIDI data to MIDI out
void XSendMidi(char *XMidi_str) {
	int Mflag;

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
	int Mflag;

	if (XMconnected) {
		Mflag = midiOutUnprepareHeader(MidiOutDevice, &MidiOutHdr, sizeof(MidiOutHdr));
		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiOutUnprepareHeader %d\n", Mflag);
		Mflag = midiOutClose(MidiOutDevice);
		if (Mflag != MMSYSERR_NOERROR) fprintf(stderr, "midiOutClose %d\n", Mflag);
		MidiOutDevice = 0;
	}
	return;
}
//
// XsendSysex: Send SYSEX midi command starting in "cmd"
// <cmd> is parsed and changed into a SYSEX midi byte array
// If a parameter is found, it is replaced by the parameter present in <param>
void XsendSysex(char* cmd) {
	int i;
	i = 1;
	while (*cmd) {								// parse full line
		if (*cmd == '#') break;					// ignore the rest of the line
		if ((*cmd != ' ') && (*cmd != '\t')) {	// ignore spaces and tabs
			if (*cmd == '[') {					// $ are for source parameters
				o_buf[i] = expression(&cmd);	// evaluate expression within '['']'
				cmd += 1;
			} else {
				if (*cmd <= 57) {					// start transforming 2 chars into
					o_buf[i] = ((*cmd) - 48) << 4;	// midi data
				} else {
					o_buf[i] = ((*cmd) - 55) << 4;
				}
				cmd += 1;							// 2nd char
				if(*cmd <= 57) {
					o_buf[i] |= ((*cmd) - 48);
				} else {
					o_buf[i] |= ((*cmd) - 55);
				}									// done with midi [00..FF]
			}
			i += 1;			// one mode Midi byte taken care of
		}
		cmd += 1;
	}
	o_buf[0] = i - 1;
	XSendMidi(o_buf);
}
//
//
// XParseAndSendOSC : OSC command, after parsing user command
// User command parsing is actually quite minimal; the file should be correctly
// written.. don't want to waste precious CPU time parsing
void XParseAndSendOSC(char* cmd) {
	char* parstr_pt;							// where OSC Out parameter list starts
// parse and send OSC command
	o_len = 0;
	parstr_pt = 0;
	while ((*cmd == ' ') || (*cmd == '\t')) cmd++;	// ignore leading tabs or spaces
	while (*cmd) {								// parse full line
		if (*cmd == '#') break;					// ignore the rest of the line
		if (*cmd == ' ') {							// a space in a command line means
			o_buf[o_len++] = 0;						// end of command or end of type tags
			while (o_len & 3) o_buf[o_len++] = 0;	// add a \0 and ensure alignment to 4 bytes
			cmd += 1;
		} else  if (*cmd == ',') {				// mark the beginning of type tags
			o_buf[o_len++] = *cmd;					// save the ','
			parstr_pt = cmd + 1;					// save pointer to first type tag
			cmd += 1;
		} else if (*cmd == '[') {				// $ are for source parameters
			(void)oxpression(&cmd, parstr_pt);	// evaluate expression within '['']' to type in parstr
												// ignore result (stored in output buffer o_buf[])
			parstr_pt += 1;						// manage next type is applicable
			cmd += 1;							// skip trailing ']'
			while ((*cmd == ' ') || (*cmd == '\t')) cmd++;	// ignore tabs or spaces between expressions
		} else {
			o_buf[o_len++] = *cmd;				// copy byte
			cmd += 1;
		}
	}
	SENDO
	return;
}
//
//
// XCommand : Parse user command set, as listed in the user file
// These are listed and indexed in the source_pt and sindex_pt tables.
// We scan the sindex_pt table using a 4byte compare to go quickly through the file
// (and therefore the source_pt file). If a possible match is found, we push the compare
// one step further, to compare the full OSC command
void XCommand() {
	int*	w_buf;
	char**	windex_pt;
	char*	cindex_pt;
	char*	c_pt;
	int		i, j;
	int		comma;
	char	ctype;
// The received command is in r_buf/r_len
// we check as quickly as possible the command against the list of commands from
// the X32Commander file we previously loaded into memory (access through sindex_pt)
//
// A first level of quick compare is made through the use of 32bits words
	w_buf = (int *) r_buf;
	windex_pt = sindex_pt;
	for (i = 0; i < num_lines; i++, windex_pt++) {
		if (*w_buf == *((int *) ((*windex_pt) + 4))) {					// avoid leading "M~~~" or "O~~~"
			// test further for an accurate match
			if (strncmp(r_buf, (*windex_pt) + 4, strlen(r_buf)) == 0) {	// avoid leading "M~~~" or "O~~~"
				// full match. now what... Parse command parameter(s)
				if ((*windex_pt)[0] == 'M') {
					// Deal with sending MIDI command
					cindex_pt = *windex_pt + 4;							// skip leading "M~~~" block
					comma = 0;
					ctype = 0;
					while (cindex_pt[comma]) {
						if (cindex_pt[comma] == '|') break;
						if (cindex_pt[comma] == ',') {
							c_pt = r_buf + comma;
							j = 0;
							// found a "," in *Windex_pt... actual comma in r_buf will be further due to possible padding
							while (*(++c_pt) != ','); // progress in r_bufuntil we find a ','
							while (*(++c_pt)) j +=1; // count parameters until we find a '\0'
							// j holds the number of arguments; comma points at the ',' char
							if (j > MAXPARAM) {
								perror("too many params\n");
								return;
							}
							// align k to the next 4 bytes boundary if needed, pointing at first parameter
							while ((int)c_pt & 3) c_pt++;
							// save parameters into param[] array
							// after comma, expect a list of 'i, f, s'; count them
							j = 0;
							while ((ctype = cindex_pt[++comma])) {
								if (ctype == 'i') {
									// int parameter; only use low byte
									param[j].c_i = *(c_pt + 3); // only keep the low byte
									j += 1;
									c_pt += 4;
								} else if (ctype == 'f') {
									// float parameter; transform [0.0...1.0] to [0..127]
									endian.c1[3] = *c_pt;
									endian.c1[2] = *(c_pt+1);
									endian.c1[1] = *(c_pt+2);
									endian.c1[0] = *(c_pt+3);
									param[j].c_i = (int)(endian.f1 * 127);
									j += 1;
									c_pt += 4;
								} else if (ctype == 's') {
									// string parameter;
									//todo
								} else {
									// exit while loop
									break;
								}
							}
						}
						++comma;
					}
					// Apply the matching MIDI command
					// ensure we're at the end of the line or at a '|' char
					while (cindex_pt[comma]) {
						if (cindex_pt[comma] == '|') {
							++comma;
							// parse and send related MIDI command
							XsendSysex(cindex_pt + comma);
							if (XScanOne) return;
						} else {
							++comma;
						}
					}
				} else {
					// Deal with sending OSC command
					cindex_pt = *windex_pt + 4;							// skip leading "O~~~" block
					comma = 0;
					ctype = 0;
					while (cindex_pt[comma]) {
						if (cindex_pt[comma] == '|') break;
						if (cindex_pt[comma] == ',') {
							c_pt = r_buf + comma;
							j = 0;
							// found a "," in *Windex_pt... actual comma in r_buf will be further due to possible padding
							while (*c_pt != ',') c_pt++; // progress in r_buf until we find a ','
							// from thereon, save all int and float parameters in the form of floats
							while (*(++c_pt)) j +=1; // count parameters until we find a '\0'
							// j holds the number of arguments; comma points at the ',' char
							if (j > MAXPARAM) {
								perror("too many params\n");
								return;
							}
							// align k to the next 4 bytes boundary if needed, pointing at first parameter
							while ((int)c_pt & 3) c_pt++;
							// save parameters into param[] array
							// after comma, expect a list of 'i, f, s'; count them
							j = 0;
							while ((ctype = cindex_pt[++comma])) {
								if (ctype == 'i') {
									// int parameter; save as float
									endian.c1[3] = *c_pt;
									endian.c1[2] = *(c_pt+1);
									endian.c1[1] = *(c_pt+2);
									endian.c1[0] = *(c_pt+3);
									param[j].c_f = (float)endian.i1;
									j += 1;
									c_pt += 4;
								} else if (ctype == 'f') {
									// float parameter;
									endian.c1[3] = *c_pt;
									endian.c1[2] = *(c_pt+1);
									endian.c1[1] = *(c_pt+2);
									endian.c1[0] = *(c_pt+3);
									param[j].c_f = endian.f1;
									j += 1;
									c_pt += 4;
								} else if (ctype == 's') {
									// string parameter;
									//todo
								} else {
									// exit while loop
									break;
								}
							}
						}
						++comma;
					}
					// Apply the matching OSC command
					// ensure we're at the end of the line or at a '|' char
					while (cindex_pt[comma]) {
						if (cindex_pt[comma] == '|') {
							++comma;
							// parse and send related command, starts with 'M' or 'O'
							XParseAndSendOSC(cindex_pt + comma);
							if (XScanOne) return;
						} else {
							++comma;
						}
					}
				}
			}
		} else {
			// no match on the first 4 chars... maybe the file line is a wildcard [codes as "*   "
			// In that case, we simply copy the OSC in to out - no translation nor parameter change
			if ((*windex_pt)[4] == '*') {
				// valid for OSC only
				if ((*windex_pt)[0] == 'O') {
					// just send in command, to out
					if (sendto (Xofd, r_buf, r_len, 0, Xop_addr, Xop_len) < 0) {
						perror ("Error while sending data");
						exit (EXIT_FAILURE);
					}
					if (XScanOne) return;
				}
			}
		}
	}
}
//
// Main program
int main(int argc, char **argv) {
	char input_ch;
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmdFile = 0;

	// set default file nale as input
	strcpy(r_buf, "./X32Commander.txt");
	while ((input_ch = getopt(argc, argv, "f:h")) != (char)0xff) {
		switch (input_ch) {
		case 'f':
			sscanf(optarg, "%s", r_buf);
			break;
		default:
		case 'h':
			printf("usage: X32Commander [-f file, sets input file]\n\n");
			return(0);
			break;
		}
	}
//
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
//
	if (Xinit()) exit(-1);
//
// We now have to tables: <sindex_pt> containing <num_lines> pointers to line start pointers
// and <source_pt> containing the actual data, all strings null terminated
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
	if (sindex_pt) {
//		printf("sindex_pt: %x\n", (unsigned int)sindex_pt);
		free(sindex_pt);
	}
	if (source_pt) {
//		printf("source_pt: %x\n", (unsigned int)source_pt);
		free(source_pt);
	}
	return(0);
}

