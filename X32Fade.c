//
// X32Fade.c
//
//  Created on: Dec 30, 2014
//      Author: Patrick-Gilles Maillot
//
//	A program to automatically control X32 Faders (in and out)
// 	Time of fade_in and fade_out and # of steps can be controlled
//	An ongoing action can be aborted too if needed
//
//	Current settings can be saved and restored from one use to another
//
//	Note - Despite ifdefs for Windows, this program will only run in
//	a Windows environment because of calls to MSC functions. The non-
//	Windows sections are used in a core version of the program that can
//	be ported or used with different GUI environments.
//
// change log:
//      v1.00: refactoring of code, passing some calls into extern modules
//      v1.01: preventing Windows resizing
//      v1.02: small changes to GUI geometry
//      v2.00: Introduced MIDI control for Fade IN, Fade OUT and STOP on user bank C, buttons 5,6,7 (see Xmidi)
//
//
#ifdef __WIN32__
#include <windows.h>
#define millisleep(a)	Sleep((a))
#else
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#define millisleep(a)	usleep((a)*1000)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#ifndef timersub
#define timersub(a, b, result)								\
	do {													\
		(result)->tv_sec = (a)->tv_sec - (b)->tv_sec;		\
		(result)->tv_usec = (a)->tv_usec - (b)->tv_usec;	\
		if ((result)->tv_usec < 0) { 						\
			  --(result)->tv_sec;							\
			  (result)->tv_usec += 1000000; 				\
		} 													\
	} while (0)
#endif

#ifndef timeradd
#define timeradd(a, b, result)								\
	do {													\
		(result)->tv_usec = (a)->tv_usec + (b)->tv_usec;	\
		(result)->tv_sec = (a)->tv_sec + (b)->tv_sec;		\
			if ((result)->tv_usec >= 1000000) {				\
			(result)->tv_usec -= 1000000;					\
			(result)->tv_sec++;								\
		}													\
	} while (0)
#endif

#ifndef timercmp
#define timercmp(a, b, CMP)									\
	do {													\
		(((a)->tv_sec == (b)->tv_sec) ?						\
		((a)->tv_usec CMP (b)->tv_usec) :					\
		((a)->tv_sec CMP (b)->tv_sec))						\
	} while (0)
#endif


#define BSIZE 256
#define ck_width 58
#define	ck_height 25;

void				Fader_init();
void 				Fader_update(int len, int f_index, int v_index, int dir);
void 				Fader_read(int len, int f_index, int v_index);
void				XFade_in();
void				XFade_out();
//
extern int			X32Connect(int Xconnected, char* Xip_str, char* port, int btime);
extern int			validateIP4Dotted(const char *s);
extern int 			Xsprint(char *bd, int index, char format, void *bs);
extern int 			Xfprint(char *bd, int index, char* text, char format, void *bs);
extern void			Xfdump(char *header, char *buf, int len, int debug);


WINBASEAPI HWND WINAPI	GetConsoleWindow(VOID);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE	hInstance = 0;
HWND		hwndipaddr, hwndfdin, hwndfdout, hwndfdstp, hwndxbank;

HMENU		hipedit = (HMENU)199;
HWND		hwndcheck[80];
HWND		all_check[5];
HWND		hwndfdact[6];
LPWSTR 		txt_fdact[6] = {L"Connect", L"Check IN", L"Check OUT", L"Fade IN", L"Fade OUT", L"STOP"};
HFONT		hfont, htmp;
HDC			hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP		hBmp;
BITMAP		bmp;




int		keep_running = 1;
int		len, x, y;
int		wWidth = 70 + 16 * ck_width;
int		check[80];

wchar_t	W32_ip_str[20], W32_fd_in[8], W32_fd_out[8], W32_fd_stp[8];
char	Xip_str[20], Xfd_in[8], Xfd_out[8], Xfd_stp[8];
int		Xconnected = 0;
char	Xport[]= "10023";

int		ck_all[5] = {0, 0, 0, 0, 0};
int		ck_for[6] = {0, 16, 32, 48, 64, 80};

int		fd_for[9] = {0, 32, 48, 56, 64, 72, 78, 79, 80};

char	*Xmsg[8] = {	"/ch/00/mix/fader", "/bus/00/mix/fader",
						"/auxin/00/mix/fader", "/fxrtn/00/mix/fader",
						"/dca/0/fader", "/mtx/00/mix/fader",
						"/main/m/mix/fader", "/main/st/mix/fader"};
char	XRemote[12] = "/xremote";	// OSC data sent every 9..10 seconds to keep receiving X32 notifications
int		ind_num[8] = {4, 5, 7, 7, 5, 5, 0, 0};
int		ind_len[8] = {2, 2, 2, 2, 1, 2, 0, 0};
int		ind_pos[8] = {24,24,24,24,20,24,24,24};

float	fader_values[80], deck_values[80];
int		More_steps = 0;
int		Fsteps = 32;
int		Fade_on = 0;
int		FadeOut_time = 0;
int		Fade_out = 0;
int		FadeIn_time = 0;
int		Fade_in = 0;
int		XMidi = 0;
//
int					Xverbose, Xdebug;
//
int					Xfd;				// our socket
struct sockaddr_in	Xip;
struct sockaddr		*Xip_addr = (struct sockaddr*)&Xip;
char				r_buf[BSIZE], s_buf[BSIZE];
int					r_len, s_len, p_status;
struct timeval		timeout;
fd_set 				ufds;
#ifdef __WIN32__
WSADATA 			wsa;
int					Xip_len = sizeof(Xip);	// length of addresses
#else
socklen_t			Xip_len = sizeof(Xip);	// length of addresses
#endif
//
struct timeval		Tstart, Tstep, Tnow;
time_t  			X32RemBfr;				// 'before' time value for Xremote controls (seconds)
time_t  			X32RemNow;				// 'now' time value for Xremote controls (seconds)//
FILE 				*res_file;
int					r_status;
//
// Union for endian conversion
union cast {
	float			f;
	int				i;
	unsigned int	u;
	char			c[4];
} mm;
//
//
#define RPOLL													\
	do {														\
		FD_ZERO (&ufds);										\
		FD_SET (Xfd, &ufds);									\
		p_status = select(Xfd+1,&ufds,NULL,NULL,&timeout);		\
	} while (0);
//
//


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
MSG	msg;
int	i, j;

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32Fade";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"X32Fade - Time Control X32 Faders",
			WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU,
			100, 220, wWidth, 225, 0, 0, hInstance, 0);

	while (keep_running) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Something from the X32? (resulting from MIDI messages)
		if (XMidi) {
			X32RemNow = time(NULL);				// Get current time
			if (X32RemNow > X32RemBfr) { 		// Need to keep xremote alive?
				if (sendto(Xfd, XRemote, 12, 0, Xip_addr, Xip_len) < 0) {
					perror ("Can't send data to X/M32\n");
					exit(EXIT_FAILURE);
				}
				X32RemBfr = X32RemNow + 9;
			}
			RPOLL;
			if (p_status < 0) {
				perror("Polling for data failed");
				Fade_in = Fade_out = 0; // stop fade
			} else if (p_status > 0) {
			// We have received data - process it!
				r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0); //(struct sockaddr *)&Xip, &Xip_len);
				//
				if (strcmp(r_buf, "/-stat/userpar/17/value") == 0) {
					// button C-17 pressed or released - FADE IN
					if (r_buf[31]) {
						XFade_in();
					}
				}
				if (strcmp(r_buf, "/-stat/userpar/18/value") == 0) {
					// button C-18 pressed or released - FADE OUT
					if (r_buf[31]) {
						XFade_out();
					}
				}
				if (strcmp(r_buf, "/-stat/userpar/19/value") == 0) {
					// button C-19 pressed or released - STOP
					if (r_buf[31]) {
						Fade_on = Fade_in = Fade_out = 0;
					}
				}
			}
		}
		if (Fade_on) {
			if (More_steps) {
				gettimeofday(&Tnow, NULL);				// get precise time
				if (!(timercmp(&Tnow, &Tstart, <))) {	// if Tnow >= Tstart
					for (len = 0; len < 8; len++) {
						for (i = fd_for[len]; i < fd_for[len+1]; i++) {
							j = i - fd_for[len];
							if (check[i]) { // if channel "i" clicked
								if (Fade_in) {
									Fader_update(len, j, i, Fsteps- More_steps + 1);
								} else if (Fade_out) {
									Fader_update(len, j, i, More_steps - 1);
								}
							}
						}
					}
					timeradd(&Tstep, &Tstart, &Tstart);	// Tstart += Tstep
					--More_steps;
				}
			} else {
				Fade_on = 0; // Done
			}
		} else {
			millisleep(1); // wait a little (avoid consuming CPU for nothing)
		}
	}
	return (int) msg.wParam;
}

void XFade_in() {
	sscanf(Xfd_stp, "%d", &Fsteps);

	if (Fsteps < 1) {
		Fsteps = 1;
		sprintf(Xfd_stp, "%d", Fsteps);
		SetWindowText(hwndfdstp, (LPSTR)Xfd_stp);
	} else 	if (Fsteps > 128) {
		Fsteps = 128;
		sprintf(Xfd_stp, "%d", Fsteps);
		SetWindowText(hwndfdstp, (LPSTR)Xfd_stp);
	}
	More_steps = Fsteps;
	len = GetWindowTextLengthW(hwndfdin) + 1;
	GetWindowTextW(hwndfdin, W32_fd_in, len);
	WideCharToMultiByte(CP_ACP, 0, W32_fd_in, len, Xfd_in, len, NULL, NULL);
	sscanf(Xfd_in, "%d", &FadeIn_time);
	Fade_out = 0;
	Fade_in = 1;
	Fade_on = 1;
	if (Xdebug) {printf("Fade IN: %s, steps: %s\n", Xfd_in, Xfd_stp); fflush(stdout);}
	gettimeofday(&Tstart, NULL);						// get precise time
	if (FadeIn_time > 0) {
		if (FadeIn_time > 120) {
			FadeIn_time = 120;
			sprintf(Xfd_in, "%d", FadeIn_time);
			SetWindowText(hwndfdin, (LPSTR)Xfd_in);
		}
		Tstep.tv_sec = 0;
		Tstep.tv_usec = FadeIn_time * 1000000 / Fsteps;
		while (Tstep.tv_usec > 1000000) {
			Tstep.tv_sec++;
			Tstep.tv_usec -= 1000000;
		}
	} else {					// set default time-step for Fade time = 0
		Tstep.tv_sec = 0;
		Tstep.tv_usec = 1000;
		FadeIn_time = 0;
		sprintf(Xfd_in, "%d", FadeIn_time);
		SetWindowText(hwndfdin, (LPSTR)Xfd_in);
	}
}
//
//
void XFade_out() {
	sscanf(Xfd_stp, "%d", &Fsteps);
	if (Fsteps < 1) {
		Fsteps = 1;
		sprintf(Xfd_stp, "%d", Fsteps);
		SetWindowText(hwndfdstp, (LPSTR)Xfd_stp);
	} else 	if (Fsteps > 128) {
		Fsteps = 128;
		sprintf(Xfd_stp, "%d", Fsteps);
		SetWindowText(hwndfdstp, (LPSTR)Xfd_stp);
	}
	More_steps = Fsteps;
	len = GetWindowTextLengthW(hwndfdout) + 1;
	GetWindowTextW(hwndfdout, W32_fd_out, len);
	WideCharToMultiByte(CP_ACP, 0, W32_fd_out, len, Xfd_out, len, NULL, NULL);
	sscanf(Xfd_out, "%d", &FadeOut_time);
	Fade_in = 0;
	Fade_out = 1;
	Fade_on = 1;
	if (Xdebug) {printf("Fade OUT: %s, steps: %s\n", Xfd_out, Xfd_stp); fflush(stdout);}
	gettimeofday(&Tstart, NULL);						// get precise time
	if (FadeOut_time > 0) {
		if (FadeOut_time > 120) {
			FadeOut_time = 120;
			sprintf(Xfd_out, "%d", FadeOut_time);
			SetWindowText(hwndfdout, (LPSTR)Xfd_out);
		}
		Tstep.tv_sec = 0;
		Tstep.tv_usec = FadeOut_time * 1000000 / Fsteps;
		while (Tstep.tv_usec > 1000000) {
			Tstep.tv_sec++;
			Tstep.tv_usec -= 1000000;
		}
	} else {					// set default time-step for Fade time = 0
		Tstep.tv_sec = 0;
		Tstep.tv_usec = 1000;
		FadeOut_time = 0;
		sprintf(Xfd_out, "%d", FadeOut_time);
		SetWindowText(hwndfdout, (LPSTR)Xfd_out);
	}
}
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
int 	i;
char 	str1[8];

	switch (msg) {
	case WM_CREATE:
//
		hwndipaddr = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				125, 35, 150, 20, hwnd, hipedit, NULL, NULL);
		hwndfdact[0] = CreateWindowW(L"button", L"Connect", WS_VISIBLE | WS_CHILD,
				280, 35, 80, 20, hwnd, (HMENU)200, NULL, NULL);
		//
		hwndxbank = CreateWindowW(L"button", L"MIDI OFF", WS_VISIBLE | WS_CHILD,
				280, 15, 80, 20, hwnd, (HMENU)206, NULL, NULL);
		//

		for (len = 1; len < 6; len++) {
			hwndfdact[len] = CreateWindowW(L"button", txt_fdact[len], WS_VISIBLE | WS_CHILD,
					465 + 87 * len, 7, 80, 50, hwnd,
					(HMENU)(len + 200),	0, 0);
		}
//
		for (len = 0; len < 5; len ++) {
			all_check[len] = CreateWindowW(L"button", L"All->", WS_VISIBLE | WS_CHILD,
					5, 70 + len * 25, 50, 20, hwnd,
					(HMENU)(100 + len), NULL, NULL);
			for (i = ck_for[len]; i < ck_for[len + 1]; i++) {
				hwndcheck[i] = CreateWindowW(L"button", L"", BS_CHECKBOX | WS_VISIBLE | WS_CHILD,
					5 + ck_width * (i - ck_for[len] + 1), 70 + len * 25, 15, 20, hwnd,
					(HMENU)i, NULL, NULL);
			}
		}
//
		hwndfdin = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				375, 2, 50, 20, hwnd, hipedit, NULL, NULL);
		hwndfdout = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				375, 21, 50, 20, hwnd, hipedit, NULL, NULL);
		hwndfdstp = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				375, 40, 50, 20, hwnd, hipedit, NULL, NULL);
//
		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./.X32Fade.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_SHARED);
		if(hBmp==NULL) {
			perror("Pixel bitmap file not found");
		}
		break;
//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
//
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 5, 2, 100, 55, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
//
		MoveToEx(hdc, 5, 62, NULL);
		LineTo(hdc, wWidth - 10, 62);
		MoveToEx(hdc, 370, 58, NULL);
		LineTo(hdc, 370, 2);
		MoveToEx(hdc, 540, 58, NULL);
		LineTo(hdc, 540, 2);
//
		SetBkMode(hdc, TRANSPARENT);
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 20, "Enter X32 IP below:", 19);
		TextOut(hdc, 240, 20, "v. 2.00", 9);
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 0, "X32Fade -2015- Patrick-Gilles Maillot", 40);
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		SetBkMode(hdc, TRANSPARENT);
		for (i = 0; i < 16; i++)
			TextOut(hdc, 80 + i * ck_width, 72, str1,
					wsprintf(str1, "Ch %02d", i + 1));
		for (i = 0; i < 16; i++)
			TextOut(hdc, 80 + i * ck_width, 97, str1,
					wsprintf(str1, "Ch %02d", i + 17));
		for (i = 0; i < 16; i++)
			TextOut(hdc, 80 + i * ck_width, 122, str1,
					wsprintf(str1, "Bus%02d", i + 1));
		for (i = 0; i < 8; i++)
			TextOut(hdc, 80 + i * ck_width, 147, str1,
					wsprintf(str1, "Aux%02d", i + 1));
		for (i = 0; i < 8; i++)
			TextOut(hdc, 80 + (i + 8) * ck_width, 147, str1,
					wsprintf(str1, "FX%02d", i + 1));
		for (i = 0; i < 8; i++)
			TextOut(hdc, 80 + i * ck_width, 172, str1,
					wsprintf(str1, "Dca %1d", i + 1));
		for (i = 0; i < 6; i++)
			TextOut(hdc, 80 + (i + 8) * ck_width, 172, str1,
					wsprintf(str1, "Mtx%02d", i + 1));
		TextOut(hdc, 80 + 14 * ck_width, 172, str1, wsprintf(str1, "M/C"));
		TextOut(hdc, 80 + 15 * ck_width, 172, str1, wsprintf(str1, "L/R"));

		TextOut(hdc, 430, 3, str1, wsprintf(str1, "Fade IN time (s)"));
		TextOut(hdc, 430, 22, str1, wsprintf(str1, "Fade OUT time (s)"));
		TextOut(hdc, 430, 42, str1, wsprintf(str1, "Fade Steps"));

		DeleteObject(htmp);
		DeleteObject(hfont);
		EndPaint(hwnd, &ps);
		break;
//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			if ((len = LOWORD(wParam)) < 80) {
				if (check[len])	SendMessage(hwndcheck[len], BM_SETCHECK, BST_UNCHECKED, 0);
				else			SendMessage(hwndcheck[len], BM_SETCHECK, BST_CHECKED, 0);
				check[len] ^= 1;
				if (Xdebug) {printf ("Check Button: %d clicked\n", len); fflush(stdout);}
			} else {
				switch(len) {
				case 100: case 101: case 102: case 103: case 104:
					len = LOWORD(wParam) - 100;
					if (Xdebug) {printf("ALL-> button %d hit\n", len); fflush(stdout);}
					ck_all[len] ^= 1;
					for (i = ck_for[len]; i < ck_for[len + 1]; i ++) {
						SendMessage(hwndcheck[i], BM_SETCHECK, ck_all[len], 0);
						check[i] = ck_all[len];
					}
					break;
				case 200: 		// Connect button hit
					if (Xdebug) {printf("Connect button hit\n"); fflush(stdout);}
					len = GetWindowTextLengthW(hwndipaddr) + 1;
					GetWindowTextW(hwndipaddr, W32_ip_str, len);
					WideCharToMultiByte(CP_ACP, 0, W32_ip_str, len, Xip_str, len, NULL, NULL);
					if (Xdebug) {printf("IP = %s\n", Xip_str); fflush(stdout);}
					if (validateIP4Dotted(Xip_str)) {
						Xconnected = X32Connect(Xconnected, Xip_str, Xport, 5000);
						if (Xconnected)	SetWindowTextW(hwndfdact[0], L"Connected");
						else			SetWindowTextW(hwndfdact[0], L"Connect");
					} else {
						perror("incorrect IP string form");
					}
					break;
				case 201:		// Check IN button hit
					if (Xdebug) {printf("Check IN button hit\n"); fflush(stdout);}
					if ((res_file = fopen(".X32Fade.ini", "r+")) > 0) {
						for (i = 0; i < 80; i++) {
							r_status = fscanf(res_file, "%d", &check[i]);
							SendMessage(hwndcheck[i], BM_SETCHECK, check[i], 0);
							r_status = fscanf(res_file, "%08x ", &mm.i);
							fader_values[i] = deck_values[i] = mm.f;
						}
						r_status = fscanf(res_file, "%d %d %d %17s", &FadeOut_time, &FadeIn_time, &Fsteps, Xip_str);
						SetWindowText(hwndipaddr, (LPSTR)Xip_str);
						sprintf(Xfd_in, "%d", FadeIn_time);
						SetWindowText(hwndfdin, (LPSTR)Xfd_in);
						sprintf(Xfd_out, "%d", FadeOut_time);
						SetWindowText(hwndfdout, (LPSTR)Xfd_out);
						sprintf(Xfd_stp, "%d", Fsteps);
						SetWindowText(hwndfdstp, (LPSTR)Xfd_stp);
						fclose (res_file);
					} else {
						perror ("Warning: Resource file not found, levels set from X32 if connected");
					}
					if (Xconnected) {
						Fader_init(); // override fader_values and deck_values from console
					}
					break;
				case 202:		// Check OUT button hit
					if (Xdebug) {printf("Check OUT button hit\n"); fflush(stdout);}
					if (Xconnected) {
						Fader_init(); // read X32 fader values
					}
					if ((res_file = fopen(".X32Fade.ini", "w")) > 0) {
						len = GetWindowTextLengthW(hwndfdstp) + 1;
						GetWindowTextW(hwndfdstp, W32_fd_stp, len);
						WideCharToMultiByte(CP_ACP, 0, W32_fd_stp, len, Xfd_stp, len, NULL, NULL);
						sscanf(Xfd_stp, "%d", &Fsteps);
						if (Fsteps > 128) {
							Fsteps = 128;
							sprintf(Xfd_stp, "%d", Fsteps);
							SetWindowText(hwndfdstp, (LPSTR)Xfd_stp);
						}
						len = GetWindowTextLengthW(hwndfdin) + 1;
						GetWindowTextW(hwndfdin, W32_fd_in, len);
						WideCharToMultiByte(CP_ACP, 0, W32_fd_in, len, Xfd_in, len, NULL, NULL);
						sscanf(Xfd_in, "%d", &FadeIn_time);
						if (FadeIn_time > 120) {
							FadeIn_time = 120;
							sprintf(Xfd_in, "%d", FadeIn_time);
							SetWindowText(hwndfdin, (LPSTR)Xfd_in);
						}
						len = GetWindowTextLengthW(hwndfdout) + 1;
						GetWindowTextW(hwndfdout, W32_fd_out, len);
						WideCharToMultiByte(CP_ACP, 0, W32_fd_out, len, Xfd_out, len, NULL, NULL);
						sscanf(Xfd_out, "%d", &FadeOut_time);
						if (FadeOut_time > 120) {
							FadeOut_time = 120;
							sprintf(Xfd_out, "%d", FadeOut_time);
							SetWindowText(hwndfdout, (LPSTR)Xfd_out);
						}
						for (i = 0; i < 80; i++) {
							// save check buttons and fader values at checkout time
							fprintf(res_file, "%d ", check[i]);
							mm.f = deck_values[i];
							fprintf(res_file, "%08x ", mm.u);
						}
						fprintf(res_file, "%d %d %d ", FadeOut_time, FadeIn_time, Fsteps);
						if (!Xconnected) {
							len = GetWindowTextLengthW(hwndipaddr) + 1;
							GetWindowTextW(hwndipaddr, W32_ip_str, len);
							WideCharToMultiByte(CP_ACP, 0, W32_ip_str, len, Xip_str, len, NULL, NULL);
						}
						fprintf(res_file, "%s ", Xip_str);
						fclose (res_file);
					} else {
						perror ("Cannot create resource file");
					}
					break;
				case 203:		// Fade IN button hit
					if (Xdebug) {printf("Fade IN button hit\n"); fflush(stdout);}
					if (Xconnected) {
						len = GetWindowTextLengthW(hwndfdstp) + 1;
						GetWindowTextW(hwndfdstp, W32_fd_stp, len);
						WideCharToMultiByte(CP_ACP, 0, W32_fd_stp, len, Xfd_stp, len, NULL, NULL);
						XFade_in();
					} else {
						perror ("Not connected to X32!");
					}
					break;
				case 204:		// Fade OUT button hit
					if (Xdebug) {printf("Fade OUT button hit\n"); fflush(stdout);}
					if (Xconnected) {
						len = GetWindowTextLengthW(hwndfdstp) + 1;
						GetWindowTextW(hwndfdstp, W32_fd_stp, len);
						WideCharToMultiByte(CP_ACP, 0, W32_fd_stp, len, Xfd_stp, len, NULL, NULL);
						XFade_out();
					} else {
						perror ("Not connected to X32!");
					}
					break;
				case 205:		// STOP button hit
					if (Xdebug) {printf("STOP button hit\n"); fflush(stdout);}
					Fade_on = Fade_in = Fade_out = 0;
					break;
				case 206:		// MIDI button
					if (Xconnected) {
						XMidi ^= 1;
						if (XMidi) {
							SetWindowTextW(hwndxbank, L"MIDI ON");
							// program USER keys in bank C depending on Xbank value
							s_len = Xfprint(s_buf, 0, "/config/userctrl/C/btn/5", 's', "MC16012");
							if (sendto(Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {
								perror("Error sending data");
							}
							s_len = Xfprint(s_buf, 0, "/config/userctrl/C/btn/6", 's', "MC16013");
							if (sendto(Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {
								perror("Error sending data");
							}
							s_len = Xfprint(s_buf, 0, "/config/userctrl/C/btn/7", 's', "MC16014");
							if (sendto(Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {
								perror("Error sending data");
							}
						} else {
							SetWindowTextW(hwndxbank, L"MIDI OFF");
							// Erase USER keys
							s_len = Xfprint(s_buf, 0, "/config/userctrl/C/btn/5", 's', "");
							if (sendto(Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {
								perror("Error sending data");
							}
							s_len = Xfprint(s_buf, 0, "/config/userctrl/C/btn/6", 's', "");
							if (sendto(Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {
								perror("Error sending data");
							}
							s_len = Xfprint(s_buf, 0, "/config/userctrl/C/btn/7", 's', "");
							if (sendto(Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {
								perror("Error sending data");
							}
						}
					} else {
						perror ("Not connected to X32!");
					}
					break;
				}
			}
		}
		break;
//
	case WM_DESTROY:
		PostQuitMessage(0);
		keep_running = 0;
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}


void Fader_init() {
	int i, j;
//
	for (len = 0; len < 8; len++) {
		for (i = fd_for[len]; i < fd_for[len+1]; i++) {
			j = i - fd_for[len];
			if (check[i]) { // if channel "j" clicked
				Fader_read(len, j, i);
			}
		}
	}
	return;
}

void Fader_read(int len, int f_index, int v_index) {
//
int		k, l;
char 	*fader_str = Xmsg[len];
int 	b_index = ind_num[len];
int 	format = ind_len[len];
int 	fad_pos = ind_pos[len];
//
// read channel i fader value
	f_index += 1;										// Fader # start at 1 or 01
	s_len = Xsprint(s_buf, 0, 's', fader_str);
	if (format == 1) {
		s_buf[b_index] = (char) (f_index + 48);			// 1 character index
	} else if (format == 2) {
		s_buf[b_index] = f_index / 10 + 48;				// 2 characters index
		s_buf[b_index + 1] = f_index -((f_index / 10) * 10) + 48;
	} 													// no index
	if (Xverbose) {Xfdump("->X", s_buf, s_len, Xdebug); fflush(stdout);}
	if (sendto(Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {
		perror("Error sending data");
		Fade_in = Fade_out = 0; // stop fade
	} else {
		RPOLL;
		if (p_status < 0) {
			perror("Polling for data failed");
			Fade_in = Fade_out = 0; // stop fade
		} else if (p_status > 0) {
		// We have received data - process it!
			r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0); //(struct sockaddr *)&Xip, &Xip_len);
			if (Xverbose) {Xfdump("X->", r_buf, r_len, Xdebug); fflush(stdout);}
			// float value starts at character index fad_pos
			for (k = 4, l = 0; l < 4; l++) {			// adapt endianness
				mm.c[--k] = r_buf[l + fad_pos];
			}
			fader_values[v_index] = deck_values[v_index] = mm.f;						// set fader value
			if(Xdebug) {printf("Set Fader[%d]: %f\n",v_index, fader_values[v_index]); fflush(stdout);}
		} else {
			perror("X32 reception timeout");
			Fade_in = Fade_out = 0; // stop fade
		}
	}
	return;
}


void Fader_update(int len, int f_index, int v_index, int ind) {
//
char 	*fader_str = Xmsg[len];
int 	b_index = ind_num[len];
int 	format = ind_len[len];


	deck_values[v_index] = (fader_values[v_index] / Fsteps) * ind;
	s_len = Xfprint(s_buf, 0, fader_str, 'f', &deck_values[v_index]);
	// set channel fader index
	f_index += 1;									// faders # start at 1 or 01
	if (format == 1) {
		s_buf[b_index] = (char) (f_index + 48);		// 1 character index
	} else if (format == 2) {
		s_buf[b_index] = f_index / 10 + 48;			// 2 characters index
		s_buf[b_index + 1] = f_index -((f_index / 10) * 10) + 48;
	} 												// no index
	if (Xverbose) {Xfdump("->X", s_buf, s_len, Xdebug); fflush(stdout);}
	if (sendto(Xfd, s_buf, s_len, 0, Xip_addr, Xip_len) < 0) {
		perror("Error sending data");
		Fade_in = Fade_out = 0; // stop fade
	}
	return;
}


int
main(int argc, char **argv)
{
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int		nCmdShow = 0;
	char	input_ch;

	Xverbose = Xdebug = 0;
	// Manage arguments
	while ((input_ch = getopt(argc, argv, "i:d:v:h")) != -1) {
		switch (input_ch) {
		case 'd':
			sscanf(optarg, "%d", &Xdebug);
			break;
		case 'v':
			sscanf(optarg, "%d", &Xverbose);
			break;
		default:
		case 'h':
			printf("usage: X32Fade [-d 0/1, debug option] -default: 0\n");
			printf("               [-v 0/1, verbose option] -default: 0\n");
			return(0);
			break;
		}
	}
//
	if (Xverbose || Xdebug) {
		ShowWindow(GetConsoleWindow(), SW_SHOW); // show console window
	} else {
		ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	}
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
	return 0;
}
