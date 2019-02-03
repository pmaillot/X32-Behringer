/*
 * X32SetLib.c
 *
 *  Created on: 23 avr. 2015
 *
 * ©2015 - Patrick-Gilles Maillot
 *
 * X32SetLib - a windows app for loading a Preset Library, or Several Preset files
 * to X32 memory from a PC HDD.
 *
 *              Even if this program is essentially a windows-only application,
 *              #ifdefs are kept for the case of a linux/MacOS implementation
 *
 * ver. 0.16: preventing window resizing
 */

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>


#ifdef __WIN32__
#include <windows.h>
#include <Commdlg.h>
#else
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#endif


#define BSIZE 			512	// Buffer sizes (enough to take into account FX parameters)
#define LINEFEED		10
#define LENPATH			256 // maximum length for directory names
#define LENFILE			64	// maximum length for file names
#define LENFULLPATH		LENPATH +  LENFILE + 10	//that's 256+64 + a little extra
												//for extension and snippet/scene digits
#define LENMULTIPATH	100 * LENFILE / 2  //should be enough for 99 file names
#define PS_CH			1
#define	PS_FX			2
#define	PS_RO			4

// Private functions
void	LaunchPresetXfer();		// return status is in PSstatus
int		X32_Connect();
void 	XRcvClean();
//
// External calls used
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int Xfprint(char *bd, int index, char* text, char format, void *bs);
extern int SetSceneParse(char *l_read);
extern void X32logf(char *header, char *buf, int len);
extern void X32logfile(FILE *f);
extern int  validateIP4Dotted(const char *s);
//

WINBASEAPI HWND WINAPI	GetConsoleWindow(VOID);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE	hInstance = 0;
HWND 		hwnd, hwndipaddr, hwndconx, hwndGO, hwndprog;
HWND		hwndstid, hwndverb, hwnddely, hwndbrowse;
//HWND		hwndFHA, hwndFCO, hwndFGA, hwndFEQ, hwndFDY, hwndFSN;

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

int		wWidth = 540;
int		wHeight = 123;

int		zero = 0;
int		one = 1;
int		ninetynine = 99;

int		SetPreset_request = 0;

//
// Windows UI related storage
wchar_t	W32_ip_str[20], W32_delay_str[8], W32_PSstartid[4], W32_PresetFileName[LENFILE];
char	Xip_str[20], Xdelay_str[8], XPSstartid[4], XPresetFileName[LENFILE];
char	Xlogpath[LENPATH], XPresetFilePath[LENMULTIPATH];
char	Xprg_str[32];
char	Xcomplete[] = "Complete";
char	Xready[] = "Ready";
char	Xerror[]    = "--Error--";
char	XPname[32];
char	PSBuffer[BSIZE];
//
// Misc. flags
int		Xconnected = 0;
int		XuseFlags = 0;		// This is a "hidden" flag - present in the .ini file only
							// to try to distinguish between channel types,
							// based on the header flag bits 8-15
int		X32SHOW = 1;		// this flags reorients verbose mode to log file rather than stdout
int		X32PRESET = 1;		// this flags reorients /ch/xx/config to not consider source
int		Xdelay = 0;			// typically not needed or low value for presets
int		Xdebug = 0;
int		Xfiles = 0;			// Flag to indicate if files are selected or not
int		Xverbose = 0; 		// Verbose
int		PSstatus = 0;		// Return status (1 = error) after reading preset file
int		X32VER = 212;		// Server version - 208, 210, 212...
int		Xlock = 0;
int		keep_running = 1;
int		PsetType = 0;		// Type of Preset. 0-unknown, 1:Channel
int		PSFlags = 0;		// Preset Header Flag first 8 bits
int		PSselidx = 0;
int		PSstartid = 0;		// starting counter for preset index in lib
int		fx[8]; // saves the FX current type for each of the fx slots

enum PSetType {UNKNOWN = 0, CHANNEL, EFFECT, ROUTING};

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
#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
struct timeval		timeout;
fd_set 				ufds;
unsigned long 		mode;
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
struct addrinfo		ip_hints, *res;
struct pollfd		ufds;
#endif
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
		if (Xverbose) X32logf("->X", (b), (l));						\
		if (sendto(Xfd, (b), (l), 0, Xip_pt, Xip_len) < 0) {		\
			fprintf (log_file, "Coundn't send data to X32\r\n");	\
			exit(EXIT_FAILURE);										\
		} 															\
		if (Xdelay > 0) MILLISLEEP(Xdelay);							\
	} while (0);
//
//
#define RECV_FR(b, l)											\
	do {														\
		if (((l) = recvfrom(Xfd, (b), BSIZE, 0, 0, 0)) > 0) {	\
			if (Xverbose) X32logf("X->", (b), (l));				\
		} 														\
	} while (0);
//
//
//
#ifdef __WIN32__
#define RPOLL															\
	do {																\
		FD_ZERO (&ufds);												\
		FD_SET (Xfd, &ufds);											\
		p_status = select(Xfd,&ufds,NULL,NULL,&timeout);				\
	} while (0);

#define MILLISLEEP(t)													\
	do {																\
		Sleep((t));														\
	} while (0);
#else
#define RPOLL															\
	do {																\
		p_status = poll(&ufds, 1, 10); 									\
	} while (0);

#define MILLISLEEP(t)													\
	do {																\
		usleep((t)*1000);												\
	} while (0);
#endif
//
//
//
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32SetLib";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"X32SetLib - Transfer multiple Presets to X32 ",
			WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU,
			100, 220, wWidth, wHeight, 0, 0, hInstance, 0);

	while (keep_running) {
		if(PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		} else {
			if (SetPreset_request) {
				XRcvClean();
				LaunchPresetXfer();
				if (PSstatus) {
					SetWindowText(hwndprog, (LPSTR)Xerror);
					fprintf (log_file, "Done transferXPresetFilePath, with errors or warnings\r\n");
				} else {
					SetWindowText(hwndprog, (LPSTR)Xcomplete);
					fprintf (log_file, "Done transfering presets!\r\n");
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

		hwndbrowse = CreateWindowW(L"button", L"Browse", WS_VISIBLE | WS_CHILD,
				305, 75, 75, 20, hwnd, (HMENU)8, NULL, NULL);

		hwndstid = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
						495, 7, 35, 20, hwnd, (HMENU)0, NULL, NULL);
		hwndGO = CreateWindowW(L"button", L"Xfer Presets", WS_VISIBLE | WS_CHILD,
				395, 30, 135, 43, hwnd, (HMENU)9, NULL, NULL);

		hwndprog = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
						395, 75, 135, 20, hwnd, (HMENU)0, NULL, NULL);

		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./.X32SetLib.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_SHARED);
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
		TextOut(hdc, 335, 18, str1, wsprintf(str1, "ver. 0.16"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 0, str1, wsprintf(str1, "X32SetLib - ©2015 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);

		hfont = CreateFont(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 395, 10, str1, wsprintf(str1, "Start at position:"));

		TextOut(hdc, 140, 77, str1, wsprintf(str1, "Verbose"));
		TextOut(hdc, 235, 77, str1, wsprintf(str1, "OSC delay"));

		DeleteObject(htmp);
		DeleteObject(hfont);
		EndPaint(hwnd, &ps);

		sprintf(XPSstartid, "%d", PSstartid);
		SetWindowText(hwndstid, XPSstartid);
		sprintf(Xdelay_str, "%d", Xdelay);
		SetWindowText(hwnddely, (LPSTR)Xdelay_str);
		SetWindowText(hwndipaddr, (LPSTR)Xip_str);
		if (Xverbose)	SendMessage(hwndverb, BM_SETCHECK, BST_CHECKED, 0);
		else			SendMessage(hwndverb, BM_SETCHECK, BST_UNCHECKED, 0);

		break;
//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
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
				if (Xverbose)SendMessage(hwndverb, BM_SETCHECK, BST_UNCHECKED, 0);
				else		 SendMessage(hwndverb, BM_SETCHECK, BST_CHECKED, 0);
				Xverbose ^= 1;
				break;

			case 8:
				Xfiles = 0;
				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = XPresetFilePath;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(XPresetFilePath);
				ofn.lpstrFilter = "Channel Presets\0*.chn\0Effect Presets\0*.efx\0Routing Presets\0*.rou\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = XPresetFileName;
				ofn.nMaxFileTitle = sizeof(XPresetFileName);
				ofn.lpstrTitle = (LPCTSTR)"Select Preset Files from the list below\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
				//
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
//					printf("Path: %s\n", XPresetFilePath);
//					printf("Name: %s\n", XPresetFileName);
//					fflush(stdout);
					Xfiles = 1;
					SetWindowText(hwndprog, (LPSTR)Xready);	// display ready status
				}
				break;
			case 9:
				// SetPreset request
				SetPreset_request = 1;
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

		i = GetWindowTextLengthW(hwnddely) + 1;
		GetWindowTextW(hwnddely, W32_delay_str, i);
		WideCharToMultiByte(CP_ACP, 0, W32_delay_str, i, Xdelay_str, i, NULL, NULL);
		sscanf(Xdelay_str, "%d", &Xdelay);

		i = GetWindowTextLengthW(hwndstid) + 1;
		GetWindowTextW(hwndstid, W32_PSstartid, i);
		WideCharToMultiByte(CP_ACP, 0, W32_PSstartid, i, XPSstartid, i, NULL, NULL);
		sscanf(XPSstartid, "%d", &PSstartid);

		res_file = fopen(Finipath, "wb");
		fprintf(res_file, "%d %d %d %d %d\n",
				wWidth, wHeight, Xverbose, Xdelay, PSstartid);
		fprintf(res_file, "%d\n", XuseFlags);
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
int X32_Connect() {
	int i;

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
#ifdef __WIN32__
			mode = 1;
			p_status = ioctlsocket(Xfd, FIONBIO, &mode); // make socket non blocking
			timeout.tv_sec = 0;
			timeout.tv_usec = 50000; //Set timeout for non blocking recvfrom(): 50ms
			ufds.fd_array[0] = Xfd;
			ufds.fd_count = 1;
#else
// Prepare for poll() on receiving data
			ufds.fd = Xfd;
			ufds.events = POLLIN; //Check for normal data
#endif
			s_len = Xsprint(s_buf, 0, 's', "/info");
			if (Xverbose) X32logf("->X", s_buf, s_len);
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
					if (Xverbose) X32logf("X->", r_buf, r_len);
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
	X32VER = 212; // 2.12
	if (strcmp(r_buf + i, "2.08") == 0) X32VER = 208; // 2.08
	if (strcmp(r_buf + i, "2.10") == 0) X32VER = 210; // 2.08
	fprintf (log_file, "X32 is running FW %s\r\n", r_buf + i);
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

int
main(int argc, char **argv)
{
	HINSTANCE	hPrevInstance = 0;
	PWSTR		pCmdLine = 0;
	int			nCmdShow = 0;

	Xverbose = 0;

	strcpy(Xlogpath, ".");
	Xip_str[0] = 0;
	XPresetFilePath[0] = 0;
	XPresetFileName[0] = 0;
	// load resource file
	if ((res_file = fopen("./.X32SetLib.ini", "r")) != NULL) {
		// get and remember real path
		if((Finiretval = GetFullPathNameA("./.X32SetLib.ini", 1024, Finipath, FinilppPart)) > 1024){
			printf ("Not enough space for file name\n");
		}
		fscanf(res_file, "%d %d %d %d %d\n",
				&wWidth, &wHeight, &Xverbose, &Xdelay, &PSstartid);
		fscanf(res_file, "%d\n", &XuseFlags);
		fgets(Xip_str, sizeof(Xip_str), res_file);
		Xip_str[strlen(Xip_str) - 1] = 0;
		fgets(Xlogpath, sizeof(Xlogpath), res_file);
		Xlogpath[strlen(Xlogpath) - 1] = 0;
		XPresetFileName[0] = 0;

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
	strcat(r_buf, "/.X32SetLib.log");
	// create logfile and run program
	if ((log_file = fopen(r_buf, "wb")) != NULL) {
		X32logfile(log_file);
		fprintf(log_file,"*\r\n*\r\n");
		fprintf(log_file,"*    X32SetLib Log data - ©2015 - Patrick-Gilles Maillot\r\n");
		fprintf(log_file,"*\r\n*\r\n");

		if (Finiretval) fprintf(log_file, ".ini file path: %s\r\n", Finipath);
		else			fprintf(log_file, "couldn't open .X32SetLib.ini file\r\n");
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
void LaunchPresetXfer() {
int		i, j;
int		PB_ind;
int		path_ind;
char	*tmpstr;
float	stfader, mfader;
int		showctrl, prepos_cur;


	if (!Xconnected) {
		fprintf(log_file, "Error: Not connected to X32\r\n");
		PSstatus = 1;
		return;
	}
	if (!Xfiles) {
		fprintf(log_file, "Error: No files\r\n");
		PSstatus = 1;
		return;
	}
	PSstatus = 0;
// read panel data
	i = GetWindowTextLengthW(hwnddely) + 1;
	GetWindowTextW(hwnddely, W32_delay_str, i);
	WideCharToMultiByte(CP_ACP, 0, W32_delay_str, i, Xdelay_str, i, NULL, NULL);
	sscanf(Xdelay_str, "%d", &Xdelay);

	i = GetWindowTextLengthW(hwndstid) + 1;
	GetWindowTextW(hwndstid, W32_PSstartid, i);
	WideCharToMultiByte(CP_ACP, 0, W32_PSstartid, i, XPSstartid, i, NULL, NULL);
	sscanf(XPSstartid, "%d", &PSstartid);

	if (PSstartid > 100) {
		PSstatus = 1;
		fprintf (log_file, "Error: Preset number too high\r\n");
		return;
	}
	fprintf (log_file, "Saving X32 State\r\n");

// Save Master levels
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
// Save selected channel strip
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


// Save state and set Master Levels to 0
	if (1) {
		// save state to scene 99
		// Save Scene at requested location (iD)
		s_len = Xsprint(s_buf, 0, 's', "/save");
		s_len = Xsprint(s_buf, s_len, 's', ",sissi");
		s_len = Xsprint(s_buf, s_len, 's', "scene");
		s_len = Xsprint(s_buf, s_len, 'i', &ninetynine);
		s_len = Xsprint(s_buf, s_len, 's', "PG Maillot");
		s_len = Xsprint(s_buf, s_len, 's', "X32SetLib");
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
	path_ind = strlen (XPresetFilePath) + 1;
	do {
		strcpy (XPresetFileName, XPresetFilePath);
		if (*(XPresetFilePath + path_ind) != 0) {
			strcat (XPresetFileName, "\\");
			strcat (XPresetFileName, XPresetFilePath + path_ind);
			path_ind += strlen (XPresetFilePath + path_ind) + 1;
		}
		if (XPresetFileName[strlen(XPresetFileName) - 3] == 'c') {
			PsetType = CHANNEL;
		} else if (XPresetFileName[strlen(XPresetFileName) - 3] == 'e') {
			PsetType = EFFECT;
		} else if (XPresetFileName[strlen(XPresetFileName) - 3] == 'r') {
			PsetType = ROUTING;
		}
		// check Preset type
		switch (PsetType) {
		case CHANNEL:
			if ((Xin = fopen(XPresetFileName, "rb")) == 0) {
				fprintf (log_file, "Cannot open preset file: %s\r\n", XPresetFileName);
				PSstatus = 1;
				return;
			}
			if (fscanf(Xin, "%s", PSBuffer) != EOF) {
				if (strcmp(PSBuffer, "#2.1#") == 0) {
					fgets(PSBuffer, 256, Xin); 	// get the rest of the line
					// Extract name from header to XPname
					i = 0;
					while((PSBuffer[i] != '"') && i < 256) i++;
					j = 0; i++; // Where preset name starts: PSBuffer + i
					while((PSBuffer[i] != '"') && j < 16) // scan for end of preset name
						XPname[j++] = PSBuffer[i++];		// no more than 16 chars
					XPname[j++] = 0; // mark end of preset name
					while((PSBuffer[i] != '%') && i < 256) i++;
					j = 0; i++; // Where preset header flags start
					for (j = 0, PSFlags = 0; j < 8; j++, i++) { //only interested in the first 8 bits
						PSFlags = (PSFlags << 1) | (PSBuffer[i] - '0');
					}
					// Set name into progression area/bar
					SetWindowText(hwndprog, (LPSTR)XPname);	// ... and display it
					WIN_LOOP
				} else {
					fprintf (log_file, "Only ver. 2.1 files are accepted at this time\r\n");
					PSstatus = 1;
					return;
				}
			}
			// Set Channel 1 as active (done by default/before)
			// prepare buffer with "/ch/01"
			sprintf(PSBuffer, "/ch/01");
			PB_ind = 6;
			if (XuseFlags) { 	//if 1: try to determine what type of channel we should use
								// based on the header flag bits
				if (PSFlags == 0x14) {	// change channel type?
					sprintf(PSBuffer, "/mtx/01");		// use Mtx (has 6EQ) instead of Ch
					PB_ind = 7;
				} else if (PSFlags == 0x30) {
					sprintf(PSBuffer, "/bus/01");		// use Bus (has 6EQ) instead of Ch
					PB_ind = 7;
				}
			} // else: do nothing. Maybe one day there will be a proper way to do this
			//
			tmpstr = PSBuffer + PB_ind;
			while (fscanf(Xin, "%s", tmpstr) != EOF) {
				if (*tmpstr == '#') {
					fgets(tmpstr, 256, Xin); 	// get & ignore the rest of the line
				} else {
					if (strcmp(PSBuffer + PB_ind, "/config") == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/config...
					} else if (strcmp(PSBuffer + PB_ind, "/delay") == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/delay...
					} else if (strcmp(PSBuffer + PB_ind, "/preamp") == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/preaamp...
					} else if (strncmp(PSBuffer + PB_ind, "/gate", 5) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/gate/...
					} else if (strncmp(PSBuffer + PB_ind, "/dyn", 4) == 0) {
						PSstatus = SetSceneParse(PSBuffer);  // /ch/xx/dyn/...
					} else if (strncmp(PSBuffer + PB_ind, "/eq", 3) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/eq...
					} else if (strncmp(PSBuffer + PB_ind, "/mix", 4) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ch/xx/mix...
					} else if (strncmp(PSBuffer + PB_ind, "/headamp", 8) == 0) {
						PSstatus = SetSceneParse(PSBuffer + PB_ind); // /headamp/...
					} else {
						fprintf (log_file, "Warning - %s is not a supported item for Channel preset\r\n", PSBuffer + PB_ind);
						PSstatus = 1;
					}
				}
			}
			// Done with the file
			if (Xin) fclose (Xin);
			fprintf (log_file, "Saving Preset %s to X32 Library, at position %d\r\n", XPname, PSstartid);

			// Save Preset to X32 Library
			s_len = Xsprint(s_buf, 0, 's', "/save");
			s_len = Xsprint(s_buf, s_len, 's', ",sisi");
			s_len = Xsprint(s_buf, s_len, 's', "libchan");		// channel preset
			i = PSstartid - 1;
			s_len = Xsprint(s_buf, s_len, 'i', &i);				// save index
			s_len = Xsprint(s_buf, s_len, 's', XPname);			// preset name
			s_len = Xsprint(s_buf, s_len, 'i', &zero);			// from channel 01
			SEND_TO(s_buf, s_len)
			// Wait for save to complete
			Xlock = 1;
			while (Xlock) {
				RPOLL; // read the desk for answer to /-show/showfile/scene/099/name
				if (p_status > 0) 	{		// ignore responses that do not correspond to what is
					RECV_FR(r_buf, r_len);	// expected: /save~~~,si~libchan~[     1]
					if (strncmp(r_buf, "/save", 5) == 0) {
						for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
						if (endian.ii != 1) {
							fprintf (log_file, "Error: Could not save preset to X32 library\r\n");
							PSstatus = 1;
							break;
						}
						Xlock = 0;
					} else {
						PSstatus = 1;
						fprintf (log_file, "Warning: Expected /save... received: %s\r\n", r_buf);
					}
				}
			}
			if (PSstatus == 0) {
				PSstartid += 1; 							// increment index counter
				if (PSstartid > 100) {
					PSstatus = 1;
					fprintf (log_file, "Error: Preset number too high\r\n");
					break;
				} else {
					sprintf(XPSstartid, "%d", PSstartid);		// set buffer
					SetWindowText(hwndstid, (LPSTR)XPSstartid); // ... and display it
				}
			}
			break;
		case EFFECT:
			// Set Effect 1 as active (not needed in fact)
			// prepare buffer with "/fx/1/"
			sprintf(PSBuffer, "/fx/1/");
			PB_ind = 6;

			if ((Xin = fopen(XPresetFileName, "rb")) == 0) {
				fprintf (log_file, "Cannot open preset file: %s\r\n", XPresetFileName);
				PSstatus = 1;
				return;
			}
			tmpstr = PSBuffer + PB_ind;
			while (fscanf(Xin, "%s", tmpstr) != EOF) {
				if (*tmpstr == '#') {
					if (strcmp(tmpstr, "#2.7#") == 0) {
						fgets(tmpstr, 256, Xin); 	// get the rest of the line
						// Extract name from header to XPname
						i = 0;
						while((tmpstr[i] != '"') && i < 256) i++;
						j = 0; i++; //Where preset name starts: tmpstr + i
						while((tmpstr[i] != '"') && j < 16) // scan for end of preset name
							XPname[j++] = tmpstr[i++];		// no more than 16 chars
						XPname[j++] = 0; // mark end of preset name
						// Set name into progression area/bar
						SetWindowText(hwndprog, (LPSTR)XPname);	// ... and display it
						WIN_LOOP
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
							fprintf (log_file, "Warning - %s is not a supported item for Channel preset\r\n", PSBuffer + PB_ind);
							PSstatus = 1;
						}
					}
				}
			}
			// Done with the file
			if (Xin) fclose (Xin);
			fprintf (log_file, "Saving Preset %s to X32 Library, at position %d\r\n", XPname, PSstartid);

			// Save Preset to X32 Library
			s_len = Xsprint(s_buf, 0, 's', "/save");
			s_len = Xsprint(s_buf, s_len, 's', ",sisi");
			s_len = Xsprint(s_buf, s_len, 's', "libfx");		// Effect preset
			i = PSstartid - 1;
			s_len = Xsprint(s_buf, s_len, 'i', &i);				// save index
			s_len = Xsprint(s_buf, s_len, 's', XPname);			// preset name
			s_len = Xsprint(s_buf, s_len, 'i', &zero);			// from fx 1
			SEND_TO(s_buf, s_len)
			// Wait for save to complete
			Xlock = 1;
			while (Xlock) {
				RPOLL; // read the desk for answer to /-show/showfile/scene/099/name
				if (p_status > 0) 	{		// ignore responses that do not correspond to what is
					RECV_FR(r_buf, r_len);	// expected: /save~~~,si~libchan~[     1]
					if (strncmp(r_buf, "/save", 5) == 0) {
						for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
						if (endian.ii != 1) {
							fprintf (log_file, "Error: Could not save preset to X32 library\r\n");
							PSstatus = 1;
							break;
						}
						Xlock = 0;
					} else {
						PSstatus = 1;
						fprintf (log_file, "Warning: Expected /save... received: %s\r\n", r_buf);
					}
				}
			}
			if (PSstatus == 0) {
				PSstartid += 1; 							// increment index counter
				if (PSstartid > 100) {
					PSstatus = 1;
					fprintf (log_file, "Error: Preset number too high\r\n");
					break;
				} else {
					sprintf(XPSstartid, "%d", PSstartid);		// set buffer
					SetWindowText(hwndstid, (LPSTR)XPSstartid); // ... and display it
				}
			}
			break;
		case ROUTING:
			// No need to prepare buffer header
			if ((Xin = fopen(XPresetFileName, "rb")) == 0) {
				fprintf (log_file, "Cannot open preset file: %s\r\n", XPresetFileName);
				PSstatus = 1;
				return;
			}
			tmpstr = PSBuffer;
			while (fscanf(Xin, "%s", tmpstr) != EOF) {
				if (*tmpstr == '#') {
					if (strcmp(tmpstr, "#2.7#") == 0) {
						fgets(tmpstr, 256, Xin); 	// get the rest of the line
						// Extract name from header to XPname
						i = 0;
						while((tmpstr[i] != '"') && i < 256) i++;
						j = 0; i++; //Where preset name starts: tmpstr + i
						while((tmpstr[i] != '"') && j < 16) // scan for end of preset name
							XPname[j++] = tmpstr[i++];		// no more than 16 chars
						XPname[j++] = 0; // mark end of preset name
						// Set name into progression area/bar
						SetWindowText(hwndprog, (LPSTR)XPname);	// ... and display it
						WIN_LOOP
					} else {
						fprintf (log_file, "Only ver. 2.7 files are accepted at this time\r\n");
						PSstatus = 1;
						return;
					}
				} else {
					if (strncmp(PSBuffer, "/config", 7) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /config...
					} else if (strncmp(PSBuffer, "/outputs", 8) == 0) {
						PSstatus = SetSceneParse(PSBuffer); // /ooutputs...
					} else {
						fprintf (log_file, "Warning - %s is not a supported item for Channel preset\r\n", tmpstr);
						PSstatus = 1;
					}
				}
			}
			// Done with the file
			if (Xin) fclose (Xin);
			fprintf (log_file, "Saving Preset %s to X32 Library, at position %d\r\n", XPname, PSstartid);

			// Save Preset to X32 Library
			s_len = Xsprint(s_buf, 0, 's', "/save");
			s_len = Xsprint(s_buf, s_len, 's', ",sis");
			s_len = Xsprint(s_buf, s_len, 's', "librout");		// Routing preset
			i = PSstartid - 1;
			s_len = Xsprint(s_buf, s_len, 'i', &i);				// save index
			s_len = Xsprint(s_buf, s_len, 's', XPname);			// preset name
			SEND_TO(s_buf, s_len)
			// Wait for save to complete
			Xlock = 1;
			while (Xlock) {
				RPOLL; // read the desk for answer to /save
				if (p_status > 0) 	{		// ignore responses that do not correspond to what is
					RECV_FR(r_buf, r_len);	// expected: /save~~~,si~librout~[     1]
					if (strncmp(r_buf, "/save", 5) == 0) {
						for (i = 0; i < 4; i++) endian.cc[3 - i] = r_buf[20 + i];
						if (endian.ii != 1) {
							fprintf (log_file, "Error: Could not save preset to X32 library\r\n");
							PSstatus = 1;
							break;
						}
						Xlock = 0;
					} else {
						PSstatus = 1;
						fprintf (log_file, "Warning: Expected /save... received: %s\r\n", r_buf);
					}
				}
			}
			if (PSstatus == 0) {
				PSstartid += 1; 							// increment index counter
				if (PSstartid > 100) {
					PSstatus = 1;
					fprintf (log_file, "Error: Preset number too high\r\n");
					break;
				} else {
					sprintf(XPSstartid, "%d", PSstartid);		// set buffer
					SetWindowText(hwndstid, (LPSTR)XPSstartid); // ... and display it
				}
			}
			break;
		default:
			fprintf (log_file, "Error - Unsupported file type\r\n");
			PSstatus = 1;
			break;
		}
	} while (*(XPresetFilePath + path_ind) != 0);


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


	}
	return;
}
