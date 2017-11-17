//
// X32SetPreset.c
//
//  Created on: 23 avr. 2015
//
// ©2017 - Patrick-Gilles Maillot
//
// X32SetPreset - a Windows app for loading a Preset to X32 memory from a PC HDD.
//
//              Although this program is essentially a Windows-only application,
//              #ifdefs are kept for the case of a linux/MacOS implementation
//
//	ver. 0.20: Support for FW 3.08
//
//

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>


#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#include <Commdlg.h>
#include <CommCtrl.h>

#else
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#endif


#define BSIZE 			512	// Buffer sizes (enough to take into account FX parameters)
#define LINEFEED		10
#define LENPATH			256 // maximum length for directory names
#define LENFILE			64	// maximum length for file names
#define LENFULLPATH		LENPATH +  LENFILE + 10	//that's 256+64 + a little extra
												//for extension and snippet/scene digits
#define PS_CH			1
#define	PS_FX			2
#define	PS_RO			4

#define PS_HA			1
#define PS_CF			2
#define PS_GA			4
#define PS_DY			8
#define PS_EQ			16
#define PS_SN			32

// Private functions
void	LaunchPresetLoad();		// return status is in PSstatus
int		X32_Connect();
int		validateIP4Dotted(const char *s);
void 	XRcvClean();
void 	Xlogf(char *header, char *buf, int len);
//
// External calls used
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int Xfprint(char *bd, int index, char* text, char format, void *bs);
extern int SetSceneParse(char *l_read);

//

WINBASEAPI HWND WINAPI	GetConsoleWindow(VOID);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE	hInstance = 0;
HWND 		hwnd, hwndipaddr, hwndconx, hwndGO, hwndprog;
HWND		hwndset2, hwndverb, hwndlevl, hwndbrowse, hwndcombo;
HWND		hwndFHA, hwndFCF, hwndFGA, hwndFEQ, hwndFDY, hwndFSN;
HPEN		hpenGreen;			// Green color - 1 pix wide
HPEN		hpenGray1;			// Light Gray - 1 pix wide
HPEN		hpenBlack;			// Black - 1 pix wide
HPEN		hpen;				// Current pen
HFONT		hfont, htmp;
HDC			hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP		hBmp;
BITMAP		bmp;
MSG			wMsg;

OPENFILENAME ofn;       			// common dialog box structure
HANDLE hf;              			// file handle

FILE	*log_file;
FILE	*Xin, *Fold;
int		delta_scene;
char	Finipath[1024];	// resolved path to .ini file
char	**FinilppPart;
int 	Finiretval;

int		wWidth = 550;
int		wHeight = 200;

int		zero = 0;
int		one = 1;
int		ninetynine = 99;

int		SetPreset_request = 0;
int		Send2slot = 0;
//
// Windows UI related storage
wchar_t	W32_ip_str[20], W32_set_to[8], W32_PresetFileName[LENFILE];
char	Xip_str[20], Xset_to[8], XPresetFileName[LENFILE];
char	Xlogpath[LENPATH], X32PreFilePath[LENPATH];
char	Xprg_str[32];
char	Xcomplete[] = "Complete";
char	Xerror[]    = "--Error--";
char	PSBuffer[BSIZE];
//
// Misc. flags
int		Xconnected = 0;
int		X32SHOW = 1;		// this flags reorients verbose mode to log file rather than stdout
int		X32PRESET = 1;		// this flags reorients /ch/xx/config to not consider source
int		Xdelay = 1;			// typically not needed or low value for presets
int		Xdebug = 0;
int		Xverbose = 0; 		// Verbose
int		PSstatus = 0;		// Return status (1 = error) after reading preset file
int		PSFSafes = 0;		// Channel Preset safes
int		X32VER = 212;		// Server version - 208, 210, 212...
int		Xmlev = 0;			// Master Level safe flag
int		Xlock = 0;
int		keep_running = 1;
int		PsetType = 0;		// Type of Preset. 0-unknown, 1:Channel
int		PSFlags = 0;
int		fx[8]; // saves the FX current type for each of the fx slots
//
enum PSetType {UNKNOWN = 0, CHANNEL, EFFECT, ROUTING};
//
char *EFType[] = {	"Hall Reverb",
					"Chamber Reverb",
					"Room Reverb",
					"Rich Plate Reverb",
					"Plate Reverb",
					"Ambiance",
					"Gated Reverb",
					"Reverse Reverb",
					"Vintage room",
					"Vintage Reverb",
					"Stereo Chorus",
					"Stereo Flanger",
					"Stereo Pitch",
					"Dual Pitch Shifter",
					"Chorus+Chamber",
					"Flanger+Chamber",
					"Delay+Chamber",
					"Delay+Chorus",
					"Delay+Flanger",
					"Modulation Delay",
					"Stereo Delay",
					"3-Tap Delay",
					"Rhythm Delay",
					"Stereo Graphic EQ",
					"Dual Graphic EQ",
					"Dual TrueEQ",
					"Strereo TrueEQ",
					"Stereo Phaser",
					"Rotary Speaker",
					"Wave Designer",
					"Precision Limiter",
					"Stereo Enhancer",
					"Dual Enhancer",
					"Stereo Exciter",
					"Dual Exciter",
					"Stereo Guitar Amp",
					"Dual Guitar Amp",
					"Stereo Tube Stage",
					"Dual Tube Stage",
					"Stereo Imager",
					"Tremolo/Panner",
					"Mood Filter",
					"Stereo DeEsser",
					"Dual DeEsser",
					"Stereo Xtec EQ1",
					"Dual Xtec EQ1",
					"Stereo Xtec EQ5",
					"Dual Xtec EQ5",
					"Fair Comp",
					"M/S Fair Comp",
					"Dual Fair Comp",
					"Leisure Comp",
					"Dual Leisure Comp",
					"Ultimo Comp",
					"Dual Ultimo Comp",
					"Sound Maxer",
					"Edison EX1",
					"Suboctaver",
					"Dimension-C",
					"Combinator",
					"Dual Combinator",
};
//
// X32 communication buffers
char				r_buf[BSIZE], s_buf[BSIZE];
int					r_len, s_len;
int					p_status;
//
// UDP related data
int					Xfd;	 // X32 socket
struct sockaddr_in	Xip;
struct sockaddr		*Xip_pt = (struct sockaddr*)&Xip;
char				Xport_str[8];
struct timeval		timeout;
fd_set 				ufds;
#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
unsigned long 		mode;
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses/WIN
#endif
//
// resource file data
FILE 				*res_file;
int					r_status;
//
// type cast union
union littlebig {
	char	cc[4];
	int		ii;
	float	ff;
} endian;
//
//
#define WIN_LOOP											\
	do {													\
		while(PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {	\
			TranslateMessage(&wMsg);						\
			DispatchMessage(&wMsg);							\
		}													\
	} while (0);
//
// X32 Communication macros
//
#define SEND_TO(b, l)												\
	do {															\
		if (Xverbose) Xlogf("->X", b, l);							\
		if (sendto(Xfd, b, l, 0, Xip_pt, Xip_len) < 0) {			\
			fprintf (log_file, "Coundn't send data to X32\r\n");	\
			exit(EXIT_FAILURE);										\
		} 															\
	} while (0);
//
#define RECV_FR(b, l)											\
	do {														\
		if ((l = recvfrom(Xfd, b, BSIZE, 0, 0, 0)) > 0) {		\
			if (Xverbose) Xlogf("X->", b, l);					\
		} 														\
	} while (0);
//
#define RPOLL													\
	do {														\
		FD_ZERO (&ufds);										\
		FD_SET (Xfd, &ufds);									\
		p_status = select(Xfd+1,&ufds,NULL,NULL,&timeout);		\
	} while (0);
//
//
#ifdef __WIN32__
#define MILLISLEEP(t)											\
	do {														\
		Sleep(t);												\
	} while (0);
#else
#define MILLISLEEP(t)											\
	do {														\
		usleep(t*1000);											\
	} while (0);
#endif
//
//
//
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32SetPreset";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	hpenGreen = CreatePen(PS_SOLID, 1, RGB(0, 200, 20));
	hpenGray1 = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
	hpenBlack = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"X32SetPreset - Load Channel, Effects or Routing Preset to X32",
			WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU,
			110, 110, wWidth, wHeight, 0, 0, hInstance, 0);
//
	while (keep_running) {
		if(PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		} else {
			if (SetPreset_request) {
				XRcvClean();
				LaunchPresetLoad();
				if (PSstatus) {
					SetWindowText(hwndprog, (LPSTR)Xerror);
					fprintf (log_file, "!! Done loading Preset, with errors or warning\r\n");
				} else {
					SetWindowText(hwndprog, (LPSTR)Xcomplete);
					fprintf (log_file, "Done loading Preset!\r\n");
				}
				XRcvClean();
				if (log_file) fflush(log_file);
				SetPreset_request = 0;
			} else {
				Sleep (100);
			}
		}
	}
	return (int) wMsg.wParam;
}
//
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

int 	i;
int		k;
char 	str1[LENFILE];
//
//
	switch (msg) {
	case WM_CREATE:
//
		hwndipaddr = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				125, 35, 170, 20, hwnd, (HMENU)0, NULL, NULL);
		hwndconx = CreateWindowW(L"button", L"Connect", WS_VISIBLE | WS_CHILD,
				305, 35, 75, 20, hwnd, (HMENU)1, NULL, NULL);
//
		hwndlevl = CreateWindowW(L"button", NULL, BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
			5, 135, 15, 20, hwnd, (HMENU)2, NULL, NULL);
		hwndverb = CreateWindowW(L"button", L"", BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
			320, 135, 15, 20, hwnd, (HMENU)3, NULL, NULL);
//
		hwndFHA = CreateWindowW(L"button", NULL, BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
				165, 107, 15, 20, hwnd, (HMENU)10, NULL, NULL);
		hwndFCF = CreateWindowW(L"button", NULL, BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
				225, 107, 15, 20, hwnd, (HMENU)11, NULL, NULL);
		hwndFGA = CreateWindowW(L"button", NULL, BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
				285, 107, 15, 20, hwnd, (HMENU)12, NULL, NULL);
		hwndFDY = CreateWindowW(L"button", NULL, BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
				405, 107, 15, 20, hwnd, (HMENU)13, NULL, NULL);
		hwndFEQ = CreateWindowW(L"button", NULL, BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
				345, 107, 15, 20, hwnd, (HMENU)14, NULL, NULL);
		hwndFSN = CreateWindowW(L"button", NULL, BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
				465, 107, 15, 20, hwnd, (HMENU)15, NULL, NULL);
//
		hwndbrowse = CreateWindowW(L"button", L"Browse", WS_VISIBLE | WS_CHILD,
				305, 75, 75, 20, hwnd, (HMENU)8, NULL, NULL);
//
        hwndcombo = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
        		435, 5, 95, 140, hwnd, (HMENU)7, NULL, NULL);
        // Load dropdown item list
        char tmpstr[8];
        for (k = 1; k < 9; k++) {
        	sprintf(tmpstr, "Ch%02d/Fx%1d", k, k);
        	SendMessage(hwndcombo, CB_ADDSTRING,(WPARAM) 0,(LPARAM)tmpstr);
        }
        for (k = 9; k < 33; k++) {
        	sprintf(tmpstr, "Ch%02d", k);
        	SendMessage(hwndcombo, CB_ADDSTRING,(WPARAM) 0,(LPARAM)tmpstr);
        }
        for (k = 1; k < 9; k++) {
        	sprintf(tmpstr, "Aux%02d", k);
        	SendMessage(hwndcombo, CB_ADDSTRING,(WPARAM) 0,(LPARAM)tmpstr);
        }
        for (k = 1; k < 5; k++) {
        	sprintf(tmpstr, "Fx%dL", k);
        	SendMessage(hwndcombo, CB_ADDSTRING,(WPARAM) 0,(LPARAM)tmpstr);
        	sprintf(tmpstr, "Fx%dR", k);
        	SendMessage(hwndcombo, CB_ADDSTRING,(WPARAM) 0,(LPARAM)tmpstr);
        }
        for (k = 1; k < 17; k++) {
        	sprintf(tmpstr, "Bus%02d", k);
        	SendMessage(hwndcombo, CB_ADDSTRING,(WPARAM) 0,(LPARAM)tmpstr);
        }
        for (k = 1; k < 7; k++) {
        	sprintf(tmpstr, "Mtx%02d", k);
        	SendMessage(hwndcombo, CB_ADDSTRING,(WPARAM) 0,(LPARAM)tmpstr);
        }
    	SendMessage(hwndcombo, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"L/R");
    	SendMessage(hwndcombo, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"M/C");
	    // Send CB_SETCURSEL message to display initial item 0 / Ch01
	    // in the selection field
	    SendMessage(hwndcombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
//
		hwndGO = CreateWindowW(L"button", L"Set Preset", WS_VISIBLE | WS_CHILD,
				395, 30, 135, 43, hwnd,
							(HMENU)9, NULL, NULL);
		hwndprog = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
						395, 75, 135, 20, hwnd, (HMENU)0, NULL, NULL);
//
		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./.X32SetPreset.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_SHARED);
		if(hBmp==NULL) {
			perror("Pixel bitmap file no found");
		}
		break;
//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 5, 2, 115, 100, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
//
		SetBkMode(hdc, TRANSPARENT);
		MoveToEx(hdc, 390, 95, NULL);
		LineTo(hdc, 390, 2);
//
		MoveToEx(hdc, 5, 100, NULL);
		LineTo(hdc, wWidth - 10, 100);
		MoveToEx(hdc, 5, 130, NULL);
		LineTo(hdc, wWidth - 10, 130);
//
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 150, 20, str1, wsprintf(str1, "Enter X32 IP below:"));
		if (PsetType == UNKNOWN)
					TextOut(hdc, 150, 60, str1, wsprintf(str1, "Select file for Preview"));else if (PsetType == CHANNEL)
			TextOut(hdc, 150, 60, str1, wsprintf(str1, "Channel Preset Preview:"));
		else if (PsetType == EFFECT)
			TextOut(hdc, 150, 60, str1, wsprintf(str1, "Effect Preset Type:"));
		TextOut(hdc, 335, 18, str1, wsprintf(str1, "ver. 0.20"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 0, str1, wsprintf(str1, "X32SetPreset - ©2015 - Patrick-Gilles Maillot"));
		TextOut(hdc, 205, 137, str1, wsprintf(str1, "- OPTIONS -"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
//		TextOut(hdc, 395, 10, str1, wsprintf(str1, "Destination:"));
		TextOut(hdc, 395, 10, str1, wsprintf(str1, "CH/FX:"));
		if (PsetType == CHANNEL) {
			TextOut(hdc, 10, 109, str1, wsprintf(str1, "Channel Preset Safes ->"));
			TextOut(hdc, 180, 109, str1, wsprintf(str1, "HA"));
			TextOut(hdc, 240, 109, str1, wsprintf(str1, "Conf"));
			TextOut(hdc, 300, 109, str1, wsprintf(str1, "Gate"));
			TextOut(hdc, 360, 109, str1, wsprintf(str1, "EQ"));
			TextOut(hdc, 420, 109, str1, wsprintf(str1, "Dyn"));
			TextOut(hdc, 480, 109, str1, wsprintf(str1, "SND"));
		}
//
		TextOut(hdc, 25, 137, str1, wsprintf(str1, "Master Levels Safe (M & ST)"));
		TextOut(hdc, 340, 137, str1, wsprintf(str1, "Verbose mode"));
		if (PsetType == CHANNEL) {
			hpen = hpenGray1;
			SelectObject(hdc, hpen);
			MoveToEx(hdc, 125 , 85, NULL);
			LineTo(hdc, 295, 85);
			if (PSFlags & 0x0100) {
				if (PSFlags & 0x00001) hpen = hpenGreen;
				else                   hpen = hpenBlack;
				SelectObject(hdc, hpen);
				Rectangle(hdc, 132, 75, 152, 95);
				MoveToEx(hdc, 149, 85, NULL);
				LineTo(hdc, 135, 92);
				LineTo(hdc, 135, 78);
				LineTo(hdc, 149, 85);
			}
			if (PSFlags & 0x0200) {
				if (PSFlags & 0x0002) hpen = hpenGreen;
				else                  hpen = hpenBlack;
				SelectObject(hdc, hpen);
				Rectangle(hdc, 157, 75, 177, 95);
				MoveToEx(hdc, 160, 92, NULL);
				LineTo(hdc, 168, 92);
				LineTo(hdc, 164, 80);
				LineTo(hdc, 160, 92);
//
				MoveToEx(hdc, 170, 80, NULL);
				LineTo(hdc, 170, 90);
				LineTo(hdc, 172, 92);
				LineTo(hdc, 175, 92);
				MoveToEx(hdc, 169, 84, NULL);
				LineTo(hdc, 172, 84);
			}
			if (PSFlags & 0x0400) {
				if (PSFlags & 0x0004) hpen = hpenGreen;
				else                  hpen = hpenBlack;
				SelectObject(hdc, hpen);
				Rectangle(hdc, 182, 75, 202, 95);
				MoveToEx(hdc, 185, 92, NULL);
				LineTo(hdc, 188, 85);
				LineTo(hdc, 199, 85);
			}
			if (PSFlags & 0x0800) {
				if (PSFlags & 0x0008) hpen = hpenGreen;
				else                 hpen = hpenBlack;
				SelectObject(hdc, hpen);
				Rectangle(hdc, 207, 75, 227, 95);
				MoveToEx(hdc, 217, 92, NULL);
				LineTo(hdc, 217, 81);
				LineTo(hdc, 224, 78);
			}
			if (PSFlags & 0x1000) {
				if (PSFlags & 0x0010) hpen = hpenGreen;
				else                  hpen = hpenBlack;
				SelectObject(hdc, hpen);
				Rectangle(hdc, 232, 75, 263, 95);
				MoveToEx(hdc, 235, 92, NULL);
				LineTo(hdc, 238, 80);
				LineTo(hdc, 241, 78);
				LineTo(hdc, 244, 80);
				LineTo(hdc, 248, 86);
				LineTo(hdc, 251, 86);
				LineTo(hdc, 254, 83);
				LineTo(hdc, 256, 84);
				LineTo(hdc, 257, 89);
			}
			if (PSFlags & 0x2000) {
				if (PSFlags & 0x0020) hpen = hpenGreen;
				else                  hpen = hpenBlack;
				SelectObject(hdc, hpen);
				Rectangle(hdc, 268, 75, 288, 95);
				MoveToEx(hdc, 272, 92, NULL);
				LineTo(hdc, 275, 84);
				LineTo(hdc, 278, 80);
				LineTo(hdc, 284, 78);
			}
		} else if (PsetType == EFFECT) {
			TextOut(hdc, 135, 78, str1, wsprintf(str1, "Effect type is: %s", EFType[PSFlags]));
//
		} else if (PsetType == ROUTING) {
			TextOut(hdc, 135, 78, str1, wsprintf(str1, "Routing Preset"));
		}
//
		DeleteObject(htmp);
		DeleteObject(hfont);
		EndPaint(hwnd, &ps);
//
		SetWindowText(hwndipaddr, (LPSTR)Xip_str);
		if (Xverbose)	SendMessage(hwndverb, BM_SETCHECK, BST_CHECKED, 0);
		else			SendMessage(hwndverb, BM_SETCHECK, BST_UNCHECKED, 0);
		if (Xmlev) 		SendMessage(hwndlevl, BM_SETCHECK, BST_CHECKED, 0);
		else			SendMessage(hwndlevl, BM_SETCHECK, BST_UNCHECKED, 0);
		if (PSFSafes & PS_HA) SendMessage(hwndFHA, BM_SETCHECK, BST_CHECKED, 0);
		else                  SendMessage(hwndFHA, BM_SETCHECK, BST_UNCHECKED, 0);
		if (PSFSafes & PS_CF) SendMessage(hwndFCF, BM_SETCHECK, BST_CHECKED, 0);
		else                  SendMessage(hwndFCF, BM_SETCHECK, BST_UNCHECKED, 0);
		if (PSFSafes & PS_GA) SendMessage(hwndFGA, BM_SETCHECK, BST_CHECKED, 0);
		else                  SendMessage(hwndFGA, BM_SETCHECK, BST_UNCHECKED, 0);
		if (PSFSafes & PS_DY) SendMessage(hwndFDY, BM_SETCHECK, BST_CHECKED, 0);
		else                  SendMessage(hwndFDY, BM_SETCHECK, BST_UNCHECKED, 0);
		if (PSFSafes & PS_EQ) SendMessage(hwndFEQ, BM_SETCHECK, BST_CHECKED, 0);
		else                  SendMessage(hwndFEQ, BM_SETCHECK, BST_UNCHECKED, 0);
		if (PSFSafes & PS_SN) SendMessage(hwndFSN, BM_SETCHECK, BST_CHECKED, 0);
		else                  SendMessage(hwndFSN, BM_SETCHECK, BST_UNCHECKED, 0);
		break;
//
	case WM_COMMAND:
		if(HIWORD(wParam) == CBN_DROPDOWN) {	// user action = dropdown
			SendMessage((HWND)lParam, CB_SHOWDROPDOWN, (WPARAM) 0, (LPARAM) 0);
//
//
//		// If the user makes a selection from the list:
//		//   Send CB_GETCURSEL message to get the index of the selected list item.
//		//   Send CB_GETLBTEXT message to get the item.
//		//   Display the item in a messagebox.
//
//			int ItemIndex = SendMessage((HWND) lParam, CB_GETCURSEL,
//				(WPARAM) 0, (LPARAM) 0);
//
//			TCHAR  ListItem[256];
//			SendMessage((HWND) lParam, CB_GETLBTEXT,
//				(WPARAM) ItemIndex, (LPARAM) ListItem);
//			MessageBox(hwnd, (LPCWSTR) ListItem, TEXT("Item Selected"), MB_OK);
		} else if (HIWORD(wParam) == BN_CLICKED) {	// user action = btn click
			i = LOWORD(wParam);
			switch(i) {
			case 1:
				i = GetWindowTextLengthW(hwndipaddr) + 1;
				GetWindowTextW(hwndipaddr, W32_ip_str, i);
				WideCharToMultiByte(CP_ACP, 0, W32_ip_str, i, Xip_str, i, NULL, NULL);
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32_Connect();
					if (Xconnected)	SetWindowTextW(hwndconx, L"Connected");
					else			SetWindowTextW(hwndconx, L"Connect");
				} else {
					fprintf (log_file, "Incorrect IP string form\r\n");
				}
				break;
			case 2:
				if (Xmlev)	SendMessage(hwndlevl, BM_SETCHECK, BST_UNCHECKED, 0);
				else		SendMessage(hwndlevl, BM_SETCHECK, BST_CHECKED, 0);
				Xmlev ^= 1;
				break;
			case 3:
				if (Xverbose)SendMessage(hwndverb, BM_SETCHECK, BST_UNCHECKED, 0);
				else		SendMessage(hwndverb, BM_SETCHECK, BST_CHECKED, 0);
				Xverbose ^= 1;
				break;
			case 7:
				break;
			case 8:
				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = X32PreFilePath;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(X32PreFilePath);
				ofn.lpstrFilter = "Channel Presets\0*.chn\0Effect Presets\0*.efx\0Routing Presets\0*.rou\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = XPresetFileName;
				ofn.nMaxFileTitle = sizeof(XPresetFileName);;
				ofn.lpstrTitle = (LPCTSTR)"Select a Preset File from the list below\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				//
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
					i = 0;
					while ((XPresetFileName[i] != '.') && (XPresetFileName[i] != 0)) ++i;
					XPresetFileName[i] = 0;
					SetWindowText(hwndprog, (LPSTR)XPresetFileName);
					if (XPresetFileName[i + 1] == 'c') {
						// read file header
						PSFlags = 0;
						if ((Xin = fopen(X32PreFilePath, "rb")) != 0) {
							if (fgets(r_buf, BSIZE, Xin) != 0) {
								PsetType = CHANNEL;		// Display preset preview data
								InvalidateRect(hwnd, NULL, TRUE);
								i = 0;
								while ((r_buf[i] != '%') && (r_buf[i] != '\n')) i++;
								i += 1;
								for (k = 0; k < 16; k++, i++) {
									PSFlags = PSFlags << 1;
									if (r_buf[i] == '1') PSFlags |= 1;
								}
							}
						}
						if (Xin) fclose (Xin);
					} else if (XPresetFileName[i + 1] == 'e') {
						// read file header
						PSFlags = 0;
						if ((Xin = fopen(X32PreFilePath, "rb")) != 0) {
							if (fgets(r_buf, BSIZE, Xin) != 0) {
								PsetType = EFFECT;		// Display preset preview data
								InvalidateRect(hwnd, NULL, TRUE);
								i = 0;
								while ((r_buf[i] != '"') && (r_buf[i] != '\n')) i++;
								i += 1;
								while ((r_buf[i] != '"') && (r_buf[i] != '\n')) i++; // skipped name
								i += 1;
								sscanf(r_buf + i, "%d %d", &i, &PSFlags);
							}
						}
						if (Xin) fclose (Xin);
					} else if (XPresetFileName[i + 1] == 'r') {
						PsetType = ROUTING;		// No preview data to display
						InvalidateRect(hwnd, NULL, TRUE);
						PSFlags = 0;
					} else {
						PsetType = UNKNOWN;		// No preview data to display
						PSFlags = 0;
					}
				}
				break;
			case 9:
				// get dropdow current item index [0..71]
				Send2slot = SendMessage(hwndcombo, CB_GETCURSEL,(WPARAM) 0, (LPARAM) 0);
				// SetPreset request
				SetPreset_request = 1;
				break;
			case 10:
				if (PSFSafes & PS_HA) SendMessage(hwndFHA, BM_SETCHECK, BST_UNCHECKED, 0);
				else                  SendMessage(hwndFHA, BM_SETCHECK, BST_CHECKED, 0);
				PSFSafes ^= PS_HA;
				break;
			case 11:
				if (PSFSafes & PS_CF) SendMessage(hwndFCF, BM_SETCHECK, BST_UNCHECKED, 0);
				else                  SendMessage(hwndFCF, BM_SETCHECK, BST_CHECKED, 0);
				PSFSafes ^= PS_CF;
				break;
			case 12:
				if (PSFSafes & PS_GA) SendMessage(hwndFGA, BM_SETCHECK, BST_UNCHECKED, 0);
				else                  SendMessage(hwndFGA, BM_SETCHECK, BST_CHECKED, 0);
				PSFSafes ^= PS_GA;
				break;
			case 13:
				if (PSFSafes & PS_DY) SendMessage(hwndFDY, BM_SETCHECK, BST_UNCHECKED, 0);
				else                  SendMessage(hwndFDY, BM_SETCHECK, BST_CHECKED, 0);
				PSFSafes ^= PS_DY;
				break;
			case 14:
				if (PSFSafes & PS_EQ) SendMessage(hwndFEQ, BM_SETCHECK, BST_UNCHECKED, 0);
				else                  SendMessage(hwndFEQ, BM_SETCHECK, BST_CHECKED, 0);
				PSFSafes ^= PS_EQ;
				break;
			case 15:
				if (PSFSafes & PS_SN) SendMessage(hwndFSN, BM_SETCHECK, BST_UNCHECKED, 0);
				else                  SendMessage(hwndFSN, BM_SETCHECK, BST_CHECKED, 0);
				PSFSafes ^= PS_SN;
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		if (Xconnected) close (Xfd);
		// read panel data
		i = GetWindowTextLengthW(hwndipaddr) + 1;
		GetWindowTextW(hwndipaddr, W32_ip_str, i);
		WideCharToMultiByte(CP_ACP, 0, W32_ip_str, i, Xip_str, i, NULL, NULL);
//
		res_file = fopen(Finipath, "wb");
		fprintf(res_file, "%d %d %d %d %d\n",
				wWidth, wHeight, Xverbose, Xmlev, PSFSafes);
		fprintf(res_file, "%s\n", Xip_str);
		fprintf(res_file, "%s\n", Xlogpath);
		fclose (res_file);
		if (log_file) fclose(log_file);
//
		keep_running = 0;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
//
// Private functions:
//
//
int X32_Connect() {
	int i;
//
	p_status = 0;
//
	if (Xconnected) {
		close (Xfd);
		return 0;
	} else {
//
// initialize communication with X32 server at IP ip and PORT port
		p_status = GetWindowTextLengthW(hwndipaddr) + 1;
		GetWindowTextW(hwndipaddr, W32_ip_str, p_status);
		WideCharToMultiByte(CP_ACP, 0, W32_ip_str, p_status, Xip_str, p_status, NULL, NULL);
//
		if (validateIP4Dotted(Xip_str) == 0) {
			fprintf (log_file, "Incorrect IP address\r\n");
			return 0;
		}
//	Xport_str[] = "10023" - change to use a different port
		strcpy (Xport_str, "10023");
#ifdef __WIN32__
//Initialize winsock
		if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
			fprintf (log_file, "WSA Startup Error\r\n");
			exit(EXIT_FAILURE);
		}
#endif
		memset(&Xip, 0, sizeof(Xip));				// Clear struct
		Xip.sin_family = AF_INET;					// Internet/IP
		Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
		Xip.sin_port = htons(atoi(Xport_str));	// server port
//
// Load the X32 address we connect to; we're a client to X32, keep it simple.
		// Create the UDP socket
		if ((Xfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
			fprintf (log_file, "X32 socket creation error\r\n");
			return 0; // Make sure we don't considered being connected
		} else {
			// Construct the server sockaddr_in structure
			memset(&Xip, 0, sizeof(Xip));				// Clear struct
			Xip.sin_family = AF_INET;					// Internet/IP
			Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
			Xip.sin_port = htons(atoi(Xport_str));	// server port
// Non blocking mode
			timeout.tv_sec = 0;
			timeout.tv_usec = 500000; //Set timeout for non blocking recvfrom(): 500ms
			s_len = Xsprint(s_buf, 0, 's', "/info");
			if (Xverbose) Xlogf("->X", s_buf, s_len);
			if (sendto(Xfd, s_buf, s_len, 0, (struct sockaddr *)&Xip, Xip_len) < 0) {
				fprintf (log_file, "Error sending data\r\n");
				return 0; // Make sure we don't considered being connected
			} else {
				RPOLL;		// poll for data in
				if (p_status < 0) {
					fprintf (log_file, "Polling for data failed\r\n");
					return 0; // Make sure we don't considered being connected
				} else if (p_status > 0) {
				// We have received data - process it!
					r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);
					if (Xverbose) Xlogf("X->", r_buf, r_len);
					if (strcmp(r_buf, "/info") != 0) {
						fprintf (log_file, "Unexpected answer from X32\r\n");
						return 0;
					}
				} else {
				// time out... Not connected or Not an X32
					fprintf (log_file, "X32 reception timeout\r\n");
					return 0; // Make sure we don't considered being connected
				}
			}
		}
	}
	// set X32VER flag
	i = 16;
	while (r_buf[i] != 0) ++i; // skip 1st string
	while (r_buf[i] == 0) ++i; // skip possible nulls
	while (r_buf[i] != 0) ++i; // skip 2nd string
	while (r_buf[i] == 0) ++i; // skip possible nulls
	while (r_buf[i] != 0) ++i; // skip 3rd string
	while (r_buf[i] == 0) ++i; // skip possible nulls
//
	X32VER = 212; // 2.12
	if (strcmp(r_buf + i, "2.08") == 0) X32VER = 208; // 2.08
	if (strcmp(r_buf + i, "2.10") == 0) X32VER = 210; // 2.10
	fprintf (log_file, "X32 is running FW %s\r\n", r_buf + i);
	timeout.tv_sec = 0;
	timeout.tv_usec = 50000; //Set timeout for next calls: 50ms
	return 1; // We are connected !
}
//
// Validate IP string function
//
int validateIP4Dotted(const char *s)
{
int i;
char tail[16];
unsigned int d[4];

    i = strlen(s);
    if (i < 7 || i > 15) return 0;
    tail[0] = 0;
    int c = sscanf(s, "%3u.%3u.%3u.%3u%s", &d[0], &d[1], &d[2], &d[3], tail);
    if (c != 4 || tail[0]) return 0;
    for (i = 0; i < 4; i++)
        if (d[i] > 255)
            return 0;
    return 1;
}
//
// Empty any pending message from X32 function
//
void XRcvClean() {
//
	if (Xverbose) fprintf (log_file, "X32 receive buffer cleanup if needed\r\n");
	do {
		RPOLL;
		if (p_status > 0) {
			RECV_FR(r_buf, r_len);
		}
	} while (p_status > 0);
	return;
}
//
//
//
void Xlogf(char *header, char *buf, int len)
{
	int i, k, n, j, l, comma = 0, data = 0, dtc = 0;
	unsigned char c;

	fprintf (log_file, "%s, %4d B: ", header, len);
	for (i = 0; i < len; i++) {
		c = (unsigned char)buf[i];
		if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
		fprintf (log_file, "%c", c);
		if (c == ',') {
			comma = i;
			dtc = 1;
		}
		if (dtc && (buf[i] == 0)) {
			data = (i + 4) & ~3;
			for (dtc = i + 1; dtc < data ; dtc++) {
				if (dtc < len) {
					fprintf (log_file, "~");
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
								fprintf (log_file, "%c", c);
						}
					}
					break;
				case 'i':
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					fprintf (log_file, "[%6d]", endian.ii);
					break;
				case 'f':
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					if (endian.ff < 10.) fprintf (log_file, "[%06.4f]", endian.ff);
					else if (endian.ff < 100.) fprintf (log_file, "[%06.3f]", endian.ff);
					else if (endian.ff < 1000.) fprintf (log_file, "[%06.2f]", endian.ff);
					else if (endian.ff < 10000.) fprintf (log_file, "[%06.1f]", endian.ff);
					break;
				case 'b':
					// Get the number of bytes
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					n = endian.ii;
					// Get the number of data (floats or ints ???) in little-endian format
					for (k = 0; k < 4; endian.cc[k++] = buf[data++]);
					if (n == endian.ii) {
						// Display blob as string
						fprintf (log_file, "%d chrs: ", n);
						for (j = 0; j < n; j++) fprintf (log_file, "%c ", buf[data++]);
					} else {
						// Display blob as floats
						n = endian.ii;
						fprintf (log_file, "%d flts: ", n);
						for (j = 0; j < n; j++) {
							//floats are little-endian format
							for (k = 0; k < 4; endian.cc[k++] = buf[data++]);
							fprintf (log_file, "%06.2f ", endian.ff);
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
	fprintf (log_file, "\r\n");
}
//
int
main(int argc, char **argv)
{
	HINSTANCE	hPrevInstance = 0;
	PWSTR		pCmdLine = 0;
	int			nCmdShow = 0;
//
	Xverbose = Xmlev = PSFSafes = 0;
//
	strcpy(Xip_str, "");
	strcpy(XPresetFileName, ".");
	strcpy(Xlogpath, ".");
	strcpy(X32PreFilePath, ".");
	// load resource file
	if ((res_file = fopen("./.X32SetPreset.ini", "r")) != NULL) {
		// get and remember real path
		if((Finiretval = GetFullPathNameA("./.X32SetPreset.ini", 1024, Finipath, FinilppPart)) > 1024){
			printf ("Not enough space for file name\n");
		}
		fscanf(res_file, "%d %d %d %d %d\n",
				&wWidth, &wHeight, &Xverbose, &Xmlev, &PSFSafes);
		fgets(Xip_str, sizeof(Xip_str), res_file);
		Xip_str[strlen(Xip_str) - 1] = 0;
		fgets(Xlogpath, sizeof(Xlogpath), res_file);
		Xlogpath[strlen(Xlogpath) - 1] = 0;
		XPresetFileName[0] = 0;
//
		fclose (res_file);
	}
#ifdef __WIN32__
	if ((p_status = CreateDirectory(Xlogpath, NULL)) == 0) {
		if (ERROR_ALREADY_EXISTS != GetLastError()) {
			printf ("Cannot create directory\n");
			Sleep(10000);
			return (0);
		}
	}
#else
	if ((p_status = mkdir(Xlogpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) != 0) {
		if (p_status != EEXIST) {
			printf ("Cannot create directory\n");
			Sleep(10000);
			return (0);
		}
	}
#endif
	strcpy(r_buf, Xlogpath);
	strcat(r_buf, "/.X32SetPreset.log");
	// create logfile and run program
	if ((log_file = fopen(r_buf, "wb")) != NULL) {
		fprintf(log_file,"*\r\n*\r\n");
		fprintf(log_file,"*    X32SetPreset Log data - ©2017 - Patrick-Gilles Maillot\r\n");
		fprintf(log_file,"*\r\n*\r\n");

		if (Finiretval) fprintf(log_file, ".ini file path: %s\r\n", Finipath);
		else			fprintf(log_file, "couldn't open .X32SetPreset.ini file\r\n");
		ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
		wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
	} else {
		printf ("Cannot create log file\n");
		Sleep(10000);
	}
	if (log_file) fclose(log_file);
	return 0;
}
//
//
void LaunchPresetLoad() {
int	PB_ind;

	if (!Xconnected) {
		fprintf(log_file, "Error: Not connected to X32\r\n");
		PSstatus = 1;
		return;
	}
	PSstatus = 0;
	// check Master Level safes
	if (Xmlev) {
		// set master level to 0. (-oo)
		s_len = Xfprint(s_buf, 0, "/main/st/mix/fader", 'f', &zero);
		SEND_TO(s_buf, s_len)
		s_len = Xfprint(s_buf, 0, "/main/m/mix/fader", 'f', &zero);
		SEND_TO(s_buf, s_len)
	}
	// check Preset type
	switch (PsetType) {
	case CHANNEL:
//		fprintf (log_file, "Send2slot= %d\r\n", Send2slot);
		// prepare buffer with "/ch/xx", "/auxin/xx", "/fxrtn/xx", "/bus/xx",
		// "/mtx/xx", "/main/st", or "/main/m" depending on the value of Send2slot
		if ((Send2slot < 0) || (Send2slot > 71)) {
			fprintf (log_file, "Error - Invalid Destination index for preset: %d\r\n", Send2slot);
			PSstatus = 1;
			return;
		} else if (Send2slot < 32) {
			PB_ind = 6;
			sprintf(PSBuffer, "/ch/%02d", Send2slot + 1);
		} else if (Send2slot < 40) {
			PB_ind = 9;
			sprintf(PSBuffer, "/auxin/%02d", Send2slot - 31);
		} else if (Send2slot < 48) {
			PB_ind = 9;
			sprintf(PSBuffer, "/fxrtn/%02d", Send2slot - 39);
		} else if (Send2slot < 64) {
			PB_ind = 7;
			sprintf(PSBuffer, "/bus/%02d", Send2slot - 47);
		} else if (Send2slot < 70) {
			PB_ind = 7;
			sprintf(PSBuffer, "/mtx/%02d", Send2slot - 63);
		} else if (Send2slot == 70) {
			PB_ind = 8;
			sprintf(PSBuffer, "/main/st");
		} else if (Send2slot == 71) {
			PB_ind = 7;
			sprintf(PSBuffer, "/main/m");
		}
		if ((Xin = fopen(X32PreFilePath, "rb")) == 0) {
			fprintf (log_file, "Cannot open preset file: %s\r\n", X32PreFilePath);
			PSstatus = 1;
			return;
		}
		while (fscanf(Xin, "%s", PSBuffer + PB_ind) != EOF) {
			if (PSBuffer[PB_ind] == '#') {
				if (strcmp(PSBuffer + PB_ind, "#2.7#") == 0) {
					fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
				} else {
					fprintf (log_file, "Only ver. 2.7 files are accepted at this time\r\n");
					PSstatus = 1;
					return;
				}
			} else {
				if (strcmp(PSBuffer + PB_ind, "/config") == 0) {
					if ((PSFSafes & PS_CF) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/config...
					} else {
						fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
					}
				} else if (strcmp(PSBuffer + PB_ind, "/delay") == 0) {
					if ((PSFSafes & PS_CF) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/delay...
					} else {
						fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
					}
				} else if (strcmp(PSBuffer + PB_ind, "/preamp") == 0) {
					if ((PSFSafes & PS_HA) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/preaamp...
					} else {
						fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
					}
				} else if (strncmp(PSBuffer + PB_ind, "/gate", 5) == 0) {
					if ((PSFSafes & PS_GA) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/gate/...
					} else {
						fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
					}
				} else if (strncmp(PSBuffer + PB_ind, "/dyn", 4) == 0) {
					if ((PSFSafes & PS_DY) == 0) {
						PSstatus = SetSceneParse(PSBuffer);  // /ch/xx/dyn/...
					} else {
						fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
					}
				} else if (strncmp(PSBuffer + PB_ind, "/eq", 3) == 0) {
					if ((PSFSafes & PS_EQ) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/eq...
					} else {
						fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
					}
				} else if (strncmp(PSBuffer + PB_ind, "/mix", 4) == 0) {
					if ((PSFSafes & PS_SN) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/mix...
					} else {
						fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
					}
				} else if (strncmp(PSBuffer + PB_ind, "/headamp", 8) == 0) {
					if ((PSFSafes & PS_HA) == 0) {
						// !! adjust headamp channel number section to match the channel,
						// if ch is 01...32. Ignore otherwise
						if (Send2slot < 32) {
							sprintf (PSBuffer + PB_ind + 9, "%03d", Send2slot);
							PSstatus = SetSceneParse(PSBuffer + PB_ind); // /headamp/...
						} else {
							fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
						}
					} else {
						fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
					}
				} else {
					fprintf (log_file, "Warning - %s is not a supported item for Channel preset\r\n", PSBuffer + PB_ind);
					PSstatus = 1;
				}
			}
		}
		break;
	case EFFECT:
		fprintf (log_file, "Send2slot= %d\r\n", Send2slot);
		// prepare buffer with "/fx/xx" depending on the value of Send2slot
		if ((Send2slot < 0) || (Send2slot > 8)) {
			fprintf (log_file, "Error - Invalid Destination index for preset: %d\r\n", Send2slot);
			PSstatus = 1;
			return;
		} else {
			PB_ind = 6;
			sprintf(PSBuffer, "/fx/%1d/", Send2slot + 1);
		}
		if ((Xin = fopen(X32PreFilePath, "rb")) == 0) {
			fprintf (log_file, "Cannot open preset file: %s\r\n", X32PreFilePath);
			PSstatus = 1;
			return;
		}
		while (fscanf(Xin, "%s", PSBuffer + PB_ind) != EOF) {
			if (PSBuffer[PB_ind] == '#') {
				if (strcmp(PSBuffer + PB_ind, "#2.7#") == 0) {
					fgets(PSBuffer + PB_ind, sizeof(PSBuffer), Xin); // ignore rest of the line
				} else {
					fprintf (log_file, "Only ver. 2.7 files are accepted at this time\r\n");
					PSstatus = 1;
					return;
				}
			} else {
				if (strcmp(PSBuffer + PB_ind, "type") == 0) {
					PSstatus = SetSceneParse(PSBuffer); // /fx/x/type...
				} else if (strcmp(PSBuffer + PB_ind, "source") == 0) {
					PSstatus = SetSceneParse(PSBuffer); // /fx/x/source...
				} else if (strcmp(PSBuffer + PB_ind, "par") == 0) {
					PSstatus = SetSceneParse(PSBuffer); // /fx/x/par...
				} else {
					if (*(PSBuffer + PB_ind) != '0') { //ignore trailing zeroes on parameters
						fprintf (log_file, "Warning - %s is not a supported item for Effects preset\r\n", PSBuffer + PB_ind);
						PSstatus = 1;
					}
				}
			}
		}
		break;
	case ROUTING:
		// No need to prepare a buffer header
		if ((Xin = fopen(X32PreFilePath, "rb")) == 0) {
			fprintf (log_file, "Cannot open preset file: %s\r\n", X32PreFilePath);
			PSstatus = 1;
			return;
		}
		while (fscanf(Xin, "%s", PSBuffer) != EOF) {
			if (PSBuffer[0] == '#') {
				fgets(PSBuffer, sizeof(PSBuffer), Xin); // ignore rest of the line
			} else {
				if (strncmp(PSBuffer, "/config",7) == 0) {
					PSstatus = SetSceneParse(PSBuffer); // /config/...
				} else if (strncmp(PSBuffer, "/outputs", 8) == 0) {
					PSstatus = SetSceneParse(PSBuffer); // /outputs/...
				} else {
					fprintf (log_file, "Warning - %s is not a supported item for Routing preset\r\n", PSBuffer);
					PSstatus = 1;
				}
			}
		}
		break;
	default:
		fprintf (log_file, "Error - Unsupported preset type\r\n");
		PSstatus = 1;
		return;
	}
	return;
}
