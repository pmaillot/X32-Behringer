/*
 * X32GetLib.c
 *
 *  Created on: 23 avr. 2015
 *
 * ©2015-17 - Patrick-Gilles Maillot
 *
 * X32GetLib - a windows app for saving a Preset Library, or Several Preset files
 * from X32 memory to a PC HDD.
 *
 * ver. 0.20: Introduced the possibility to save ALL libraries, not just type by type
 * ver. 0.21: fixed logic issue and timing limit to remove -error- signature
 * ver. 0.22: eliminated race condition with sleep 30ms on preset loading in the desk
 * ver. 0.23: preventing window resizing & support for 3.08FW
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <limits.h>
#include <sys/time.h>
#include <time.h>

#include <winsock2.h>
#include <windows.h>
#include <Commdlg.h>
#include <WinError.h>

#define BSIZE 			512	// Buffer sizes (enough to take into account FX parameters)
#define LINEFEED		10
#define LENPATH			256 // maximum length for directory names
#define LENFILE			64	// maximum length for file names
#define LENFULLPATH		LENPATH +  LENFILE + 10	//that's 256+64 + a little extra for extension
#define PS_CH			1
#define	PS_FX			2
#define	PS_RO			4

// Private functions
void	LaunchGetLib();		// return status is in PSstatus
void 	XRcvClean();
void 	Xlogf(char *header, char *buf, int len);
void	Xsave(int Type);
//
// External calls used
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int Xfprint(char *bd, int index, char* text, char format, void *bs);
extern int X32Connect(int Xconnected, char* Xip_str, int btime);
extern int validateIP4Dotted(const char *s);

//
//
char *PComList[] = {"ch/01/config",
					"ch/01/delay",
					"ch/01/preamp",
					"ch/01/gate",
					"ch/01/gate/filter",
					"ch/01/dyn",
					"ch/01/dyn/filter",
					"ch/01/eq",
					"ch/01/eq/1",
					"ch/01/eq/2",
					"ch/01/eq/3",
					"ch/01/eq/4",
					"ch/01/mix",
					"ch/01/mix/01",
					"ch/01/mix/02",
					"ch/01/mix/03",
					"ch/01/mix/04",
					"ch/01/mix/05",
					"ch/01/mix/06",
					"ch/01/mix/07",
					"ch/01/mix/08",
					"ch/01/mix/09",
					"ch/01/mix/10",
					"ch/01/mix/11",
					"ch/01/mix/12",
					"ch/01/mix/13",
					"ch/01/mix/14",
					"ch/01/mix/15",
					"ch/01/mix/16"};
//
#define PCom_max	(sizeof (PComList) / sizeof (char*))
//
char *PCfxList[] = {"fx/1/type", "fx/1/source", "fx/1/par"};
#define PCfx_max	(sizeof (PCfxList) / sizeof (char*))
//
char *PCroList[] = {"config/routing/IN",
					"config/routing/AES50A",
					"config/routing/AES50B",
					"config/routing/CARD",
					"config/routing/OUT",
					"config/routing/PLAY",
					"outputs/main/01",
					"outputs/main/01/delay",
					"outputs/main/02",
					"outputs/main/02/delay",
					"outputs/main/03",
					"outputs/main/03/delay",
					"outputs/main/04",
					"outputs/main/04/delay",
					"outputs/main/05",
					"outputs/main/05/delay",
					"outputs/main/06",
					"outputs/main/06/delay",
					"outputs/main/07",
					"outputs/main/07/delay",
					"outputs/main/08",
					"outputs/main/08/delay",
					"outputs/main/09",
					"outputs/main/09/delay",
					"outputs/main/10",
					"outputs/main/10/delay",
					"outputs/main/11",
					"outputs/main/11/delay",
					"outputs/main/12",
					"outputs/main/12/delay",
					"outputs/main/13",
					"outputs/main/13/delay",
					"outputs/main/14",
					"outputs/main/14/delay",
					"outputs/main/15",
					"outputs/main/15/delay",
					"outputs/main/16",
					"outputs/main/16/delay",
					"outputs/aux/01",
					"outputs/aux/02",
					"outputs/aux/03",
					"outputs/aux/04",
					"outputs/aux/05",
					"outputs/aux/06",
					"outputs/p16/01",
					"outputs/p16/01/iQ",
					"outputs/p16/02",
					"outputs/p16/02/iQ",
					"outputs/p16/03",
					"outputs/p16/03/iQ",
					"outputs/p16/04",
					"outputs/p16/04/iQ",
					"outputs/p16/05",
					"outputs/p16/05/iQ",
					"outputs/p16/06",
					"outputs/p16/06/iQ",
					"outputs/p16/07",
					"outputs/p16/07/iQ",
					"outputs/p16/08",
					"outputs/p16/08/iQ",
					"outputs/p16/09",
					"outputs/p16/09/iQ",
					"outputs/p16/10",
					"outputs/p16/10/iQ",
					"outputs/p16/11",
					"outputs/p16/11/iQ",
					"outputs/p16/12",
					"outputs/p16/12/iQ",
					"outputs/p16/13",
					"outputs/p16/13/iQ",
					"outputs/p16/14",
					"outputs/p16/14/iQ",
					"outputs/p16/15",
					"outputs/p16/15/iQ",
					"outputs/p16/16",
					"outputs/p16/16/iQ",
					"outputs/aes/01",
					"outputs/aes/02"
};
#define PCro_max	(sizeof (PCroList) / sizeof (char*))
//
WINBASEAPI HWND WINAPI	GetConsoleWindow(VOID);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE	hInstance = 0;
HWND 		hwnd, hwndipaddr, hwndconx, hwndGO, hwndprog;
HWND		hwndPtyp, hwndverb, hwnddely, hwndsaveto;

RECT		Rect;
HFONT		hfont, htmp;
HDC			hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP		hBmp;
BITMAP		bmp;
MSG			wMsg;

OPENFILENAME ofn;       			// common dialog box structure
HANDLE hf;              			// file handle

FILE	*log_file;
FILE	*Pfile_pt;
int		delta_scene;
char	Finipath[1024];	// resolved path to .ini file
char	**FinilppPart;
int 	Finiretval;

int		wWidth = 545;
int		wHeight = 135;

int		zero = 0;
int		one = 1;
int		sixtythree = 63;
int		ninetynine = 99;

int		GetLib_request = 0;

//
// Windows UI related storage
char	Xip_str[20],    Xdelay_str[8];
char	Xlogpath[LENPATH], XPresetLibPath[LENPATH], XPresetFile[LENFULLPATH];
char	Pfilename[32];
char	Xprg_str[32];
char	Xcomplete[] = "Complete";
char	Xready[] = "Ready";
char	Xerror[]    = "--Error--";
char	XPname[32];
char	PSBuffer[BSIZE];
char	PFlags[16];
//
// Misc. flags
int		Xconnected = 0;
int		X32SHOW = 1;		// this flags reorients verbose mode to log file rather than stdout
int		X32PRESET = 1;		// this flags reorients /ch/xx/config to not consider source
int		Xdelay = 0;			// typically not needed or low value for presets
int		Xdebug = 0;
int		Xfiles = 0;			// Flag to indicate if files are selected or not
int		Xverbose = 0; 		// Verbose
int		PSstatus = 0;		// Return status (1 = error) after reading preset file
int		Xlock = 0;
int		keep_running = 1;
int		PsetType = 0;		// Type of Preset. 0-ALL, 1-Channel, 2-Effects, 3-Routing
int		Phasdata = 0;
int		PSselidx = 0;
int		fx[8]; // saves the FX current type for each of the fx slots
enum PSetType {ALL = 0, CHANNEL, EFFECTS, ROUTING, LAST};

//
// X32 communication buffers
char				r_buf[BSIZE], s_buf[BSIZE];
int					r_len, s_len;
int					p_status;
//
// UDP related data
WSADATA				wsa;						// Windows Sockets
int 				Xfd;						// X32 socket
struct sockaddr_in	Xip;						// IP structure
struct sockaddr		*Xip_addr = (struct sockaddr*) &Xip;
struct timeval		timeout;					// timeout for non blocking udp comm
fd_set 				ufds;						// file descriptor
int 				Xip_len = sizeof(Xip);		// length of addresses
//
// resource file data
FILE 				*res_file;
int					r_status;
//
//
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
		if (sendto(Xfd, b, l, 0, Xip_addr, Xip_len) < 0) {			\
			fprintf (log_file, "Coundn't send data to X32\r\n");	\
			exit(EXIT_FAILURE);										\
		} 															\
		if (Xdelay > 0) MILLISLEEP(Xdelay);							\
	} while (0);
//
//
#define RECV_FR(b, l)											\
	do {														\
		if ((l = recvfrom(Xfd, b, BSIZE, 0, 0, 0)) > 0) {		\
			if (Xverbose) Xlogf("X->", b, l);					\
		} 														\
	} while (0);
//
//
//
#define RPOLL															\
	do {																\
		FD_ZERO (&ufds);												\
		FD_SET (Xfd, &ufds);											\
		p_status = select(Xfd,&ufds,NULL,NULL,&timeout);				\
	} while (0);

#define MILLISLEEP(t)													\
	do {																\
		Sleep(t);														\
	} while (0);
//
//
//
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32GetLib";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"X32GetLib - Get All Presets from X32 ",
			WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU,
			100, 220, wWidth, wHeight, 0, 0, hInstance, 0);

	while (keep_running) {
		if(PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		} else {
			if (GetLib_request) {
				XRcvClean();
				LaunchGetLib();
				if (PSstatus) {
					SetWindowText(hwndprog, (LPSTR)Xerror);
					fprintf (log_file, "Done transferXPresetFilePath, with errors or warnings\r\n");
				} else {
					SetWindowText(hwndprog, (LPSTR)Xcomplete);
					fprintf (log_file, "Done transfering presets!\r\n");
				}
				XRcvClean();
				if (log_file) fflush(log_file);
				GetLib_request = 0;
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
char 	str1[LENFILE];


	switch (msg) {
	case WM_CREATE:
//
		hwndipaddr = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				125, 40, 170, 20, hwnd, (HMENU)0, NULL, NULL);
		hwndconx = CreateWindowW(L"button", L"Connect", WS_VISIBLE | WS_CHILD,
				305, 40, 75, 20, hwnd, (HMENU)1, NULL, NULL);

		hwnddely = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
						205, 75, 25, 20, hwnd, (HMENU)0, NULL, NULL);
		hwndverb = CreateWindowW(L"button", L"", BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
			125, 75, 15, 20, hwnd, (HMENU)2, NULL, NULL);

		hwndsaveto = CreateWindowW(L"button", L"Save To", WS_VISIBLE | WS_CHILD,
				305, 75, 75, 20, hwnd, (HMENU)8, NULL, NULL);

		hwndPtyp = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				440, 7, 92, 90, hwnd, (HMENU)7, NULL, NULL);
        // Load dropdown item list
        SendMessage(hwndPtyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"ALL");
        SendMessage(hwndPtyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Channel");
        SendMessage(hwndPtyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Effects");
        SendMessage(hwndPtyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Routing");
        // SEt Default value
	    SendMessage(hwndPtyp, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		hwndGO = CreateWindowW(L"button", L"Xfer Presets", WS_VISIBLE | WS_CHILD,
				395, 30, 135, 43, hwnd, (HMENU)9, NULL, NULL);

		hwndprog = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
						395, 75, 135, 20, hwnd, (HMENU)0, NULL, NULL);

		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./.X32GetLib.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_SHARED);
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

		SetBkMode(hdc, TRANSPARENT);

		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 150, 25, str1, wsprintf(str1, "Enter X32 IP below:"));
		TextOut(hdc, 335, 18, str1, wsprintf(str1, "ver. 0.23"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 0, str1, wsprintf(str1, "X32GetLib - ©2015 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);

		hfont = CreateFont(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 395, 10, str1, wsprintf(str1, "Library:"));

		TextOut(hdc, 140, 77, str1, wsprintf(str1, "Verbose"));
		TextOut(hdc, 235, 77, str1, wsprintf(str1, "OSC delay"));

		DeleteObject(htmp);
		DeleteObject(hfont);
		EndPaint(hwnd, &ps);

		sprintf(Xdelay_str, "%d", Xdelay);
		SetWindowText(hwnddely, (LPSTR)Xdelay_str);
		SetWindowText(hwndipaddr, (LPSTR)Xip_str);
		if (Xverbose)	SendMessage(hwndverb, BM_SETCHECK, BST_CHECKED, 0);
		else			SendMessage(hwndverb, BM_SETCHECK, BST_UNCHECKED, 0);

		break;
//
	case WM_COMMAND:
		if(HIWORD(wParam) == CBN_DROPDOWN) {	// user action = dropdown
			SendMessage((HWND)lParam, CB_SHOWDROPDOWN, (WPARAM) 0, (LPARAM) 0);
		} else if (HIWORD(wParam) == BN_CLICKED) {	// user action
			i = LOWORD(wParam);
			switch(i) {
			case 1:
				GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32Connect(Xconnected, Xip_str, 100000);
					if (Xconnected)	SetWindowTextW(hwndconx, L"Connected");
					else			SetWindowTextW(hwndconx, L"Connect");
				} else {
					fprintf (log_file, "Incorrect IP string form\r\n");
				}
				break;
			case 2:
				if (Xverbose)SendMessage(hwndverb, BM_SETCHECK, BST_UNCHECKED, 0);
				else		 SendMessage(hwndverb, BM_SETCHECK, BST_CHECKED, 0);
				Xverbose ^= 1;
				break;
			case 8:
				Xfiles = 0;
				strcpy(XPresetLibPath, "Filename will be ignored");
				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = XPresetLibPath;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				//ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(XPresetLibPath);
				ofn.lpstrFilter = NULL;
				ofn.nFilterIndex = 0;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrTitle = (LPCTSTR)"Save Preset Library to directory selected below\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_EXPLORER | OFN_CREATEPROMPT |
						OFN_HIDEREADONLY | OFN_NOVALIDATE |
						OFN_PATHMUSTEXIST | OFN_READONLY;
				//
				// Display the Open dialog box.
				if (GetSaveFileName(&ofn)) {
					// remove filename from returned path so we can either save to
					// existing directory or newly created one
					i = strlen(XPresetLibPath);
					while (i && (XPresetLibPath[i] != '\\')) --i;
					XPresetLibPath[i] = 0;
//					printf("Path: %s\n", XPresetLibPath);
//					fflush(stdout);
					Xfiles = 1;
					SetWindowText(hwndprog, (LPSTR)Xready);	// display ready status
					MessageBox(NULL, XPresetLibPath, "Destination directory: ", MB_OK);
				}
				break;
			case 9:
				// GetLib request
				GetLib_request = 1;
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		if (Xconnected) close (Xfd);
		// read panel data
		GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);

		GetWindowText(hwnddely, Xdelay_str, GetWindowTextLength(hwnddely) + 1);
		sscanf(Xdelay_str, "%d", &Xdelay);

		res_file = fopen(Finipath, "wb");
		GetWindowRect(hwnd, &Rect);
		fprintf(res_file, "%d %d %d %d\n",(int)(Rect.right - Rect.left), (int)(Rect.bottom - Rect.top), Xverbose, Xdelay);
		fprintf(res_file, "%s\n", Xip_str);
		fprintf(res_file, "%s\n", Xlogpath);
		fclose (res_file);
		if (log_file) fclose(log_file);

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

int
main(int argc, char **argv)
{
	HINSTANCE	hPrevInstance = 0;
	PWSTR		pCmdLine = 0;
	int			nCmdShow = 0;

	Xverbose = 0;

	strcpy(Xlogpath, ".");
	Xip_str[0] = 0;
	XPresetLibPath[0] = 0;
	// load resource file
	if ((res_file = fopen("./.X32GetLib.ini", "r")) != NULL) {
		// get and remember real path

		if((Finiretval = GetFullPathNameA("./.X32GetLib.ini", 1024, Finipath, FinilppPart)) > 1024){
			printf ("Not enough space for file name\n");
		}
		fscanf(res_file, "%d %d %d %d\n",
				&wWidth, &wHeight, &Xverbose, &Xdelay);
		fgets(Xip_str, sizeof(Xip_str), res_file);
		Xip_str[strlen(Xip_str) - 1] = 0;
		fgets(Xlogpath, sizeof(Xlogpath), res_file);
		Xlogpath[strlen(Xlogpath) - 1] = 0;

		fclose (res_file);
	}
	if ((p_status = CreateDirectory(Xlogpath, NULL)) == 0) {
		if (ERROR_ALREADY_EXISTS != GetLastError()) {
			printf ("Cannot create directory\n");
			Sleep(10000);
			return (0);
		}
	}
	strcpy(r_buf, Xlogpath);
	strcat(r_buf, "/.X32GetLib.log");
	// create logfile and run program
	if ((log_file = fopen(r_buf, "wb")) != NULL) {
		fprintf(log_file,"*\r\n*\r\n");
		fprintf(log_file,"*    X32GetLib Log data - ©2015 - Patrick-Gilles Maillot\r\n");
		fprintf(log_file,"*\r\n*\r\n");

		if (Finiretval) fprintf(log_file, ".ini file path: %s\r\n", Finipath);
		else			fprintf(log_file, "couldn't open .X32GetLib.ini file\r\n");
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
void LaunchGetLib() {
int		i;

float	stfader, mfader;
int		showctrl, prepos_cur;


	if (!Xconnected) {
		fprintf(log_file, "Error: Not connected to X32\r\n");
		PSstatus = 1;
		return;
	}
	if (!Xfiles) {
		fprintf(log_file, "Error: No destination directory\r\n");
		PSstatus = 1;
		return;
	}
	PSstatus = 0;
// read panel data
	GetWindowText(hwnddely, Xdelay_str, GetWindowTextLength(hwnddely) + 1);
	sscanf(Xdelay_str, "%d", &Xdelay);

    PsetType = SendMessage(hwndPtyp, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
    fprintf (log_file,"Lib Type: %d, Save to: %s\n", PsetType, XPresetLibPath);
	fprintf (log_file, "Saving X32 State\r\n");
	fflush(log_file);

// Save Master levels (L/R and M/C)
	s_len = Xsprint(s_buf, 0, 's', "/main/st/mix/fader");
	SEND_TO(s_buf, s_len)
// get reply
	RPOLL; // read the desk for answer
	if (p_status > 0) 	{		// ignore responses that do not correspond to what is
		RECV_FR(r_buf, r_len);	// expected: /main/st/mix/fader...[int]
		if (strncmp(r_buf, "/main/st/mix/fader", 18) == 0) {
			for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[24 + i];
			stfader = endian.ff;
		} else {
			fprintf (log_file, "Warning: expected /main/st/mix/fader, received %s\r\n", r_buf);
			PSstatus = 1;
		}
	}
	s_len = Xsprint(s_buf, 0, 's', "/main/m/mix/fader");
	SEND_TO(s_buf, s_len)
// get reply
	RPOLL; // read the desk for answer
	if (p_status > 0) 	{		// ignore responses that do not correspond to what is
		RECV_FR(r_buf, r_len);	// expected: /main/m/mix/fader...[int]
		if (strncmp(r_buf, "/main/m/mix/fader", 17) == 0) {
			for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[24 + i];
			mfader = endian.ff;
		} else {
			fprintf (log_file, "Warning: expected /main/m/mix/fader, received %s\r\n", r_buf);
			PSstatus = 1;
		}
	}
	// set master levels to 0. (-oo)
	s_len = Xfprint(s_buf, 0, "/main/st/mix/fader", 'f', &zero);
	SEND_TO(s_buf, s_len)
	s_len = Xfprint(s_buf, 0, "/main/m/mix/fader", 'f', &zero);
	SEND_TO(s_buf, s_len)
// Save selected channel strip select
	s_len = Xsprint(s_buf, 0, 's', "/-stat/selidx");
	SEND_TO(s_buf, s_len)
// get reply
	RPOLL; // read the desk for answer
	if (p_status > 0) 	{		// ignore responses that do not correspond to what is
		RECV_FR(r_buf, r_len);	// expected: /-stat/selidx...[int]
		if (strncmp(r_buf, "/-stat/selidx", 13) == 0) {
			for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
			PSselidx = endian.ii;
		} else {
			fprintf (log_file, "Warning: expected -/stat/selidx, received %s\r\n", r_buf);
			PSstatus = 1;
		}
	}
// Select Channel 1 (idx = 0)
	s_len = Xfprint(s_buf, 0, "/-stat/selidx", 'i' , &zero);
	SEND_TO(s_buf, s_len)
// Save scene pointer: Get Show_control value and set to 1
	s_len = Xsprint(s_buf, 0, 's', "/-prefs/show_control");
	SEND_TO(s_buf, s_len)
	// get reply
	RPOLL; // read the desk for answer
	if (p_status > 0) 	{		// ignore responses that do not correspond to what is
		RECV_FR(r_buf, r_len);	// expected: /-prefs/show_control...[int]
		if (strncmp(r_buf, "/-prefs/show_control", 20) == 0) {
			for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[28 + i];
			showctrl = endian.ii;
		} else {
			fprintf (log_file, "Warning: expected /-prefs/show_control, received %s\r\n", r_buf);
			PSstatus = 1;
		}
	}
	s_len = Xfprint(s_buf, 0, "/-prefs/show_control", 'i', &one);
	SEND_TO(s_buf, s_len)
// Save scene pointer: Get scene pointer value
	s_len = Xsprint(s_buf, 0, 's', "/-show/prepos/current");
	SEND_TO(s_buf, s_len)
	// get reply
	RPOLL; // read the desk for answer
	if (p_status > 0) 	{		// ignore responses that do not correspond to what is
		RECV_FR(r_buf, r_len);	// expected: /-show/prepos/current...[int]
		if (strncmp(r_buf, "/-show/prepos/current", 21) == 0) {
			for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[28 + i];
			prepos_cur = endian.ii;
		} else {
			fprintf (log_file, "Warning: expected /-show/prepos/current, received %s\r\n", r_buf);
			PSstatus = 1;
		}
	}
// Save state
	if (1) {
		// save state to scene 99
		fprintf (log_file, "Notice: Saving state to scene 99 with Name: PG Maillot, Type: X32GetLib\r\n");
		// Save Scene at requested location (iD)
		s_len = Xsprint(s_buf, 0, 's', "/save");
		s_len = Xsprint(s_buf, s_len, 's', ",sissi");
		s_len = Xsprint(s_buf, s_len, 's', "scene");
		s_len = Xsprint(s_buf, s_len, 'i', &ninetynine);
		s_len = Xsprint(s_buf, s_len, 's', "PG Maillot");
		s_len = Xsprint(s_buf, s_len, 's', "X32GetLib");
		s_len = Xsprint(s_buf, s_len, 'i', &zero);
		SEND_TO(s_buf, s_len)
		// Wait for save to complete
		Xlock = 1;
		while (Xlock) {
			RPOLL; // read the desk for answer to /-show/showfile/scene/099/name
			if (p_status > 0) 	{		// ignore responses that do not correspond to what is
				RECV_FR(r_buf, r_len);	// expected: /save~~~,si~scene~~~[     1]
				if (strncmp(r_buf, "/save", 5) == 0) {
					for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
					if (endian.ii != 1) {
						fprintf (log_file, "Error: Could not save state to scene 99\r\n");
						PSstatus = 1;
						return;
					}
					Xlock = 0;
				} else {
					PSstatus = 1;
					fprintf (log_file, "Warning: Expected /save... received: %s\r\n", r_buf);
				}
			}
		}

	}
//
// Create directory if needed
	if ((p_status = CreateDirectory(XPresetLibPath, NULL)) == 0) {
		if (ERROR_ALREADY_EXISTS != GetLastError()) {
			fprintf (log_file, "Cannot create directory\r\n");
			PSstatus = 1;
			return;
		}
	}
//
// Should we save absolutely ALL?
	if (PsetType == ALL) {
		for (PsetType = CHANNEL; PsetType < LAST; PsetType++) {
			Xsave(PsetType);
		}
	} else {
// Save the selected Type
		Xsave(PsetType);
	}

// Restore State
	fprintf (log_file, "Restoring X32 State\r\n");
	if (1) {
		//restore scene 99 / general state
		s_len = Xsprint(s_buf, 0, 's', "/load");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "scene");
		s_len = Xsprint(s_buf, s_len, 'i', &ninetynine);
		SEND_TO(s_buf, s_len)
		Xlock = 1;
		while (Xlock) {
			RPOLL; // read the desk for answer to /load
			if (p_status > 0) 	{		// ignore responses that do not correspond to what is
				RECV_FR(r_buf, r_len);	// expected: /load~~~,si~scene~~~[     1]
				if (strncmp(r_buf, "/load", 5) == 0) {
					for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
					if (endian.ii != 1) {
						fprintf (log_file, "Error: Could not restore state from scene 99\r\n");
						PSstatus = 1;
					}
					Xlock = 0;
				} else {
					PSstatus = 1;
					fprintf (log_file, "Warning: Expected /load... received: %s\r\n", r_buf);
				}
			}
		}

		// Delete scene 99
		s_len = Xsprint(s_buf, 0, 's', "/delete");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "scene");
		s_len = Xsprint(s_buf, s_len, 'i', &ninetynine);
		SEND_TO(s_buf, s_len)
		Xlock = 1;
		while (Xlock) {
			RPOLL; // read the desk for answer to /delete
			if (p_status > 0) 	{		// ignore responses that do not correspond to what is
				RECV_FR(r_buf, r_len);	// expected: /delete~,si~scene~~~[     1]
				if (strncmp(r_buf, "/delete", 7) == 0) {
					for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
					if (endian.ii != 1) {
						fprintf (log_file, "Error: Could not delete scene 99\r\n");
						PSstatus = 1;
					}
					Xlock = 0;
				} else {
					PSstatus = 1;
					fprintf (log_file, "Warning: Expected /delete... received: %s\r\n", r_buf);
				}
			}
		}
	}
	// restore scene pointer
	s_len = Xfprint(s_buf, 0, "/-show/prepos/current", 'i', &prepos_cur);
	SEND_TO(s_buf, s_len)
	// restore show control
	s_len = Xfprint(s_buf, 0, "/-prefs/show_control", 'i', &showctrl);
	SEND_TO(s_buf, s_len)
	// restore selected channel
	s_len = Xfprint(s_buf, 0, "/-stat/selidx", 'i' , &PSselidx);
	SEND_TO(s_buf, s_len)
	// restore master levels
	s_len = Xfprint(s_buf, 0, "/main/m/mix/fader", 'f', &mfader);
	SEND_TO(s_buf, s_len)
	s_len = Xfprint(s_buf, 0, "/main/st/mix/fader", 'f', &stfader);
	SEND_TO(s_buf, s_len)
	return;
}

void Xsave(int Type) {
	int		i, j;
	int		Pind;
	char	tmpstr[64];
//
	tmpstr[0] = 0;
//
// For the directory where to save files
// Loop through Library to save files, one by one
	switch (Type) {
	case CHANNEL:
		for (Pind = 1; Pind < 101; Pind++ ) {
			// Does Preset 'Pind' have data?
			sprintf(tmpstr, "/-libs/ch/%03d/hasdata", Pind);
			s_len = Xsprint(s_buf, 0, 's', tmpstr);		// channel preset has data?
			SEND_TO(s_buf, s_len)
			// get reply
			RPOLL; // read the desk for answer
			if (p_status > 0) 	{		// ignore responses that do not correspond to what is
				RECV_FR(r_buf, r_len);	// expected: /-libs/ch/xxx/hasdata...[int]
				if (strncmp(r_buf, "/-libs/ch", 9) == 0) {
					for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[28 + i];
					Phasdata = endian.ii;
				} else {
					fprintf (log_file, "Warning: expected /-libs/ch/xxx/hasdata, received %s\r\n", r_buf);
					PSstatus = 1;
					break;
				}
			}
			if (Phasdata) { // A valid preset, with data
				s_len = Xsprint(s_buf, 0, 's', "/node");	// Get header with node command
				s_len = Xsprint(s_buf, s_len, 's', ",s");
				sprintf(tmpstr, "-libs/ch/%03d", Pind);
				s_len = Xsprint(s_buf, s_len, 's', tmpstr);		//
				// Returned elements: pos, name, type, flags, hasdata
				SEND_TO(s_buf, s_len)
				// get reply
				RPOLL; // read the desk for answer
				if (p_status <= 0) 	{		// ignore responses that do not correspond to what is
					fprintf (log_file, "Error or Timeout on /-node command for Effect Preset number %d\r\n", Pind);
					PSstatus = 1;
					break;
				} else {
					RECV_FR(r_buf, r_len);	// expected: node...
					if (strncmp(r_buf, "node", 4) != 0) {
						fprintf (log_file, "Warning: expected /node..., received %s\r\n", r_buf);
						PSstatus = 1;
						break;
					}
				}
				// extract name from preset header
				i = 0;
				while ((r_buf[i] != '"') && (i < 128)) ++i;
				++i; j = 0;
				while ((r_buf[i] != '"') && (j < 16)) Pfilename[j++] = r_buf[i++];
				Pfilename[j] = 0;
				fprintf (log_file, "Processing Channel preset: %s\r\n", Pfilename);
				// echo name in progress bar
				SetWindowText(hwndprog, (LPSTR)Pfilename);	// ... and display it
				WIN_LOOP
//				// extract flags from preset header
//				while ((r_buf[i] != '%') && (i < 128)) ++i;
//				++i; j = 0;
//				while (j < 16) PFlags[j++] = r_buf[i++];
				//
				// Create file name to save Preset data to
				strcpy (XPresetFile, XPresetLibPath);
				strcat (XPresetFile, "\\");
				strcat (XPresetFile, Pfilename);
				strcat (XPresetFile, ".chn");
				if ((Pfile_pt = fopen(XPresetFile, "wb")) == 0) {
					fprintf (log_file, "Error: Cannont create Channel Preset File %s\r\n", XPresetFile);
					PSstatus = 1;
					break;
				}
				// writing Preset File header
				fprintf (Pfile_pt, "#2.1#");
				fputs (r_buf + 25, Pfile_pt);
				//
				// Load the Preset
				s_len = Xsprint(s_buf, 0, 's', "/load");
				s_len = Xsprint(s_buf, s_len, 's', ",siii");
				s_len = Xsprint(s_buf, s_len, 's', "libchan");
				j = Pind - 1;
				s_len = Xsprint(s_buf, s_len, 'i', &j);		// Load channel preset index j
				s_len = Xsprint(s_buf, s_len, 'i', &zero);	// Load channel preset (ch/01 is selected)
				s_len = Xsprint(s_buf, s_len, 'i', &sixtythree);	// Load all channel preset elements
				SEND_TO(s_buf, s_len)
				RPOLL; // read the desk for answer to /load ,si libchan <index>
				if (p_status > 0) 	{		// ignore responses that do not correspond to what is
					RECV_FR(r_buf, r_len);	// expected: /load~~~,si~libchan~~~[     1]
					if (strncmp(r_buf, "/load", 5) == 0) {
						for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
						if (endian.ii != 1) {
							fprintf (log_file, "Error: Could not load preset number %d, named %s\r\n", Pind, Pfilename);
							PSstatus = 1;
							break;
						}
					} else {
						PSstatus = 1;
						fprintf (log_file, "Warning: Expected /load... received: %s\r\n", r_buf);
					}
				} else {
					PSstatus = 1;
					fprintf (log_file, "Error or timeout on Channel Preset, cmd: %s\r\n", s_buf);
				}
				// Wait for load to complete with a 30ms sleep
				Sleep(30);
				// Fetch all channel preset elements, independently from header
				// write Preset data to file
				j = Xsprint (s_buf, 0, 's', "/node");
				j = Xsprint (s_buf, j, 's', ",s");
				for (i = 0; i < PCom_max; i++) {
					s_len = Xsprint(s_buf, j, 's', PComList[i]);
					SEND_TO(s_buf, s_len);
					RPOLL;
					if (p_status > 0) {
						RECV_FR(r_buf, r_len);
						if (r_len > 0) {
							// remove the "source" element of /config
							if (i == 0) {
								while((r_buf[r_len] != ' ') && (r_len > 0)) --r_len;
								r_buf[r_len++] = '\n'; r_buf[r_len++] = '\0';
							}
							// remove "/node.../ch/01" from returned string
							fprintf(Pfile_pt, "%s", r_buf + 18);
						}
					} else {
						PSstatus = 1;
						fprintf (log_file, "Error or timeout on Channel Preset, cmd: %s\r\n", s_buf);
					}
				}
				// special case of headamp command
				s_len = Xsprint(s_buf, j, 's', "headamp/000");
				SEND_TO(s_buf, s_len);
				RPOLL;
				if (p_status > 0) {
					RECV_FR(r_buf, r_len);
					if (r_len > 0) {
						// remove "/node..." from returned string
						fprintf(Pfile_pt, "%s", r_buf + 12);
					}
				} else {
					PSstatus = 1;
					fprintf (log_file, "Error or timeout on cmd: %s\r\n", s_buf);
				}
				// Done
				fclose(Pfile_pt);
			}
		}
		break;
	case EFFECTS:
		for (Pind = 1; Pind < 101; Pind++ ) {
			// Does Preset 'Pind' have data?
			sprintf(tmpstr, "/-libs/fx/%03d/hasdata", Pind);
			s_len = Xsprint(s_buf, 0, 's', tmpstr);		// Effcets preset has data?
			SEND_TO(s_buf, s_len)
			// get reply
			RPOLL; // read the desk for answer
			if (p_status > 0) 	{		// ignore responses that do not correspond to what is
				RECV_FR(r_buf, r_len);	// expected: /-libs/fx/xxx/hasdata...[int]
				if (strncmp(r_buf, "/-libs/fx", 9) == 0) {
					for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[28 + i];
					Phasdata = endian.ii;
				} else {
					fprintf (log_file, "Warning: expected /-libs/fx/xxx/hasdata, received %s\r\n", r_buf);
					PSstatus = 1;
					break;
				}
			}
			if (Phasdata) { // A valid preset, with data
				s_len = Xsprint(s_buf, 0, 's', "/node");	// Get header with node command
				s_len = Xsprint(s_buf, s_len, 's', ",s");
				sprintf(tmpstr, "-libs/fx/%03d", Pind);
				s_len = Xsprint(s_buf, s_len, 's', tmpstr);		//
				// Returned elements: pos, name, type, flags, hasdata
				SEND_TO(s_buf, s_len)
				// get reply
				RPOLL; // read the desk for answer
				if (p_status <= 0) 	{		// ignore responses that do not correspond to what is
					fprintf (log_file, "Error or Timeout on /-node command for Effect Preset number %d\r\n", Pind);
					PSstatus = 1;
					break;
				} else {
					RECV_FR(r_buf, r_len);	// expected: node...
					if (strncmp(r_buf, "node", 4) != 0) {
						fprintf (log_file, "Warning: expected /node..., received %s\r\n", r_buf);
						PSstatus = 1;
						break;
					}
				}
				// extract name from preset header
				i = 0;
				while ((r_buf[i] != '"') && (i < 128)) ++i;
				++i; j = 0;
				while ((r_buf[i] != '"') && (j < 16)) Pfilename[j++] = r_buf[i++];
				Pfilename[j] = 0;
				fprintf (log_file, "Processing Effect preset: %s\r\n", Pfilename);
				// echo name in progress bar
				SetWindowText(hwndprog, (LPSTR)Pfilename);	// ... and display it
				WIN_LOOP
				//
				// Create file name to save Preset data to
				strcpy (XPresetFile, XPresetLibPath);
				strcat (XPresetFile, "\\");
				strcat (XPresetFile, Pfilename);
				strcat (XPresetFile, ".efx");
				if ((Pfile_pt = fopen(XPresetFile, "wb")) == 0) {
					fprintf (log_file, "Error: Cannot create Effect Preset File %s\r\n", XPresetFile);
					PSstatus = 1;
					break;
				}
				// writing Preset File header
				fprintf (Pfile_pt, "#2.1#");
				fputs (r_buf + 25, Pfile_pt);
				//
				// Load the Preset
				s_len = Xsprint(s_buf, 0, 's', "/load");
				s_len = Xsprint(s_buf, s_len, 's', ",sii");
				s_len = Xsprint(s_buf, s_len, 's', "libfx");
				j = Pind - 1;
				s_len = Xsprint(s_buf, s_len, 'i', &j);		// Load effect preset index j
				s_len = Xsprint(s_buf, s_len, 'i', &zero);	// Load effect preset (fx/1 is selected)
				SEND_TO(s_buf, s_len)
				RPOLL; // read the desk for answer to /load ,si libchan <index>
				if (p_status > 0) 	{		// ignore responses that do not correspond to what is
					RECV_FR(r_buf, r_len);	// expected: /load~~~,si~libfx~~~[     1]
					if (strncmp(r_buf, "/load", 5) == 0) {
						for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
						if (endian.ii != 1) {
							fprintf (log_file, "Error: Could not load effect preset number %d, named %s\r\n", Pind, Pfilename);
							PSstatus = 1;
							break;
						}
					} else {
						PSstatus = 1;
						fprintf (log_file, "Warning: Expected /load... received: %s\r\n", r_buf);
					}
				} else {
					PSstatus = 1;
					fprintf (log_file, "Error or timeout on Effect Preset, cmd: %s\r\n", s_buf);
				}
				// Wait for load to complete with a 30ms sleep
				Sleep(30);
				// Fetch all effect preset elements, write Preset data
				j = Xsprint (s_buf, 0, 's', "/node");
				j = Xsprint (s_buf, j, 's', ",s");
				for (i = 0; i < PCfx_max; i++) {
					s_len = Xsprint(s_buf, j, 's', PCfxList[i]);
					SEND_TO(s_buf, s_len);
					RPOLL;
					if (p_status > 0) {
						RECV_FR(r_buf, r_len);
						if (r_len > 0) {
							// remove "/node.../fx/1/" from returned string
							fprintf(Pfile_pt, "%s", r_buf + 18);
						}
					} else {
						PSstatus = 1;
						fprintf (log_file, "Error or timeout on Effect Preset, cmd: %s\r\n", s_buf);
					}
				}
				// Done
				fclose(Pfile_pt);
			}
		}
		break;
	case ROUTING:
		for (Pind = 1; Pind < 101; Pind++ ) {
			// Does Preset 'Pind' have data?
			sprintf(tmpstr, "/-libs/r/%03d/hasdata", Pind);
			s_len = Xsprint(s_buf, 0, 's', tmpstr);		// Effcets preset has data?
			SEND_TO(s_buf, s_len)
			// get reply
			RPOLL; // read the desk for answer
			if (p_status > 0) 	{		// ignore responses that do not correspond to what is
				RECV_FR(r_buf, r_len);	// expected: /-libs/r/xxx/hasdata...[int]
				if (strncmp(r_buf, "/-libs/r", 8) == 0) {
					for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[28 + i];
					Phasdata = endian.ii;
				} else {
					fprintf (log_file, "Warning: expected /-libs/r/xxx/hasdata, received %s\r\n", r_buf);
					PSstatus = 1;
					break;
				}
			}
			if (Phasdata) { // A valid preset, with data
				s_len = Xsprint(s_buf, 0, 's', "/node");	// Get header with node command
				s_len = Xsprint(s_buf, s_len, 's', ",s");
				sprintf(tmpstr, "-libs/r/%03d", Pind);
				s_len = Xsprint(s_buf, s_len, 's', tmpstr);		//
				// Returned elements: pos, name, type, flags, hasdata
				SEND_TO(s_buf, s_len)
				// get reply
				RPOLL; // read the desk for answer
				if (p_status <= 0) 	{		// ignore responses that do not correspond to what is
					fprintf (log_file, "Error or Timeout on /-node command for Routing Preset number %d\r\n", Pind);
					PSstatus = 1;
					break;
				} else {
					RECV_FR(r_buf, r_len);	// expected: node...
					if (strncmp(r_buf, "node", 4) != 0) {
						fprintf (log_file, "Warning: expected /node..., received %s\r\n", r_buf);
						PSstatus = 1;
						break;
					}
				}
				// extract name from preset header
				i = 0;
				while ((r_buf[i] != '"') && (i < 128)) ++i;
				++i; j = 0;
				while ((r_buf[i] != '"') && (j < 16)) Pfilename[j++] = r_buf[i++];
				Pfilename[j] = 0;
				fprintf (log_file, "Processing Effect preset: %s\r\n", Pfilename);
				// echo name in progress bar
				SetWindowText(hwndprog, (LPSTR)Pfilename);	// ... and display it
				WIN_LOOP
				//
				// Create file name to save Preset data to
				strcpy (XPresetFile, XPresetLibPath);
				strcat (XPresetFile, "\\");
				strcat (XPresetFile, Pfilename);
				strcat (XPresetFile, ".rou");
				if ((Pfile_pt = fopen(XPresetFile, "wb")) == 0) {
					fprintf (log_file, "Error: Cannont create Routing Preset File %s\r\n", XPresetFile);
					PSstatus = 1;
					break;
				}
				// writing Preset File header
				fprintf (Pfile_pt, "#2.1#");
				fputs (r_buf + 24, Pfile_pt);
				//
				// Load the Preset
				s_len = Xsprint(s_buf, 0, 's', "/load");
				s_len = Xsprint(s_buf, s_len, 's', ",si");
				s_len = Xsprint(s_buf, s_len, 's', "librout");
				j = Pind - 1;
				s_len = Xsprint(s_buf, s_len, 'i', &j);		// Load routing preset index j
				SEND_TO(s_buf, s_len)
				RPOLL; // read the desk for answer to /load ,si libchan <index>
				if (p_status > 0) 	{		// ignore responses that do not correspond to what is
					RECV_FR(r_buf, r_len);	// expected: /load~~~,si~librout~[     1]
					if (strncmp(r_buf, "/load", 5) == 0) {
						for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
						if (endian.ii != 1) {
							fprintf (log_file, "Error: Could not load routing preset number %d, named %s\r\n", Pind, Pfilename);
							PSstatus = 1;
							break;
						}
						Xlock = 0;
					} else {
						PSstatus = 1;
						fprintf (log_file, "Warning: Expected /load... received: %s\r\n", r_buf);
					}
				} else {
					PSstatus = 1;
					fprintf (log_file, "Error or timeout on Routing Preset, cmd: %s\r\n", s_buf);
				}
				// Wait for load to complete with a 30ms sleep
				Sleep(30);
				// Fetch all routing preset elements, write Preset data
				j = Xsprint (s_buf, 0, 's', "/node");
				j = Xsprint (s_buf, j, 's', ",s");
				for (i = 0; i < PCro_max; i++) {
					s_len = Xsprint(s_buf, j, 's', PCroList[i]);
					SEND_TO(s_buf, s_len);
					RPOLL;
					if (p_status > 0) {
						RECV_FR(r_buf, r_len);
						if (r_len > 0) {
							// remove "/node...,s~~" from returned string
							fprintf(Pfile_pt, "%s", r_buf + 12);
						}
					} else {
						fprintf (log_file, "Error or timeout on Routing Preset, cmd: %s\r\n", s_buf);
					}
				}
				// Done
				fclose(Pfile_pt);
			}
		}
		break;
	}
	return;
}
