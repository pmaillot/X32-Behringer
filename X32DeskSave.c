/*
 * X32DeskSave.c
 *
 *  Created on: May 6, 2016
 *
 * ©2016 - Patrick-Gilles Maillot
 *
 * X32DeskSave - a Command-line / windows app for saving a all -prefs and -stat
 * X32 memory states to a PC HDD.
 *
 *
 * Change log
 *    0.91: cleaned the code for first release with an attempt at .snp, .efx, and .chn handling
 *          not working as initially thought of but code commented; I may get back to this
 *    0.92: code refactoring - moved some functions to extern
 *    0.93: adapted to FW ver 3.04
 *    0.94: preventing windows resizing
 *    1.00: Added Command-line capability
 */
#ifdef _WIN32
#include <winsock2.h>	// Windows functions for std GUI & sockets
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

#include "X32ds_node.h"

#define BSIZE 512
// External calls used
extern int		Xsprint(char *bd, int index, char format, void *bs);
extern int		X32Connect(int Xconnected, char* Xip_str, int btime);
extern int		validateIP4Dotted(const char *s);
//
// Private functions
char*	getFileNameFromPath(char* path);
int		X32DS_GetFile();
void 	XRcvClean();
//
//
#ifdef _WIN32
WINBASEAPI HWND WINAPI	GetConsoleWindow(VOID);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE	hInstance = 0;
HWND 		hwndipaddr, hwndconx, hwndprog, hwndrssf, hwndfsave, hwndfname, hwndGO;
HWND 		hwndptyp, hwndffrom, hwndfpatn;
RECT		Rect;
HFONT		hfont, htmp;
HDC			hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP		hBmp;
BITMAP		bmp;
MSG			wMsg;

OPENFILENAME ofn;       // common dialog box structure
HANDLE hf;              // file handle
//
char	Xnotconnected[] = "Not Connected";
char	Xcomplete[]     = "Complete";
char	Xready[]        = "Ready";
char	Xerror[]        = "--Error--";
char	Xnofile[]	    = "No file selected";
char	Xnotype[]	    = "Choose first!";

char	*Xfilter[] = {"X32 DeskSave\0*.xds\0\0",
		"X32 Scene\0*.scn\0\0",
		"X32 Routing Preset\0*.rou\0\0",
		"X32 from pattern file, add your extension\0*.*\0\0"
		//		"X32 Snippet\0*.snp\0\0",
		//		"X32 Channel Preset\0*.chn\0\0",
		//		"X32 Effect Preset\0*.efx\0\0",
};

char	*Xext[] = {".xds\0",
				".scn\0",
				".rou\0,"
				".*\0"
//				".snp\0",
//				".chn\0",
//				".efx\0",
};

char		*ActType[] = {"Choose:", "DeskSave", "Scene", "Routing", "Pattern", "\0"};
int			Actsize = sizeof(ActType) / sizeof(char*) - 1;
//SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Choose:");
//SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"DeskSave");
//SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Scene");
//SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Snippet");
//SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Channel");
//SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Effects");
//SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Routing");
//SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Pattern");


FILE 	*res_file;
char	Finipath[1024];	// resolved path to .ini file
char	**FinilppPart;
int 	Finiretval;

int		keep_running = 1;
int		wWidth = 545;
int		wHeight = 130;
#endif
//
char	Xip_str[20];
char	Xfile_w_path[256];
char	Xfile_r_path[256];
char	Xfile_w_name[32];	// used to save the selected file name to create
char	Xfile_r_name[32];	// used to save the selected file name used as pattern file

int		Xconnected = 0;	// Flag indicating if we're connected to X32
int		Xfiles = 0;		// Flag indicating if a file has been selected for saving
int		Xptype = 0;		// Flag indicating the type of file we're saving

// X32 Communication buffers (in and out)
char				r_buf[BSIZE], s_buf[BSIZE];
int					r_len, s_len;

struct sockaddr_in	Xip;					// X32 IP address
#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
#endif
int					Xfd;					// X32 socket
struct sockaddr		*Xip_addr = (struct sockaddr*)&Xip;
struct timeval		timeout;				// non-blocking timeout
int					p_status;				// X32 read  status
fd_set 				ufds;					// file descriptor
//
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
			MESSAGE("Can't send data to X32","");			\
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
//
//
//
#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdFile) {

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32DeskSave";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"X32DeskSave - Retrieve and save X32 State, Scene, data from pattern file information",
			WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU,
			100, 220, wWidth, wHeight, 0, 0, hInstance, 0);
//
	while (keep_running) {
		if(PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		} else {
			Sleep(10);
		}
	}
	return (int) wMsg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

int 	i;
char 	str1[64];

	switch (msg) {
	case WM_CREATE:
//
		hwndipaddr = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				125, 35, 130, 20, hwnd,(HMENU)0, NULL, NULL);
		hwndconx = CreateWindowW(L"button", L"Connect", WS_VISIBLE | WS_CHILD,
				260, 35, 85, 20, hwnd,(HMENU)1, NULL, NULL);

		hwndGO = CreateWindowW(L"button", L"SaveDeskData", WS_VISIBLE | WS_CHILD,
				395, 5, 135, 75, hwnd,(HMENU)2, NULL, NULL);

		hwndptyp = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				300, 60, 85, 170, hwnd, (HMENU)7, NULL, NULL);
        // Load drop-down item list
		for (i = 0; i < Actsize; i++) {
			SendMessage(hwndptyp, CB_ADDSTRING, (WPARAM)0, (LPARAM)ActType[i]);
		}
        // Set Default value
	    SendMessage(hwndptyp, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	    hwndffrom = CreateWindowW(L"button", L"Pat. File", WS_VISIBLE | WS_CHILD,
				125, 60, 65, 20, hwnd,(HMENU)4, NULL, NULL);
		hwndfpatn = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				195, 60, 100, 20, hwnd, (HMENU)0, NULL, NULL);

		hwndfsave = CreateWindowW(L"button", L"Save As", WS_VISIBLE | WS_CHILD,
				125, 85, 65, 20, hwnd,(HMENU)3, NULL, NULL);
		hwndfname = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				195, 85, 190, 20, hwnd, (HMENU)0, NULL, NULL);

		hwndprog = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				395, 85, 135, 20, hwnd, (HMENU)0, NULL, NULL);

		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./.X32DeskSave.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_SHARED);
		if(hBmp==NULL) {
			perror("Pixel bitmap file no found");
		}
		break;
		//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		//
		SetWindowText(hwndfname, (LPSTR)Xnofile);
		SetWindowText(hwndipaddr, (LPSTR)Xip_str);
		//
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 0, 2, 120, 100, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
		//
		SetBkMode(hdc, TRANSPARENT);
		MoveToEx(hdc, 390, wHeight - 30, NULL);
		LineTo(hdc, 390, 2);
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
		TextOut(hdc, 125, 0, str1, wsprintf(str1, "X32DeskSave - ©2016 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		EndPaint(hwnd, &ps);
		break;
		//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			switch(LOWORD(wParam)) {
			case 1:
				GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32Connect(Xconnected, Xip_str, 10000);
					if (Xconnected)	{
						SetWindowTextW(hwndconx, L"Connected");
						if( Xfiles)SetWindowText(hwndprog, (LPSTR)Xready);
					} else {
						SetWindowTextW(hwndconx, L"Connect");
						SetWindowText(hwndprog, (LPSTR)Xnotconnected);
					}
				} else {
					MESSAGE("Error", "Incorrect IP string form");
				}
				break;
			case 2:
				// GetDeskData request
				XRcvClean();
				if (X32DS_GetFile()) {
					SetWindowText(hwndprog, (LPSTR)Xerror);
				} else {
					SetWindowText(hwndprog, (LPSTR)Xcomplete);
				}
				XRcvClean();
				break;
			case 3:	// Save to file name obtained from dialog
				Xfiles = 0;
			    Xptype = SendMessage(hwndptyp, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			    if  (Xptype) {
					strcpy(Xfile_w_path, Xnofile);
					SetWindowText(hwndprog, (LPSTR)"");
					SetWindowText(hwndfname, (LPSTR)Xnofile);
					// Initialize OPENFILENAME
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hwnd;
					ofn.lpstrFile = Xfile_w_path;
					// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
					// use the contents of szFile to initialize itself.
					//ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(Xfile_w_path);
					ofn.lpstrFilter = Xfilter[Xptype - 1];
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrTitle = (LPCTSTR)"Select File to save your X32 data to\0";
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_EXPLORER | OFN_CREATEPROMPT |
								OFN_HIDEREADONLY | OFN_NOVALIDATE |
								OFN_PATHMUSTEXIST | OFN_READONLY;
					//
					// Display the Open dialog box.
					if (GetSaveFileName(&ofn)) {
						// test if an extension is already present; if not add one
						i = strlen(Xfile_w_path);
						while (i && (Xfile_w_path[i] != '.')) --i;
						if (i == 0) {
							// add an extension
							strcat(Xfile_w_path, Xext[Xptype - 1]);
						}
						// Extract filename from returned path so we can either save to
						// existing directory or newly created one
						strcpy(Xfile_w_name, getFileNameFromPath(Xfile_w_path));
						if (MessageBox(NULL, Xfile_w_path, "Destination File path: ", MB_OKCANCEL) == IDOK) {
							SetWindowText(hwndfname, (LPSTR)Xfile_w_name);
							Xfiles = 1;
							if (Xconnected) SetWindowText(hwndprog, (LPSTR)Xready);
							else SetWindowText(hwndprog, (LPSTR)Xnotconnected);
						}
					}
			    } else {
			    	SetWindowText(hwndprog, (LPSTR)Xnotype);
			    }
				break;
			case 4:	// pattern file file name obtained from dialog
				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = Xfile_r_path;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(Xfile_r_path);
				ofn.lpstrFilter = 0;
				ofn.nFilterIndex = 0;
//				ofn.lpstrFilter = "Channel Presets\0*.chn\0Effect Presets\0*.efx\0Routing Presets\0*.rou\0\0";
//				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = Xfile_r_name;
				ofn.nMaxFileTitle = sizeof(Xfile_r_name);
				ofn.lpstrTitle = (LPCTSTR)"Select Preset Files from the list below\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
				//
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
//					printf("Path: %s\n", Xfile_r_path);
//					printf("Name: %s\n", Xfile_r_name);
//					fflush(stdout);
					SetWindowText(hwndfpatn, (LPSTR)Xfile_r_name);
				}
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		if (Xconnected) close (Xfd);
		// Read window and panel data
		GetWindowRect(hwnd, &Rect);
		i = GetWindowTextLength(hwndipaddr) + 1;
		GetWindowText(hwndipaddr, Xip_str, i);
		// save window and panel data
		res_file = fopen(Finipath, "wb");
		fprintf(res_file, "%d %d\r\n", (int)(Rect.right - Rect.left), (int)(Rect.bottom - Rect.top));
		fprintf(res_file, "%s\n", Xip_str);
		fclose (res_file);
		keep_running = 0;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
#else
#endif
//
//
// Private functions:
//
//
int X32DS_GetFile() {
	int			i, j, k, p_status;
//	int			number;
	FILE		*Xfile_w_pt = NULL;
	FILE		*Xfile_r_pt = NULL;
	int			num_nodes, keep_reading;
	char*		*nodes_ptr, *read_status;
	char		l_read[BSIZE];

	// Check if Connected
	if (!Xconnected) {
		MESSAGE("Not connected to X32!","");
		return (1);
	}
	// Check if destination path OK
	if (!Xfiles) {
		MESSAGE("Error", "No file selected");
		return (1);
	}
	nodes_ptr = 0;
	num_nodes = 0;
	//
    // Open the destination file for writing
    if ((Xfile_w_pt = fopen (Xfile_w_path, "wb")) != NULL) {
    	//
    	// Chose what set of data to query to X32
    	switch (Xptype) {
    	case 1: //.dsk
    		num_nodes = ds_node_max;
    		nodes_ptr = ds_node;
    		fprintf(Xfile_w_pt,"%s%s%s\n", "#2.1# 0 \"", Xfile_w_name, "\" 0 %000000000 1");
    		break;
    	case 2:	//.scn
    		num_nodes = sc_node_max;
    		nodes_ptr = sc_node;
    		fprintf(Xfile_w_pt,"%s%s%s\n", "#2.1# \"", Xfile_w_name, "\" \"\" %000000000 1");
    		break;
    	case 3:	//.rou
    		num_nodes = ro_node_max;
    		nodes_ptr = ro_node;
    		fprintf(Xfile_w_pt,"%s%s%s\n", "#2.1# 0 \"", Xfile_w_name, "\" 0 %0000000000000000 1");
    		break;
    	case 4: // pattern file
    		keep_reading = 1;
    		break;
//    	case 3:	//.snp
//    		num_nodes = sn_node_max;
//    		nodes_ptr = sn_node;
//    		break;
//    	case 4:	//.chn
//    		num_nodes = ch_node_max;
//    		nodes_ptr = ch_node;
//    		// get number from hwndfpatn handle
//    		GetWindowTextW(hwndfpatn, l_read, GetWindowTextLengthW(hwndfpatn) + 1);
//    		sscanf(l_read, "%i", &number);
//			fprintf(Xfile_w_pt,"%s%s%s\n", "#2.1# 1 \"", Xfile_w_name, "\" 0 %0011111100000000 1");
//    		break;
//    	case 5:	//.efx
//    		num_nodes = fx_node_max;
//    		nodes_ptr = fx_node;
//       		// get number from hwndfpatn handle
//			GetWindowTextW(hwndfpatn, l_read, GetWindowTextLengthW(hwndfpatn) + 1);
//			sscanf(l_read, "%i", &number);
//			fprintf(Xfile_w_pt,"%s%s%s\n", "#2.1# 1 \"", Xfile_w_name, "\" 1 31 1");
//    		break;
    	default:
    		break;
    	}
		//
		// At this point of the program, we send /node commands to X32 and
		// receive data back from the desk, saving the node strings in files,
		// as we receive them. Node strings are ASCII, ending with a linefeed.
        switch (Xptype) {
        case 1: //.dsk
        case 2:	//.scn
    	case 3:	//.rou
			k = Xsprint(s_buf, 0, 's', "/node");
			k = Xsprint(s_buf, k, 's', ",s");
			for (i= 0; i < num_nodes; i++) {
				s_len = Xsprint(s_buf, k, 's', (nodes_ptr[i] + 1)); // "+ 1" to avoid the heading '/'
				SEND_TO(s_buf, s_len);
				RPOLL;
				if (p_status > 0) {
					RECV_FR(r_buf, r_len);
					if (r_len > 0) {
						j = 0;
						while((r_buf[j] != '/') && (j < r_len)) j++;
						// print reply (starting at first '/' found) to Xfile_w_pt;
						// data comes with trailing \a and no \0 chars
						fprintf(Xfile_w_pt, "%s", r_buf + j);
					}
				} else {
					MESSAGE("Error or timeout", "on receiving data from X32");
				}
			}
			break;
//    	case 4:	//.chn
//    		// Channel preset from CH in "number"
//    		// build request /node command
//			s_len = Xsprint(s_buf, 0, 's', "/node");
//			s_len = Xsprint(s_buf, s_len, 's', ",s");
//			k = s_len;
//			sprintf (l_read, "ch/%02i/", number);
//			for (i= 0; i < num_nodes; i++) {
//				strcpy(l_read + 5, nodes_ptr[i]);
//				s_len = Xsprint(s_buf, k, 's', l_read);
//				SEND_TO(s_buf, s_len);
//				RPOLL;
//				if (p_status > 0) {
//					RECV_FR(r_buf, r_len);
//					if (r_len > 0) {
//						// print reply (starting at char 5 to Xfile_w_pt;
//						// data comes with trailing \a and no \0 chars
//						fprintf(Xfile_w_pt, "%s", r_buf + 18);
//					}
//				} else {
//					MESSAGE("Error or timeout", "on receiving data from X32");
//				}
//			}
//			s_len = Xsprint(s_buf, k, 's', "/headamp/000");
//			SEND_TO(s_buf, s_len);
//			RPOLL;
//			if (p_status > 0) {
//				RECV_FR(r_buf, r_len);
//				if (r_len > 0) {
//					// print reply (starting at char 5 to Xfile_w_pt;
//					// data comes with trailing \a and no \0 chars
//					fprintf(Xfile_w_pt, "%s", r_buf + 18);
//				}
//			} else {
//				MESSAGE("Error or timeout", "on receiving data from X32");
//			}
//    		break;
//    	case 5:	//.efx
//    		// effect preset from FX in "number"
//    		// build request /node command
//			s_len = Xsprint(s_buf, 0, 's', "/node");
//			s_len = Xsprint(s_buf, s_len, 's', ",s");
//			k = s_len;
//			sprintf (l_read, "fx/%1i/", number);
//			for (i= 0; i < num_nodes; i++) {
//				strcpy(l_read + 5, nodes_ptr[i]);
//				s_len = Xsprint(s_buf, k, 's', l_read);
//				SEND_TO(s_buf, s_len);
//				RPOLL;
//				if (p_status > 0) {
//					RECV_FR(r_buf, r_len);
//					if (r_len > 0) {
//						// print reply (starting at char 5 to Xfile_w_pt;
//						// data comes with trailing \a and no \0 chars
//						fprintf(Xfile_w_pt, "%s", r_buf + 18);
//					}
//				} else {
//					MESSAGE("Error or timeout", "on receiving data from X32");
//				}
//			}
//    		break;
    	case 4:	// use pattern file
    		// reading from pattern (can be any type)
    		// open pattern file as read only
    	    if ((Xfile_r_pt = fopen (Xfile_r_path, "r")) != NULL) {
    			while (keep_reading) {
    				if ((read_status = fgets(l_read, BSIZE, Xfile_r_pt)) != NULL) {
    					// ignore comment lines
    					if (l_read[0] != '#') {
							//
							// Use the following bloc
    						// skip the "/" and stop at the first " "
    						i = 1;
    						while ((i < BSIZE) && (l_read[i] != ' ')) i++;
    						if (i < BSIZE) l_read[i] = 0;
    						// send a /~~~ command to set data from file directly to X32
    						s_len = Xsprint(s_buf, 0, 's', "/node");
    						s_len = Xsprint(s_buf, s_len, 's', ",s");
    						s_len = Xsprint(s_buf, s_len, 's', l_read + 1); // skip leading '/'
    						SEND_TO(s_buf, s_len)
    						RPOLL
    						if (p_status < 0) {
    							MESSAGE("" ,"Polling for data failed");
    							return 0;	// Make sure we don't considered being connected
    						} else if (p_status > 0) {
    							// We have received data - process it!
    							r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);
    							if (r_len > 0) {
    								j = 0;
    								while((r_buf[j] != '/') && (j < r_len)) j++;
    								// print reply (starting at first '/' found) to Xfile_w_pt;
    								// data comes with trailing \a and no \0 chars
    								fprintf(Xfile_w_pt, "%s", r_buf + j);
    							}
    						} else {
    							// time out... Not connected or Not an X32
    							MESSAGE("", "X32 reception timeout");
    							return 0;	// Make sure we don't considered being connected
    						}
    					}
    				} else {
    					keep_reading = 0;
    				}
    			}
    			// all done, close file
    			fclose(Xfile_r_pt);
    		} else {
    			// cannot open file
    			MESSAGE("Error opening .xds file:", Xfile_r_path);
    			return (1);
    		}
    	    break;
    	default:
    		break;
    	}
		// all done, close file
		fclose(Xfile_w_pt);
    } else {
    	// cannot open file
		MESSAGE ("Error creating destination file:", Xfile_w_path);
		return (1);
	}
	return (0);
}
//
//
//
void XRcvClean() {
//
// empty reception buffer while data is available
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

int
main(int argc, char **argv)
{
#ifdef _WIN32
	HINSTANCE	hPrevInstance = 0;
	PWSTR		pCmdLine = 0;
	int			nCmdFile = 0;
#else
	int input_intch, keep_on;
#endif
	Xip_str[0] = 0;
	//
#ifdef _WIN32
	// load resource file
	if ((res_file = fopen("./.X32DeskSave.ini", "r")) != NULL) {
		// get and remember real path
		if((Finiretval = GetFullPathNameA("./.X32DeskSave.ini", 1023, Finipath, FinilppPart)) > 1024) {
			printf ("Not enough space for file name\n");
		}
		fscanf(res_file, "%d %d\r\n", &wWidth, &wHeight);
		fgets(Xip_str, sizeof(Xip_str), res_file);
		Xip_str[strlen(Xip_str) - 1] = 0;
		fclose (res_file);
	}
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
	return 0;
#else
	keep_on = 1;
	Xptype = 1;
	strcpy(Xip_str, "192.168.1.64");
	// manage command-line parameters
	while ((input_intch = getopt(argc, argv, "i:p:d:s:r:h")) != -1) {
		switch ((char)input_intch) {
			case 'i':
				strcpy(Xip_str, optarg);
				if (validateIP4Dotted(Xip_str) == 0) {
					printf("Invalid IP address\n");
					return -1;
				}
				break;
			case 'p':
				strcpy(Xfile_r_path, optarg);
				strcpy(Xfile_r_name, getFileNameFromPath(optarg));
				Xptype = 4;
				break;
			case 'd':
				Xptype = 1;
				break;
			case 's':
				Xptype = 2;
				break;
			case 'r':
				Xptype = 3;
				break;
			default:
			case 'h':
				printf("X32DeskSave - ver 1.00 - ©2018 - Patrick-Gilles Maillot\n\n");
				printf("usage: X32DeskSave [-i X32 console ipv4 address, default: 192.168.1.64]\n");
				printf("                   [-p <pattern file> File path to pattern input file]\n");
				printf("                   [-d 0/1 DeskSave file]\n");
				printf("                   [-s 0/1 Scene file]\n");
				printf("                   [-r 0/1 Routing file]\n");
				printf("                   <Destination file name/path>\n");
				printf("X32DeskSave saves those parameters that are not handled  by shows, scenes,\n");
				printf("snippets, cues, presets, routing or effects files... So when you connect\n");
				printf("to your desk, it is exactly as you want it: screen, light brightness, view, etc.\n");
				printf("You can also save scenes, routing presets or any type of file or settings based\n");
				printf("on a pattern file (a scene, any set of commands, etc.); the -p <file> option\n");
				printf("reads the provided file, then extracts X32 commands from it to retrieve values\n");
				printf("from your X32. These are saved in the file provided as destination file\n");
				printf("Option \"-d 1\" is optional and the default case. Only one of options -d, -r, -s\n");
				printf(" or -p should be used, only the last one will be taken into account\n\n");
				return(0);
			break;
		}
	}
	if (argv[optind]) {
		strcpy(Xfile_w_path, argv[optind]);
		strcpy(Xfile_w_name, getFileNameFromPath(argv[optind]));
		Xfiles = 1;
	} else {
		MESSAGE("", "No Destination file");
		return 1;
	}
	if ((Xconnected = X32Connect(0, Xip_str, 20000)) == 1) {
		XRcvClean();
		return X32DS_GetFile();
	}
	MESSAGE("", "No X32 found!");
	return 1;
#endif
}

