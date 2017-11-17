/*
 * X32DeskSave.c
 *
 *  Created on: May 6, 2016
 *
 * ©2016 - Patrick-Gilles Maillot
 *
 * X32DeskSave - a windows app for saving a all -prefs and -stat
 * X32 memory states to a PC HDD.
 *
 *
 * Change log
 *    0.91: cleaned the code for first release with an attempt at .snp, .efx, and .chn handling
 *          not working as initially thought of but code commented; I may get back to this
 *    0.92: code refactoring - moved some functions to extern
 *    0.93: adapted to FW ver 3.04
 *    0.94: preventing windows resizing
 */

#include <winsock2.h>

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
int		X32DS_GetFile();
void 	XRcvClean();
//
//
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
FILE 	*res_file;
char	Finipath[1024];	// resolved path to .ini file
char	**FinilppPart;
int 	Finiretval;

int		keep_running = 1;
int		wWidth = 545;
int		wHeight = 130;

char	Xip_str[20];
char	Xfile_w_path[256];
char	Xfile_r_path[256];
char	Xfile_w_name[32];	// used to save the selected file name to create
char	Xfile_r_name[32];	// used to save the selected file name used as pattern file
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

int		Xconnected = 0;	// Flag indicating if we're connected to X32
int		Xfiles = 0;		// Flag indicating if a file has been selected for saving
int		Xfilep = 0;		// Flag indicating if a pattern file has been selected
int		Xptype = 0;		// Flag indicating the type of file we're saving

// X32 Communication buffers (in and out)
char				r_buf[BSIZE], s_buf[BSIZE];
int					r_len, s_len;

WSADATA 			wsa;
int					Xfd;					// X32 socket
struct sockaddr_in	Xip;					// X32 IP address
int					Xip_len = sizeof(Xip);	// length of addresses
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
//
//
//
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
        SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Choose:");
        SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"DeskSave");
        SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Scene");
//        SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Snippet");
//        SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Channel");
//        SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Effects");
        SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Routing");
        SendMessage(hwndptyp, CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Pattern");
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
					printf ("Incorrect IP string form\n");
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
						// remove filename from returned path so we can either save to
						// existing directory or newly created one
						i = strlen(Xfile_w_path);
						while (i && (Xfile_w_path[i] != '\\')) --i;
						Xfile_w_path[i] = 0;
						strcpy(Xfile_w_name, Xfile_w_path + i + 1);
						sprintf(s_buf, "Directory: %s\nFile Name: %s", Xfile_w_path, Xfile_w_name);
						if (MessageBox(NULL, s_buf, "Destination directory & File name: ", MB_OKCANCEL) == IDOK) {
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
				Xfilep = 0;
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
					Xfilep = 1;
					SetWindowText(hwndfpatn, (LPSTR)Xfile_r_name);

//					SetWindowText(hwndprog, (LPSTR)Xready);	// display ready status
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
//
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
		printf ("Not connected to X32!\n");
		return (1);
	}
	// Check if destination path OK
	if (!Xfiles) {
		printf("Error: No file selected\n");
		return (1);
	}
	nodes_ptr = 0;
	num_nodes = 0;
	//
    // Open the destination file for writing
    if ((Xfile_w_pt = fopen (Xfile_w_name, "wb")) != NULL) {
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
					printf ("Error or timeout while receiving data from X32\n");
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
//					printf ("Error or timeout while receiving data from X32\n");
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
//				printf ("Error or timeout while receiving data from X32\n");
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
//					printf ("Error or timeout while receiving data from X32\n");
//				}
//			}
//    		break;
    	case 4:	// use pattern file
    		// reading from pattern (can be any type)
    		// open pattern file as read only
    	    if ((Xfile_r_pt = fopen (Xfile_r_name, "r")) != NULL) {
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
    							printf("Polling for data failed\n");
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
    							printf("X32 reception timeout\n");
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
    			printf("Error opening .xds file: %s\n", Xfile_r_name);
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
		printf ("Error creating destination file: %s\n", Xfile_w_name);
		return (1);
	}
	return (0);
}


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

int
main(int argc, char **argv)
{
	HINSTANCE	hPrevInstance = 0;
	PWSTR		pCmdLine = 0;
	int			nCmdFile = 0;

	Xip_str[0] = 0;
	// load resource file
	if ((res_file = fopen("./.X32DeskSave.ini", "r")) != NULL) {
		// get and remember real path
		if((Finiretval = GetFullPathNameA("./.X32DeskSave.ini", 1024, Finipath, FinilppPart)) > 1024){
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
}

