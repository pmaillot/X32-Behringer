/*
 * X32Jog4Xlive.c
 *
 *  Created on: Nov 21, 2017
 *      ©2017 - Patrick-Gilles Maillot
 *
 *		This application brings two rotary knobs to the X-Live! expansion board on X32.
 *		When the X-Live! board is present in an X32, the FW (3.08 and more) sets Bank C
 *		of User Assign section to a number of parameters. The leftmost rotary knob enables
 *		circulating through sections of the SD card(s), the next knob travels through markers,
 *		the 3rd one is unused, and the last one is used to set the time for adding markers.
 *
 *		We replace knobs 1 and 3 with this program; Here knob 1 is used to act as an audio
 *		jog, and enables moving up and down in a song. Knob #3 is used to set the difference
 *		in time between two consecutive increments of knob #1. This because X32 knobs do not
 *		provide inertia, and are not real jogs of course.
 *
 *		The time difference is displayed in the user interface in the format 00m00s00 (minutes,
 *		seconds, tens of milliseconds) varying from 10ms to 2m41s30
 *
 *		ver 0.10: initial release
 *
 */
#include <winsock2.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
//
#define BSIZE 			512	// Buffer sizes
//
// Windows calls & callbacks
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
// External calls used
extern int 		Xfprint(char *bd, int index, char* text, char format, void *bs);
extern int		Xsprint(char *bd, int index, char format, void *bs);
extern int		X32Connect(int Xconnected, char* Xip_str, int btime);
extern int		validateIP4Dotted(const char *s);
//
// Private functions
void			JOG();
void 			X32UsrCtrlC();
//
//
#define SEND														\
	do {															\
		if (sendto(Xfd, Xb_s, Xb_ls, 0, Xip_addr, Xip_len) < 0) {	\
			printf ("Can't send data to X32\n");					\
			exit(EXIT_FAILURE);										\
		} 															\
	} while (0);
//
//
#define RECV												\
	do {													\
		Xb_lr = recvfrom(Xfd, Xb_r, BSIZE, 0, 0, 0);		\
	} while (0);
//
//
#define POLL												\
	do {													\
		FD_ZERO (&ufds);									\
		FD_SET (Xfd, &ufds);								\
		p_status = select(Xfd+1,&ufds,NULL,NULL,&timeout);	\
	} while (0);
//
//
//
// Global variables
//
// X32 Communication buffers (in and out)
char				Xb_r[BSIZE], Xb_s[BSIZE];
int					Xb_lr, Xb_ls;
char				Xip_str[20];
//
WSADATA 			wsa;
int					Xfd;					// X32 socket
struct sockaddr_in	Xip;					// X32 IP address
int					Xip_len = sizeof(Xip);	// length of addresses
struct sockaddr		*Xip_addr = (struct sockaddr*)&Xip;
struct timeval		timeout;				// non-blocking timeout
int					p_status;				// X32 read  status
fd_set 				ufds;					// file descriptor
//
time_t before, now;			// timers for Xremote controls
//
HINSTANCE	hInstance = 0;
HWND 		hwndipaddr, hwndconx, hwndfdtime;
HFONT		hfont, htmp;
HDC			hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP		hBmp;
BITMAP		bmp;
MSG			wMsg;
//
// Window size (defaults overwritten by actual size)
int wWidth = 385;
int wHeight = 135;
//
int zero = 0;
int one = 1;
int two = 2;
int six4 = 64;
int delta_time = 1;
//
// Misc. flags
int keep_running = 1;		// main loop flag
int	Xtransport_on = 1;
int Xconnected = 0;		// 1 when communication is running
int XJogON = 1;
//
//-------------------------------------------------------------------------
//
// Window create and mainloop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
//
	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32Jog4Xlive";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"X32Jog4Xlive - Jog for X32 X-Live!",
			WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 100, 220, wWidth, wHeight, 0, 0, hInstance, 0);
//
// Entering main loop
	while (keep_running) {
		if(PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		}
		// If connected/running, consume X32 messages
		if (Xconnected) {
			if (XJogON) {
				now = time(NULL); 			// get time in seconds
				if (now > before + 9) { 	// need to keep xremote alive?
					sendto(Xfd, "/xremote\0\0\0\0", 12, 0, Xip_addr, Xip_len);
					before = now;
				}
				JOG();
			}
		} else {
			Sleep(10);						// avoid high CPU use
		}
	}
	return (int) wMsg.wParam;
}
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
char 	str1[64];
//
	switch (msg) {
	case WM_CREATE:
//
		hwndipaddr = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				150, 45, 115, 20, hwnd,(HMENU)0, NULL, NULL);
		hwndconx = CreateWindowW(L"button", L"Connect", WS_VISIBLE | WS_CHILD,
				270, 45, 100, 20, hwnd,(HMENU)1, NULL, NULL);
		hwndfdtime = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				270, 70, 100, 20, hwnd, (HMENU)0, NULL, NULL);
//
		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./X32Jog4Xlive.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_SHARED);
		if(hBmp==NULL) {
			perror("Pixel bitmap file no found");
		}
		break;
//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
//
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 2, 2, 100, 100, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
//
		SetBkMode(hdc, TRANSPARENT);
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 320, 15, str1, wsprintf(str1, "ver. 0.10"));
		TextOut(hdc, 150, 30, str1, wsprintf(str1, "Enter X32 IP below:"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 110, 0, str1, wsprintf(str1, "X32Jog4Xlive - ©2017 - Patrick-Gilles Maillot"));
		TextOut(hdc, 150, 72, str1, wsprintf(str1, "Time between tics:"));
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
						SetWindowText(hwndconx, "Connected");
						X32UsrCtrlC();
						SetWindowText(hwndfdtime, "   00m00s01");
					} else {
						SetWindowText(hwndconx, "Connect");
					}
				} else {
					MessageBox(NULL, "Incorrect IP address!", NULL, MB_OK);
				}
				break;
			default:
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		// Read window and panel data
		keep_running = 0;
		PostQuitMessage(0);
		break;
//
	default:
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
//
int
main(int argc, char **argv)
{
	HINSTANCE	hPrevInstance = 0;
	PWSTR		pCmdLine = 0;
	int			nCmdFile = 0;
	//
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
	return 0;
}
//
// Private functions:
//
// This function initializes User Assign section bank C encoders 1 and 3
// and sets bank C as the selected bank
//
void X32UsrCtrlC() {
	//
	if (Xtransport_on) {
		//
		// Encoders 1 and 3 (2 and 4 untouched)
		// Set X32 Bank C Encoder 1 to its default value: 64
		Xb_ls = Xfprint(Xb_s, 0, "/config/userctrl/C/enc/1", 's', "MP13000");
		SEND
		Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/33/value", 'i', &six4);
		SEND
		// Set X32 Bank C Encoder 3 to its default value: 0
		Xb_ls = Xfprint(Xb_s, 0, "/config/userctrl/C/enc/3", 's', "MP14000");
		SEND
		Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/35/value", 'i', &zero);
		SEND
//		//
//		// Color : black
//		Xb_ls = Xfprint(Xb_s, 0, "/config/userctrl/C/color", 'i', &zero);
//		SEND
		//
		// Select X32 Bank C
		Xb_ls = Xfprint(Xb_s, 0, "/-stat/userbank", 'i', &two);
		SEND
	}
}
//
//
// JOG() takes into account incoming data from X32 and manages the Jog dials accordingly
//
void JOG() {
	int move, play, etim;
	int minutes, seconds, tensofms;
	char tmpstr[16];
	//
	// We're only interested in certain X32 OSC messages to start operating on dials
	// Make sure we only start on rotary knobs 1 and 3
	do {
		POLL				// X32 sent something?
		if (p_status > 0) {	// yes!
			RECV
			// Rotary button C #1?
			// acts as a '+' or '-' around mid-position (64)
			if (strcmp(Xb_r, "/-stat/userpar/33/value") == 0) {
				move = ntohl(*(int*)(Xb_r+28));
				// Get X-Live! transport status; Effectively with X-Live!, Jog functionality
				// will only be valid (i.e. working for X32) when the X-Live! is in play (2) or
				// ppause (1) state. Tried other selections and no luck.
				Xb_ls = Xsprint(Xb_s, 0, 's', "/-stat/urec/state");
				SEND
				// Careful here... X-Live! sends data every ~350ms when playing; these messages
				// are coming no matter what. When we send an OSC request, these messages can
				// come a read data rather that what's expected; we have to filter them out.
				// This is the reason for the use of nested do {...} while(); constructions
				// below
				do {
					POLL
					if (p_status > 0) {
						RECV
						if (strcmp(Xb_r, "/-stat/urec/state") == 0) {
							play = ntohl(*(int*)(Xb_r+24));
							// Only interested in the case where status is play or ppause
							if (play & 3) {
								// We're in play or ppause, let's read our current position
								Xb_ls = Xsprint(Xb_s, 0, 's', "/-stat/urec/etime");
								SEND
								do {
									POLL
									if (p_status > 0) {
										RECV
										if (strcmp(Xb_r, "/-stat/urec/etime") == 0) {
											// Read data from X32 into etim
											etim = ntohl(*(int*)(Xb_r+24));
											// Jump forward or backward depending on 'move'
											if (move > six4) {
												etim += delta_time;	//delta_time depends on shuttle data value
											} else {
												etim -= delta_time;	//delta_time depends on shuttle data value
											}
											etim += 1;
											// Set new position
											Xb_ls = Xfprint(Xb_s, 0, "/-action/setposition", 'i', &etim);
											SEND
											// Reset rotary cursor to its center position
											Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/33/value", 'i', &six4);
											SEND
										}
										p_status = 0;
									}
								} while(p_status > 0);
							}
						}
						p_status = 0;
					}
				} while(p_status > 0);
			} else if (strcmp(Xb_r, "/-stat/userpar/35/value") == 0) {
				// Rotary button C #3?
				// returns a value [0..127]
				move = ntohl(*(int*)(Xb_r+28));
				delta_time = move * move + 1;
				// delta_time: [1..16130]ms set as a non-linear scale
				tensofms = delta_time / 100;
				minutes = tensofms / 60;
				seconds = tensofms - minutes * 60;
				tensofms = delta_time - tensofms * 100;
				delta_time *= 10;
				sprintf(tmpstr, "   %02dm%02ds%02d", minutes, seconds, tensofms);
				SetWindowText(hwndfdtime, (LPSTR)tmpstr);
				p_status = 0;
			}
		}
	} while (p_status > 0);
	return;
}
