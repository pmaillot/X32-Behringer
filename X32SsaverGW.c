/*
 * X32Ssaver.c
 *	(Windows GUI version)
 *  Created on: May 7, 2015
 *      Author: Patrick-Gilles Maillot
 *
 *      Copyright 2015, Patrick-Gilles Maillot
 *      This software is distributed under he GNU GENERAL PUBLIC LICENSE.
 *  Changelog:
 *      v 0.11: Use of select() rather than poll() for unblocking IO
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
//
#include <winsock2.h>
#include <windows.h>
//
#define BSIZE				512		// receive buffer size
#define XREMOTE_TIMEOUT		9		// time-out set to 9 seconds
#define TRUE				1
#define FALSE				0
//
void RunSsaver();		// return status is in PSstatus
int  X32_Connect();
int  validateIP4Dotted(const char *s);
void ExitSsaver();
//
// Macros:
//
#define RPOLL												\
do {														\
	FD_ZERO (&ufds);										\
	FD_SET (Xfd, &ufds);									\
	p_status = select(Xfd+1, &ufds, NULL, NULL, &timeout);	\
} while (0);
//
#define RECV										\
do {												\
	r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0);	\
} while (0);
//
#define SEND(a,l)										\
do {													\
	if (sendto (Xfd, a, l, 0, Xip_addr, Xip_len) < 0) {	\
		perror ("Error while sending data");			\
		exit (1);										\
	}													\
} while(0);
//
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE	hInstance = 0;
HWND		hwnd, hwndipaddr, hwndconx, hwndGO, hwnddely;

HFONT		hfont, htmp;
HDC			hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP		hBmp;
BITMAP		bmp;
MSG			wMsg;
//
char **FinilppPart;
char Finifile[1024];
int Finiretval;
//
struct sockaddr_in Xip;
struct sockaddr* Xip_addr = (struct sockaddr *) &Xip;
int Xfd;						// X32 socket
WSADATA wsa;
int Xip_len = sizeof(Xip);		// length of addresses
struct timeval timeout;
fd_set ufds;
unsigned long mode;
//
int wWidth = 470;
int wHeight = 123;
//
int r_len, p_status;			// length and status for receiving
char r_buf[BSIZE];				// receive buffer
//
int X32ssdelay = 5;				// Default Ssaver time-out value

char xremote[12] = "/xremote";	// automatic trailing zeroes
char xinfo[8] = "/info";		// automatic trailing zeroes
int keep_on = 1;				// Loop flag
int ssave_on = 0;				// Screen Saver ON state
time_t X32Rmte_bfr, X32Rmte_now;	// For /xremote timer
time_t X32Ssav_bfr, X32Ssav_now;	// For Screen Saver timer
char LcdOldBright[24];			// value of X32 screen brightness
char LedOldBright[28];			// value of X32 LED brightness
//
char LcdLowBright[] = "/-prefs/bright\0\0,f\0\0\0\0\0\0";
char LedLowBright[] = "/-prefs/ledbright\0\0\0,f\0\0\0\0\0\0";
//

wchar_t W32_ip_str[20], W32ssdelay_str[4];
char Xip_str[20], X32ssdelay_str[4];
int Xconnected = 0;
int Ssaver_request = 0;
int keep_running = 1;
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			PWSTR lpCmdLine, int nCmdShow) {

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32Ssaver";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
			L"X32Ssaver - Set Low Light mode for X32 ",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 220, wWidth, wHeight, 0,
			0, hInstance, 0);

	while (keep_running) {
		if (PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		} else {
			if (Ssaver_request) {
				RunSsaver();
			} else {
				Sleep(100);
			}
		}
	}
	return (int) wMsg.wParam;
}
//
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
int i;
char str1[32];
FILE *res_file;
//
	switch (msg) {
	case WM_CREATE:
//
		hwndipaddr = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 125, 40, 145, 20, hwnd,
				(HMENU)0, NULL, NULL);
		hwndconx = CreateWindowW(L"button", L"Connect",
				WS_VISIBLE | WS_CHILD, 280, 40, 75, 20, hwnd, (HMENU)1,
				NULL, NULL);
		hwnddely = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 280, 70, 25, 20, hwnd,
				(HMENU)0, NULL, NULL);
		hwndGO = CreateWindowW(L"button", L"OFF",
				WS_VISIBLE | WS_CHILD, 375, 10, 80, 83, hwnd, (HMENU)2,
				NULL, NULL);
		hBmp = (HBITMAP) LoadImage(NULL, (LPCTSTR) "./.X32Ssaver.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_SHARED);
		if (hBmp == NULL) {
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
//
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 150, 25, str1, wsprintf(str1, "Enter X32 IP below:"));
		TextOut(hdc, 325, 18, str1, wsprintf(str1, "ver. 0.11"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 0, str1,
		wsprintf(str1, "X32Ssaver - ©2015 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		TextOut(hdc, 125, 72, str1, wsprintf(str1, "Delay before Low Light:"));
		TextOut(hdc, 310, 72, str1, wsprintf(str1, "seconds"));
//
		DeleteObject(htmp);
		DeleteObject(hfont);
		EndPaint(hwnd, &ps);
//
		sprintf(X32ssdelay_str, "%d", X32ssdelay);
		SetWindowText(hwnddely, (LPSTR) X32ssdelay_str);
		SetWindowText(hwndipaddr, (LPSTR) Xip_str);
		break;
//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			i = LOWORD(wParam);
			switch (i) {
			case 1:
				i = GetWindowTextLengthW(hwndipaddr) + 1;
				GetWindowTextW(hwndipaddr, W32_ip_str, i);
				WideCharToMultiByte(CP_ACP, 0, W32_ip_str, i, Xip_str, i,
				NULL, NULL);
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32_Connect();
					if (Xconnected)
						SetWindowTextW(hwndconx, L"Connected");
					else {
						SetWindowTextW(hwndconx, L"Connect");
						Ssaver_request = 0;
						SetWindowTextW(hwndGO, L"OFF");
						ExitSsaver();
					}
				}
				break;
			case 2:
				// Ssaver request
				if (Xconnected) {
					if (Ssaver_request) {
						SetWindowTextW(hwndGO, L"OFF");
						ExitSsaver();
					} else {
						SetWindowTextW(hwndGO, L"ON");
						i = GetWindowTextLengthW(hwnddely) + 1;
						GetWindowTextW(hwnddely, W32ssdelay_str, i);
						WideCharToMultiByte(CP_ACP, 0, W32ssdelay_str, i, X32ssdelay_str, i,
						NULL, NULL);
						sscanf(X32ssdelay_str, "%d", &X32ssdelay);
						X32Rmte_bfr = 0;
						X32Ssav_bfr = time(NULL);
					}
					Ssaver_request ^= 1;
				}
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		if (Xconnected && Ssaver_request) {
			ExitSsaver();
			close(Xfd);
		}
		// read panel data
		i = GetWindowTextLengthW(hwndipaddr) + 1;
		GetWindowTextW(hwndipaddr, W32_ip_str, i);
		WideCharToMultiByte(CP_ACP, 0, W32_ip_str, i, Xip_str, i, NULL,
		NULL);
//
		i = GetWindowTextLengthW(hwnddely) + 1;
		GetWindowTextW(hwnddely, W32ssdelay_str, i);
		WideCharToMultiByte(CP_ACP, 0, W32ssdelay_str, i, X32ssdelay_str, i,
		NULL, NULL);
		sscanf(X32ssdelay_str, "%d", &X32ssdelay);
//
		if((res_file = fopen(Finifile, "wb")) > 0) {
			fprintf(res_file, "%d %d %d\n", wWidth, wHeight, X32ssdelay);
			fprintf(res_file, "%s\n", Xip_str);
			fclose(res_file);
		}
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
	p_status = 0;
	if (Xconnected) {
		close(Xfd);
		return 0;
	}
// initialize communication with X32 server at IP ip and PORT port
	p_status = GetWindowTextLengthW(hwndipaddr) + 1;
	GetWindowTextW(hwndipaddr, W32_ip_str, p_status);
	WideCharToMultiByte(CP_ACP, 0, W32_ip_str, p_status, Xip_str,
			p_status, NULL, NULL);
	if (validateIP4Dotted(Xip_str) == 0) {
		return 0;
	}
//Initialize winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(EXIT_FAILURE);
	}
// Load the X32 address we connect to; we're a client to X32, keep it simple.
		// Create the UDP socket
	if ((Xfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		return 0; // Make sure we don't considered being connected
	}
// Construct the server sockaddr_in structure
	memset(&Xip, 0, sizeof(Xip));				// Clear struct
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
	Xip.sin_port = htons(atoi("10023"));		// server port
// Non blocking mode
	timeout.tv_sec = 0;
	timeout.tv_usec = 50000; //Set timeout for non blocking recvfrom(): 50ms
	FD_ZERO (&ufds);
	FD_SET (Xfd, &ufds);
	if (sendto(Xfd, xinfo, 8, 0, Xip_addr, Xip_len) >= 0) { // send /info
		RPOLL				// wait for answer
        if (p_status > 0) {
			RECV			// get and compare data
			if ((strncmp(r_buf, xinfo, 5)) == 0) {
				return 1;	// We are connected
			}
		}
	}
	WSACleanup();
	return 0; // Make sure we don't considered being connected
}
//
// Validate IP string function
//
int validateIP4Dotted(const char *s) {
	int i;
	char tail[16];
	unsigned int d[4];

	i = strlen(s);
	if (i < 7 || i > 15)
		return 0;
	tail[0] = 0;
	int c = sscanf(s, "%3u.%3u.%3u.%3u%s", &d[0], &d[1], &d[2], &d[3],
			tail);
	if (c != 4 || tail[0])
		return 0;
	for (i = 0; i < 4; i++)
		if (d[i] > 255)
			return 0;
	return 1;
}
//
//
int main(int argc, char **argv) {
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmdShow = 0;
	FILE *res_file;

	Xip_str[0] = 0;
	// load resource file
	if ((res_file = fopen("./.X32Ssaver.ini", "r")) != NULL) {
		// get and remember real path
		if ((Finiretval = GetFullPathNameA("./.X32Ssaver.ini", 1024, Finifile, FinilppPart)) > 1024) {
			printf("Not enough space for file name\n");
		}
		fscanf(res_file, "%d %d %d\n", &wWidth, &wHeight, &X32ssdelay);
		fgets(Xip_str, sizeof(Xip_str), res_file);
		Xip_str[strlen(Xip_str) - 1] = 0;
		fclose(res_file);
		ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
		wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
	}
	return 0;
}
//
void ExitSsaver() {
	if (ssave_on) {
		sendto(Xfd, LcdOldBright, 24, 0, Xip_addr, Xip_len);
		sendto(Xfd, LedOldBright, 28, 0, Xip_addr, Xip_len);
	}
	ssave_on = 0; // S-saver mode is OFF
	X32Ssav_bfr = time(NULL);	// remember time
}
//
void RunSsaver() {

	X32Rmte_now = time(NULL);	// register for X32 data echo
	if (X32Rmte_now > X32Rmte_bfr + XREMOTE_TIMEOUT) {
		if (sendto(Xfd, xremote, 12, 0, Xip_addr, Xip_len) < 0)
			exit(1);
		X32Rmte_bfr = X32Rmte_now;
	}
	RPOLL
	// X32 sent something?
    if (p_status > 0) {
		RECV
		// Exit screen saver if needed
		if (r_len && ssave_on) { // Restore main LCD and LED brightness
			SEND(LcdOldBright, 24)
			SEND(LedOldBright, 28)
			ssave_on = FALSE; // S-saver mode is OFF
		}
		X32Ssav_bfr = time(NULL);	// remember time
//
	} else if (p_status == 0) {	// no data back from X32, enter screen saver?
		X32Ssav_now = time(NULL);
		if (X32Ssav_now > X32Ssav_bfr + X32ssdelay) {
			if (!ssave_on) {	// No need to enter saver mode if already ON
				SEND(LcdLowBright, 16)
				RPOLL
                if (p_status > 0) {
					RECV	// expected: /-prefs/bright...[float]
					memcpy(LcdOldBright, r_buf, 24);
				} // main screen brightness saved, ignore errors (p_status < 0)
				SEND(LedLowBright, 20)
				RPOLL
                if (p_status > 0) {
					RECV	// expected: /-prefs/ledbright...[float]
					memcpy(LedOldBright, r_buf, 28);
				} // Leds and Scribbles brightness saved, ignore errors (p_status < 0)
				  // Set LCD screen and LEDs to their lowest values
				SEND(LcdLowBright, 24)
				SEND(LedLowBright, 28)
				ssave_on = TRUE;	// S-saver is ON
			}
		}
	} else
		keep_on = 0; // Exit on error (p_status < 0)
	return;
}