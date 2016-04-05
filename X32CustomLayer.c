//
// X32CustomLayer.c
//
//  Created on: March 1, 2016
//      Author: Patrick-Gilles Maillot
//
//    A program to create X32 Custom Layers
//
// This functionality tries to mimic some of the "custom Layers" functions found on Yamaha
// digital mixers.
// It enables you to reassign on the fly X32 Channels strips (IN and AUX only) to match
// your needs or adapt to a special case where you have to quickly reorganize your X32 layering.
//
//
// Note - Despite ifdefs for Windows, this program will *only* run in
// a Windows environment because of calls to MSC functions. The non-
// Windows sections can be used in a core version of the program that can
// be ported or used with different GUI environments.
//

#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#define  millisleep(a)    Sleep(a)
#else
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#define  millisleep(a)    usleep(a*1000)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>



#define BSIZE     256       // X32 communication buffer size
#define MINCHN    01        // min number of channels (INPUTs and AUXs)
#define MAXSET    40        // max number of channels (INPUTs and AUXs)
#define MAXPRE    99        // max index for libchan presets
#define B_XSIZE   25        // default box x size
#define B_YSIZE   20        // default box y size
#define XBUTTON    7        // number of control buttons, including "connect"
#define YPOS      80;       // top/start y value for box positions
#define TPOS      65;       // top/start y value for box-text positions

int             X32Connect();
int             validateIP4Dotted(const char *s);
int             GetASource(int k);
void            LoadCStrip(int k, int i);
void            LoadNStrip(int k, int source);
int             SaveSet(int max, int set);
void            CustomSet();
void            InsertSet();

extern int      Xsprint(char *bd, int index, char format, void *bs);
extern void     Xfdump(char *header, char *buf, int len, int debug);

WINBASEAPI HWND WINAPI  GetConsoleWindow(VOID);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInstance = 0;
HMENU   hipedit = (HMENU)199;   // Windows handle to text to read
HWND    hwndact[XBUTTON];       // WIndows handles to clickable buttons (button texts below)
LPWSTR  txt_act[XBUTTON] = {L"Connect", L"Clear", L"Set", L"Insert", L"New", L"List", L"Reset"};
int     p_x_act[XBUTTON] = {155, 295, 350, 405, 405, 350, 240};    // Associated X positions
int     p_y_act[XBUTTON] = { 35,  35,  35,  35,  10,  10,  35};    // Associated Y positions
int     s_x_act[XBUTTON] = { 80,  50,  50,  50,  50,  50,  50};    // Associated sizes (widths)

HWND    hwndSst[MAXSET];    // Windows handles to source strips channels
int     p_x_Sst[MAXSET];    // Associated X positions

HFONT   hfont, htmp;
HDC     hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP hBmp;
BITMAP  bmp;

FILE    *res_file;          // file handle to resource file
char    Finipath[1024];     // resolved path to .ini file
char    **FinilppPart;
int     Finiretval;

char    MessStr[512];       // holds the list of channel/sources ("List" button)
char    scscStr[64];        // intermediary buffer for above

int     wWidth = 16 * (B_XSIZE + 5) + 20;    // window width/height and box width/height values
int     wHeight = 6 * (B_YSIZE + 5) + 80;    // will be in fact recalculated from resource file
int     bWidth = B_XSIZE;                    // values, but these 4 variables are used as
int     bHeight = B_YSIZE;                   // default values if the resource file cannot be read
int     yPos = YPOS;        // top/start y value for box positions
int     tPos = TPOS;        // top/start y value for box-text positions
int     Sixty3 = 63;        // mask value for X32 /load and /save command
int     bWidth5 = B_XSIZE + 5;               // bWidth + 5

wchar_t W32_ip_str[20];     // X32 IP address in Windows text format
char    Xip_str[20];        // X32 IP address in C text format
int     Xconnected = 0;     // X32 connection state (0 is not connected)

wchar_t W32_Sst_str[8];     // To get the channel # from boxes (Window string)
char    Sst_str[8];         // To get the channel # from boxes (C string)
int     Sst[MAXSET];        // Where we store the 32 source-strip channels (source channels are fixed 1...32)
int     Asrc[MAXSET];       // To store audio-source of channels before potential update
//
// index values 00 -> 31 are in fact IN channels 01-32
// index values 32 -> 40 are in fact AUX channels 01-08
//
int                 MinChn = MINCHN;    // channel number start
int                 MaxSet = MAXSET;    // channel number stop values for Set/Reset
int                 MaxIns = MAXSET;    // channel number stop values for Insert/New
int                 MaxPre = MAXPRE;    // max value used for libchan presets
int                 Xdebug;
//
int                 Xfd;    // X32 IP socket
struct sockaddr_in  Xip;
char                r_buf[BSIZE], s_buf[BSIZE];    //X32 read and send buffers
int                 r_len, s_len, p_status;        //X32 read and send lengths and read status
char                chn_Asrc[36] = "/ch/00/config/source";
char                aux_Asrc[40] = "/auxin/00/config/source";
char                Zero[8] = "- - -";  // If the user entered invalid data
char                Xport_str[8];       // will hold the X32 port number (i.e. 10023)

struct timeval      timeout;            // timeout value for read actions
fd_set              ufds;               // X32 file descriptor

char *Ch_inistr[]= {"/ch/01/config", "/ch/01/config/name", "/ch/01/delay", "/ch/01/preamp",
"/ch/01/gate", "/ch/01/dyn", "/ch/01/dyn/filter", "/ch/01/insert", "/ch/01/eq/on",
"/ch/01/eq/1", "/ch/01/eq/2", "/ch/01/eq/3", "/ch/01/eq/4", "/ch/01/mix", "/ch/01/mix/01",
"/ch/01/mix/02", "/ch/01/mix/03", "/ch/01/mix/04", "/ch/01/mix/05", "/ch/01/mix/06",
"/ch/01/mix/07", "/ch/01/mix/08", "/ch/01/mix/09", "/ch/01/mix/10", "/ch/01/mix/11",
"/ch/01/mix/12", "/ch/01/mix/13", "/ch/01/mix/14", "/ch/01/mix/15", "/ch/01/mix/16", "/ch/01/grp"};
int Ch_strings_max = sizeof(Ch_inistr) / sizeof(char *);

char *Aux_inistr[]= {"/auxin/01/config", "/auxin/01/config/name", "/auxin/01/preamp",
"/auxin/01/eq/on", "/auxin/01/eq/1", "/auxin/01/eq/2", "/auxin/01/eq/3", "/auxin/01/eq/4",
"/auxin/01/mix", "/auxin/01/mix/01", "/auxin/01/mix/02", "/auxin/01/mix/03", "/auxin/01/mix/04",
"/auxin/01/mix/05", "/auxin/01/mix/06", "/auxin/01/mix/07", "/auxin/01/mix/08", "/auxin/01/mix/09",
"/auxin/01/mix/10", "/auxin/01/mix/11", "/auxin/01/mix/12", "/auxin/01/mix/13", "/auxin/01/mix/14",
"/auxin/01/mix/15", "/auxin/01/mix/16", "/auxin/01/grp"};
int Aux_strings_max = sizeof(Aux_inistr) / sizeof(char *);

char *srcnme[] = {"", "In01", "In02", "In03", "In04", "In05", "In06", "In07", "In08",
                  "In09", "In10", "In11", "In12", "In13", "In14", "In15", "In16",
                  "In17", "In18", "In19", "In20", "In21", "In22", "In23", "In24",
                  "In25", "In26", "In27", "In28", "In29", "In30", "In31", "In32",
                  "Aux1", "Aux2", "Aux3", "Aux4", "Aux5", "Aux6", "USBL", "USBR",
                  "Fx1L", "Fx1R", "Fx2L", "Fx2R", "Fx3L", "Fx3R", "Fx4L", "FX4R",
                  "Bs01", "Bs02", "Bs03", "Bs04", "Bs05", "Bs06", "Bs07", "Bs08",
                  "Bs09", "Bs10", "Bs11", "Bs12", "Bs13", "Bs14", "Bs15", "Bs16"
};

char Ch_strings[32][32];
char Aux_strings[32][32];
//
// variables used for default values (New or Reset)
int i0 = 0;
int i1 = 1;
int i2 = 2;
int i3 = 3;
int i4 = 4;
int i5 = 5;
int i6 = 6;
int i55 = 55;      // icon = jack
int i60 = 60;      // icon = tape
float f02 = 0.2;
float f05 = 0.5;
float f09 = 0.9;
float f10 = 1.0;
float f08 = 0.0083;
float f83 = 0.0833;
float f88 = 0.88;
float f79 = 0.79;
float f51 = 0.51;
float f54 = 0.54;
float f56 = 0.565;
float f26 = 0.265;
float f46 = 0.4648;
float f45 = 0.465;
float f66 = 0.665;
char *USB[] = {"USB L", "USB R"};

#ifdef __WIN32__
WSADATA             wsa;
int                 Xip_len = sizeof(Xip);    // length of addresses
#else
socklen_t           Xip_len = sizeof(Xip);    // length of addresses
#endif
//
// Union for endian conversion
union cast {
    float           ff;
    int             ii;
    char            cc[4];
} endian;
//
#define SEND_TO(b, l)                                                     \
    do {                                                                  \
        if (Xdebug){Xfdump("->X", s_buf, s_len, 0); fflush(stdout);}      \
        if (sendto(Xfd, b, l, 0, (struct sockaddr *)&Xip, Xip_len) < 0) { \
            perror("Coundn't send data to X32\n");                        \
            exit(EXIT_FAILURE);                                           \
        }                                                                 \
    } while (0);
//
#define RECV_FR(b, l)                                                  \
    do {                                                               \
        l = recvfrom(Xfd, b, BSIZE, 0, 0, 0);                          \
        if (Xdebug) {Xfdump("X->", r_buf, r_len, 0); fflush(stdout);}  \
    } while (0);
//
#define RPOLL                                                \
    do {                                                     \
        FD_ZERO (&ufds);                                     \
        FD_SET (Xfd, &ufds);                                 \
        p_status = select(Xfd+1,&ufds,NULL,NULL,&timeout);   \
    } while (0);
//
//
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
MSG    msg;
//
    WNDCLASSW wc = { 0 };
    wc.lpszClassName = L"X32CustomLayer";
    wc.hInstance = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"X32CustomLayer - Create X32 Custom Layers",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            50, 50, wWidth + 1, wHeight + 1, 0, 0, hInstance, 0);

    while (GetMessage(&msg, NULL, 0, 0)) {    // Let Windows manage the main loop
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
static  HWND hwndipaddr;
int     i, j, nb_assigns, len, set;
char    str1[8];

    switch (msg) {
    case WM_CREATE:
        // create main window
        hwndipaddr = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
                5, 35, 140, 20, hwnd, hipedit, NULL, NULL);
        // Set action buttons (Connect,...Reset)
        for (i = 0; i < XBUTTON; i++) {
            hwndact[i] = CreateWindowW(L"button", txt_act[i], WS_VISIBLE | WS_CHILD,
                    p_x_act[i], p_y_act[i], s_x_act[i], 20, hwnd, (HMENU)(200 + i), NULL, NULL);
        }
        // create 40 editable boxes in two rows of 16 + one row of 8
        yPos = YPOS;
        for (i = 0; i < 16; i++) {
            hwndSst[i] = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
                    5 + i * bWidth5, yPos, bWidth, bHeight, hwnd, hipedit, NULL, NULL);
        }
        yPos += (2 * bHeight + 5);
        for (i = 0; i < 16; i++) {
            hwndSst[i + 16] = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
                    5 + i * bWidth5, yPos, bWidth, bHeight, hwnd, hipedit, NULL, NULL);
        }
        yPos += (2 * bHeight + 5);
        for (i = 0; i < 8; i++) {
            hwndSst[i + 32] = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
                    5 + i * bWidth5, yPos, bWidth, bHeight, hwnd, hipedit, NULL, NULL);
        }
        break;
//
    case WM_PAINT:
// WIndows repaint request
        hdc = BeginPaint(hwnd, &ps);
        MoveToEx(hdc, 5, 60, NULL);
        LineTo(hdc, wWidth - 20, 60);
//
        SetBkMode(hdc, TRANSPARENT);
//
        hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
        htmp = (HFONT) SelectObject(hdc, hfont);
        TextOut(hdc, 15, 2, "X32CustomLayer - V1.03 - ©2016 - Patrick-Gilles Maillot", 55);
        DeleteObject(htmp);
        DeleteObject(hfont);
//
        hfont = CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
        htmp = (HFONT) SelectObject(hdc, hfont);
        TextOut(hdc, 15, 20, "Enter X32 IP below:", 18);
        DeleteObject(htmp);
        DeleteObject(hfont);
//
        hfont = CreateFont(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
        htmp = (HFONT) SelectObject(hdc, hfont);
        SetBkMode(hdc, TRANSPARENT);
        // write /display the channel numbers on top of the boxes
        j = bWidth / 2 - 2;
        tPos = TPOS;
        for (i = 0; i < 16; i++) {
            TextOut(hdc, j + i * bWidth5, tPos, str1,
                    wsprintf(str1, "%02d", i + 1));
        }
        tPos += (2 * bHeight + 5);
        for (i = 0; i < 16; i++) {
            TextOut(hdc, j + i * bWidth5, tPos, str1,
                    wsprintf(str1, "%02d", i + 17));
        }
        tPos += (2 * bHeight + 5);
        for (i = 0; i < 8; i++) {
            TextOut(hdc, j + i * bWidth5, tPos, str1,
                    wsprintf(str1, "%02d", i + 33));
        }
        tPos += 15;
        TextOut(hdc, 9 * bWidth5, tPos, "values 33-40 are AUX 01-08", 26);
        //
        DeleteObject(htmp);
        DeleteObject(hfont);
        //
        // display the X32 IP address (user changeable)
        SetWindowText(hwndipaddr, (LPSTR)Xip_str);
        // initialize the boxes to empty (user changeable)
        for  (i = MinChn - 1; i < MaxSet; i++ ) {
            SetWindowTextW(hwndSst[i], L"");
        }
        EndPaint(hwnd, &ps);
        break;
//
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {    // user action
            switch(LOWORD(wParam)) {
            case 200:         // Connect button hit
                if (Xdebug) {printf("Connect button hit\n"); fflush(stdout);}
                len = GetWindowTextLengthW(hwndipaddr) + 1;
                GetWindowTextW(hwndipaddr, W32_ip_str, len);
                WideCharToMultiByte(CP_ACP, 0, W32_ip_str, len, Xip_str, len, NULL, NULL);
                if (Xdebug) {printf("IP = %s\n", Xip_str); fflush(stdout);}
                if (validateIP4Dotted(Xip_str)) {
                    Xconnected = X32Connect();
                    if (Xconnected) SetWindowTextW(hwndact[0], L"Connected");
                    else            SetWindowTextW(hwndact[0], L"Connect");
                } else {
                    perror("incorrect IP string form");
                }
                break;
            case 201:        // Clear button hit
                if (Xdebug) {printf("Clear button hit\n"); fflush(stdout);}
                for  (i = 0; i < MAXSET; i++ ) {
                    SetWindowTextW(hwndSst[i], L"");
                }
                break;
            case 202:        // Set button hit
                if (Xdebug) {printf("Set button hit\n"); fflush(stdout);}
                if (Xconnected) {
                    len = 0;
                    for (i = 0; i < MinChn - 1; i++) {
                        len = len + GetWindowTextLengthW(hwndSst[i]);
                    }
                    for (i = MaxSet; i < MAXSET; i++) {
                        len = len + GetWindowTextLengthW(hwndSst[i]);
                    }
                    if (len == 0) {
                        for (i = 0; i < MAXSET; i++ ) {
                            Asrc[i] = -1;
                            Sst[i] = 0;
                        }
                        for (i = MinChn - 1; i < MaxSet; i++) {
                            len = GetWindowTextLengthW(hwndSst[i]) + 1;
                            GetWindowTextW(hwndSst[i], W32_Sst_str, len);
                            WideCharToMultiByte(CP_ACP, 0, W32_Sst_str, len, Sst_str, len, NULL, NULL);
                            if (Sst_str[0] != 0) {
                                sscanf(Sst_str, "%d", &Sst[i]);
                                if (Sst[i] < MinChn || Sst[i] > MaxSet) {   // manage wrong user data
                                    Sst[i] = 0;                // force ch number to 0 and erase data
                                    SetWindowText(hwndSst[i], (LPSTR)Zero);
                                }
                            }
                        }
                        i = SaveSet(MaxSet, 1);
                        CustomSet();        // Manage X32 update
                    } else {
                        for  (i = 0; i < MAXSET; i++ ) {
                            SetWindowText(hwndSst[i], (LPSTR)Zero);
                        }
                    }
                }
                break;
            case 203:        // Insert button hit
            case 204:        // New button hit
                set = LOWORD(wParam) == 203;
                if (Xdebug) {printf("Insert button hit\n"); fflush(stdout);}
                if (Xconnected) {
                    nb_assigns = 0;
                    len = 0;
                    for (i = 0; i < MinChn - 1; i++) {
                        len = len + GetWindowTextLengthW(hwndSst[i]);
                    }
                    for (i = MaxIns; i < MAXSET; i++) {
                        len = len + GetWindowTextLengthW(hwndSst[i]);
                    }
                    if (len == 0) {
                        for (i = 0; i < MAXSET; i++ ) {
                            Asrc[i] = -1;
                            Sst[i] = 0;
                        }
                        for (i = MinChn - 1; i < MaxIns; i++) {
                            len = GetWindowTextLengthW(hwndSst[i]) + 1;
                            GetWindowTextW(hwndSst[i], W32_Sst_str, len);
                            WideCharToMultiByte(CP_ACP, 0, W32_Sst_str, len, Sst_str, len, NULL, NULL);
                            if (Sst_str[0] != 0) {
                                sscanf(Sst_str, "%d", &Sst[i]);
                                if (set) {
                                    if (Sst[i] < MinChn || Sst[i] > MaxIns) { // manage wrong user data
                                        Sst[i] = 0;              // force ch number to 0 and erase data
                                        SetWindowText(hwndSst[i], (LPSTR)Zero);
                                    }
                                } else {
                                    if (Sst[i] < 0 || Sst[i] > 64) {          // manage wrong user data
                                        Sst[i] = -1;             // force ch number to 0 and erase data
                                        SetWindowText(hwndSst[i], (LPSTR)Zero);
                                    }
                                }
                            }
                        }
                        // Manage X32 update with Insert an existing or New (blank) strip
                        if ((nb_assigns = SaveSet(MaxIns, set)) == 1) {
                            InsertSet(set);
                        } else {
                            for  (i = 0; i < MAXSET; i++ ) {
                                SetWindowText(hwndSst[i], (LPSTR)Zero);
                            }
                        }
                    } else {
                        for  (i = 0; i < MAXSET; i++ ) {
                            SetWindowText(hwndSst[i], (LPSTR)Zero);
                        }
                    }
                }
                break;
            case 205:        // List button hit
                if (Xdebug) {printf("List button hit\n"); fflush(stdout);}
                if (Xconnected) {
                    *MessStr = 0;
                    strcat(MessStr, "  Channel\tSource\tChannel\tSource\n");
                    // Fill out channel and input source data
                    for (i = 0; i < 16; i++) {
                        sprintf(scscStr, "     %02d\t  %s\t  %02d\t %s\n",
                                i + 1, srcnme[GetASource(i + 1)], i + 17, srcnme[GetASource(i + 17)]);
                        strcat(MessStr, scscStr);
                    }
                    strcat(MessStr, "\n  AuxIN\tSource\tAuxIN\tSource\n");
                    for (i = 0; i < 4; i++) {
                        sprintf(scscStr, "     %02d\t  %s\t  %02d\t %s\n",
                                i + 1, srcnme[GetASource(i + 33)], i + 5, srcnme[GetASource(i + 37)]);
                        strcat(MessStr, scscStr);
                    }
                    MessageBox(NULL, (LPCSTR)MessStr,
                        (LPCSTR)"X32CustomLayer: Channels & Sources list",
                        MB_OK
                    );
                }
                break;
            case 206:        // Reset button hit
                if (Xdebug) {printf("Reset button hit\n"); fflush(stdout);}
                if (Xconnected) {
                    nb_assigns = 0;
                    len = 0;
                    for (i = 0; i < MinChn - 1; i++) {
                        len = len + GetWindowTextLengthW(hwndSst[i]);
                    }
                    for (i = MaxIns; i < MAXSET; i++) {
                        len = len + GetWindowTextLengthW(hwndSst[i]);
                    }
                    if (len == 0) {
                        for (i = 0; i < MAXSET; i++ ) {
                            Sst[i] = 0;
                        }
                        for (i = MinChn - 1; i < MaxIns; i++) {
                            len = GetWindowTextLengthW(hwndSst[i]) + 1;
                            GetWindowTextW(hwndSst[i], W32_Sst_str, len);
                            WideCharToMultiByte(CP_ACP, 0, W32_Sst_str, len, Sst_str, len, NULL, NULL);
                            if (Sst_str[0] == 'r' || Sst_str[0] == 'R') {
                                Sst[i] = 1;
                                nb_assigns += 1;
                            }
                        }
                        if (nb_assigns) {    // Selective reset
                            if (MessageBox(NULL,
                                (LPCSTR)"Are you sure?\nThis will reset selected channels\nto their default values",
                                (LPCSTR)"X32CustomLayer: Selective Reset",
                                MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
                                //
                                // reset selected channels (user said so)
                                if (Xdebug) {printf("Applying selective Reset!\n"); fflush(stdout);}
                                for (i = MinChn - 1; i < MaxSet; i++) {
                                    if (Sst[i]) LoadNStrip(i, i + 1);
                                }
                            }
                        } else {             // Full reset
                            if (MessageBox(NULL,
                                (LPCSTR)"Are you sure?\nThis will reset ALL active channels\nto their default values",
                                (LPCSTR)"X32CustomLayer: Full Reset",
                                MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
                                //
                                // reset all channels (user said so)
                                if (Xdebug) {printf("Applying full Reset!\n"); fflush(stdout);}
                                for (i = MinChn - 1; i < MaxSet; i++) {
                                    LoadNStrip(i, i + 1);
                                }
                            }
                        }
                    } else {
                        for  (i = 0; i < MAXSET; i++ ) {
                            SetWindowText(hwndSst[i], (LPSTR)Zero);
                        }
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
        i = GetWindowTextLengthW(hwndipaddr) + 1;
        GetWindowTextW(hwndipaddr, W32_ip_str, i);
        WideCharToMultiByte(CP_ACP, 0, W32_ip_str, i, Xip_str, i, NULL, NULL);
        // update resource data file
        res_file = fopen(Finipath, "wb");
        fputs("#\r\n# X32CustomLayer.ini - configuration file - ©2016 Patrick-Gilles Maillot\r\n", res_file);
        fputs("#\r\n# Line 1:\r\n", res_file);
        fputs("# The 2 numbers are respectively representing the width and height of input \r\n", res_file);
        fputs("# boxes used in the application\r\n#\r\n# Line 2:\r\n", res_file);
        fputs("# The data are numbers in the range [01..40] for the channel number start\r\n", res_file);
        fputs("# followed by the channel number stop values for Set/Reset and Insert/New functions\r\n", res_file);
        fputs("# The last number of the line, in the range [0..99] is the max index value used for \r\n", res_file);
        fputs("# X32 libchan presets\r\n#\r\n# Line 3:\r\n# Enter the IP address of your X32\r\n#\r\n# Line 4:\r\n", res_file);
        fputs("# The digit {0, 1} is a debug flag. Leave to 0, unless you need or are asked\r\n", res_file);
        fputs("# to debug the application. The last number represents 6 bits selecting what\r\n", res_file);
        fputs("# part of the Custom Strip is actually updated with Set or Insert [0..63].\r\n#\r\n", res_file);
        fprintf(res_file, "%02d %02d\r\n", bWidth, bHeight);
        fprintf(res_file, "%02d %02d %02d %02d\r\n", MinChn, MaxSet, MaxIns, MaxPre);
        fprintf(res_file, "%s\r\n", Xip_str);
        fprintf(res_file, "%1d %02d\r\n", Xdebug, Sixty3);
        fclose (res_file);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
// Connecting to X32 (or disconnecting)
// returns state: (1) if connected, or (0) if disconnected
//
// create a UDP socket, non-blocking mode
//
// This is done by sending a /info command to X32, and waiting for the X32 to
// reply with known data.
//
int X32Connect() {
//
    if (Xconnected) {
        close (Xfd);
        return 0;
    } else {
        //
        // initialize communication with X32 server at IP ip and PORT port
        //    Xport_str[] = "10023" - change to use a different port
        strcpy (Xport_str, "10023");
#ifdef __WIN32__
        //Initialize winsock
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
            perror("WSA Startup Error");
            exit(EXIT_FAILURE);
        }
#endif
        // Load the X32 address we connect to; we're a client to X32, keep it simple.
        // Create the UDP socket
        if ((Xfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
            perror("X32 socket creation error");
            return 0; // Make sure we don't considered being connected
        } else {
            // Construct the server sockaddr_in structure
            memset(&Xip, 0, sizeof(Xip));                // Clear struct
            Xip.sin_family = AF_INET;                    // Internet/IP
            Xip.sin_addr.s_addr = inet_addr(Xip_str);    // IP address
            Xip.sin_port = htons(atoi(Xport_str));       // server port
            // Non blocking mode
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000; //Set timeout for non blocking recvfrom(): 100ms
            //
            s_len = Xsprint(s_buf, 0, 's', "/info");
            if (sendto(Xfd, s_buf, s_len, 0, (struct sockaddr *)&Xip, Xip_len) < 0) {
                perror("Error sending data");
                return 0;     // Make sure we don't considered being connected
            } else {
                RPOLL;        // poll for data in
                if (p_status < 0) {
                    perror("Polling for data failed");
                    return 0; // Make sure we don't considered being connected
                } else if (p_status > 0) {
                // We have received data - process it!
                    r_len = recvfrom(Xfd, r_buf, BSIZE, 0, 0, 0); //(struct sockaddr *)&Xip, &Xip_len);
                    if (Xdebug) {Xfdump("X->", r_buf, r_len, 0); fflush(stdout);}
                    if (strcmp(r_buf, "/info") != 0) {
                        perror ("Unexpected answer from X32");
                        return 0;
                    }
                } else {
                // time out... Not connected or Not an X32
                    perror("X32 reception timeout");
                    return 0; // Make sure we don't considered being connected
                }
            }
        }
    }
    return 1; // We are connected !
}
//
//
// Small utility function to validate the construction of
// and IP address in the form of a quad, dot-separated, max-3digits strings
//
// Return (1) if OK, (0) is not OK
//
int validateIP4Dotted(const char *s)
{
int i, len;
char tail[16];
unsigned int d[4];

    len = strlen(s);
    if (len < 7 || len > 15) return 0;
    tail[0] = 0;
    int c = sscanf(s, "%3u.%3u.%3u.%3u%s", &d[0], &d[1], &d[2], &d[3], tail);
    if (c != 4 || tail[0]) return 0;
    for (i = 0; i < 4; i++)
        if (d[i] > 255)
            return 0;
    return 1;
}
//
//
// Get the audio-source input number associated with channel k
//
int GetASource(int k) {
//
    int j, Xlock;
    //
    // INput or AUXin channel?
    if (k < 33) {
        // send /ch/xx/config/source to get the information
        j = k / 10;
        chn_Asrc[4] = (char) (j + 48);
        chn_Asrc[5] = (char) (k - j * 10 + 48);
        s_len = Xsprint(s_buf, 0, 's', chn_Asrc);
        j = 6;
    } else {
        // send /auxin/xx/config/source to get the information
        j = k - 32;
        aux_Asrc[8] = (char) (j + 48);
        s_len = Xsprint(s_buf, 0, 's', aux_Asrc);
        j = 9;
    }
    SEND_TO(s_buf, s_len)
    // read data back from X32 and save it for later use
    Xlock = 1;
    while (Xlock) {
        RPOLL; // read the desk for answer to /save
        if (p_status > 0)     {        // ignore responses that do not correspond to what is
            RECV_FR(r_buf, r_len);    // expected: /auxin/xx/config/source~,i~~[SRC]
            if (strncmp(r_buf + j, "/config/source", 14) == 0) {
                for (j = 0; j < 4; j++) endian.cc[3 - j] = r_buf[28 + j];
                Xlock = 0;
            } else {
                perror ("Waiting /xxxxx/xx/config/source... \n");
            }
        }
    }
    return endian.ii; // return audio-source data
}
//
//
// Set audio-source data n into channel index k
//
void SetASource(int k, int n) {
    //
    int j;
    //
    if (k < 32) {
        // send /ch/xx/config/source ,i xx to set the information
        j = (k + 1) / 10;
        chn_Asrc[4] = (char) (j + 48);
        chn_Asrc[5] = (char) (k + 1 - j * 10 + 48);
        s_len = Xsprint(s_buf, 0, 's', chn_Asrc);
    } else {
        // send /auxin/xx/config/source ,i xx to set the information
        j = k + 1 - 32;
        aux_Asrc[8] = (char) (j + 48);
        s_len = Xsprint(s_buf, 0, 's', aux_Asrc);
    }
    s_len = Xsprint(s_buf, s_len, 's', ",i");
    endian.ii = n;
    for (j = 0; j < 4; j++) s_buf[s_len + j] = endian.cc[3 - j];
    s_len += 4;
    SEND_TO(s_buf, s_len)
    return;
}
//
//
// Load into X32 at channel strip index i the channel strip #k (saved at
// position MaxPre + 1 -k in presets)
//
void LoadCStrip(int k, int i) {
//
    int j;
    int Xlock;
//
    s_len = Xsprint(s_buf, 0, 's', "/load");
    s_len = Xsprint(s_buf, s_len, 's', ",siii");
    s_len = Xsprint(s_buf, s_len, 's', "libchan");
    j = MaxPre + 1 - k;
    s_len = Xsprint(s_buf, s_len, 'i', &j);
    j = i;
    s_len = Xsprint(s_buf, s_len, 'i', &j);
    s_len = Xsprint(s_buf, s_len, 'i', &Sixty3);
    SEND_TO(s_buf, s_len)
    Xlock = 1;
    while (Xlock) {
        RPOLL; // read the desk for answer to /load
        if (p_status > 0)     {       // ignore responses that do not correspond to what
            RECV_FR(r_buf, r_len);    // is expected: /load~~~,si~libchan~[     1]
            if (strncmp(r_buf, "/load", 5) == 0) {
                for (j = 0; j < 4; j++) endian.cc[3 - j] = r_buf[20 + j];
                if (endian.ii != 1) {
                    perror ("Error: Could not load channel data\n");
                    return;
                }
                Xlock = 0;
            } else {
                perror ("Waiting /load... \n");
            }
        }
    }
    return;
}
//
//
// Set a blank strip at index 'k' with audio source 'source'
//
void LoadNStrip(int k, int source) {
//
    int i;
//
    // set value vs index for k
    k += 1;
    //
    if (k < 33) { // standard input channels
        for (i = 0; i < Ch_strings_max; i++) {
            // replace index with value of k as a channel value
            Ch_strings[i][4] = (char)(48 + (k / 10));
            Ch_strings[i][5] = (char)(48 + k - (k / 10) * 10);
        }
        //    /ch/01/config ,iiii 0 1 3 <source>
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[0]);
        s_len = Xsprint(s_buf, s_len, 's', ",iiii");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i3);
        s_len = Xsprint(s_buf, s_len, 'i', &source);
        SEND_TO(s_buf, s_len)
        //    /ch/01/config/name ,s ""
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[1]);
        s_len = Xsprint(s_buf, s_len, 's', ",s");
        s_len = Xsprint(s_buf, s_len, 's', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/delay ,if 0 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[2]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/preamp ,fiiif 0.5 0 0 2 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[3]);
        s_len = Xsprint(s_buf, s_len, 's', ",fiiif");
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        s_len = Xsprint(s_buf, s_len, 'f', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/gate ,iifffffiiif 0 3 0.0 1.0 0.0083 0.88 0.79 0 0 6 0.565
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[4]);
        s_len = Xsprint(s_buf, s_len, 's', ",iifffffiiif");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i3);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &f10);
        s_len = Xsprint(s_buf, s_len, 'f', &f08);
        s_len = Xsprint(s_buf, s_len, 'f', &f88);
        s_len = Xsprint(s_buf, s_len, 'f', &f79);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i6);
        s_len = Xsprint(s_buf, s_len, 'i', &f56);
        SEND_TO(s_buf, s_len)
        //    /ch/01/dyn ,iiiififffffiifi 0 0 0 1 1.0 5 0.2 0.0 0.0833 0.54 0.51 1 0 1.0 0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[5]);
        s_len = Xsprint(s_buf, s_len, 's', ",iiiififffffiifi");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'f', &f10);
        s_len = Xsprint(s_buf, s_len, 'i', &i5);
        s_len = Xsprint(s_buf, s_len, 'f', &f02);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f83);
        s_len = Xsprint(s_buf, s_len, 'f', &f54);
        s_len = Xsprint(s_buf, s_len, 'f', &f51);
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f10);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/dyn/filter ,iif 0 6 0.565
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[6]);
        s_len = Xsprint(s_buf, s_len, 's', ",iif");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i6);
        s_len = Xsprint(s_buf, s_len, 'f', &f56);
        SEND_TO(s_buf, s_len)
        //    /ch/01/insert ,iii 0 1 9
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[7]);
        s_len = Xsprint(s_buf, s_len, 's', ",iii");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'f', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/eq/on ,i 0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[8]);
        s_len = Xsprint(s_buf, s_len, 's', ",i");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/eq/1 ,ifff 2 0.265 0.5 0.4648
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[9]);
        s_len = Xsprint(s_buf, s_len, 's', ",ifff");
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        s_len = Xsprint(s_buf, s_len, 'f', &f26);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'f', &f46);
        SEND_TO(s_buf, s_len)
        //    /ch/01/eq/2 ,ifff 2 0.465 0.5 0.4648
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[10]);
        s_len = Xsprint(s_buf, s_len, 's', ",ifff");
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        s_len = Xsprint(s_buf, s_len, 'f', &f45);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'f', &f46);
        SEND_TO(s_buf, s_len)
        //    /ch/01/eq/3 ,ifff 2 0.665 0.5 0.4648
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[11]);
        s_len = Xsprint(s_buf, s_len, 's', ",ifff");
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        s_len = Xsprint(s_buf, s_len, 'f', &f66);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'f', &f46);
        SEND_TO(s_buf, s_len)
        //    /ch/01/eq/4 ,ifff 4 0.9 0.5 0.4648
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[12]);
        s_len = Xsprint(s_buf, s_len, 's', ",ifff");
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        s_len = Xsprint(s_buf, s_len, 'f', &f09);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'f', &f46);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix ,ififif 1 0.0 1 0.5 0 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[13]);
        s_len = Xsprint(s_buf, s_len, 's', ",ififif");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/01 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[14]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/02 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[15]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/03 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[16]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/04 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[17]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/05 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[18]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/06 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[19]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/07 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[20]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/08 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[21]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/09 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[22]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/10 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[23]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/11 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[24]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/12 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[25]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/13 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[26]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/14 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[27]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/15 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[28]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        SEND_TO(s_buf, s_len)
        //    /ch/01/mix/16 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[29]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /ch/01/grp ,ii 0 0
        s_len = Xsprint(s_buf,     0, 's', Ch_strings[30]);
        s_len = Xsprint(s_buf, s_len, 's', ",ii");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
    } else {
        k -=32;
        for (i = 0; i < Aux_strings_max; i++) {
            // replace index with value of k as a channel value in /auxin/01
            Aux_strings[i][7] = (char)(48 + (k / 10));
            Aux_strings[i][8] = (char)(48 + k - (k / 10) * 10);
        }
        //    /auxin/01/config ,iiii 0 1 3 <source>
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[0]);
        s_len = Xsprint(s_buf, s_len, 's', ",iiii");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        if (k < 7) {
            s_len = Xsprint(s_buf, s_len, 'i', &i55);
            s_len = Xsprint(s_buf, s_len, 'i', &i2);
            s_len = Xsprint(s_buf, s_len, 'i', &source);
            SEND_TO(s_buf, s_len)
            //    /auxin/01/config/name ,s ""
            s_len = Xsprint(s_buf,     0, 's', Aux_strings[1]);
            s_len = Xsprint(s_buf, s_len, 's', ",s");
            s_len = Xsprint(s_buf, s_len, 's', &i0);
            SEND_TO(s_buf, s_len)
        } else {
            s_len = Xsprint(s_buf, s_len, 'i', &i60);
            s_len = Xsprint(s_buf, s_len, 'i', &i3);
            s_len = Xsprint(s_buf, s_len, 'i', &source);
            SEND_TO(s_buf, s_len)
            //    /auxin/01/config/name ,s ""
            s_len = Xsprint(s_buf,     0, 's', Aux_strings[1]);
            s_len = Xsprint(s_buf, s_len, 's', ",s");
            s_len = Xsprint(s_buf, s_len, 's', USB[k-7]);
            SEND_TO(s_buf, s_len)
        }
        //    /auxin/01/preamp ,fi 0.5 0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[2]);
        s_len = Xsprint(s_buf, s_len, 's', ",fi");
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/eq/on ,i 0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[3]);
        s_len = Xsprint(s_buf, s_len, 's', ",i");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/eq/1 ,ifff 2 0.265 0.5 0.4648
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[4]);
        s_len = Xsprint(s_buf, s_len, 's', ",ifff");
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        s_len = Xsprint(s_buf, s_len, 'f', &f26);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'f', &f46);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/eq/2 ,ifff 2 0.465 0.5 0.4648
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[5]);
        s_len = Xsprint(s_buf, s_len, 's', ",ifff");
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        s_len = Xsprint(s_buf, s_len, 'f', &f45);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'f', &f46);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/eq/3 ,ifff 2 0.665 0.5 0.4648
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[6]);
        s_len = Xsprint(s_buf, s_len, 's', ",ifff");
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        s_len = Xsprint(s_buf, s_len, 'f', &f66);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'f', &f46);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/eq/4 ,ifff 4 0.9 0.5 0.4648
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[7]);
        s_len = Xsprint(s_buf, s_len, 's', ",ifff");
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        s_len = Xsprint(s_buf, s_len, 'f', &f09);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'f', &f46);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix ,ififif 1 0.0 1 0.5 0 0.0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[8]);
        s_len = Xsprint(s_buf, s_len, 's', ",ififif");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/01 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[9]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/02 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[10]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/03 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[11]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/04 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[12]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/05 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[13]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/06 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[14]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/07 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[15]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i2);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/08 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[16]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/09 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[17]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/10 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[18]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/11 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[19]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/12 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[20]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/13 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[21]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/14 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[22]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/15 ,iffi 1 0.0 0.5 2
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[23]);
        s_len = Xsprint(s_buf, s_len, 's', ",iffi");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'f', &f05);
        s_len = Xsprint(s_buf, s_len, 'i', &i4);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/mix/16 ,if 1 0.0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[24]);
        s_len = Xsprint(s_buf, s_len, 's', ",if");
        s_len = Xsprint(s_buf, s_len, 'i', &i1);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
        //    /auxin/01/grp ,ii 0 0
        s_len = Xsprint(s_buf,     0, 's', Aux_strings[25]);
        s_len = Xsprint(s_buf, s_len, 's', ",ii");
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        s_len = Xsprint(s_buf, s_len, 'i', &i0);
        SEND_TO(s_buf, s_len)
    }
    return;
}
//
//
// Shift up channel strip at index i - 1 to channel strip at index i
// This acts on actual channel data, using libchan presets as a temp buffer
//
void ShiftCStrip(int i) {
    //
    int    j, Xlock;
    char   channel[14] = "CustLayer";
    //
    // shift is done by saving i-1 to, then loading from channel preset into i
    // sending a /save ,sisi "libchan" [MaxPre + 1 - i] "ch<i+1>" [i]
    s_len = Xsprint(s_buf, 0, 's', "/save");
    s_len = Xsprint(s_buf, s_len, 's', ",sisi");
    s_len = Xsprint(s_buf, s_len, 's', "libchan");
    j = MaxPre + 1 - i;
    s_len = Xsprint(s_buf, s_len, 'i', &j);
    j = i / 10;
    channel[ 9] = 48 + j;
    channel[10] = 48 + i  - j * 10;
    channel[11] = 0;
    s_len = Xsprint(s_buf, s_len, 's', channel);
    j = i - 1;
    s_len = Xsprint(s_buf, s_len, 'i', &j);
    SEND_TO(s_buf, s_len)
    Xlock = 1;
    while (Xlock) {
        RPOLL; // read the desk for answer to /save
        if (p_status > 0)     {       // ignore responses that do not correspond to what
            RECV_FR(r_buf, r_len);    // is expected: /save~~~,si~libchan~[     1]
            if (strncmp(r_buf, "/save", 5) == 0) {
                for (j = 0; j < 4; j++) endian.cc[3 - j] = r_buf[20 + j];
                if (endian.ii != 1) {
                    perror ("Error: Could not save channel data\n");
                    return;
                }
                Xlock = 0;
            } else {
                perror ("Waiting /save... \n");
            }
        }
    }
    // Get the audio-source of the saved channel strip
    Asrc[i - 1] = GetASource(i);
    //
    // load the data we just saved into the next channel strip (i)
    LoadCStrip(i, i);
    // Set audio-source we just saved to new channel strip
    SetASource(i, Asrc[i - 1]);
    return;
}
//
//
// This function manages saving channel strips that will be modified by
// being assigned new channel strips. This is optimized (limiting OSC I/O):
// - save only those channels that will be modified
// - do not save twice the same channel
//
// returns the number of saved channels strips
//
int SaveSet(int max, int set) {
    int     i, j, k, nb_assigns;
    int     Xlock;
    char    channel[8];

    nb_assigns = 0;
    for (i = MinChn - 1; i < max; i++) {
        // Source := i + 1, will be saved if used in specific conditions
        // Sst[i] is set from  user settings
        // we first preserve source data in case it is needed later
        // could be systematically done but we try to avoid some unnecessary
        // X32 UDP communication and save time
        // if the source-strip channel is blank (or 0), it means no change to channel
        k = Sst[i];
        if ((k > 0) && (k != i+1)) { // save only if there a  need (i.e the channel strip may be modified)
            if (set) {
                if (Asrc[k - 1] < 0) { // avoid saving the same channel strip several times
                    // save is done by saving to libchan preset MaxPre - (Sst[i] - 1)
                    // sending a /save ,sisi "libchan" [MaxPre + 1 - k] "ch<Sst[i]>" k - 1
                    s_len = Xsprint(s_buf, 0, 's', "/save");
                    s_len = Xsprint(s_buf, s_len, 's', ",sisi");
                    s_len = Xsprint(s_buf, s_len, 's', "libchan");
                    j = MaxPre + 1 - k;
                    s_len = Xsprint(s_buf, s_len, 'i', &j);
                    channel[0] = 'c'; channel[1] = 'h';
                    j = k / 10;
                    channel[2] = 48 + j;
                    channel[3] = 48 + k  - j * 10;
                    channel[4] = 0;
                    s_len = Xsprint(s_buf, s_len, 's', channel);
                    j = k - 1;
                    s_len = Xsprint(s_buf, s_len, 'i', &j);
                    SEND_TO(s_buf, s_len)
                    Xlock = 1;
                    while (Xlock) {
                        RPOLL; // read the desk for answer to /save
                        if (p_status > 0)     {       // ignore responses that do not correspond to what
                            RECV_FR(r_buf, r_len);    // is expected: /save~~~,si~libchan~[     1]
                            if (strncmp(r_buf, "/save", 5) == 0) {
                                for (j = 0; j < 4; j++) endian.cc[3 - j] = r_buf[20 + j];
                                if (endian.ii != 1) {
                                    perror ("Error: Could not save channel data\n");
                                    return 0;
                                }
                                Xlock = 0;
                            } else {
                                perror ("Waiting /save... n");
                            }
                        }
                    }
                    // set Asrc[k - 1] to the audio-source used for channel at index (k - 1)
                    Asrc[k - 1] = GetASource(k);
                }
            }
            nb_assigns++; // Up the number of assignments (including duplicates!)
        }
    }
    return nb_assigns;
}
//
//
// Building/assigning a custom set layer to X32
// This is done by using the data saved at a previous stage (with SaveSet(xxx))
// This time, we apply to each channel strip the setting that was saved for it to
// X32 libchan presets.
// After loading the proper preset to X32, the channel strip audio-source is set
// to a value according to Asrc[k - 1]
// This only applies to channel strips that are modified (i.e. those for which
// Sst[i] > 0 and Sst[i] != the destination strip number)
void CustomSet() {
    int        i, k;
    //
    for (i = MinChn - 1; i < MaxSet; i++) {
        k = Sst[i];
        if ((k > 0) && (k != i+1)) { // Only if necessary, load from X32's
            //  libchan presets the preset "k" into channel strip index i
            LoadCStrip(k, i);
            // now set audio source of channel i+1
            SetASource(i, Asrc[k - 1]);
        }
    }
    return;
}
//
//
// Insert a channel strip at the only position where (Sst[k] != 0).
// All channel strips right of the insert-selected one are shift right one position.
//
void InsertSet(int set) {
    int        i, k;
    //
    // Find strip where a channel is to be inserted
    //
    for (k = MinChn - 1; k < MaxIns; k++) {
        if (Sst[k] != 0) break;
    }
    if (set) {
        // Sst[k] contains the channel to be used. Remember the audio-source for this
        // channel strip is in Asrc[Sst[k] - 1];
        if (Asrc[Sst[k] - 1] < 0) Asrc[Sst[k] - 1] = GetASource(Sst[k]);
    }
    //
    // k represents the insertion point, and the last channel to shift as we'll
    // start from the very last channel to perform right shift operations below
    //
    for (i = MaxIns - 1; i > k; i--) {
        ShiftCStrip(i);
    }
    //
    // Done shifting channels strips,
    if (set) {
        // copy channel (Sst[k]) to position k
        LoadCStrip(Sst[k], k);
        // set audio-source of channel k (previously saved in Asrc[Sst[k] - 1])
        SetASource(k, Asrc[Sst[k] - 1]);
    } else {
        //
        // set a new (blank)strip at index k with audio source = Sst[k]
        LoadNStrip(k, Sst[k]);
    }
    return;
}
//
//
// The main program
//
// Reads resource file and set variables accordingly
// registers with wWinMain() to launch the main window, and main loop
//
int
main(int argc, char **argv)
{
    HINSTANCE  hPrevInstance = 0;
    PWSTR      pCmdLine = 0;
    int        nCmdShow = 0;
    int        i;

    Xdebug = 0;
    p_status = 0;
    //
    // Open and read resource file
    if ((res_file = fopen("./.X32CustomLayer.ini", "r")) != NULL) {
        // get and remember real path
        if((Finiretval = GetFullPathNameA("./.X32CustomLayer.ini", 1024, Finipath, FinilppPart)) > 1024){
            printf ("Not enough space for file name\r\n");
        }
        *r_buf = '#';
        while (*r_buf == '#') fgets(r_buf, sizeof(r_buf), res_file);
        sscanf(r_buf, "%d %d\n", &bWidth, &bHeight);
        wWidth = 16 * bWidth5 + 20;
        wHeight = 6 * (bHeight + 5) + 80;
        fscanf(res_file, "%d %d %d %d\n", &MinChn, &MaxSet, &MaxIns, &MaxPre);
        fgets(Xip_str, sizeof(Xip_str), res_file);
        Xip_str[strlen(Xip_str)] = 0;
        fscanf(res_file, "%d %d\n", &Xdebug, &Sixty3);
        fclose (res_file);
    }
    for (i = 0; i < Ch_strings_max; i++) {
        strcpy(Ch_strings[i], Ch_inistr[i]);
    }
    for (i = 0; i < Aux_strings_max; i++) {
        strcpy(Aux_strings[i], Aux_inistr[i]);
    }
    if (Xdebug) {
        ShowWindow(GetConsoleWindow(), SW_SHOW); // show console window
    } else {
        ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
    }
    wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
    return 0;
}
