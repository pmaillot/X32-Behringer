/*
 * X32DeskRestore.c
 *
 *  Created on: May 8, 2016
 *
 * ©2016 - Patrick-Gilles Maillot
 *
 * X32DeskRestore - a command-line / windows app for restoring a all -prefs and -stat
 * X32 memory states from a PC HDD/file.
 *
 * Change log
 *    0.91: cleaned the code, replacing longer parsing commands by the use of "/" X32 commands
 *          the older code is commented at the end of the file.
 *    0.92: code refactoring - moved some functions to extern
 *    0.93: adapted to FW ver 3.04
 *    0.94: preventing window resizing
 *    1.00: added command-line capability
 *
 */
#ifdef _WIN32
#include <winsock2.h>	// Windows functions for std GUI & sockets
#include <Commdlg.h>
#define MESSAGE(s1,s2)	\
			MessageBox(NULL, s1, s2, MB_OK);
#define zeromem(a1, a2) \
		ZeroMemory(a1, a2);
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_PATH 256	// file name/path size
#define MESSAGE(s1,s2)	\
			printf("%s - %s\n",s2, s1);
#define zeromem(a1, a2) \
		memset((void*)a1, 0, a2);
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#define BSIZE 512
#define MAXLR 512

extern int		X32Connect(int Xconnected, char* Xip_str, int btime);
extern int		validateIP4Dotted(const char *s);
// Private functions
char* getFileNameFromPath(char* path);
int X32DS_GetFile();
void XRcvClean();

//int XDS_parse(char *buf);
//int X_List(char *line_in, int l_index, char** list, int l_len);
//int X_OnOff(char *line_in, int l_index);
//int X_Bits(char *line_in, int l_index);
//int X_Flin(char *line_in, int l_index, float xmin, float lmaxmin, float xstep);
//int X_Flog(char *line_in, int l_index, float xmin, float lmaxmin, float nsteps);
//int X_Int(char *line_in, int l_index);
//int SetiQ(char* line_in, int l, char* st);

//
// External calls used
extern int Xsprint(char *bd, int index, char format, void *bs);
//
//
#ifdef _WIN32
// Windows stuff
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInstance = 0;
HWND		hwndipaddr, hwndconx, hwndprog, hwndrssf, hwndfsave, hwndfname, hwndGO;
RECT		Rect;
HFONT		hfont, htmp;
HDC hdc,	hdcMem;
PAINTSTRUCT	ps;
HBITMAP 	hBmp;
BITMAP		bmp;
MSG			wMsg;

OPENFILENAME ofn;       			// common dialog box structure
HANDLE hf;
//
char Xcomplete[] = "Complete";
char Xready[] = "Ready";
char Xerror[] = "--Error--";
char Xnofile[] = "No file selected";
char Xnotconnected[] = "Not Connected";
FILE *res_file;
char Finipath[1024];			// resolved path to .ini file
char **FinilppPart;
int Finiretval;
//
int wWidth = 545;				// Default window size, superseeded by data read
int wHeight = 130;				// from .ini file
#endif
//
// Global variables
int keep_running = 1;
//
char Xip_str[20], Xpath[256];	// IP in str format; file path
char Xfilename[32];				// used to save the selected file name
char d_version[] = "ver. 1.00";
int Xconnected = 0;				// flags
int Xfiles = 0;					// file info available (valid)

char r_buf[BSIZE], s_buf[BSIZE];	// X32 receive and send buffers
int r_len, s_len;					// associated data lengths
int p_status;						// "polling" status

int Xfd;							// X32 socket
struct sockaddr_in Xip;
struct sockaddr *Xip_addr = (struct sockaddr*) &Xip;
struct timeval timeout;
fd_set ufds;

#ifdef __WIN32__
WSADATA wsa;
int Xip_len = sizeof(Xip);			// length of addresses
#else
socklen_t Xip_len = sizeof(Xip);	// length of addresses
#endif
//
// Macros
#define ISOLATE_ITEM()                                  \
do {                                                    \
	tmp_pt = tmp;										\
	if(line_in[l_index] == '\n') return -1;   			\
        while(line_in[l_index] > 0x20) {                \
          *tmp_pt = line_in[l_index];                   \
          l_index++;                                    \
          tmp_pt++;                                     \
        }                                               \
        *tmp_pt = 0;                                    \
} while (0);

#define WIN_LOOP											\
	do {													\
		while(PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {	\
			TranslateMessage(&wMsg);						\
			DispatchMessage(&wMsg);							\
		}													\
	} while (0);
//
//
#define SEND_TO(b, l)										\
	do {													\
		if (sendto(Xfd, b, l, 0, Xip_addr, Xip_len) < 0) {	\
			printf ("Can't send data to X32\n");			\
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
#define RPOLL												\
	do {													\
		FD_ZERO (&ufds);									\
		FD_SET (Xfd, &ufds);								\
		p_status = select(Xfd+1,&ufds,NULL,NULL,&timeout);	\
	} while (0);
//
#ifdef _WIN32
//
//
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		PWSTR lpCmdLine, int nCmdFile) {

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32DeskRestore";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
		L"X32DeskRestore - Restore X32 State, Scene, data from pattern file information",
		WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 100, 220, wWidth, wHeight, 0, 0, hInstance, 0);
//
// Main loop
	while (keep_running) {
		if (PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		} else {
			Sleep(10);			// this avoids high CPU for nothing
		}
	}
	return (int) wMsg.wParam;
}
//
// Windows Callbacks
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int i;
	char str1[64];

	switch (msg) {
	case WM_CREATE:
//
		hwndipaddr = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 125, 40, 150, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndconx = CreateWindowW(L"button", L"Connect", WS_VISIBLE | WS_CHILD,
				280, 40, 85, 20, hwnd, (HMENU )1, NULL, NULL);

		hwndGO = CreateWindowW(L"button", L"RestoreDeskData",
				WS_VISIBLE | WS_CHILD, 415, 5, 135, 65, hwnd, (HMENU )2, NULL,
				NULL);

		hwndfsave = CreateWindowW(L"button", L"Restore From",
				WS_VISIBLE | WS_CHILD, 125, 75, 95, 20, hwnd, (HMENU )3, NULL,
				NULL);
		hwndfname = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 225, 75, 180, 20, hwnd,
				(HMENU )0, NULL, NULL);

		hwndprog = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 415, 75, 135, 20, hwnd,
				(HMENU )0, NULL, NULL);

		hBmp = (HBITMAP) LoadImage(NULL, (LPCTSTR) "./.X32DeskRestore.bmp",
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_SHARED);
		if (hBmp == NULL) {
			perror("Pixel bitmap file no found");
		}
		break;
//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetWindowText(hwndfname, (LPSTR) Xnofile);
		SetWindowText(hwndipaddr, (LPSTR) Xip_str);
//
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 0, 2, 120, 95, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
//
		SetBkMode(hdc, TRANSPARENT);
		MoveToEx(hdc, 410, wHeight - 35, NULL);
		LineTo(hdc, 410, 2);
//
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 150, 20, str1, wsprintf(str1, "Enter X32 IP below:"));
		TextOut(hdc, 340, 18, str1, wsprintf(str1, d_version));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 0, str1, wsprintf(str1, "X32DeskRestore - ©2016 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		EndPaint(hwnd, &ps);
		break;
//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			switch (LOWORD(wParam)) {
			case 1:
				GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32Connect(Xconnected, Xip_str, 10000);
					if (Xconnected)	{
						SetWindowTextW(hwndconx, L"Connected");
						if (Xfiles)SetWindowText(hwndprog, (LPSTR)Xready);
					} else {
						SetWindowTextW(hwndconx, L"Connect");
						SetWindowText(hwndprog, (LPSTR)Xnotconnected);
					}
				} else {
					printf("Incorrect IP string form\n");
				}
				break;
			case 2:
				// GetDeskData request
				XRcvClean();
				if (X32DS_GetFile()) SetWindowText(hwndprog, (LPSTR) Xerror);
				else                 SetWindowText(hwndprog, (LPSTR) Xcomplete);
				XRcvClean();
				break;
			case 3:	// Restore to file name obtained from dialog
				Xfiles = 0;
				strcpy(Xpath, Xnofile);
				SetWindowText(hwndfname, (LPSTR) Xnofile);
				// Initialize OPENFILENAME
				zeromem(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = Xpath;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				//ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(Xpath);
				ofn.lpstrFilter = "X32 DeskSave\0*.xds\0X32 Scene\0*.scn\0X32 Routing preset\0*.rou\0X32 Pattern-based\0*.*\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrTitle = (LPCTSTR) "Select File to restore your X32 data from\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				//
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
					// Extract filename from returned path so we can either save to
					// existing directory or newly created one
					strcpy(Xfilename, getFileNameFromPath(Xpath));
					if (MessageBox(NULL, Xpath, "Source file path: ", MB_OKCANCEL) == IDOK) {
						SetWindowText(hwndfname, (LPSTR) Xfilename);
						Xfiles = 1;
						if (Xconnected) SetWindowText(hwndprog, (LPSTR)Xready);
						else SetWindowText(hwndprog, (LPSTR)Xnotconnected);
					}
				}
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		if (Xconnected) close(Xfd);
		// Read window and panel data
		GetWindowRect(hwnd, &Rect);
		GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);
		// save window and panel data
		res_file = fopen(Finipath, "wb");
		fprintf(res_file, "%d %d\r\n", (int) (Rect.right - Rect.left), (int) (Rect.bottom - Rect.top));
		fprintf(res_file, "%s\n", Xip_str);
		fclose(res_file);
		keep_running = 0;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
#endif
//
//
//
// Private functions:
//
char* getFileNameFromPath(char* path)
{
   for(size_t i = strlen(path) - 1; i; i--) {
        if (path[i] == '/') {
            return &path[i+1];
        }
    }
    return path;
}
//
//
int X32DS_GetFile() {
	FILE *Xdsfile_pt = NULL;
	char* read_status;
	int err_flag;
	char l_read[MAXLR];
	int keep_reading;

	// Check if Connected
	if (!Xconnected) {
		printf("Not connected to X32!\n");
		return (1);
	}
	// Check if destination path OK
	if (!Xfiles) {
		printf("Error: No file selected\n");
		return (1);
	}
	err_flag = -1;		// consider we may be in an error state (early-end)
	keep_reading = 1;
	//
	// Open the .xds file for reading
	if ((Xdsfile_pt = fopen(Xpath, "r")) != NULL) {
		//
		// At this point of the program, we read /node commands data from the file
		// and parse them into actual X32 OSC commands we eventually send to X32.
		// Node strings are ASCII, ending with a line-feed.
		while (keep_reading) {
			if ((read_status = fgets(l_read, MAXLR, Xdsfile_pt)) != NULL) {
				// ignore comment lines
				if (l_read[0] != '#') {
//
// Use the following bloc of code, lines 374 to 396
					// send a /~~~ command to set data from file directly to X32
					s_len = Xsprint(s_buf, 0, 's', "/");
					s_len = Xsprint(s_buf, s_len, 's', ",s");
					l_read[strlen(l_read) - 1] = 0;
					s_len = Xsprint(s_buf, s_len, 's', l_read); // skip leading '/'
					SEND_TO(s_buf, s_len)
					// manage the echo of the command from X32, ignoring the answer
					RPOLL
					if (p_status < 0) {
						printf("Polling for data failed\n");
						return 0;
					} else if (p_status > 0) {
						// We have received data - process it! (well... ignore it)
						r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);
						if (0) {
							printf("Unexpected answer from X32\n");
							return 0;
						}
					} else {
						// time out...
						printf("X32 reception timeout\n");
//						return 0;
					}
//
// or the call to XDS_parse below
//					// test early-end case
//					if ((err_flag = XDS_parse(l_read)) < 0) keep_reading = 0;
//
// Don't forget to also un-comment the lines at the end of this file
// after the main() {} program
//
				}
			} else {
				keep_reading = 0;
				err_flag = 0;
			}
		}
		// all done, close file
		fclose(Xdsfile_pt);
	} else {
		// cannot open file
		printf("Error opening .xds file: %s\n", Xfilename);
		return (1);
	}
	return (err_flag);
}

void XRcvClean() {
//
// empty reception buffer while data is available
	do {
		RPOLL
		if (p_status > 0) {
			RECV_FR(r_buf, r_len);
		}
	} while (p_status > 0);
	return;
}

int main(int argc, char **argv) {
#ifdef _WIN32
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmdFile = 0;
#else
	int input_intch;
#endif
	Xip_str[0] = 0;
#ifdef WIN32
	// load resource file
	if ((res_file = fopen("./.X32DeskRestore.ini", "r")) != NULL) {
		// get and remember real path
		if ((Finiretval = GetFullPathNameA("./.X32DeskRestore.ini", 1024,
				Finipath, FinilppPart)) > 1024) {
			printf("Not enough space for file name\n");
		}
		fscanf(res_file, "%d %d\r\n", &wWidth, &wHeight);
		fgets(Xip_str, sizeof(Xip_str), res_file);
		Xip_str[strlen(Xip_str) - 1] = 0;
		fclose(res_file);
	}
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
	return 0;
#else
	strcpy(Xip_str, "192.168.1.64");
	// manage command-line parameters
	while ((input_intch = getopt(argc, argv, "i:h")) != -1) {
		switch ((char)input_intch) {
			case 'i':
				strcpy(Xip_str, optarg);
				if (validateIP4Dotted(Xip_str) == 0) {
					printf("Invalid IP address\n");
					return -1;
				}
				break;
			default:
			case 'h':
				printf("X32DeskRestore - ver 1.00 - ©2018 - Patrick-Gilles Maillot\n\n");
				printf("usage: X32DeskRestore [-i X32 console ipv4 address, default: 192.168.1.64]\n");
				printf("                      <Source file name/path>\n");
				printf("X32DeskRestore restores an X32 based on data previously saved to a file using\n");
				printf("X32DeskSave or from a X32 Scene or an X32 Snippet file\n\n");
				return(0);
			break;
		}
	}
	if (argv[optind]) {
		strcpy(Xpath, argv[optind]);
		strcpy(Xfilename, getFileNameFromPath(argv[optind]));
		Xfiles = 1;
	} else {
		MESSAGE(NULL, "No Source file");
		return 1;
	}
	if ((Xconnected = X32Connect(0, Xip_str, 20000)) == 1) {
		XRcvClean();
		return X32DS_GetFile();
	}
	MESSAGE(NULL, "No X32 found!");
	return 1;
#endif
}

////////////////////////////////////////////////////////////////////////////////
////
//// Parsing functions for interpreting /node lines
////
//// String parsing
//int X_Str(char *line_in, int l_index) {
//
//	char tmp[16];
//	char* tmp_pt;
//
//	tmp_pt = tmp;
//	if(line_in[l_index] == '\n') return -1;
//	while (line_in[l_index] != '"') l_index++;
//	l_index += 1;
//	while(line_in[l_index] != '"') {
//		*tmp_pt = line_in[l_index];
//		l_index++;
//		tmp_pt++;
//	}
//	*tmp_pt = 0;
//// deploy value to s_buf[]/s_len
//    s_len = Xsprint(s_buf, s_len, 's', ",s");
//    s_len = Xsprint(s_buf, s_len, 's', tmp);
//	return (l_index + 2);
//}
////
//// 32 bit int parsing
//int X_Int(char *line_in, int l_index) {
//
//	char tmp[16];
//	char* tmp_pt;
//	int value;
//
//	ISOLATE_ITEM() // gets next item in tmp
//// convert to value;
//	sscanf(tmp, "%i", &value);
//// deploy value to s_buf[]/s_len
//    s_len = Xsprint(s_buf, s_len, 's', ",i");
//	s_len = Xsprint(s_buf, s_len, 'i', &value);
//	return (++l_index);
//}
////
//// float following a linear scale parsing
//int X_Flin(char *line_in, int l_index, float xmin, float lmaxmin, float xstep) {
//
//	char tmp[16];
//	char* tmp_pt;
//	float value;
//
//	ISOLATE_ITEM() // gets next item in tmp
//// convert to value;
//	sscanf(tmp, "%f", &value);
//	value = (value - xmin) / lmaxmin;
//	xstep = lmaxmin/xstep;
//	value = roundf(value*xstep) / xstep;
//	if (value <= 0.) value = 0.; // avoid -0.0 values (0x80000000)
//	if (value > 1.) value = 1.;
//// deploy value to s_buf[]/s_len
//    s_len = Xsprint(s_buf, s_len, 's', ",f");
//	s_len = Xsprint(s_buf, s_len, 'f', &value);
//	return (++l_index);
//}
////
//// float following a log scale parsing
//int X_Flog(char *line_in, int l_index, float xmin, float lmaxmin, float nsteps) {
//	char tmp[16];
//	char* tmp_pt;
//	float value;
//
//	ISOLATE_ITEM() // gets next item in tmp
//// convert to value;
//	sscanf(tmp, "%f", &value);
//	value = log(value / xmin) / lmaxmin; // lmaxmin = log(xmax / xmin)
//// round to nsteps' value of log()
//	value = roundf(value * nsteps) / nsteps;
//	if (value <= 0.) value = 0.; // avoid -0.0 values (0x80000000)
//	if (value > 1.) value = 1.;
//// deploy value to s_buf[]/s_len
//	s_len = Xsprint(s_buf, s_len, 's', ",f");
//	s_len = Xsprint(s_buf, s_len, 'f', &value);
//	return (++l_index);
//}
////
//// bit field from string parsing (results in an int)
//int X_Bits(char *line_in, int l_index) {
//
//	int value, i;
//
//	value = 0;
//	i = l_index;
//	if(line_in[l_index] == '\n') return -1;
//	while (line_in[i] != '%') i++;
//	i += 1;
//	while (line_in[i] > 32) {
//		value = value << 1;
//		if (line_in[i] == '1') {
//			value = value | 1;
//		} else if (line_in[i] != '0') return -1;
//		i += 1;
//	}
//// deploy value to s_buf[]/s_len
//    s_len = Xsprint(s_buf, s_len, 's', ",i");
//	s_len = Xsprint(s_buf, s_len, 'i', &value);
//	return (i + 1);
//}
////
//// Boolean (OFF/ON) parsing (results in an int)
//int X_OnOff(char *line_in, int l_index) {
//
//	char tmp[16];
//	char* tmp_pt;
//	int value;
//
//	ISOLATE_ITEM() // gets next item in tmp
//// convert to value;
//	value = 0;
//	if (strcmp(tmp, "OFF") != 0) value = 1;
//// deploy value to s_buf[]/s_len
//    s_len = Xsprint(s_buf, s_len, 's', ",i");
//	s_len = Xsprint(s_buf, s_len, 'i', &value);
//	return (++l_index);
//}
////
//// List choice (itemA, itemB, ...) parsing (results in an int)
//int X_List(char *line_in, int l_index, char** list, int l_len) {
//
//	char tmp[16];
//	char* tmp_pt;
//	int value;
//
//	ISOLATE_ITEM() // gets next item in tmp
//// convert to value;
//	while (l_len--) {
//		if (strcmp(tmp, list[l_len]) == 0) {
//			value = l_len;
//// deploy value to s_buf[]/s_len
//		    s_len = Xsprint(s_buf, s_len, 's', ",i");
//			s_len = Xsprint(s_buf, s_len, 'i', &value);
//			return (++l_index);
//		}
//	}
//	return -2;  // error: not found in list
//}
//
//
///////////////
////
//// List of constants and arrays of datasets used for list parsing
//char* r_prot[] = { "MC", "HUI", "CC" };
//int l_prot = (sizeof(r_prot) / sizeof(char*));
//char* r_port[] = { "MIDI", "CARD", "RTP" };
//int l_port = (sizeof(r_port) / sizeof(char*));
//char* r_iQm[] = { "none", "iQ8", "iQ10", "iQ12", "iQ15", "iQ15B", "iQ18B" };
//int l_iQm = (sizeof(r_iQm) / sizeof(char*));
//char* r_iQe[] = { "none", "Linear", "Live", "Speech", "Playback", "User" };
//int l_iQe = (sizeof(r_iQe) / sizeof(char*));
//char* r_UF[] = { "FW", "USB" };
//int l_UF = (sizeof(r_UF) / sizeof(char*));
//char* r_US[] = { "32/32", "16/16", "32/8", "8/32", "8/8", "2/2" };
//int l_US = (sizeof(r_US) / sizeof(char*));
//char* r_CR[] = { "48K", "44K1" };
//int l_CR = (sizeof(r_CR) / sizeof(char*));
//char* r_CS[] = { "INT", "AES50A", "AES50B", "CARD" };	//Todo: Validate "CARD"
//int l_CS = (sizeof(r_CS) / sizeof(char*));
//char* r_SH[] = { "CUES", "SCENES", "SNIPPETS" };
//int l_SH = (sizeof(r_SH) / sizeof(char*));
//char* r_CL[] = { "24h", "12h" };
//int l_CL = (sizeof(r_CL) / sizeof(char*));
//char* r_IM[] = { "NORM", "INV" };
//int l_IM = (sizeof(r_IM) / sizeof(char*));
//char* r_ST[] = { "STOP", "PPAUSE", "PLAY", "RPAUSE", "REC", "FF", "REW" };
//int l_ST = (sizeof(r_ST) / sizeof(char*));
//char* r_sSs[] = { "CHAN", "METER", "ROUTE", "SETUP", "LIB", "FX", "MON", "USB", "SCENE", "ASSIGN", "LOCK" };
//int l_sSs = (sizeof(r_sSs) / sizeof(char*));
//char* r_sCp[] = { "HOME", "FX1", "FX2", "FX3", "FX4", "FX5", "FX6", "FX7", "FX8" };
//int l_sCp = (sizeof(r_sCp) / sizeof(char*));
//char* r_sEp[] = { "CHANNEL", "MIXBUS", "AUX/FX", "IN/OUT", "RTA" };
//int l_sEp = (sizeof(r_sEp) / sizeof(char*));
//char* r_sRp[] = { "HOME", "ANAOUT", "AUXOUT", "P16OUT", "CARDOUT", "AES50A", "AES50B", "XLROUT" };
//int l_sRp = (sizeof(r_sRp) / sizeof(char*));
//char* r_sTp[] = { "GLOB", "CONF", "REMOTE", "NETW", "NAMES", "PREAMPS", "CARD" };
//int l_sTp = (sizeof(r_sTp) / sizeof(char*));
//char* r_sLp[] = { "CHAN", "EFFECT", "ROUTE" };
//int l_sLp = (sizeof(r_sLp) / sizeof(char*));
//char* r_sFp[] = { "HOME", "FX1", "FX2", "FX3", "FX4", "FX5", "FX6", "FX7", "FX8" };
//int l_sFp = (sizeof(r_sFp) / sizeof(char*));
//char* r_sMp[] = { "MONITOR", "TALKA", "TALKB", "OSC" };
//int l_sMp = (sizeof(r_sMp) / sizeof(char*));
//char* r_sUp[] = { "HOME", "CONFIG" };
//int l_sUp = (sizeof(r_sUp) / sizeof(char*));
//char* r_sSp[] = { "HOME", "SCENES", "BITS", "PARSAFE", "CHNSAFE", "MIDI" };
//int l_sSp = (sizeof(r_sSp) / sizeof(char*));
//char* r_sAp[] = { "HOME", "SETA", "SETB", "SETC" };
//int l_sAp = (sizeof(r_sAp) / sizeof(char*));
//char* r_sEl[] = {"Ch01", "Ch02", "Ch03", "Ch04", "Ch05", "Ch06", "Ch07", "Ch08", "Ch09", "Ch10",
//		"Ch11", "Ch12", "Ch13", "Ch14", "Ch15", "Ch16", "Ch17", "Ch18", "Ch19", "Ch20",
//		"Ch21", "Ch22", "Ch23", "Ch24", "Ch25", "Ch26", "Ch27", "Ch28", "Ch29", "Ch30",
//		"Ch31", "Ch32",
//		"Aux01", "Aux02", "Aux03", "Aux04", "Aux05", "Aux06", "Aux07", "Aux08",
//		"Fx1L", "Fx1R", "Fx2L", "Fx2R", "Fx3L", "Fx3R", "Fx4L", "Fx4R",
//		"Bus1", "Bus2", "Bus3", "Bus4", "Bus5", "Bus6", "Bus7", "Bus8", "Bus9",
//		"Bs10", "Bs11", "Bs12", "Bs13", "Bs14", "Bs15", "Bs16",
//		"Mtx1", "Mtx2", "Mtx3", "Mtx4", "Mtx5", "Mtx6",
//		"LR", "M/C" };
//int l_sEl = (sizeof(r_sEl) / sizeof(char*));
//
//char* r_bAr[] = { "BAR", "SPEC" };
//int l_bAr = (sizeof(r_bAr) / sizeof(char*));
//char* r_pRe[] = { "PRE", "POST" };
//int l_pRe = (sizeof(r_pRe) / sizeof(char*));
//char* r_rMs[] = { "RMS", "PEAK" };
//int l_rMs = (sizeof(r_rMs) / sizeof(char*));
//char* r_rPh[] = { "OFF", "1", "2", "3", "4", "5", "6", "7", "8" };
//int l_rPh = (sizeof(r_rPh) / sizeof(char*));
//char* r_rTv[] = { "OFF", "25%", "30%", "35%", "40%", "45%", "50%", "55%", "60%", "65%", "70%", "75%", "80%"};
//int l_rTv = (sizeof(r_rTv) / sizeof(char*));
////
//// Utility functions
//int SetiQ(char* line_in, int l, char* st) {
//char	tmp[32];
//
//	strcpy(tmp, st);
//	s_len = Xsprint(s_buf, 0, 's', strcat(tmp, "/iQmodel"));
//	if ((l = X_List(line_in, l, r_iQm, l_iQm)) < 0) return (l);
//	SEND_TO(s_buf, s_len)
//	// send to X32
//	strcpy(tmp, st);
//	s_len = Xsprint(s_buf, 0, 's', strcat(tmp, "/iQeqset"));
//	if ((l = X_List(line_in, l, r_iQe, l_iQe)) < 0) return (l);
//	SEND_TO(s_buf, s_len)
//	// send to X32
//	strcpy(tmp, st);
//	s_len = Xsprint(s_buf, 0, 's', strcat(tmp, "/iQsound"));
//	if ((l = X_Int(line_in, l)) < 0) return (l);
//	SEND_TO(s_buf, s_len)
//	// send to X32
//	return (0);
//}
////
////
//// XDE_parse,
////
//// This is the main parsing function for the program, restoring X32 State as we progress
//// buffers we receive here.
////
//// The buffers are made of "node" command replies.
//// Each buffer corresponds to a node line
//// Each of these lines will be in the following format:
//// /-xxx abc def ghi ... xyz\n
////
//// /-xxx is one of the strings included in ds_node[]
////    then for each /-xxx line, a specific format (list of abc...) exists and conveys
////                             a series of respective X32 commands; There are typically
////                             several values for abc, and a specific parsing needs to take
////                             place, such as done in X32SetScene, but for specific node
////                             commands.
////
//// Despite the fact the node commands will most likely always come from the same save
//// program, we'll use the same principles as for X32SetScene, where /-xxx commands can be
//// in any order and possibly incomplete without generating errors. This comes to the
//// expense of additional computing, but this is no big deal here.
////
//// todo: validate if the use of the "/~~~" command would work here; is it
//// as flexible as an argument to argument parsing as done her, enabling
//// early-end? Benefits are obvious though (a lot less data and lees code lines), plus
//// unknown arguments are automagically taken into account by the X32
////
//int XDS_parse(char *line_in) {
//
//	int node_idx;
//	char node_string[32], solotxt[32];
//	int i, l;
//
//	// find out which node command we need to parse (skip the systematic "/-"
//	// the buffer starts with
//	sscanf(line_in + 1, "%s", node_string);
//	for (node_idx = 0; node_idx < ds_node_max; node_idx++) {
//		if (strcmp(node_string, ds_node[node_idx] + 1) == 0) { // "+ 1" to skip heading '/'
//			// advance index (don't forget heading '/' and trailing ' '
//			l = strlen(ds_node[node_idx]) + 1;
//			//
//			// start specific parsing per command
//			switch (node_idx) {
//			case stat:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/selidx");
//				if ((l = X_List(line_in, l, r_sEl, l_sEl)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/chfaderbank");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/grpfaderbank");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/sendsonfader");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/bussendbank");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/eqband");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/solo");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/keysolo");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/userbank");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/autosave");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/lock");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/usbmounted");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/remote");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/rtamodeeq");
//				if ((l = X_List(line_in, l, r_bAr, l_bAr)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/rtamodegeq");
//				if ((l = X_List(line_in, l, r_bAr, l_bAr)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/rtaeqpre");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/rtaeqpost");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/rtasource");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/xcardtype");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/xcardsync");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/geqonfdr");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/geqpos");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				break;
//			case stat_screen:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/screen");
//				if ((l = X_List(line_in, l, r_sSs, l_sSs)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/mutegrp");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/utils");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				break;
//			case stat_screen_CHAN:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/CHAN/page");
//				if ((l = X_List(line_in, l, r_sCp, l_sCp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_screen_METER:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/METER/page");
//				if ((l = X_List(line_in, l, r_sEp, l_sEp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_screen_ROUTE:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/ROUTE/page");
//				if ((l = X_List(line_in, l, r_sRp, l_sRp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_screen_SETUP:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/SETUP/page");
//				if ((l = X_List(line_in, l, r_sTp, l_sTp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_screen_LIB:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/LIB/page");
//				if ((l = X_List(line_in, l, r_sLp, l_sLp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_screen_FX:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/FX/page");
//				if ((l = X_List(line_in, l, r_sFp, l_sFp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_screen_MON:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/MON/page");
//				if ((l = X_List(line_in, l, r_sMp, l_sMp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_screen_USB:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/USB/page");
//				if ((l = X_List(line_in, l, r_sUp, l_sUp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_screen_SCENE:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/SCENE/page");
//				if ((l = X_List(line_in, l, r_sSp, l_sSp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				break;
//			case stat_screen_ASSIGN:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/screen/ASSIGN/page");
//				if ((l = X_List(line_in, l, r_sAp, l_sAp)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				break;
//			case stat_tape:
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/tape/state");
//				if ((l = X_List(line_in, l, r_ST, l_ST)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/tape/file");
//				if ((l = X_Str(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/tape/etime");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-stat/tape/rtime");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_osc:
//				s_len = Xsprint(s_buf, 0, 's', ds_node[node_idx]);
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case stat_solosw:
////todo: one or 80 lines?
//				for (i = 0; i < 80; i++) {
//					Sleep(3);
//					sprintf(solotxt, "/-stat/solosw/%02d", i);
//					s_len = Xsprint(s_buf, 0, 's', solotxt);
//					if ((l = X_OnOff(line_in, l)) < 0) return (l);
//					SEND_TO(s_buf, s_len) // send to X32
//				}
//				break;
//			case prefs:
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/style");
//				if ((l = X_Str(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/bright");
//				if ((l = X_Flin(line_in, l, 10., 90., 5.)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/lcdcont");
//				if ((l = X_Flin(line_in, l, 0., 100., 2.)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ledbright");
//				if ((l = X_Flin(line_in, l, 10., 90., 5.)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/lamp");
//				if ((l = X_Flin(line_in, l, 10., 90., 10.)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/lampon");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/clockrate");
//				if ((l = X_List(line_in, l, r_CR, l_CR)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/clocksource");
//				if ((l = X_List(line_in, l, r_CS, l_CS)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/confirm_general");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/confirm_overwrite");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/confirm_sceneload");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/viewrtn");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/selfollowbank");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/sceneadvance");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/safe_masterlevels");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/haflags");
//				if ((l = X_Bits(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/autosel");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/show_control");
//				if ((l = X_List(line_in, l, r_SH, l_SH)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/clockmode");
//				if ((l = X_List(line_in, l, r_CL, l_CL)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/hardmute");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/dcsmute");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/invertmute");
//				if ((l = X_List(line_in, l, r_IM, l_IM)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
////todo: ... Last one is a string, unknown for now... could it correspond to iQ/group???
////				s_len = Xsprint(s_buf, 0, 's', "/-prefs/xxxxxxxx");
////				if ((l = X_Str(line_in, l)) < 0 ) return (l);
////				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case prefs_card:
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/card/UFifc");
//				if ((l = X_List(line_in, l, r_UF, l_UF)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/card/UFmode");
//				if ((l = X_List(line_in, l, r_US, l_US)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/card/USBmode");
//				if ((l = X_List(line_in, l, r_US, l_US)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				// The rest of card... is unknown at this time
//				break;
//			case prefs_rta:
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/visibility");
//				if ((l = X_List(line_in, l, r_rTv, l_rTv)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/gain");
//				if ((l = X_Flin(line_in, l, 0.0, 60.0, 6.)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/autogain");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/source");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/pos");
//				if ((l = X_List(line_in, l, r_pRe, l_pRe)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/mode");
//				if ((l = X_List(line_in, l, r_bAr, l_bAr)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/option");
//				if ((l = X_Bits(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/det");
//				if ((l = X_List(line_in, l, r_rMs, l_rMs)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
////todo: Decay sent as a logf or as a float with 2 digits after decimal point?
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/decay");
//				if ((l = X_Flog(line_in, l, .25, 15.75, 19)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/rta/peakhold");
//				if ((l = X_List(line_in, l, r_rPh, l_rPh)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case prefs_ip_dhcp:
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/dhcp");
//				if ((l = X_OnOff(line_in, l)) < 0 ) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case prefs_ip_addr:
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/addr/0");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/addr/1");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/addr/2");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/addr/3");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case prefs_ip_mask:
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/mask/0");
//				if ((l = X_Int(line_in, l))< 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/mask/1");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/mask/2");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/mask/3");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case prefs_ip_gateway:
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/gateway/0");
//				if ((l = X_Int(line_in, l))< 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/gateway/1");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/gateway/2");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/ip/gateway/3");
//				if ((l = X_Int(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case prefs_remote:
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/remote/enable");
//				if ((l = X_OnOff(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/remote/protocol");
//				if ((l = X_List(line_in, l, r_prot, l_prot)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/remote/port");
//				if ((l = X_List(line_in, l, r_port, l_port)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				s_len = Xsprint(s_buf, 0, 's', "/-prefs/remote/ioenable");
//				if ((l = X_Bits(line_in, l)) < 0) return (l);
//				SEND_TO(s_buf, s_len) // send to X32
//				break;
//			case prefs_iQ_01: case prefs_iQ_02: case prefs_iQ_03: case prefs_iQ_04:
//			case prefs_iQ_05: case prefs_iQ_06: case prefs_iQ_07: case prefs_iQ_08:
//			case prefs_iQ_09: case prefs_iQ_10: case prefs_iQ_11: case prefs_iQ_12:
//			case prefs_iQ_13: case prefs_iQ_14: case prefs_iQ_15: case prefs_iQ_16:
//				if (SetiQ(line_in, l, ds_node[node_idx]) < 0) return (1);
//				break;
//			case action:
//				// -action returned node data is ignored at this time, not sure
//				// it needs to be taken into consideration
//				break;
//			default: // ignore unknown data
//				break;
//			}
//		}
//	}
//	// all OK
//	return (0);
//}




