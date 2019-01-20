//
// X32CustomLayer.c
//
//  Created on: March 1, 2016
//      Author: Patrick-Gilles Maillot
//
//    A program to create X32 Custom Layers
//
// This functionality tries to mimic some of the "custom Layers" functions found on Yamaha
// digital mixers.
// It enables you to reassign on the fly X32 Channels strips (IN and AUX only) to match
// your needs or adapt to a special case where you have to quickly reorganize your X32 layering.
//
//
// Note - Despite ifdefs for Windows/Linux, this program will *only* run in
// a Windows environment because of calls to MSC functions. The non-
// Windows sections can be used in a core version of the program that can
// be ported or used with different GUI environments.
//
// Change log:
//	ver 1.03: first released version
//	ver 2.02: replaced reset commands for Channels and Auxins with "/" commands
//            implemented a SAVE and RESTORE option
//            using other characters than 'r' or 'R' is no longer permitted, ensuring a correct logic
//            fixed the MaxSet vs. MaxIns values limit for Reset
//            code cleanup
//	ver 2.03: saved file is 1:1 mapped with X32 Snippets
//	ver 2.04: included /headamp and /ch/xx/delay settings
//	ver 2.05: changed some functions to extern
//	ver 2.06: preventing window resizing
//	ver 2.07: taking into account DCA and Mute Groups assignments for channels
//	ver 2.08: incorrect mask applied at loading saved presets implied wrong channel settings
//
#ifdef __WIN32__
#include <winsock2.h>
#define  millisleep(a)    Sleep(a)
#else
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#define  millisleep(a)    usleep(a*1000)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define BSIZE     256       // X32 communication buffer size
#define MINCHN    01        // min number of channels (INPUTs and AUXs)
#define MAXSET    40        // max number of channels (INPUTs and AUXs)
#define MAXPRE    99        // max index for libchan presets
#define B_XSIZE   25        // default box x size
#define B_YSIZE   20        // default box y size
#define XBUTTON    9        // number of control buttons, including "connect"
#define YPOS      90        // top/start y value for box positions
#define TPOS      75        // top/start y value for box-text positions
#define MAXCHN    35        // max number of channel nodes lines
#define MAXAUX    29        // max number of auxin nodes lines
#define MAXNODES  32 * MAXCHN + 8 * MAXAUX	// number of lines created by SAVE command


int  XCtestMinMax(int max);
void XCSetToZero();
int  GetASource(int k);
int  GetGroups(int k);
void SetASource(int k, int n);
void SetGroups(int k, int n);
void LoadCStrip(int k, int i);
void LoadNStrip(int k, int source);
int  SaveSet(int max, int set);
void CustomSet();
void InsertSet();
void SaveAll();
void RestoreStrip(int k);

extern int	X32Connect(int Xconnected, char* Xip_str, int btime);
extern int  validateIP4Dotted(const char *s);
extern int  Xsprint(char *bd, int index, char format, void *bs);
extern void Xfdump(char *header, char *buf, int len, int debug);

WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInstance = 0;
HMENU hipedit = (HMENU) 199;   // Windows handle to text to read
HWND hwndact[XBUTTON]; // WIndows handles to clickable buttons (button texts below)
LPWSTR txt_act[XBUTTON] = { L"Connect", L"Clear", L"Set", L"Insert", L"New",
                            L"List", L"Reset", L"Save", L"Restore" };
int p_x_act[XBUTTON] = { 155, 295, 360, 415, 415, 360, 240, 240, 295 }; // Associated X positions
int p_y_act[XBUTTON] = { 20, 45, 45, 45, 20, 20, 45, 20, 20 }; // Associated Y positions
int s_x_act[XBUTTON] = { 80, 60, 50, 50, 50, 50, 50, 50, 60 }; // Associated sizes (widths)
int s_y_act[XBUTTON] = { 45, 20, 20, 20, 20, 20, 20, 20, 20 }; // Associated sizes (heights)

HWND hwndSst[MAXSET];   // Windows handles to source strips channels
int p_x_Sst[MAXSET];    // Associated X positions

HFONT hfont, htmp;
HDC hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP hBmp;
BITMAP bmp;

FILE *res_file;          // file handle to resource file
char Finipath[1024];     // resolved path to .ini file
char **FinilppPart;
int Finiretval;

char MessStr[512];      // holds the list of channel/sources ("List" button)
char scscStr[64];       // intermediary buffer for above

int wWidth = 16 * (B_XSIZE + 5) + 12;  // window width/height and box width/height values
int wHeight = 6 * (B_YSIZE + 5) + 73; // will be in fact recalculated from resource file
int bWidth = B_XSIZE;                  // values, but these 4 variables are used as
int bHeight = B_YSIZE;  // default values if the resource file cannot be read
int yPos = YPOS;        // top/start y value for box positions
int tPos = TPOS;        // top/start y value for box-text positions
int Sixty2 = 62;        // mask value for X32 /load and /save command
int bWidth5 = B_XSIZE + 5;             // bWidth + 5

wchar_t W32_ip_str[20]; // X32 IP address in Windows text format
char Xip_str[20];       // X32 IP address in C text format
int Xconnected = 0;     // X32 connection state (0 is not connected)

wchar_t W32_Sst_str[8]; // To get the channel # from boxes (Window string)
char Sst_str[8];        // To get the channel # from boxes (C string)
int Sst[MAXSET];        // Where we store the 32 source-strip channels (source channels are fixed 1...32)
int Asrc[MAXSET];       // To store audio-source of channels before potential update
int Agrp[MAXSET];       // To store audio-source dca and mute groups assignments
//
// index values 00 -> 31 are in fact IN channels 01-32
// index values 32 -> 40 are in fact AUX channels 01-08
//
int MinChn = MINCHN;    // channel number start
int MaxSet = MAXSET;    // channel number stop values for Set/Reset
int MaxIns = MAXSET;    // channel number stop values for Insert/New
int MaxPre = MAXPRE;    // max value used for libchan presets
int Xdebug;				// debug flag
int Xsave;				// save ch and aux strip data at program start
int Xa_saved;			// save already performed?
//
int Xfd;                // X32 IP socket
struct sockaddr_in 	Xip;
struct sockaddr* 	Xip_addr = (struct sockaddr *)&Xip;

char r_buf[BSIZE], s_buf[BSIZE];   //X32 read and send buffers
int  r_len, s_len, p_status;       //X32 read and send lengths and read status
char chn_Asrc[36] = "/ch/00/config/source";
char aux_Asrc[40] = "/auxin/00/config/source";
char chn_Agrp[36] = "ch/00/grp";
char aux_Agrp[40] = "auxin/00/grp";
struct timeval timeout;  // timeout value for read actions
fd_set ufds;             // X32 file descriptor

char* SnipHead = "#2.1# \"CustLayer\" 8191 -1 255 0 1        \n";

char* Ch_inistr[] = {
		"/headamp/000 +0.0 OFF",
		"/ch/01/config \"\" 1 YE 1",
		"/ch/01/delay OFF 0.3",
		"/ch/01/eq OFF",
		"/ch/01/eq/1 PEQ 124.7 +0.00 2.0",
		"/ch/01/eq/2 PEQ 496.6 +0.00 2.0",
		"/ch/01/eq/3 PEQ 1k97 +0.00 2.0",
		"/ch/01/eq/4 HShv 10k02 +0.00 2.0",
		"/ch/01/gate OFF GATE -80.0 60.0 1 502 983 0",
		"/ch/01/gate/filter OFF 3.0 990.9",
		"/ch/01/dyn OFF COMP PEAK LOG 0.0 3.0 1 0.00 10 10.0 151 POST 0 100 OFF",
		"/ch/01/dyn/filter OFF 3.0 990.9",
		"/ch/01/insert OFF POST OFF",
		"/ch/01/grp %00000000 %000000",
		"/ch/01/mix/fader -oo",
		"/ch/01/mix/pan +0",
		"/ch/01/mix/on ON",
		"/ch/01/mix/01 ON -oo +0 EQ->",
		"/ch/01/mix/02 ON -oo",
		"/ch/01/mix/03 ON -oo +0 EQ->",
		"/ch/01/mix/04 ON -oo",
		"/ch/01/mix/05 ON -oo +0 EQ->",
		"/ch/01/mix/06 ON -oo",
		"/ch/01/mix/07 ON -oo +0 EQ->",
		"/ch/01/mix/08 ON -oo",
		"/ch/01/mix/09 ON -oo +0 POST",
		"/ch/01/mix/10 ON -oo",
		"/ch/01/mix/11 ON -oo +0 POST",
		"/ch/01/mix/12 ON -oo",
		"/ch/01/mix/13 ON -oo +0 POST",
		"/ch/01/mix/14 ON -oo",
		"/ch/01/mix/15 ON -oo +0 POST",
		"/ch/01/mix/16 ON -oo",
		"/ch/01/mix/mono OFF",
		"/ch/01/mix/mlevel -oo" };
int Ch_strings_max = sizeof(Ch_inistr) / sizeof(char *);

char* Aux_inistr[] = {
		"/headamp/000 +0.0 OFF",
		"/auxin/01/config \"\" 55 GN 33",
		"/auxin/01/eq OFF",
		"/auxin/01/eq/1 PEQ 124.7 +0.00 2.0",
		"/auxin/01/eq/2 PEQ 496.6 +0.00 2.0",
		"/auxin/01/eq/3 PEQ 1k97 +0.00 2.0",
		"/auxin/01/eq/4 HShv 10k02 +0.00 2.0",
		"/auxin/01/grp %00000000 %000000",
		"/auxin/01/mix/fader -oo",
		"/auxin/01/mix/pan +0",
		"/auxin/01/mix/on ON",
		"/auxin/01/mix/01 ON -oo +0 PRE",
		"/auxin/01/mix/02 ON -oo",
		"/auxin/01/mix/03 ON -oo +0 PRE",
		"/auxin/01/mix/04 ON -oo",
		"/auxin/01/mix/05 ON -oo +0 PRE",
		"/auxin/01/mix/06 ON -oo",
		"/auxin/01/mix/07 ON -oo +0 PRE",
		"/auxin/01/mix/08 ON -oo",
		"/auxin/01/mix/09 ON -oo +0 POST",
		"/auxin/01/mix/10 ON -oo",
		"/auxin/01/mix/11 ON -oo +0 POST",
		"/auxin/01/mix/12 ON -oo",
		"/auxin/01/mix/13 ON -oo +0 POST",
		"/auxin/01/mix/14 ON -oo",
		"/auxin/01/mix/15 ON -oo +0 POST",
		"/auxin/01/mix/16 ON -oo",
		"/auxin/01/mix/mono OFF",
		"/auxin/01/mix/mlevel -oo" };
int Aux_strings_max = sizeof(Aux_inistr) / sizeof(char *);

char* srcnme[] = { "OFF", "In01", "In02", "In03", "In04", "In05", "In06", "In07",
		"In08", "In09", "In10", "In11", "In12", "In13", "In14", "In15", "In16",
		"In17", "In18", "In19", "In20", "In21", "In22", "In23", "In24", "In25",
		"In26", "In27", "In28", "In29", "In30", "In31", "In32", "Aux1", "Aux2",
		"Aux3", "Aux4", "Aux5", "Aux6", "USBL", "USBR", "Fx1L", "Fx1R", "Fx2L",
		"Fx2R", "Fx3L", "Fx3R", "Fx4L", "FX4R", "Bs01", "Bs02", "Bs03", "Bs04",
		"Bs05", "Bs06", "Bs07", "Bs08", "Bs09", "Bs10", "Bs11", "Bs12", "Bs13",
		"Bs14", "Bs15", "Bs16" };

char* SCh_inistr[] = {
		"/headamp/000",
		"/ch/01/config",
		"/ch/01/delay",
		"/ch/01/eq",
		"/ch/01/eq/1",
		"/ch/01/eq/2",
		"/ch/01/eq/3",
		"/ch/01/eq/4",
		"/ch/01/gate",
		"/ch/01/gate/filter",
		"/ch/01/dyn",
		"/ch/01/dyn/filter",
		"/ch/01/insert",
		"/ch/01/grp",
		"/ch/01/mix/fader",
		"/ch/01/mix/pan",
		"/ch/01/mix/on",
		"/ch/01/mix/01",
		"/ch/01/mix/02",
		"/ch/01/mix/03",
		"/ch/01/mix/04",
		"/ch/01/mix/05",
		"/ch/01/mix/06",
		"/ch/01/mix/07",
		"/ch/01/mix/08",
		"/ch/01/mix/09",
		"/ch/01/mix/10",
		"/ch/01/mix/11",
		"/ch/01/mix/12",
		"/ch/01/mix/13",
		"/ch/01/mix/14",
		"/ch/01/mix/15",
		"/ch/01/mix/16",
		"/ch/01/mix/mono",
		"/ch/01/mix/mlevel" };
int SCh_strings_max = sizeof(SCh_inistr) / sizeof(char *);

char *SAux_inistr[] = {
		"/headamp/000",
		"/auxin/01/config",
		"/auxin/01/eq",
		"/auxin/01/eq/1",
		"/auxin/01/eq/2",
		"/auxin/01/eq/3",
		"/auxin/01/eq/4",
		"/auxin/01/grp",
		"/auxin/01/mix/fader",
		"/auxin/01/mix/pan",
		"/auxin/01/mix/on",
		"/auxin/01/mix/01",
		"/auxin/01/mix/02",
		"/auxin/01/mix/03",
		"/auxin/01/mix/04",
		"/auxin/01/mix/05",
		"/auxin/01/mix/06",
		"/auxin/01/mix/07",
		"/auxin/01/mix/08",
		"/auxin/01/mix/09",
		"/auxin/01/mix/10",
		"/auxin/01/mix/11",
		"/auxin/01/mix/12",
		"/auxin/01/mix/13",
		"/auxin/01/mix/14",
		"/auxin/01/mix/15",
		"/auxin/01/mix/16",
		"/auxin/01/mix/mono",
		"/auxin/01/mix/mlevel" };
int SAux_strings_max = sizeof(SAux_inistr) / sizeof(char *);
//
// need to "double" the storage of above strings as we'll be modifying the strips numbers dynamically
char Ch_resets[MAXCHN][80];
char Aux_resets[MAXAUX][48];

char SCh_nodes[MAXCHN][32];
char SAux_nodes[MAXAUX][32];

#ifdef __WIN32__
WSADATA wsa;
int Xip_len = sizeof(Xip);          // length of addresses
#else
socklen_t Xip_len = sizeof(Xip);    // length of addresses
#endif
//
// Union for endian conversion
union cast {
	float ff;
	int ii;
	char cc[4];
} endian;
//
#define SEND_TO(b, l)                                                      \
    do {                                                                   \
        if (Xdebug) {Xfdump("->X", s_buf, s_len, 0); fflush(stdout);}      \
        if (sendto(Xfd, b, l, 0, Xip_addr, Xip_len) < 0) {                 \
            perror ("Coundn't send data to X32\n");                        \
            exit(EXIT_FAILURE);                                            \
        }                                                                  \
    } while (0);
//
#define RECV_FR(b, l)                                                  \
    do {                                                               \
        l = recvfrom(Xfd, b, BSIZE, 0, 0, 0);                          \
        if (Xdebug) {Xfdump("X->", r_buf, r_len, 0); fflush(stdout);}  \
    } while (0);
//
#define RPOLL                                               \
    do {                                                    \
        FD_ZERO(&ufds);                                     \
        FD_SET(Xfd, &ufds);                                 \
        p_status = select(Xfd+1,&ufds,NULL,NULL,&timeout);  \
    } while (0);
//
//
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
	MSG msg;
//
	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32CustomLayer";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
			L"X32CustomLayer - Create X32 Custom Layers",
			WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 50, 50, wWidth + 1, wHeight + 13,
			0, 0, hInstance, 0);

	while (GetMessage(&msg, NULL, 0, 0)) {   // Let Windows manage the main loop
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HWND hwndipaddr;
	int i, j, nb_assigns, len;
	char str1[8];

	switch (msg) {
	case WM_CREATE:
		// create main window
		hwndipaddr = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 45, 140, 20, hwnd,
				hipedit, NULL, NULL);
		// Set action buttons (Connect,...Restore)
		for (i = 0; i < XBUTTON; i++) {
			hwndact[i] = CreateWindowW(L"button", txt_act[i],
					WS_VISIBLE | WS_CHILD, p_x_act[i], p_y_act[i], s_x_act[i],
					s_y_act[i], hwnd, (HMENU )(200 + i), NULL, NULL);
		}
		// create 40 editable boxes in two rows of 16 + one row of 8
		yPos = YPOS;
		for (i = 0; i < 16; i++) {
			hwndSst[i] = CreateWindowW(L"Edit", NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER, 5 + i * bWidth5, yPos,
					bWidth, bHeight, hwnd, hipedit, NULL, NULL);
		}
		yPos += (2 * bHeight + 5);
		for (i = 0; i < 16; i++) {
			hwndSst[i + 16] = CreateWindowW(L"Edit", NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER, 5 + i * bWidth5, yPos,
					bWidth, bHeight, hwnd, hipedit, NULL, NULL);
		}
		yPos += (2 * bHeight + 5);
		for (i = 0; i < 8; i++) {
			hwndSst[i + 32] = CreateWindowW(L"Edit", NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER, 5 + i * bWidth5, yPos,
					bWidth, bHeight, hwnd, hipedit, NULL, NULL);
		}
		break;
//
	case WM_PAINT:
// WIndows repaint request
		hdc = BeginPaint(hwnd, &ps);
		MoveToEx(hdc, 5, 70, NULL);
		LineTo(hdc, wWidth - 12, 70);
//
		SetBkMode(hdc, TRANSPARENT);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 15, 2, "X32CustomLayer - V2.08 - ©2016 - Patrick-Gilles Maillot", 55);
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 15, 30, "Enter X32 IP below:", 18);
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		SetBkMode(hdc, TRANSPARENT);
		// write /display the channel numbers on top of the boxes
		j = bWidth / 2 - 2;
		tPos = TPOS;
		for (i = 0; i < 16; i++) {
			TextOut(hdc, j + i * bWidth5, tPos, str1,
			wsprintf(str1, "%02d", i + 1));
		}
		tPos += (2 * bHeight + 5);
		for (i = 0; i < 16; i++) {
			TextOut(hdc, j + i * bWidth5, tPos, str1,
			wsprintf(str1, "%02d", i + 17));
		}
		tPos += (2 * bHeight + 5);
		for (i = 0; i < 8; i++) {
			TextOut(hdc, j + i * bWidth5, tPos, str1,
			wsprintf(str1, "%02d", i + 33));
		}
		tPos += 15;
		TextOut(hdc, 9 * bWidth5, tPos, "values 33-40 are AUX 01-08", 26);
		//
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		// display the X32 IP address (user changeable)
		SetWindowText(hwndipaddr, (LPSTR) Xip_str);
		// initialize the boxes to empty (user changeable)
		for (i = MinChn - 1; i < MaxSet; i++) {
			SetWindowTextW(hwndSst[i], L"");
		}
		EndPaint(hwnd, &ps);
		break;
//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {    // user action
			switch (LOWORD(wParam)) {
			case 200:         // Connect button hit
				if (Xdebug) { printf("Connect button hit\n"); fflush(stdout); }
				len = GetWindowTextLengthW(hwndipaddr) + 1;
				GetWindowTextW(hwndipaddr, W32_ip_str, len);
				WideCharToMultiByte(CP_ACP, 0, W32_ip_str, len, Xip_str, len, NULL, NULL);
				if (Xdebug) { printf("IP = %s\n", Xip_str); fflush(stdout); }
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32Connect(Xconnected, Xip_str, 10000);
					if (Xconnected) {
						if (Xsave & !Xa_saved) {
							Xa_saved = 1;
							if (Xdebug) { printf("Init Saving Channel and Aux strip data\n"); fflush(stdout); }
							SaveAll();
						}
						SetWindowTextW(hwndact[0], L"Connected");
					} else {
						SetWindowTextW(hwndact[0], L"Connect");
					}
				} else {
					perror("incorrect IP string form");
				}
				break;
			case 201:        // Clear button hit
				if (Xdebug) { printf("Clear button hit\n"); fflush(stdout); }
				for (i = 0; i < MAXSET; i++) {
					SetWindowTextW(hwndSst[i], L"");
				}
				break;
			case 202:        // Set button hit
				if (Xdebug) { printf("Set button hit\n"); fflush(stdout); }
				if (Xconnected) {
					// make sure there's no unwanted selection (outside of min/max sets)
					if (XCtestMinMax(MaxSet) == 0) {
						for (i = MinChn - 1; i < MaxSet; i++) {
							len = GetWindowTextLengthW(hwndSst[i]) + 1;
							GetWindowTextW(hwndSst[i], W32_Sst_str, len);
							WideCharToMultiByte(CP_ACP, 0, W32_Sst_str, len, Sst_str, len, NULL, NULL);
							if (Sst_str[0] != 0) {
								sscanf(Sst_str, "%d", &Sst[i]);
								if (Sst[i] < MinChn || Sst[i] > MaxSet) { // manage wrong user data
									Sst[i] = -1; // force ch number to <nil> and erase data
									SetWindowTextW(hwndSst[i], L"- - -");
								}
							}
						}
						i = SaveSet(MaxSet, 1);
						CustomSet();        // Manage X32 update
					} else {
						XCSetToZero(); // all channels to "- - -"
					}
				}
				break;
			case 203:        // Insert button hit
				if (Xdebug) { printf("Insert button hit\n"); fflush(stdout); }
				if (Xconnected) {
					nb_assigns = 0;
					if (XCtestMinMax(MaxIns) == 0) {
						for (i = MinChn - 1; i < MaxIns; i++) {
							len = GetWindowTextLengthW(hwndSst[i]) + 1;
							GetWindowTextW(hwndSst[i], W32_Sst_str, len);
							WideCharToMultiByte(CP_ACP, 0, W32_Sst_str, len, Sst_str, len, NULL, NULL);
							if (Sst_str[0] != 0) {
								sscanf(Sst_str, "%d", &Sst[i]);
								if (Sst[i] < MinChn || Sst[i] > MaxIns) { // manage wrong user data
									Sst[i] = -1; // force ch number to <nil> and erase data
									SetWindowTextW(hwndSst[i], L"- - -");
								}
							}
						}
						// Manage X32 update with Insert an existing or New (blank) strip
						if ((nb_assigns = SaveSet(MaxIns, 1)) == 1) {
							InsertSet(1);
						} else {
							XCSetToZero(); // all channels to "- - -"
						}
					} else {
						XCSetToZero(); // all channels to "- - -"
					}
				}
				break;
			case 204:        // New button hit
				if (Xdebug) { printf("New button hit\n"); fflush(stdout); }
				if (Xconnected) {
					nb_assigns = 0;
					if (XCtestMinMax(MaxIns) == 0) {
						for (i = MinChn - 1; i < MaxIns; i++) {
							len = GetWindowTextLengthW(hwndSst[i]) + 1;
							GetWindowTextW(hwndSst[i], W32_Sst_str, len);
							WideCharToMultiByte(CP_ACP, 0, W32_Sst_str, len, Sst_str, len, NULL, NULL);
							if (Sst_str[0] != 0) {
								sscanf(Sst_str, "%d", &Sst[i]);
								if (Sst[i] < 0 || Sst[i] > 64) { // manage wrong user data
									Sst[i] = -1; // force ch number to -1 and erase data
									SetWindowTextW(hwndSst[i], L"- - -");
								} else {
									nb_assigns += 1;
								}
							}
						}
						// Manage X32 update with Insert an existing or New (blank) strip
						if (nb_assigns == 1) {
							InsertSet(0);
						} else {
							XCSetToZero(); // all channels to "- - -"
						}
					} else {
						XCSetToZero(); // all channels to "- - -"
					}
				}
				break;
			case 205:        // List button hit
				if (Xdebug) { printf("List button hit\n"); fflush(stdout); }
				if (Xconnected) {
					*MessStr = 0;
					strcat(MessStr, "  Channel\tSource\tChannel\tSource\n");
					// Fill out channel and input source data
					for (i = 0; i < 16; i++) {
						sprintf(scscStr, "     %02d\t  %s\t  %02d\t %s\n",
								i + 1, srcnme[GetASource(i + 1)], i + 17,
								srcnme[GetASource(i + 17)]);
						strcat(MessStr, scscStr);
					}
					strcat(MessStr, "\n  AuxIN\tSource\tAuxIN\tSource\n");
					for (i = 0; i < 4; i++) {
						sprintf(scscStr, "     %02d\t  %s\t  %02d\t %s\n",
								i + 1, srcnme[GetASource(i + 33)], i + 5,
								srcnme[GetASource(i + 37)]);
						strcat(MessStr, scscStr);
					}
					MessageBox(NULL, (LPCSTR) MessStr,
							(LPCSTR) "X32CustomLayer: Channels & Sources list",
							MB_OK);
				}
				break;
			case 206:        // Reset button hit
				if (Xdebug) { printf("Reset button hit\n"); fflush(stdout); }
				if (Xconnected) {
					nb_assigns = 0;
					if (XCtestMinMax(MaxSet) == 0) {
						for (i = MinChn - 1; i < MaxSet; i++) {
							len = GetWindowTextLengthW(hwndSst[i]) + 1;
							GetWindowTextW(hwndSst[i], W32_Sst_str, len);
							WideCharToMultiByte(CP_ACP, 0, W32_Sst_str, len, Sst_str, len, NULL, NULL);
							if (Sst_str[0] == 'r' || Sst_str[0] == 'R') {
								Sst[i] = 1;
								nb_assigns += 1;
							} else {	// refuse characters other than 'r' or 'R'
								if (len > 1) {
									XCSetToZero(); // all channels to "- - -"
									return DefWindowProcW(hwnd, msg, wParam, lParam);
								}
							}
						}
						if (nb_assigns) {    // Selective Reset
							if (MessageBox(NULL,
									(LPCSTR) "Are you sure?\nThis will reset selected channels\nto their default values",
									(LPCSTR) "X32CustomLayer: Selective Reset",
									MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
								//
								// reset selected channels (user said so)
								if (Xdebug) { printf("Applying selective Reset!\n"); fflush(stdout); }
								for (i = MinChn - 1; i < MaxSet; i++) {
									if (Sst[i] != -1) LoadNStrip(i, i + 1);
								}
							}
						} else {             // Full Reset
							if (MessageBox(NULL,
									(LPCSTR) "Are you sure?\nThis will reset ALL active channels\nto their default values",
									(LPCSTR) "X32CustomLayer: Full Reset",
									MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
								//
								// reset all channels (user said so)
								if (Xdebug) { printf("Applying full Reset!\n"); fflush(stdout); }
								for (i = MinChn - 1; i < MaxSet; i++) {
									LoadNStrip(i, i + 1);
								}
							}
						}
					} else {
						XCSetToZero(); // all channels to "- - -"
					}
				}
				break;
			case 207:        // Save button hit
				if (Xdebug) { printf("Save button hit\n"); fflush(stdout); }
				if (Xconnected) {
					if (MessageBox(NULL,
							(LPCSTR) "Are you sure?\nThis will replace already saved data",
							(LPCSTR) "X32CustomLayer: Save all strips data",
							MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
						SaveAll();
					}
				}
				break;
			case 208:        // Restore button hit
				if (Xdebug) { printf("Restore button hit\n"); fflush(stdout); }
				if (Xconnected) {
					nb_assigns = 0;
					if (XCtestMinMax(MaxSet) == 0) {
						for (i = MinChn - 1; i < MaxSet; i++) {
							len = GetWindowTextLengthW(hwndSst[i]) + 1;
							GetWindowTextW(hwndSst[i], W32_Sst_str, len);
							WideCharToMultiByte(CP_ACP, 0, W32_Sst_str, len,
									Sst_str, len, NULL, NULL);
							if (Sst_str[0] == 'r' || Sst_str[0] == 'R') {
								Sst[i] = 1;
								nb_assigns += 1;
							} else {	// refuse characters other than 'r' or 'R'
								if (len > 1) {
									XCSetToZero(); // all channels to "- - -"
									return DefWindowProcW(hwnd, msg, wParam, lParam);
								}
							}
						}
						if (nb_assigns) {    // Selective Restore
							if (MessageBox(NULL,
									(LPCSTR) "Are you sure?\nThis will Restore selected channels\nto their previously saved values",
									(LPCSTR) "X32CustomLayer: Selective Restore",
									MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
								//
								// reset selected channels (user said so)
								if (Xdebug) { printf("Applying selective Restore!\n"); fflush(stdout); }
								for (i = MinChn - 1; i < MaxSet; i++) {
									if (Sst[i] != -1) RestoreStrip(i);
								}
							}
						} else {             // Full Restore
							if (MessageBox(NULL,
									(LPCSTR) "Are you sure?\nThis will Restore ALL active channels\nto their previously saved values",
									(LPCSTR) "X32CustomLayer: Full Restore",
									MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
								//
								// reset all channels (user said so)
								if (Xdebug) { printf("Applying full Restore!\n"); fflush(stdout); }
								RestoreStrip(-1);
							}
						}
					} else {
						XCSetToZero(); // all channels to "- - -"
					}
				}
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		if (Xconnected)
			close(Xfd);
		// read panel data
		i = GetWindowTextLengthW(hwndipaddr) + 1;
		GetWindowTextW(hwndipaddr, W32_ip_str, i);
		WideCharToMultiByte(CP_ACP, 0, W32_ip_str, i, Xip_str, i, NULL, NULL);
		// update resource data file
		res_file = fopen(Finipath, "wb");
		fputs(
				"#\r\n# X32CustomLayer.ini - configuration file - ©2016 Patrick-Gilles Maillot\r\n",
				res_file);
		fputs("#\r\n# Line 1:\r\n", res_file);
		fputs("# The 2 numbers are respectively representing the width and height of input \r\n", res_file);
		fputs("# boxes used in the application\r\n#\r\n# Line 2:\r\n", res_file);
		fputs("# The data are numbers in the range [01..40] for the channel number start\r\n", res_file);
		fputs("# followed by the channel number stop values for Set/Reset/Restore and Insert/New functions\r\n", res_file);
		fputs("# The last number of the line, in the range [0..99] is the max index value used for \r\n", res_file);
		fputs("# X32 libchan presets\r\n#\r\n# Line 3:\r\n# Enter the IP address of your X32\r\n#\r\n# Line 4:\r\n", res_file);
		fputs("# The first digit {0, 1} is a debug flag. Leave to 0, unless you need or are asked\r\n", res_file);
		fputs("# to debug the application. The second digit {0, 1} is a flag indicating whether a\r\n", res_file);
		fputs("# save of the channel and auxin strips data are systematically done when the program\r\n", res_file);
		fputs("# starts {1} or if the user has to click on save prior to any change being made {0}.\r\n#\r\n", res_file);
		fprintf(res_file, "%02d %02d\r\n", bWidth, bHeight);
		fprintf(res_file, "%02d %02d %02d %02d\r\n", MinChn, MaxSet, MaxIns, MaxPre);
		fprintf(res_file, "%s\r\n", Xip_str);
		fprintf(res_file, "%1d %1d\r\n", Xdebug, Xsave);
		fclose(res_file);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
// Utility functions
//
void XCSetToZero() {
	int i;

	// use "- - -" to indicate an error in entering data to channel boxes
	for (i = 0; i < MAXSET; i++) {
		SetWindowTextW(hwndSst[i], L"- - -");
	}
}
//
//
int XCtestMinMax(int max) {
int i, len;
	//
	// make sure there's no unwanted selection (outside of min/max sets)
	len = 0;
	for (i = 0; i < MinChn - 1; i++) {
		len = len + GetWindowTextLengthW(hwndSst[i]);
	}
	if (len) return len;
	for (i = max; i < MAXSET; i++) {
		len = len + GetWindowTextLengthW(hwndSst[i]);
	}
	if (len) return len;
	for (i = 0; i < MAXSET; i++) {
		Asrc[i] = -1;
		Sst[i] = -1;
	}
	return 0;
}
//
//
// Save all Channel and AuxIn strips data to file
//
void SaveAll() {
int i, j, k, l;
	//
	l = 0;
	if ((res_file = fopen("./X32CustLayer.snp", "wb")) != NULL) {
		if (fputs(SnipHead, res_file) == EOF) {
			if (Xdebug) printf("Could not write to file\n");
		} else {
			l += 1;
		}
		fflush(res_file);
		for (j = 0; j < 40; j++) {
			if (j < 32) { // standard input channels
				k = j + 1;
				for (i = 0; i < MAXCHN; i++) {
					if (i == 0) {
						// replace headamp index with value of k as a channel value
						SCh_nodes[i][10] = (char) (48 + (j / 10));
						SCh_nodes[i][11] = (char) (48 + j - (j / 10) * 10);
					} else {
						// replace ch index with value of k as a channel value
						SCh_nodes[i][4] = (char) (48 + (k / 10));
						SCh_nodes[i][5] = (char) (48 + k - (k / 10) * 10);
					}
					s_len = Xsprint(s_buf, 0, 's', "/node");
					s_len = Xsprint(s_buf, s_len, 's', ",s");
					s_len = Xsprint(s_buf, s_len, 's', SCh_nodes[i] + 1); // skip first '/'
					SEND_TO(s_buf, s_len)
					RPOLL
					if (p_status < 0) {
						printf("Polling for data failed\n");
					} else if (p_status > 0) {
						// We have received data - process it!
						r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);
						//... and save to file
						if (fputs(r_buf + 12, res_file) == EOF) {
							if (Xdebug) printf("Could not write to file\n");
						} else {
							l += 1;
						}
					} else {
						// time out...
						printf("X32 reception timeout\n");
					}
				}
				fflush(res_file);
			} else {
				k = j - 31;
				for (i = 0; i < MAXAUX; i++) {
					if (i == 0) {
						// replace headamp index with value of k as a channel value
						SAux_nodes[i][10] = (char) (48 + (j / 10));
						SAux_nodes[i][11] = (char) (48 + j - (j / 10) * 10);
					} else {
						// replace aux index with value of k as a channel value
						SAux_nodes[i][7] = (char) (48 + (k / 10));
						SAux_nodes[i][8] = (char) (48 + k - (k / 10) * 10);
					}
					s_len = Xsprint(s_buf, 0, 's', "/node");
					s_len = Xsprint(s_buf, s_len, 's', ",s");
					s_len = Xsprint(s_buf, s_len, 's', SAux_nodes[i] + 1); // skip first '/'
					SEND_TO(s_buf, s_len)
					RPOLL
					if (p_status < 0) {
						printf("Polling for data failed\n");
					} else if (p_status > 0) {
						// We have received data - process it!
						r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);
						//... and save to file
						if (fputs(r_buf + 12, res_file) == EOF) {
							if (Xdebug) printf("Could not write to file\n");
						} else {
							l += 1;
						}
					} else {
						// time out...
						printf("X32 reception timeout\n");
					}
				}
			}
		}
		fflush(res_file);
		fclose(res_file);
		if (l < (MAXNODES + 1)) { // all CH / AUX lines plus header
			MessageBox(NULL,
					(LPCSTR) "WARNING, not all lines saved,\n Hit SAVE again!",
					(LPCSTR) "X32CustomLayer: SAVE strips data",
					MB_ICONERROR | MB_OK);
		}
	} else {
		if (Xdebug) { printf("Could not open X32CustLayer.snp file for writing\n"); fflush(stdout); }
	}
	return;
}
//
//
// Restore channel/Aux strip index k from file
//

void RestoreStrip(int k) {
int i, l;
	//
	// prepare to skip first line of file
	if ((res_file = fopen("./X32CustLayer.snp", "r")) != NULL) {
		if (fgets(r_buf, BSIZE - 2, res_file) == NULL) {
			MessageBox(NULL, (LPCSTR) "Error reading file!",
					(LPCSTR) "X32CustomLayer: RESTORE strips data",
					MB_ICONERROR | MB_OK);
			return;
		}
		// if k < 0, we restore all channels
		if (k < 0) {
			k = MAXNODES;
		} else {
			if (k < 32) {
				// skip channels unselected lines until "k"
				l = (k * MAXCHN);
				k = MAXCHN;
			} else {
				l = 32 * MAXCHN + ((k - 32) * MAXAUX);
				k = MAXAUX;
			}
			for (i = 0; i < l; i++) {
				if (fgets(r_buf, BSIZE - 2, res_file) == NULL) {
					MessageBox(NULL, (LPCSTR) "Error reading file!",
							(LPCSTR) "X32CustomLayer: RESTORE strips data",
							MB_ICONERROR | MB_OK);
					return;
				}
			}
		}
		// now restore CH or AuxIn strip at index k [0..39] (or all)
		for (i = 0; i < k; i++) {
			if (fgets(r_buf, BSIZE - 2, res_file) != NULL) {
				s_len = Xsprint(s_buf, 0, 's', "/");
				s_len = Xsprint(s_buf, s_len, 's', ",s");
				s_len = Xsprint(s_buf, s_len, 's', r_buf); //
				SEND_TO(s_buf, s_len)
				RPOLL
				if (p_status < 0) {
					printf("Polling for data failed\n");
					return;
				} else if (p_status > 0) {
					// We have received data - process it!
					r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);
					//... and quietly ignore data...
				} else {
					// time out...
					printf("X32 reception timeout\n");
					return;
				}
			} else {
				MessageBox(NULL, (LPCSTR) "Error reading file!",
						(LPCSTR) "X32CustomLayer: RESTORE strips data",
						MB_ICONERROR | MB_OK);
				return;
			}
		}
	} else {
		MessageBox(NULL, (LPCSTR) "Error, no file found,\n Hit SAVE first!",
				(LPCSTR) "X32CustomLayer: RESTORE strips data",
				MB_ICONERROR | MB_OK);
	}
	return;
}
//
//
// Get the audio-source dca and mute groups assignments for strip/channel k
//
int GetGroups(int k) {
int j, imut, idca, Xlock, grpmut, grpdca;
	//
	s_len = Xsprint(s_buf, 0, 's', "/node");
	s_len = Xsprint(s_buf, s_len, 's', ",s");
	//
	// INput or AUXin channel?
	if (k < 33) {
		// send /ch/xx/config/source to get the information
		j = k / 10;
		chn_Agrp[3] = (char) (j + 48);
		chn_Agrp[4] = (char) (k - j * 10 + 48);
		s_len = Xsprint(s_buf, s_len, 's', chn_Agrp);
	} else {
		// send /auxin/xx/config/source to get the information
		j = k - 32;
		aux_Agrp[7] = (char) (j + 48);
		s_len = Xsprint(s_buf, s_len, 's', aux_Agrp);
	}
	// send /node ,s ch|auxin/[01…32]/grp command to get dca and mutes, and save bits
	SEND_TO(s_buf, s_len)
	// read data back from X32 and save it for later use
	// will receive two %bits groups, first 8bts are DCA, next 6bits are Mute Groups
	Xlock = 1;
	while (Xlock) {
		RPOLL // read the desk for answer to /save
		if (p_status > 0) { // ignore responses that do not correspond to what is
			RECV_FR(r_buf, r_len); // expected: node auxin or /ch  /xx/config/source~,i~~[SRC]
			if (strcmp(r_buf, "node") == 0) {
				if (k < 33) idca = 24;
				else        idca = 27;
				imut = idca + 10;
				// save dca bits in bits 15 to 8 of Agrp word and mutes in the rightmost bits
				grpdca = grpmut = 0;
				for (j = 0; j < 8; j++)
					if (*(r_buf + idca + j) == '1') grpdca |= (1 << (7-j));
				for (j = 0; j < 6; j++)
					if (*(r_buf + imut + j) == '1') grpmut |= (1 << (5-j));
				Xlock = 0;
			} else {
				fprintf(stderr, "Waiting node..., got %s \n", r_buf);
			}
		}
	}
	// return data to caller; dca bits in bits 15 to 8 and mutes in the rightmost bits
	return grpdca << 8 | grpmut;
}
//
//
// Get the audio-source input number associated with channel k
//
int GetASource(int k) {
int j, Xlock;
	//
	// INput or AUXin channel?
	if (k < 33) {
		// send /ch/xx/config/source to get the information
		j = k / 10;
		chn_Asrc[4] = (char) (j + 48);
		chn_Asrc[5] = (char) (k - j * 10 + 48);
		s_len = Xsprint(s_buf, 0, 's', chn_Asrc);
		j = 6;
	} else {
		// send /auxin/xx/config/source to get the information
		j = k - 32;
		aux_Asrc[8] = (char) (j + 48);
		s_len = Xsprint(s_buf, 0, 's', aux_Asrc);
		j = 9;
	}
	SEND_TO(s_buf, s_len)
	// read data back from X32 and save it for later use
	Xlock = 1;
	while (Xlock) {
		RPOLL // read the desk for answer to /save
		if (p_status > 0) { // ignore responses that do not correspond to what is
			RECV_FR(r_buf, r_len); // expected: /ch|auxin/xx/config/source~,i~~[SRC]
			if (strcmp(r_buf + j, "/config/source") == 0) {
				for (j = 0; j < 4; j++)
					endian.cc[3 - j] = r_buf[28 + j];
				Xlock = 0;
			} else {
				fprintf(stderr, "Waiting /xxxxx/xx/config/source..., got %s \n", r_buf);
			}
		}
	}
	return endian.ii; // return audio-source data
}
//
//
// Set dca and mute groups data contained in n into channel index k
//
void SetGroups(int k, int n) {
// n has dca data in bits 15-8 and mute groups in bits 5-0
char tmpbuf[32];
int Xlock;
	//
	if (k < 32) sprintf(tmpbuf, "ch/%02d/grp %d %d", k+1, n >> 8, n & 0xFF);
	else        sprintf(tmpbuf, "auxin/%02d/grp %d %d", k+1, n >> 8, n & 0xFF);
	s_len = Xsprint(s_buf, 0, 's', "/");
	s_len = Xsprint(s_buf, s_len, 's', ",s");
	s_len = Xsprint(s_buf, s_len, 's', tmpbuf);
	SEND_TO(s_buf, s_len)
	Xlock = 1;
	while (Xlock) {
		RPOLL // read the desk for answer to /save
		if (p_status > 0) { // ignore responses that do not correspond to what is
			RECV_FR(r_buf, r_len); // expected: what has been sent above
			if (memcmp(r_buf, s_buf, s_len) == 0) {
				Xlock = 0;
			} else {
				fprintf(stderr, "Unexpected reply, got %s \n", r_buf);
			}
		}
	}
	return;
}
//
//
// Set audio-source data n into channel index k
//
void SetASource(int k, int n) {
int j;
	//
	if (k < 32) {
		// send /ch/xx/config/source ,i xx to set the information
		j = (k + 1) / 10;
		chn_Asrc[4] = (char) (j + 48);
		chn_Asrc[5] = (char) (k + 1 - j * 10 + 48);
		s_len = Xsprint(s_buf, 0, 's', chn_Asrc);
	} else {
		// send /auxin/xx/config/source ,i xx to set the information
		j = k + 1 - 32;
		aux_Asrc[8] = (char) (j + 48);
		s_len = Xsprint(s_buf, 0, 's', aux_Asrc);
	}
	s_len = Xsprint(s_buf, s_len, 's', ",i");
	endian.ii = n;
	for (j = 0; j < 4; j++)
		s_buf[s_len + j] = endian.cc[3 - j];
	s_len += 4;
	SEND_TO(s_buf, s_len)
	return;
}
//
//
// Load into X32 at channel strip index i the channel strip #k (saved at
// position MaxPre + 1 -k in presets)
//
void LoadCStrip(int k, int i) {
int j;
int Xlock;
//
	s_len = Xsprint(s_buf, 0, 's', "/load");
	s_len = Xsprint(s_buf, s_len, 's', ",siii");
	s_len = Xsprint(s_buf, s_len, 's', "libchan");
	j = MaxPre + 1 - k;
	s_len = Xsprint(s_buf, s_len, 'i', &j);
	j = i;
	s_len = Xsprint(s_buf, s_len, 'i', &j);
	s_len = Xsprint(s_buf, s_len, 'i', &Sixty2);
	SEND_TO(s_buf, s_len)
	Xlock = 1;
	while (Xlock) {
		RPOLL // read the desk for answer to /load
		if (p_status > 0) {   // ignore responses that do not correspond to what
			RECV_FR(r_buf, r_len);  // is expected: /load~~~,si~libchan~[     1]
			if (strncmp(r_buf, "/load", 5) == 0) {
				for (j = 0; j < 4; j++)
					endian.cc[3 - j] = r_buf[20 + j];
				if (endian.ii != 1) {
					perror("Error: Could not load channel data\n");
					return;
				}
				Xlock = 0;
			} else {
				fprintf(stderr, "Waiting /load..., got %s \n", r_buf);
			}
		}
	}
	return;
}
//
//
// Set a blank strip at index 'k' with audio source 'source'
//
void LoadNStrip(int k, int source) {
int i, j;
//
	// set value vs index for k
	j = k;
	k += 1;
	//
	if (j < 32) { // standard input channels
		for (i = 0; i < MAXCHN; i++) {
			if (i == 0) {
				// replace headamp index with value of k as a channel value
				Ch_resets[i][10] = (char) (48 + (j / 10));
				Ch_resets[i][11] = (char) (48 + j - (j / 10) * 10);
			} else {
				// replace index with value of k as a channel value
				Ch_resets[i][4] = (char) (48 + (k / 10));
				Ch_resets[i][5] = (char) (48 + k - (k / 10) * 10);
				// special handling of first command
				if (i == 1) {
					// set Channel strip source value to source given in parameter
					Ch_resets[i][22] = (char) (48 + (source / 10));
					Ch_resets[i][23] = (char) (48 + source - (source / 10) * 10);
				}
			}
			s_len = Xsprint(s_buf, 0, 's', "/");
			s_len = Xsprint(s_buf, s_len, 's', ",s");
			s_len = Xsprint(s_buf, s_len, 's', Ch_resets[i]); //
			SEND_TO(s_buf, s_len)
			RPOLL
			if (p_status < 0) {
				printf("Polling for data failed\n");
				return;
			} else if (p_status > 0) {
				// We have received data - process it!
				r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);
				//... and quietly ignore data...
			} else {
				// time out...
				printf("X32 reception timeout\n");
				return;
			}
		}
	} else {
		k -= 32;
		for (i = 0; i < MAXAUX; i++) {
			if (i == 0) {
				// replace headamp index with value of k as a channel value
				Aux_resets[i][10] = (char) (48 + (j / 10));
				Aux_resets[i][11] = (char) (48 + j - (j / 10) * 10);
			} else {
				// replace index with value of k as a channel value in /auxin/01
				Aux_resets[i][7] = (char) (48 + (k / 10));
				Aux_resets[i][8] = (char) (48 + k - (k / 10) * 10);
				// special handling of first command
				if (i == 1) {
					// set Auxin strip source to value given in parameters
					Aux_resets[i][26] = (char) (48 + (source / 10));
					Aux_resets[i][27] = (char) (48 + source - (source / 10) * 10);
					// set color and icon depending on Auxin strip number
					if (k > 6) {
						Aux_resets[i][20] = '6';
						Aux_resets[i][21] = '0';
						Aux_resets[i][23] = 'Y';
						Aux_resets[i][24] = 'E';
					} else {
						Aux_resets[i][20] = '5';
						Aux_resets[i][21] = '5';
						Aux_resets[i][23] = 'G';
						Aux_resets[i][24] = 'N';
					}
				}
			}
			s_len = Xsprint(s_buf, 0, 's', "/");
			s_len = Xsprint(s_buf, s_len, 's', ",s");
			s_len = Xsprint(s_buf, s_len, 's', Aux_resets[i]);
			SEND_TO(s_buf, s_len)
			RPOLL
			if (p_status < 0) {
				printf("Polling for data failed\n");
				return;
			} else if (p_status > 0) {
				// We have received data - process it!
				r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);
				//... and quietly ignore data...
			} else {
				// time out...
				printf("X32 reception timeout\n");
				return;
			}
		}
	}
	return;
}
//
//
// Shift up channel strip at index i - 1 to channel strip at index i
// This acts on actual channel data, using libchan presets as a temp buffer
//
void ShiftCStrip(int i) {
int j, Xlock, Asrcl, Agrpl;
char channel[14] = "CustLayer";
	//
	// Get the audio-source, dca and mute groups for the channel strip to move up
	Asrcl = GetASource(i);
	Agrpl = GetGroups(i);
	// shift is done by saving i-1 to, then loading from channel preset into i
	// sending a /save ,sisi "libchan" [MaxPre + 1 - i] "ch<i+1>" [i]
	s_len = Xsprint(s_buf, 0, 's', "/save");
	s_len = Xsprint(s_buf, s_len, 's', ",sisi");
	s_len = Xsprint(s_buf, s_len, 's', "libchan");
	j = MaxPre + 1 - i;
	s_len = Xsprint(s_buf, s_len, 'i', &j);
	j = i / 10;
	channel[9] = 48 + j;
	channel[10] = 48 + i - j * 10;
	channel[11] = 0;
	s_len = Xsprint(s_buf, s_len, 's', channel);
	j = i - 1;
	s_len = Xsprint(s_buf, s_len, 'i', &j);
	SEND_TO(s_buf, s_len)
	Xlock = 1;
	while (Xlock) {
		RPOLL
		; // read the desk for answer to /save
		if (p_status > 0) {   // ignore responses that do not correspond to what
			RECV_FR(r_buf, r_len);  // is expected: /save~~~,si~libchan~[     1]
			if (strncmp(r_buf, "/save", 5) == 0) {
				for (j = 0; j < 4; j++)
					endian.cc[3 - j] = r_buf[20 + j];
				if (endian.ii != 1) {
					perror("Error: Could not save channel data\n");
					return;
				}
				Xlock = 0;
			} else {
				fprintf(stderr, "Waiting /save..., got %s \n", r_buf);
			}
		}
	}
	// load the data we just saved into the next channel strip (i)
	LoadCStrip(i, i);
	// Set audio-source, dca and mute groups we saved to new channel strip
	SetASource(i, Asrcl);
	SetGroups(i, Agrpl);
	return;
}
//
//
// This function manages saving channel strips that will be modified by
// being assigned new channel strips. This is optimized (limiting OSC I/O):
// - save only those channels that will be modified
// - do not save twice the same channel
//
// returns the number of saved channels strips
//
int SaveSet(int max, int insert) {
int i, j, k, nb_assigns;
int Xlock;
char channel[8];
//
	nb_assigns = 0;
	for (i = MinChn - 1; i < max; i++) {
		// Source := i + 1, will be saved if used in specific conditions
		// Sst[i] is set from  user settings
		// we first preserve source data in case it is needed later
		// could be systematically done but we try to avoid some unnecessary
		// X32 UDP communication and save time
		// if the source-strip channel is blank (or 0), it means no change to channel
		k = Sst[i];
		if ((k > 0) && (k != i + 1)) { // save only if there a  need (i.e the channel strip may be modified)
			if (insert) {
				if (Asrc[k - 1] < 0) { // avoid saving the same channel strip several times
					// save is done by saving to libchan preset MaxPre - (Sst[i] - 1)
					// sending a /save ,sisi "libchan" [MaxPre + 1 - k] "ch<Sst[i]>" k - 1
					s_len = Xsprint(s_buf, 0, 's', "/save");
					s_len = Xsprint(s_buf, s_len, 's', ",sisi");
					s_len = Xsprint(s_buf, s_len, 's', "libchan");
					j = MaxPre + 1 - k;
					s_len = Xsprint(s_buf, s_len, 'i', &j);
					channel[0] = 'c';
					channel[1] = 'h';
					j = k / 10;
					channel[2] = 48 + j;
					channel[3] = 48 + k - j * 10;
					channel[4] = 0;
					s_len = Xsprint(s_buf, s_len, 's', channel);
					j = k - 1;
					s_len = Xsprint(s_buf, s_len, 'i', &j);
					SEND_TO(s_buf, s_len)
					Xlock = 1;
					while (Xlock) {
						RPOLL // read the desk for answer to /save
						if (p_status > 0) { // ignore responses that do not correspond to what
							RECV_FR(r_buf, r_len); // is expected: /save~~~,si~libchan~[     1]
							if (strncmp(r_buf, "/save", 5) == 0) {
								for (j = 0; j < 4; j++)
									endian.cc[3 - j] = r_buf[20 + j];
								if (endian.ii != 1) {
									perror ("Error: Could not save channel data\n");
									return 0;
								}
								Xlock = 0;
							} else {
								fprintf (stderr, "Waiting /save..., got %s \n", r_buf);
							}
						}
					}
					// set Asrc/Agrp[k - 1] to the audio-sourc, dca and mute groups used for channel k at index (k - 1)
					Asrc[k - 1] = GetASource(k);
					Agrp[k - 1] = GetGroups(k);
				}
			}
			nb_assigns++; // Up the number of assignments (including duplicates!)
		}
	}
	return nb_assigns;
}
//
//
// Building/assigning a custom set layer to X32
// This is done by using the data saved at a previous stage (with SaveSet(xxx))
// This time, we apply to each channel strip the setting that was saved for it to
// X32 libchan presets.
// After loading the proper preset to X32, the channel strip audio-source is set
// to a value according to Asrc[k - 1]
// This only applies to channel strips that are modified (i.e. those for which
// Sst[i] > 0 and Sst[i] != the destination strip number)
void CustomSet() {
int i, k;
	//
	for (i = MinChn - 1; i < MaxSet; i++) {
		k = Sst[i];
		if ((k > 0) && (k != i + 1)) { // Only if necessary, load from X32's
			//  libchan presets the preset "k" into channel strip index i
			LoadCStrip(k, i);
			// now set audio source of channel i+1
			SetASource(i, Asrc[k - 1]);
			SetGroups(i, Agrp[k - 1]);
		}
	}
	return;
}
//
//
// Insert a channel strip at the only position where (Sst[k] > 0).
// All channel strips right of the insert-selected one will shift right one position.
//
void InsertSet(int set) {
int i, k, strip;
	//
	// Find strip index k where a channel is to be inserted
	strip = -1;
	for (k = MinChn - 1; k < MaxIns; k++) {
		if (Sst[k] >= 0) {
			strip = Sst[k];
			break;
		}
	}
	if (strip > -1) {
		//
		// k represents the insertion point, and the last channel to shift as we'll
		// start from the very last channel to perform right shift operations below
		for (i = MaxIns - 1; i > k; i--) {
			ShiftCStrip(i);
		}
		//
		// Done shifting channels strips,
		if (set) {
			// copy channel 'strip' to position k
			LoadCStrip(strip, k);
			// set audio-source & dca/mute groups of channel k previously saved in Asrc/Agrp
			SetASource(k, Asrc[strip-1]);
			SetGroups(k, Agrp[strip-1]);
		} else {
			//
			// set a new (blank)strip at index k with audio source = strip
			LoadNStrip(k, strip);
			SetGroups(k, 0);
		}
	}
	return;
}
//
// The main program
//
// Reads resource file and set variables accordingly
// registers with wWinMain() to launch the main window, and main loop
//
int main(int argc, char **argv) {
HINSTANCE hPrevInstance = 0;
PWSTR pCmdLine = 0;
int nCmdShow = 0;
int i;
	//
	Xdebug = 0;
	Xsave = 0;
	Xa_saved = 0;
	p_status = 0;
	// Open and read resource file
	if ((res_file = fopen("./.X32CustomLayer.ini", "r")) != NULL) {
		// get and remember real path
		if ((Finiretval = GetFullPathNameA("./.X32CustomLayer.ini", 1024,
				Finipath, FinilppPart)) > 1024) {
			printf("Not enough space for file name\r\n");
			exit (-3);
		}
		// skip header comments lines
		*r_buf = '#';
		while (*r_buf == '#') fgets(r_buf, sizeof(r_buf), res_file);
		// read parameter dataset
		sscanf(r_buf, "%d %d\n", &bWidth, &bHeight);
		wWidth = 16 * bWidth5 + 12;
		wHeight = 6 * (bHeight + 5) + 72;
		fscanf(res_file, "%d %d %d %d\n", &MinChn, &MaxSet, &MaxIns, &MaxPre);
		fgets(Xip_str, sizeof(Xip_str), res_file);
		Xip_str[strlen(Xip_str)] = 0;
		fscanf(res_file, "%d %d\n", &Xdebug, &Xsave);
		fclose(res_file);
	}
	if ((Ch_strings_max != MAXCHN) || (SCh_strings_max != MAXCHN)) {
		printf("Inconsistencies in storage capacity and number of channel node lines\r\n");
		exit (-1);
	}
	if ((Aux_strings_max != MAXAUX) || (SAux_strings_max != MAXAUX)) {
		printf("Inconsistencies in storage capacity and number of auxin node lines\r\n");
		exit (-2);
	}
	for (i = 0; i < MAXCHN; i++) {
		strcpy(Ch_resets[i], Ch_inistr[i]);
		strcpy(SCh_nodes[i], SCh_inistr[i]);
	}
	for (i = 0; i < MAXAUX; i++) {
		strcpy(Aux_resets[i], Aux_inistr[i]);
		strcpy(SAux_nodes[i], SAux_inistr[i]);
	}
	if (Xdebug) {
		ShowWindow(GetConsoleWindow(), SW_SHOW); // show console window
	} else {
		ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	}
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
	exit (0);
}
