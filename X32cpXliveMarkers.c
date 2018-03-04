/*
 * X32cpXliveMarkers.c
 *
 *  Created on: Feb 9, 2018
 *
 * ©2018 - Patrick-Gilles Maillot
 *
 *
 *		X32cpXliveMarkers:
 *			Reads markers from an XLive! session/SE_LOG.BIN file and send data to the clipboard.
 *
 *
 *			Within REAPER, the clipboard data can be pasted to a REAPER project by clicking on
 *				Extensions->Marker utilities->Import marker set from slipboard
 *			in order to set markers in the respective REAPER tracks, which obviously should
 *			contain the wav data corresponding to the selected session. This data can either
 *			set into REAPER using REAPER import track data, or using X32Xlive_Wav.
 *
 *
 *	History:
 *	v 0.10	Initial file
 *	v 0.20	Support different types (REAPER, Audition)
 *	v 0.21	Small fix in Audition conversion
 *
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <windows.h>	// Windows functions for std GUI & sockets
#include <Shlobj.h>		// Windows shell functions
//
#define	LINEHI			25
#define NLINES			2
#define BSIZE			1052
#define MESSAGE(s1,s2)	MessageBox(NULL, s1, s2, MB_OK);
//
//
int CopytoClipboard();
//
//
// Windows Declarations
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE		hInstance = 0;
HWND			hwndCopy, hwndInDir, hwndSource, hwndMHbut, hwndMHead, hwndtype;
HGLOBAL			hdst;
LPSTR			dst;
HFONT			hfont, htmp;
HDC				hdc, hdcMem;
HBITMAP			hBmp;
BITMAP			bmp;
PAINTSTRUCT		ps;
MSG				wMsg;				// Windows msg event
BROWSEINFO 		bi;					// Windows Shell structure
ITEMIDLIST 		*pidl;				// dir item list
//
int			wWidth = 480;			// Default window size
int			wHeight = 53 + LINEHI * NLINES;
int			keep_running;			// mainloop flag
char		Xspath[MAX_PATH];		// source directory path, and buffer
char		str1[MAX_PATH];			// used for Windows strings conversions
char		mhead[8];				// marker header
char		*bufmarker;				// memory for clipboard
FILE		*Xin;					// FILE handle
//
char		*DAWtype[] = {"REAPER", "Audition", "\0"};
int			Dawsize = sizeof(DAWtype) / sizeof(char*) - 1;
//
//
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmWfile) {
//
	WNDCLASSW wc = {0};
	wc.lpszClassName = L"X32cpXliveMarkers";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
		L"X32cpXliveMarkers - Xlive Markers to Clipboard",
		WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 100, 140,
		wWidth, wHeight, 0, 0, hInstance, 0);
//
//
// Main loop
	while (keep_running) {
		if(PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		} else {
			Sleep(10);	// avoid too high a processor use when nothing to do
		}
	}
	return (int) wMsg.wParam;
}
//
//
// Windows Callbacks
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//
	int		i;
	//
	switch (msg) {
	case WM_CREATE:
		//
		hwndCopy = CreateWindow("button", "Copy",
				WS_VISIBLE | WS_CHILD, 400, LINEHI-5, 70, 2*LINEHI, hwnd, (HMENU)1, NULL, NULL);

		hwndInDir = CreateWindow("button", "Session",
				WS_VISIBLE | WS_CHILD, 128, LINEHI, 60, 20, hwnd, (HMENU)2, NULL, NULL);
		hwndSource = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 190, LINEHI, 208, 20, hwnd, (HMENU)0, NULL, NULL);

		hwndtype = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				128, 2*LINEHI-2, 118, (Dawsize + 1)* 20, hwnd, (HMENU)0, NULL, NULL);
		for (i = 0; i < Dawsize; i++) {
			SendMessage(hwndtype, CB_ADDSTRING, (WPARAM)0, (LPARAM)DAWtype[i]);
		}
		// display initial item in the selection field to REAPER
		SendMessage(hwndtype, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		hwndMHbut = CreateWindow("button", "Marker Head",
				WS_VISIBLE | WS_CHILD, 250, 2*LINEHI, 95, 20, hwnd, (HMENU)3, NULL, NULL);
		hwndMHead = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 348, 2*LINEHI, 50, 20, hwnd, (HMENU)0, NULL, NULL);

		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./sdcard2.bmp", IMAGE_BITMAP, 0, 0,
				LR_LOADFROMFILE|LR_SHARED);
		if(hBmp == NULL) {
			perror("Pixel bitmap file no found");
		}
		break;
	//
	case WM_PAINT:
		//
		hdc = BeginPaint(hwnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		MoveToEx(hdc, 2, 1, NULL);
		LineTo(hdc, wWidth-8, 1);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 3, 3, 120, 72, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);

		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 128, 3, str1, wsprintf(str1, "X32cpXliveMarkers -v. 0.21- ©2018 - Patrick-Gilles Maillot"));

		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		// update user information
		EndPaint(hwnd, &ps);
		break;
	//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			switch (LOWORD(wParam)) {
			case 1:
				// Copy button clicked! -- copy data to clipboard
				if (Xspath[0]) {
					if (CopytoClipboard()) {
						MessageBox(NULL, "Something went wrong", NULL, MB_OK);
					}
				} else {
					MessageBox(NULL, "Select Session dir. first!", NULL, MB_OK);
				}
				break;
			case 2:
				// Select Source Directory (must contain session files)
				Xspath[0] = 0; // no/empty directory name
				bi.hwndOwner = hwnd;
				bi.pidlRoot = 0;
				bi.pszDisplayName = Xspath;
				bi.lpszTitle = "Select source/session directory";
				bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
				bi.lpfn = 0;
				bi.lParam = 0;
				bi.iImage = 0;
				pidl = SHBrowseForFolder(&bi);
				if (SHGetPathFromIDList(pidl, Xspath) == TRUE) {
					if ((i = strlen(Xspath)) > 24) {
						strncpy(str1, Xspath, 10);
						strcpy(str1 + 10, "...");
						strcpy(str1 + 13, Xspath + i - 14);
					} else {
						strcpy(str1, Xspath);
					}
					SetWindowText(hwndSource, (LPSTR)str1);
					strcat(Xspath + strlen(Xspath), "\\SE_LOG.BIN");
				}
				break;
			case 3:
				// set marker headers (up to 5 chars)
				GetWindowText(hwndMHead, mhead, GetWindowTextLength(hwndMHead) + 1);
				mhead[min(5, strlen(mhead))] = 0;
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		// Quit
		keep_running = 0;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
//
// Main()
//
int main(int argc, char **argv) {
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmWfile = 0;

	keep_running = 1;		// mainloop flag
	Xspath[0] = 0;			// No source directory
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmWfile);
	//
	return 0;
}
//
//
// CopytoClipboard(): Read Markers from Session log file; prepare clipboard buffer
//
int CopytoClipboard() {

	int 			i, nbmarker, len;
	unsigned int	samprate, marker;
	int				xmkh, xmkm, xmks, xmkt;
	float  			 xmk;
	char			buffer[BSIZE];
	//
	//	SE_LOG.BIN is a 2kB (2048) file made of:
	// 		   1:	<session name> = session name built from yymmhhmm in hex form (on 32 bits)
	//		   5:	<nb of channels> = 32bit int
	//		   9:	<sample rate> = 32bits unsigned int
	// 		  13:	<session name> = session name built from yymmhhmm in hex form (on 32 bits)
	//		  17:	<nb of takes> = number of xxx.wav files associated to session on 32bit int
	//		  21:	<nb markers> = number of markers on 32bit int
	//		  25:	<total length> = length on unsigned int
	//		  29:	<take size> = size of take[i] on int i:[0...256[
	//		...		<filler = 0> = fill with 0 until address 1051.
	//		1052:	<markers> = marker position in second * sample rate on 32bit unsigned int
	//		...		<filler = 0> = fill with 0 until address 1151
	//		1553:	<name (user)> 16 char max name
	//		...		<filler = 0> = fill with 0 until address 2047, inclusive
	//
	len = 0;
	if ((Xin = fopen(Xspath, "r")) != NULL) {
		// jump to Markers
		fread(buffer, sizeof(buffer), 1, Xin);
		samprate = *(unsigned int *)(buffer + 8);
		nbmarker = *(unsigned int *)(buffer + 20);
		if (nbmarker) {
			switch (SendMessage(hwndtype, CB_GETCURSEL,(WPARAM) 0, (LPARAM) 0) + 1) {
			case 1:			// (REAPER)
				nbmarker += 1;
				// allocate enough memory for the markers
				if ((bufmarker = malloc(nbmarker * 64 * sizeof(char))) != NULL) {
					for (i = 1; i < nbmarker; i++) {
						fread(&marker, sizeof(marker), 1, Xin);
						// build REAPER marker 'file'
						sprintf(bufmarker + len, "%d %f %s%d 0 -1.0 0\n", i, (float)marker / samprate, mhead, i);
						len += strlen(bufmarker + len);
					}
				} else {
					MESSAGE("Memory error", NULL);
					return 1;
				}
				break;
			case 2:			// Audition
				nbmarker += 1;
				// allocate enough memory for the markers
				if ((bufmarker = malloc(nbmarker * 64 * sizeof(char))) != NULL) {
					for (i = 1; i < nbmarker; i++) {
						fread(&marker, sizeof(marker), 1, Xin);
						xmk = (float)marker / samprate;
						xmkh = (int)xmk / 3600;
						xmkm = (int)xmk / 60;
						xmks = (int)xmk % 60;
						xmkt = (int)((xmk - (int)xmk) * 100 / 4); //conversion for 25fps
						// build REAPER marker 'file'
						sprintf(bufmarker + len, "%s%d, %02d:%02d:%02d:%02d, 00:00:00:00, 25fps, Cue, -\n",
								mhead, i, xmkh, xmkm, xmks, xmkt);
						len += strlen(bufmarker + len);
					}
				} else {
					MESSAGE("Memory error", NULL);
					return 1;
				}
				break;
			}
		}
		// allocate global memory
		hdst = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (len + 1) * sizeof(TCHAR));
		dst = (TCHAR*)GlobalLock(hdst);
		memcpy(dst, bufmarker, len * sizeof(TCHAR));
		dst[len] = 0;
		GlobalUnlock(hdst);
		// set clipboard data
		if (!OpenClipboard(NULL)) return GetLastError();
		EmptyClipboard();
		if (!SetClipboardData(CF_TEXT, hdst)) return GetLastError();
		CloseClipboard();
		// free resources
		free(bufmarker);
		fclose(Xin);
		Xin = NULL;
	} else {
		MESSAGE(NULL, "No Markers");
	}
	return 0;
}

