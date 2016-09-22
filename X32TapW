//
// X32Tap.c
//
//  Created on: Apr 10, 2015
//
//      Author: Patrick-Gilles Maillot
//
// Changelog:
// v 0.10: removed incorrect use of FD_ISSET(Xfd, &ufds)) in receiving data
// v 0.20: added capability to auto detect tap from X32 channel + code cleanup
// v 0.21: changed timing value of /meter command
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
//
#include <windows.h>
//
WINBASEAPI HWND WINAPI	GetConsoleWindow(VOID);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE	hInstance = 0;
HWND 		hwnd, hwndipaddr, hwndconx, hwnddelslot, hwndprog, hwndGO, hwndcheck;
HWND 		hwndauto, hwndsens, hwndchnl;
HFONT		hfont, htmp;
HDC			hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP		hBmp;
BITMAP		bmp;
MSG			wMsg;
OPENFILENAME ofn;       // common dialog box structure
HANDLE hf;              // file handle
//
// Windows UI related storage
char	Xip_str[20], Xdel_str[20];
char	Xprg_str[32];
char	XTempo[32] = "";
char	Xerror[]   = "DLY not found";
//
// Private functions
int			LaunchXtap(float f);
void		XDeltaTime();
void 		XRcvClean();
void		XRunAutoTap();
//
// External references
extern int 	validateIP4Dotted(const char *s);
extern int	Xsprint(char *bd, int index, char format, void *bs);
extern int	Xfprint(char *bd, int index, char* text, char format, void *bs);
extern int	X32Connect(int Xconnected, char* Xip_str, int btime);
//
#define BSIZE 	512	// Buffer sizes (enough to take into account FX parameters)
#define DLY		10  // Stereo delay FX number
//
int XDelays[] =     {10, 11, 12, 21, 24, 25, 26};
char* XDelayNames[] = {"DLY", "3TAP", "4TAP", "D/RV", "D/CR", "D/FL", "MODD"};
//
int XDnums = sizeof(XDelays) / sizeof(int);
int Xdelay; //saves current number/index in XDelays
//
//
struct timeval		t_1, t_2, t_dlta;
float  d_tap;
//
int		wWidth = 515;
int		wHeight = 145;
//
FILE	*res_file;
char	Finipath[1024];	// resolved path to .ini file
char	**FinilppPart;
int 	Finiretval;
int		Xdel_slot = 0;
int		Xdel_found = 0;
time_t	X32Rmte_bfr, X32Rmte_now;	// For /xremote timer
char	xremote[12] = "/xremote";	// automatic trailing zeroes
char	Meters[] = "/meters\0,siii\0\0\0/meters/6\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
//                  /meters~,siii~~~/meters/6~~~ ~~~c ~~~~ ~~~~ ------------
//												28   32   36   40
// Misc. flags
int					Xconnected = 0;
int					XAuto = 0;
int					XChannel = 1;
float				XChSens = 0.5;
int					keep_running = 1;
char				XChSens_str[8];
char				XChannel_str[8];
//
// X32 communication buffers
char				r_buf[BSIZE], s_buf[BSIZE];
int					r_len, s_len;
int					p_status;
//
// UDP related data
WSADATA 			wsa;
fd_set 				ufds;
int					Xfd;				// our socket
struct sockaddr_in	Xip;
struct sockaddr*	Xip_addr = (struct sockaddr *)&Xip;
int					Xip_len = sizeof(Xip);	// length of addresses
struct timeval		timeout;
//
// type cast union
union littlebig {
	char	cc[4];
	int		ii;
	float	ff;
} endian;
//
//
#ifndef timersub
#	define timersub(a, b, result)							\
	do {													\
		(result)->tv_sec = (a)->tv_sec - (b)->tv_sec;		\
		(result)->tv_usec = (a)->tv_usec - (b)->tv_usec;	\
		if ((result)->tv_usec < 0) { 						\
			  --(result)->tv_sec;							\
			  (result)->tv_usec += 1000000; 				\
		} 													\
	} while (0)
#endif
//
//
// X32 Communication macros
//
#define SEND_TO(b, l)								\
	do {											\
		sendto(Xfd, b, l, 0, Xip_addr, Xip_len);	\
	} while (0);
//
//
#define RECV_FR(b, l)								\
	do {											\
		l = recvfrom(Xfd, b, BSIZE, 0, 0, 0);		\
	} while (0);
//
//
//
#define RPOLL												\
	do {													\
		FD_ZERO (&ufds);									\
		FD_SET (Xfd, &ufds);								\
		p_status = select(Xfd+1,&ufds,NULL,NULL,&timeout);	\
	} while (0);

#define MILLISLEEP(t)										\
	do {													\
		Sleep(t);											\
	} while (0);
//
//
//
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdTap)
{
	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32SetTap";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"X32Tap - Set Tap tempo for X32 Delay FX",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			100, 220, wWidth, wHeight, 0, 0, hInstance, 0);

	while (keep_running) {
		if (PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		}
		if (Xconnected) {
			// Read channel and sensitivity
			if (XAuto) XRunAutoTap();
		} else {
			Sleep(10); // avoid high CPU
		}
	}
	return (int) wMsg.wParam;
}
//
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
int 	i;
char 	str1[32];
//
	switch (msg) {
	case WM_CREATE:
		hwndprog = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
						360, 85, 135, 20, hwnd, (HMENU)0, NULL, NULL);
		hwndGO = CreateWindowW(L"button", L"TAP", WS_VISIBLE | WS_CHILD,
							360, 5, 135, 75, hwnd,
							(HMENU)3, NULL, NULL);

		hwndipaddr = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				125, 35, 135, 20, hwnd, (HMENU)0, NULL, NULL);
		hwndconx = CreateWindowW(L"button", L"Connect", WS_VISIBLE | WS_CHILD,
				265, 35, 75, 20, hwnd, (HMENU)1, NULL, NULL);

		hwnddelslot = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				230, 60, 30, 20, hwnd, (HMENU)0, NULL, NULL);
		hwndcheck = CreateWindowW(L"button", L"Check", WS_VISIBLE | WS_CHILD,
				265, 60, 75, 20, hwnd, (HMENU)2, NULL, NULL);

		hwndchnl = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 145, 85, 30, 20, hwnd,
				(HMENU)0, NULL, NULL);
		hwndsens = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 205, 85, 55, 20, hwnd,
				(HMENU)0, NULL, NULL);

		hwndauto = CreateWindowW(L"button", L"Auto", WS_VISIBLE | WS_CHILD,
				265, 85, 75, 20, hwnd, (HMENU)4, NULL, NULL);

		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./.X32Tap.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_SHARED);
		if(hBmp==NULL) {
			perror("Pixel bitmap file no found");
		}
		break;
//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 3, 3, 115, 110, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
//
		SetBkMode(hdc, TRANSPARENT);
		MoveToEx(hdc, 116, 105, NULL);
		LineTo(hdc, 116, 2);
		MoveToEx(hdc, 355, 105, NULL);
		LineTo(hdc, 355, 2);
//
		hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FIXED_PITCH, NULL); //TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 285, 15, str1, wsprintf(str1, "ver. 0.21"));
		TextOut(hdc, 125, 20, str1, wsprintf(str1, "Enter X32 IP below:"));
		TextOut(hdc, 125, 62, str1, wsprintf(str1, "Delay slot:"));
		TextOut(hdc, 185, 87, str1, wsprintf(str1, "Se:"));
		TextOut(hdc, 125, 87, str1, wsprintf(str1, "Ch:"));
		DeleteObject(htmp);
		DeleteObject(hfont);
//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 0, str1, wsprintf(str1, "X32Tap - ©2015 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		EndPaint(hwnd, &ps);
		SetWindowText(hwndipaddr, (LPSTR)Xip_str);
		SetWindowText(hwnddelslot, (LPSTR)Xdel_str);
		SetWindowText(hwndchnl, (LPSTR)XChannel_str);
		SetWindowText(hwndsens, (LPSTR)XChSens_str);
		break;
//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			i = LOWORD(wParam);

			switch(i) {
			case 1:
				i = GetWindowTextLength(hwndipaddr) + 1;
				GetWindowText(hwndipaddr, Xip_str, i);
				if (validateIP4Dotted(Xip_str)) {
					Xconnected = X32Connect(Xconnected, Xip_str, 10000);
					if (Xconnected)	SetWindowTextW(hwndconx, L"Connected");
					else {
						SetWindowTextW(hwndconx, L"Connect");
						SetWindowTextW(hwndcheck, L"Check");
						Xdel_found = 0;
					}
				}
				break;
			case 2:
				SetWindowText(hwndcheck, "Check");
				Xdel_found = 0;
				i = GetWindowTextLength(hwnddelslot) + 1;
				GetWindowText(hwnddelslot, Xdel_str, i);
				sscanf(Xdel_str, "%d", &Xdel_slot);
				if ((Xdel_slot > 0) && (Xdel_slot < 5)) {
					if (Xconnected) {
						// interrogate X32 on what FX is there
						sprintf(str1, "/fx/%1d/type", Xdel_slot);
						s_len = Xsprint(s_buf, 0, 's', str1);
						SEND_TO(s_buf, s_len)	// send command
						RPOLL					// read UDP status
						if (p_status > 0) {
							RECV_FR(r_buf, r_len)
							if ((strcmp(r_buf, str1)) == 0) {
								// FX type number starts at index 16
								for (i = 0 ; i < 4; i++) endian.cc[3 - i] = r_buf[16 + i];
								for (i = 0; i < XDnums; i++) {
									if (endian.ii == XDelays[i]) {
										Xdelay = i;
										SetWindowText(hwndcheck, XDelayNames[i]);
										Xdel_found = 1;
									}
								}
							}
						}
					} else {
						SetWindowText(hwndprog, ("Not connected!"));
					}
				} else {
					SetWindowText(hwndprog, ("DLY: 1..4!"));
				}
				break;
			case 3:
				// SetTap request
				if (Xdel_found) {
					if (Xconnected) {
						XRcvClean();
						XDeltaTime();
						d_tap = (float)(t_dlta.tv_sec * 1000 + t_dlta.tv_usec / 1000);
						sprintf(XTempo, "%d", LaunchXtap(d_tap));
						SetWindowText(hwndprog, (LPSTR)XTempo);
					} else {
						SetWindowText(hwndprog, ("Not connected!"));
					}
				} else {
					SetWindowText(hwndprog, ("No DLY found!"));
				}
				break;
			case 4:
				// Auto/Manual mode
				XAuto ^= 1;
				SetWindowText(hwndauto, ((XAuto)? "Manual": "Auto"));
				if (XAuto) {
					if (Xconnected) {
						if (Xdel_found) {
							i = GetWindowTextLength(hwndchnl) + 1;
							GetWindowText(hwndchnl, XChannel_str, i);
							sscanf(XChannel_str, "%d", &XChannel);
							i = GetWindowTextLength(hwndsens) + 1;
							GetWindowText(hwndsens, XChSens_str, i);
							sscanf(XChSens_str, "%f", &XChSens);
							//
							X32Rmte_bfr = 0;
							Meters[31] = (char)(XChannel - 1);	// set channel number [00..31]
//					fprintf(stderr, "XChannel = %d\n", XChannel);
//					for (i = 0; i < 40; i++) fprintf(stderr, "%02x ", Meters[i]);
//					fprintf(stderr, "\n");
							SetWindowText(hwndprog, ("Ready!"));
						} else{
							SetWindowText(hwndprog, ("No DLY found!"));
						}
					} else {
						SetWindowText(hwndprog, ("Not connected!"));
					}
				}
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		if (Xconnected) close (Xfd);
		// read panel data
		GetWindowText(hwndipaddr, Xip_str, GetWindowTextLength(hwndipaddr) + 1);
		GetWindowText(hwnddelslot, Xdel_str, GetWindowTextLength(hwnddelslot) + 1);
		GetWindowText(hwndchnl, XChSens_str, GetWindowTextLength(hwndchnl) + 1);
		GetWindowText(hwndsens, XChSens_str, GetWindowTextLength(hwndsens) + 1);
		// update resource file
		res_file = fopen(Finipath, "w");
		fprintf(res_file, "%d %d %s %s %s\n", wWidth, wHeight, Xdel_str, XChannel_str, XChSens_str);
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
// Private functions:
//
// Empty any pending message from X32 function
//
void XRcvClean() {
//
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
void XDeltaTime() {
	gettimeofday(&t_2, NULL);			// get precise time
	timersub(&t_2, &t_1, &t_dlta);		// t_dlta = t2 - t1
	t_1 = t_2;
}
//
int LaunchXtap(float f) {
	char		tmpstr[48];
//
	f = f / 3000.0;
	if (f < 0.) f = 0.;
	if (f > 1.) f = 1.;
	if (Xdelay == 0) {
		sprintf(tmpstr, "/fx/%1d/par/02", Xdel_slot);
	} else {
		sprintf(tmpstr, "/fx/%1d/par/01", Xdel_slot);
	}
	s_len = Xfprint(s_buf, 0, tmpstr, 'f', &f);
	SEND_TO(s_buf, s_len)
	return ((int)(f * 3000.0));
}
//
//
void XRunAutoTap() {
float*	ff;

	X32Rmte_now = time(NULL);	// register for X32 data echo
	if (X32Rmte_now > X32Rmte_bfr) {
		SEND_TO(xremote, 12);
		SEND_TO(Meters, 40);
//		int i;
//		for (i = 0; i < 40; i++) fprintf(stderr, "%02x ", Meters[i]);
//		fprintf(stderr, "\n");
		X32Rmte_bfr = X32Rmte_now + 9;
	}
	RPOLL				// X32 sent something?
	if (p_status > 0) {	// yes! (ignore errors and timeouts)
		RECV_FR(r_buf, r_len)
//		fprintf(stderr, "r_len: %i\n", r_len);
		if (strncmp(r_buf, Meters + 16, 9) == 0) {	// test for meter levels command back from X32
			ff = (float *)(r_buf + 28);	// ff points to gate meter
//			fprintf(stderr, "data %7.5f %7.5f %7.5f %7.5f\n", *(float *)(r_buf + 24), *(float *)(r_buf + 28), *(float *)(r_buf + 32), *(float *)(r_buf + 36));
			if (*ff > XChSens) {			// if passed threshold level
//				fprintf(stderr,"TAP!\n");
				XDeltaTime();
				d_tap = (float)(t_dlta.tv_sec * 1000 + t_dlta.tv_usec / 1000);
				if (d_tap > 60.) {		// minimum resolution set by X32 meters
					sprintf(XTempo, "%d", LaunchXtap(d_tap));
					SetWindowText(hwndprog, XTempo);
				}
			}
		} else {
			// Treat other commands of interest (fader levels)
		}
		//	Ignore other data from X32
	}
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
	int			nCmdTap = 0;

	// load resource file
	if ((res_file = fopen("./.X32Tap.ini", "r")) != NULL) {
		// get and remember real path
		if((Finiretval = GetFullPathNameA("./.X32Tap.ini", 1024, Finipath, FinilppPart)) > 1024){
			fprintf (stderr, "Not enough space for file name\n");
		}
		fscanf(res_file, "%d %d %d %d %f\n", &wWidth, &wHeight, &Xdel_slot, &XChannel, &XChSens);
		fgets(Xip_str, sizeof(Xip_str), res_file);
		Xip_str[strlen(Xip_str) - 1] = 0;
		fclose (res_file);
		if (Xdel_slot != 0) {
			sprintf(Xdel_str, "%d", Xdel_slot);
		} else {
			Xdel_str[0] = 0;
		}
		sprintf(XChannel_str, "%d", XChannel);
		sprintf(XChSens_str, "%6.4f", XChSens);

		ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
		wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdTap);
	}
	return 0;
}
