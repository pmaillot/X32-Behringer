//
// X32Automix.c
//
//  Created on: 17 juin 2015
//      Author: Patrick-Gilles Maillot
//
// This X32 Utility (Windows GUI based using MFC) is aiming at providing
// Automixing functionality;
//
// After connecting, Ch 01..32 meters are read to validate if data is incoming
// on a given channel (meter level is converted and compared to a threshold value)
//
// A timer enables setting a delay for reacting to a channel level change
//
// If above the threshold level, the fader level for that channel is activated towards
// its pre-recoded high value, a timer is set to remember the time at which the last
// high level was recorded.
// If below the threshold level, and time expired (time - last_high_level time), the
// fader for that channel will be lowered to its pre-recorded low value.
//
// Pre-recorded values can be set at anytime, as "equivalent" to the current setting for
// a given channel; i.e. changing a fader while in its low position (no sound) will set the
// new pre-recorded low value, and same for high values.
//
// NOM (Number Of Mixes) is a feature that will change the Overall Mix (-3dB or +3dB) each
// time the number of active (i.e. high value sound) inputs will double (or be divided by 2)
// to try to keep the Overall Mix to a reasonable value.
// Similarly to channels, pre-recorded high and low values for bus (or L/R) Overall Mix
// can be set at anytime.
//
// Overall Mix can be the L/R bus or an X32 selected mixBus
//
//
// Note: This is not trying to be a Duncan-like automix system, but sure can help in
// Studios, Theaters, or situations where several speakers will participate to a talk.
//
// Change Log:
//		0.21: refactoring code: some functions moved to extern
//		0.22: preventing window resizing
//		0.23: Saving min and max of all faders to keep them from one session to the next
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

//
#include <winsock2.h>
#include <windows.h>
//
#define BSIZE			1024	// receive buffer size
#define XTIMEOUT		9		// time-out set to 9 seconds
#define TRUE			1
#define FALSE			0
//
extern int			X32Connect(int Xconnected, char* Xip_str, int btime);
extern int 			validateIP4Dotted(const char *s);

void RunAutomix();		// return status is in PSstatus
void ExitAutomix();
void GetPanelData();
void add_3db();
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
HWND		hwnd, hwndipaddr, hwndconx, hwndGO, hwnodely, hwnidely, hwndsens;
HWND		hwndstart, hwndstop, hwnduseb, hwndbnum, hwndNOM;

HFONT		hfont, htmp;
HDC			hdc, hdcMem;
RECT		Rect;
PAINTSTRUCT ps;
HBITMAP		hBmp;
BITMAP		bmp;
MSG			wMsg;
WSADATA		wsa;

//
char	**FinilppPart;
int		Finiretval;
//
struct	sockaddr_in Xip;
struct	sockaddr* Xip_addr = (struct sockaddr *) &Xip;
int		Xfd;						// X32 socket
int		Xip_len = sizeof(Xip);		// length of addresses
struct	timeval timeout;
fd_set	ufds;
//
int		wWidth = 475;
int		wHeight = 190;
//
int		r_len, p_status;			// length and status for receiving
char	r_buf[BSIZE];				// receive buffer
//
int		X32o_delay = 5;				// Default Automix time-out value [0...20]s
int		X32i_delay = 50;			// Default Automix time-in value [0...1000]ms
float	X32sensitivity = 0.005;		// Default Automix sensitivity
int		keep_on = 1;				// Loop flag
time_t	X32Rmte_bfr, X32Rmte_now;	// For /xremote timer
float	*ff;
int		chstart = 0;				// from channel ...
int		chstop = 31;				// to channel...
int		usebus = 0;					// use bus (vs. L/R)
int		NOM = 0;					// implement Number Of Mics feature
int		active_ch;					// number of active channels
int		old_active_ch;				// backup value
int		act_ch_sav = 1;				// saved number of active channels
//
char	xremote[12] = "/xremote";	// automatic trailing zeroes
char	xinfo[8] = "/info";			// automatic trailing zeroes
char	Meters[] = "/meters\0,siii\0\0\0/meters/1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char	Fader[] = "/mix/fader";		// automatic trailing zeroes
char	Bus[] = "/mix/01/level";	// automatic trailing zeroes
int		FaderH[32];
char	Fader0[32][32];
char	Fader1[32][32];
char	LRmix[32];
char	RFader[32][8];				// saved values of the floats for faders min and max (32 faders, 2x4bytes)

time_t	X32Ftim_bfr[32], X32Ftim_now;// For fader timer

//
union littlebig {		//Endian conversion union
	char	cc[4];
	int		ii;
	float	ff;
} endian;
//
char	Xip_str[20], X32o_delay_str[4], X32i_delay_str[8], X32sens_str[16];
char	X32_start_str[4], X32_stop_str[4], X32_busnum[4];
;
int		Xconnected = 0;
int		Automix_on = 0;
int		keep_running = 1;
//
FILE	*res_file;
char	Fullfilename[1024];
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			PWSTR lpCmdLine, int nCmdShow) {

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32Automix";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
			L"X32Automix - AutoMix mode for X32 ",
//			WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 220, wWidth, wHeight, 0,
			WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 100, 220, wWidth, wHeight, 0,
			0, hInstance, 0);

	while (keep_running) {		// main GUI events loop
		if (PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		} else {
			if (Automix_on) {
				RunAutomix();
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
int i, k;
char str1[32];
//
	switch (msg) {
	case WM_CREATE:
//
		hwndipaddr = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 125, 40, 160, 20, hwnd,
				(HMENU)0, NULL, NULL);
		hwndconx = CreateWindowW(L"button", L"Connect",
				WS_VISIBLE | WS_CHILD, 290, 40, 75, 20, hwnd, (HMENU)1,
				NULL, NULL);

		hwnodely = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 70, 25, 20, hwnd,
				(HMENU)0, NULL, NULL);

		hwnidely = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 245, 90, 40, 20, hwnd,
				(HMENU)0, NULL, NULL);

		hwndsens = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 230, 110, 55, 20, hwnd,
				(HMENU)0, NULL, NULL);

		hwndstart = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 297, 130, 24, 20, hwnd,
				(HMENU)0, NULL, NULL);

		hwndstop = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 340, 130, 24, 20, hwnd,
				(HMENU)0, NULL, NULL);

		hwnduseb = CreateWindowW(L"button", L"", BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
				125, 130, 15, 20, hwnd, (HMENU)2, NULL, NULL);

		hwndbnum = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 130, 25, 20, hwnd,
				(HMENU)0, NULL, NULL);

		if (NOM) {
			hwndNOM = CreateWindowW(L"button", L"NOM ON",
					WS_VISIBLE | WS_CHILD, 375, 110, 80, 42, hwnd, (HMENU)3,
					NULL, NULL);
		} else {
			hwndNOM = CreateWindowW(L"button", L"NOM OFF",
					WS_VISIBLE | WS_CHILD, 375, 110, 80, 42, hwnd, (HMENU)3,
					NULL, NULL);
		}


		hwndGO = CreateWindowW(L"button", L"OFF",
				WS_VISIBLE | WS_CHILD, 375, 10, 80, 95, hwnd, (HMENU)4,
				NULL, NULL);

		hBmp = (HBITMAP) LoadImage(NULL, (LPCTSTR) "./.X32Automix.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_SHARED);
		if (hBmp == NULL) {
			perror("Pixel bitmap file not found");
		}
		break;
//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 5, 2, 115, 150, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);

		SetBkMode(hdc, TRANSPARENT);

		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 150, 25, str1, wsprintf(str1, "Enter X32 IP below:"));
		TextOut(hdc, 325, 18, str1, wsprintf(str1, "ver. 0.23"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 0, str1,
		wsprintf(str1, "X32Automix ©2015 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);

		TextOut(hdc, 125, 72, str1, wsprintf(str1, "Delay to Fader Down:"));
		TextOut(hdc, 290, 72, str1, wsprintf(str1, "seconds"));

		TextOut(hdc, 125, 92, str1, wsprintf(str1, "Delay to Fader Up:"));
		TextOut(hdc, 290, 92, str1, wsprintf(str1, "ms"));

		TextOut(hdc, 125, 112, str1, wsprintf(str1, "Sensitivity:"));

		TextOut(hdc, 305, 112, str1, wsprintf(str1, "Channels:"));
		TextOut(hdc, 323, 132, str1, wsprintf(str1, "to:"));

		TextOut(hdc, 145, 132, str1, wsprintf(str1, "Use Bus, number"));

		MoveToEx(hdc, 290 , 110, NULL);
		LineTo(hdc, 370, 110);
		LineTo(hdc, 370, 151);
		LineTo(hdc, 290, 151);
		LineTo(hdc, 290 , 110);

		DeleteObject(htmp);
		DeleteObject(hfont);
		EndPaint(hwnd, &ps);


		sprintf(X32o_delay_str, "%2d", X32o_delay);
		SetWindowText(hwnodely, (LPSTR) X32o_delay_str);
		sprintf(X32i_delay_str, "%4d", X32i_delay);
		SetWindowText(hwnidely, (LPSTR) X32i_delay_str);
		sprintf(X32sens_str, "%6.4f", X32sensitivity);
		SetWindowText(hwndsens, (LPSTR) X32sens_str);

		SetWindowText(hwndipaddr, (LPSTR) Xip_str);

		sprintf(X32_start_str, "%2d", chstart);
		SetWindowText(hwndstart, (LPSTR) X32_start_str);
		sprintf(X32_stop_str, "%2d", chstop);
		SetWindowText(hwndstop, (LPSTR) X32_stop_str);

		if (usebus)	SendMessage(hwnduseb, BM_SETCHECK, BST_CHECKED, 0);
		else		SendMessage(hwnduseb, BM_SETCHECK, BST_UNCHECKED, 0);
		SetWindowText(hwndbnum, (LPSTR) X32_busnum);

		break;
//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			i = LOWORD(wParam);
			switch (i) {
			case 1:
				GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32Connect(Xconnected, Xip_str, 10000);
					if (Xconnected)
						SetWindowTextW(hwndconx, L"Connected");
					else {
						SetWindowTextW(hwndconx, L"Connect");
						if (Automix_on) {
							Automix_on = 0;
							SetWindowTextW(hwndGO, L"OFF");
							ExitAutomix();
						}
					}
				}
				break;
			case 2:
				if (usebus) {
					SendMessage(hwnduseb, BM_SETCHECK, BST_UNCHECKED, 0);
				} else {
					SendMessage(hwnduseb, BM_SETCHECK, BST_CHECKED, 0);
				}
				usebus ^= 1;
				if (Xconnected) {
					if (Automix_on) {
						SetWindowTextW(hwndGO, L"OFF");
						ExitAutomix();
						Automix_on = 0;
					}
				}
				break;
			case 3:
				// NOM request
				if (NOM) {
					SetWindowTextW(hwndNOM, L"NOM OFF");
				} else {
					SetWindowTextW(hwndNOM, L"NOM ON");
				}
				NOM ^= 1;
				if (Xconnected) {
					if (Automix_on) {
						SetWindowTextW(hwndGO, L"OFF");
						ExitAutomix();
						Automix_on = 0;
					}
				}
				break;
			case 4:
				// Automix request
				if (Xconnected) {
					if (Automix_on) {
						SetWindowTextW(hwndGO, L"OFF");
						ExitAutomix();
					} else {
						GetPanelData();
						// Select if the L/R bus of a Bus is used for mixing
						if(usebus) {
							memcpy(LRmix, "/bus/01/mix/fader\0\0\0,f\0\0\0\0\0\0", 28);
							LRmix[5] = X32_busnum[0];
							LRmix[6] = X32_busnum[1];
							for (i = 0; i < 32; i++) {	// use bus rather than general LR mix
								memcpy(Fader0[i], "/ch/01/mix/01/level\0,f\0\0", 24);
								memcpy(Fader1[i], "/ch/01/mix/01/level\0,f\0\0", 24);
								k = i + 1;
								Fader0[i][4] = Fader1[i][4] = k / 10 + '0';
								Fader0[i][5] = Fader1[i][5] = k - ((k / 10) * 10) + '0';
								Fader0[i][11] = Fader1[i][11] = X32_busnum[0];
								Fader0[i][12] = Fader1[i][12] = X32_busnum[1];
								Fader0[i][24] = RFader[i][0];
								Fader0[i][25] = RFader[i][1];
								Fader0[i][26] = RFader[i][2];
								Fader0[i][27] = RFader[i][3];
								Fader1[i][24] = RFader[i][4];
								Fader1[i][25] = RFader[i][5];
								Fader1[i][26] = RFader[i][6];
								Fader1[i][27] = RFader[i][7];
							}
						} else {
							memcpy(LRmix, "/main/st/mix/fader\0\0,f\0\0\0\0\0\0", 28);
							for (i = 0; i < 32; i++) {	// use general LR mix
								memcpy(Fader0[i], "/ch/01/mix/fader\0\0\0\0,f\0\0", 24);
								memcpy(Fader1[i], "/ch/01/mix/fader\0\0\0\0,f\0\0", 24);
								k = i + 1;
								Fader0[i][4] = Fader1[i][4] = k / 10 + '0';
								Fader0[i][5] = Fader1[i][5] = k - ((k / 10) * 10) + '0';
								Fader0[i][24] = RFader[i][0];
								Fader0[i][25] = RFader[i][1];
								Fader0[i][26] = RFader[i][2];
								Fader0[i][27] = RFader[i][3];
								Fader1[i][24] = RFader[i][4];
								Fader1[i][25] = RFader[i][5];
								Fader1[i][26] = RFader[i][6];
								Fader1[i][27] = RFader[i][7];
							}
						}
						i = X32i_delay / 50;
//						printf("i = %d\n", i); fflush(stdout);
						Meters[39] = (char)i;
						if (chstart < 1) chstart = 1;
						if (chstart > 32) chstart = 32;
						if (chstop < 1) chstop = 1;
						if (chstop > 32) chstop = 32;
						old_active_ch = active_ch = X32Rmte_bfr = 0;
						for (i = chstart - 1; i < chstop; i++) X32Ftim_bfr[i] = time(NULL);
						SetWindowTextW(hwndGO, L"ON");
					}
					Automix_on ^= 1;
				}
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		// read panel data
		GetPanelData();
		// Read window and panel data
		GetWindowRect(hwnd, &Rect);
		// update .ini file
		if((res_file = fopen(Fullfilename, "wb")) > 0) {
			fprintf(res_file, "%d %d %d %d %d %d %d %d %f\n",
					(int)(Rect.right - Rect.left), (int)(Rect.bottom - Rect.top), X32o_delay,
					X32i_delay, chstart, chstop, usebus, NOM, X32sensitivity);
			fprintf(res_file, "%s\n", X32_busnum);
			fprintf(res_file, "%s\n", Xip_str);
			for (i = 0; i < 32; i++) {
				fprintf(res_file, "%c%c%c%c", Fader0[i][24], Fader0[i][25], Fader0[i][26], Fader0[i][27]);
				fprintf(res_file, "%c%c%c%c", Fader1[i][24], Fader1[i][25], Fader1[i][26], Fader1[i][27]);
			}
			fclose(res_file);
		}
		if (Xconnected && Automix_on) {
			ExitAutomix();
			close(Xfd);
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
void GetPanelData() {

	GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);

	GetWindowText(hwnodely, X32o_delay_str, GetWindowTextLength(hwnodely) + 1);
	sscanf(X32o_delay_str, "%d", &X32o_delay);

	GetWindowText(hwnidely, X32i_delay_str, GetWindowTextLength(hwnidely) + 1);
	sscanf(X32i_delay_str, "%d", &X32i_delay);

	GetWindowText(hwndsens, X32sens_str, GetWindowTextLength(hwndsens) + 1);
	sscanf(X32sens_str, "%f", &X32sensitivity);

	GetWindowText(hwndstart, X32_start_str, GetWindowTextLength(hwndstart) + 1);
	sscanf(X32_start_str, "%d", &chstart);

	GetWindowText(hwndstop, X32_stop_str, GetWindowTextLength(hwndstop) + 1);
	sscanf(X32_stop_str, "%d", &chstop);

	GetWindowText(hwndbnum, X32_busnum, GetWindowTextLength(hwndbnum) + 1);
	Bus[5] = X32_busnum[0];
	Bus[6] = X32_busnum[1];

	return;
}
//
//
int main(int argc, char **argv) {
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmdShow = 0;
	int i, j;

	Xip_str[0] = 0;
	// load resource file
	if ((res_file = fopen("./.X32Automix.ini", "r")) != NULL) {
		// get and remember real path
		if ((Finiretval = GetFullPathNameA("./.X32Automix.ini", 1024, Fullfilename,	FinilppPart)) > 1024) {
			printf("Not enough room for file name/path\n");
		}
		fscanf(res_file, "%d %d %d %d %d %d %d %d %f\n", &wWidth, &wHeight, &X32o_delay, &X32i_delay,
												   &chstart, &chstop,
												   &usebus, &NOM, &X32sensitivity);
		fgets(X32_busnum, sizeof(X32_busnum), res_file);
		X32_busnum[strlen(X32_busnum) - 1] = 0;

		fgets(Xip_str, sizeof(Xip_str), res_file);
		Xip_str[strlen(Xip_str) - 1] = 0;

		for (i = 0; i < 32; i++) {
			for (j = 0; j < 8; j++) fscanf(res_file, "%c", &RFader[i][j]);
		}
		fclose(res_file);

		ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
		wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
	}
	return 0;
}
//
void ExitAutomix() {
	int i;
	if (Xconnected) {
		for (i = chstart - 1; i < chstop; i++) {
			FaderH[i] = FALSE;
			if (sendto(Xfd, Fader0[i], 28, 0, Xip_addr, Xip_len) < 0) exit(1);
			X32Ftim_bfr[i] = time(NULL);
		}
	}
}
//
void add_3db(int a) {
	int i;
	float f;
//
	printf("adding: %d to main/bus level\n", a); fflush(stdout);

// add 'a' to L/R or Bus mix level. 'a' is in db.
//
//	level: a float with value in [0.0...1.0 (+10 dB), steps]  ->  4	"linear" dB ranges:
//	0.0...0.0625 (-oo, -90...-60 dB),
//	0.0625...0.25 (-60...-30 dB),
//	0.25...0.5 (-30...-10dB) and
//	0.5...1.0 (-10...+10dB)
//
// read L/R or Bus mix level & convert to db
	if (sendto(Xfd, LRmix, 20, 0, Xip_addr, Xip_len) < 0) exit(1);
	do { RPOLL								// X32 sent something?
		if (p_status > 0) { RECV				// yes!
		}										// wait for the right data...
	} while (strncmp(r_buf, LRmix, 17) != 0);	// meters are coming in too!
// test for LR/bus levels command back from X32
//			convert to db and update
	for (i = 0; i < 4; i++) endian.cc[i] = r_buf[27 - i];
	f = endian.ff;
//	printf("f before: %f\n", f); fflush(stdout);
	if (f >= 0.5)			f = f * 40. - 30.;
	else if (f >= 0.25)		f = f * 80. - 50.;
	else if (f >= 0.0625)	f = f * 160. - 70.;
	else if (f >= 0.0)		f = f * 480. - 90.;
// update in db and manage min/max situations
//	printf("f before in db: %f\n", f); fflush(stdout);
	f += a;
	if (f > 10.) f = 10.;
	if (f < -90.) f = -90.;
//	printf("f after in db: %f\n", f); fflush(stdout);
// convert back to float
	if (f < -60.) 			f = (f + 90.) / 480.;
	else if (f < -30.)		f = (f + 70.) / 160.;
	else if (f < -10.)		f = (f + 50.) / 80.;
	else if (f <= 10.)		f = (f + 30.) / 40.;
	f = roundf(f * 1024) / 1024;
	if (f < 0.0) f = 0.0;
	if (f > 1.0) f = 1.0;
	endian.ff = f;
	for (i = 0; i < 4; i++) LRmix[27 - i] = endian.cc[i];
//	printf("f after: %f\n", f); fflush(stdout);
	if (sendto(Xfd, LRmix, 28, 0, Xip_addr, Xip_len) < 0) exit(1);
// write back L/R or Bus mix level in float.
// ignore time outs and errors
	return;
}


//
void RunAutomix() {
int ch, i;

	X32Rmte_now = time(NULL);	// register for X32 data echo
	if (X32Rmte_now > X32Rmte_bfr) {
		if (sendto(Xfd, xremote, 12, 0, Xip_addr, Xip_len) < 0)
			exit(1);
		if (sendto(Xfd, Meters, 40, 0, Xip_addr, Xip_len) < 0)
			exit(1);
		X32Rmte_bfr = X32Rmte_now + XTIMEOUT;
//		printf("sent xstuff\n"); fflush(stdout);
	}
	RPOLL				// X32 sent something?
	if (p_status > 0) {	// yes!
		RECV
//		printf("r_len: %i\n", r_len); fflush(stdout);
		if (strncmp(r_buf, Meters + 16, 9) == 0) {	// test for meter levels command back from X32
			for (ch = chstart - 1; ch < chstop; ch++) {
				ff = (float *)(r_buf + 24 + 4*ch);	// ff points to channel 'ch' level
//				printf("ch: %d %f\n", ch, *ff); fflush(stdout);
				if (*ff > X32sensitivity) {			// if passed threshold level
					X32Ftim_bfr[ch] = time(NULL);	// update ch Fader timer
					if (!FaderH[ch]) {				// if not already "up"/active
						FaderH[ch] = TRUE;			// Fader is "up"
						active_ch += 1;				// one more channel active
						if (sendto(Xfd, Fader1[ch], 28, 0, Xip_addr, Xip_len) < 0)
							exit(1);
					}
				} else {
					if (FaderH[ch]) {				// if not already "down" and delay passed
						if((X32Ftim_bfr[ch] + X32o_delay) < time(NULL)) {
							active_ch -= 1;			// one less channel active
							FaderH[ch] = FALSE;		// Fader is "down"
							if (sendto(Xfd, Fader0[ch], 28, 0, Xip_addr, Xip_len) < 0)
								exit(1);
						}
					}
				}
				if (NOM) {	// manage Number Of Mixes
					if (active_ch != old_active_ch) {
//						printf("active %d, oldactive %d, save %d\n", active_ch,old_active_ch, act_ch_sav); fflush (stdout);
						if (active_ch >= (act_ch_sav * 2)) {
							add_3db(-3);			// NOM doubled -> -3dB on mix/LR
							act_ch_sav *= 2;		// save new NOM level
							if (act_ch_sav > 32) act_ch_sav = 32;
						} else if (active_ch <= (act_ch_sav / 2)) {
							if (active_ch) add_3db(+3);
							act_ch_sav /= 2;		// less that half NOM -> +3dB on mix/LR
							if (act_ch_sav < 1) act_ch_sav= 1;
						}
						old_active_ch = active_ch;
					}
				}
			}
		} else {			// Treat other commands of interest (fader levels)
			if (usebus) {	// Update bus mix or ch mix levels as requested
				if (strncmp(r_buf + 6, Bus, 13) == 0) { // test for bus level change
					ch = ((r_buf [4] - '0') * 10) + (r_buf[5] - '0') - 1;
//					printf("ch= %d\n", ch); fflush (stdout);
					if (FaderH[ch]) {		// Updade bus level "up" or "down" position
						for (i = 24; i < 28; i++) Fader1[ch][i] = r_buf[i];
					} else {
						for (i = 24; i < 28; i++) Fader0[ch][i] = r_buf[i];
					}
				}
			} else {
				if (strncmp(r_buf + 6, Fader, 10) == 0) { // test for fader change
					ch = ((r_buf [4] - '0') * 10) + (r_buf[5] - '0') - 1;
//					printf("ch= %d\n", ch); fflush (stdout);
					if (FaderH[ch]) {		// Updade fader "up" or "down" position
						for (i = 24; i < 28; i++) Fader1[ch][i] = r_buf[i];
					} else {
						for (i = 24; i < 28; i++) Fader0[ch][i] = r_buf[i];
					}
				}
			}
		}
		//	Ignore other data from X32
	} else if (p_status < 0) {	// no data back from X32
		keep_on = 0; // Exit on error (p_status < 0)
		printf("Error\n"); fflush(stdout);
	}
	return;
}


