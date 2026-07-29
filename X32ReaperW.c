/*
 * X32Reaper.c
 *
 *  Created on: 20 mars 2015
 *      Author: Patrick-Gilles Maillot
 *
 *
 * Ver 1.7: first version being published
 * Ver 1.8: added variable TrackSendOffset and associated code to deal with REAPER increasing the
 *          logical Track Send values when one also assigns a HW output to a REAPER track
 * Ver 1.9: A user correctly reported a pb with several track being simultaneously selected... System
 *          behaves badly. This was due to my code trying to align the REAPER slider values to X32
 *          fader values. I don't send the X32 values back to REAPER anymore (code is commented, for now)
 * Ver 1.91: For Windows, save interactive window size changes
 *           removed useless ioctl() call - use of select() is enough for non-blocking mode
 * Ver 2.0: Introduce REAPER DCA tracks; multiple REAPER tracks can be controlled from X32 DCA
 *          channels
 * Ver 2.1: Added bank selection (up/down) capability on X32 side, only controlling REAPER. For X32 Channels
 *          only, and by blocks of 32. optimized some Windows calls
 * Ver 2.2: Updated with bank selection recall (up/down) capability, reflecting on X32 the corresponding
 *          REAPER bank data and updating REAPER selected channel to reflect X32 select button value (in
 *          Xselected); Xselected is also setup at init by reading the X32 state.
 *          Additionally, this version enables to set icon type and scribble colors with the track names
 *          by using the following syntax "<name>[ %icon[ %color]]" (spaces are optional.
 * Ver 2.21 & 2.22: bug fixes around memory allocation logic when changing Channel Bank Select state
 * Ver 2.3: removed /action commands 53808 and 53809 which seem to not exist anymore
 * Ver 2.4: Added the capability to enable bank selection (up/down)without transport control, adding two
 * 			values to the resource file.
 * Ver 2.41: small optimizations in dealing with channel bank changes
 * Ver 2.5: Updates for setting buttons added in 2.4 in the GUI - removed Master select flag (always ON)
 * 			Modifications to preset and resource files contents and handling
 * Ver 2.51: bug fix. Some channels could be set to > 32 for X32... which obviously was wrong
 * Ver 2.52: small fix on rounding the fader values to X32 known ones
 *           also enables filtering what is sent to X32 from REAPER
 * Ver 2.53: preventing window resizing
 * Ver 2.6: Adding capability to set REAPER markers from bank C buttons (exclusive from Transport)
 * Ver 2.61: Fixed the fact that Marker insert request was sent twice, and added a possible bank C color :)
 * Ver 2.62: Bank size can be less than 32 (8 or 16 are good options for X32).
 *           Also limits the actual number physical channels that can be used
 * Ver 2.63: Small bug fixes to 2.62
 * Ver 2.64: Added a specific delay for banks (Xdelayb)
 * Ver 2.65: Fixed bug where the first RDCA REAPER track would not update X32 desk
 * Ver 2.66: inclusion of eq handling for X32 (fx/1 for REAPER)
 * Ver 2.67: inclusion a bitwise mask to prevent X32 changes to affect REAPER
 * Ver 2.68: Setting name, icon, or color wasn't correctly reflected for bank other than lowest
 * Ver 2.69: EQ FX gets modified and modifiable only if EQ UI Control flag is set; Requires X32Reaper restart.
 *           Correctly init bkchsz to BNKSZ if Xchbank_on is 0 or is reset to 0
 * Ver 2.70: Changed REAPER ReaEQ index to be variable between 1-9 only; managed through the REQindex variable.
 *           Similarly, REAPER ReaCMP index is also variable between 1-9 only; managed through the RCindex variable
 * Ver 2.71: Fixed attack (comp effect) curve to match correctly. Init of FX data to better values (0.5 for eq for ex.)
 * Ver 2.72: Added track record arm toggle functionality (action 40294) with combination of keys: Beg Loop/Repeat
 *           (i.e. Beg Loop acts as a shift key - works too if bank is on, then it's UP that acts as a shift key)
 * Ver 2.73: Xdelayb and Xdelayg were mixed up; plus removed some of the Xdelayb delays to speedup bank changes,
 *           otherwise too slow, even with a 1ms delay.
 * Ver 2.80: Added keyboard shortcuts for all button-press GUI functions: connect, save, load, transport, bank, marker, EQ
 *           Window must have keyboard focus; shortcuts are C, S, L, T, B, M, E, respectively
 * Ver 2.81: Display connect, save, load, transport, bank, marker, EQ states in the title bar to provide NVDA support
 * Ver 2.82: Fixed crash when ch bank select unselected and user sets EQ on (was the same for other effects such as dyn)
 * Ver 2.83: Added buttons up and down to the UI for providing up and down bak capability to X32Rack users
 * Ver 2.84: Added option for buttons up and down to be larger; Presets can be loaded and saved without being connected/running
 * Ver 2.85: Fixed indexing error in Bank.Mixbus updates, as a result mutes were not correctly managed
 * Ver 2.86: Added a REAPER tracks refresh action request right after init to ensure sync.
 */
#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
//
#define BNKSZ 			32	// size (number of tracks) of a bank
#define BSIZE 			1024	// Buffer sizes (enough to take into account FX parameters)
#define LENPATH			256 // maximum length for directory names
#define LENFILE			64	// maximum length for file names
#define LENFILEPATH		LENPATH +  LENFILE + 10	//that's 256+64 + a little extra
												//for extension and digits
//
// X32 to communicate on X32_IP (192.168.0.64) and X32 port (10023)
// REAPER to communicate on Rea_IP (192.168.0.64) and REAPER ports
//
// Buffers: Xb_r, Xb_s, Xb_lr, Xb_ls - read, send and lengths for X32
// Buffers: Rb_r, Rb_s, Rb_lr, Rb_ls - read, send and lengths for Reaper
// Defines: XBsmax, RBsmax, XBrmax, RBrmax - maximum lengths for buffers
//
#define XBsmax	BSIZE
#define RBsmax	BSIZE
#define XBrmax	BSIZE * 2
#define RBrmax	BSIZE * 4
//
// defining bits for enabling sending certain commands to X32 following REAPER parsing
#define TRACKPAN		0x0001
#define TRACKFADER		0x0002
#define TRACKNAME		0x0004
#define TRACKMUTE		0x0008
#define TRACKSELECT		0x0010
#define TRACKSEND		0x0020
#define TRACKSOLO		0x0040
#define TRACKFX			0x0080
#define MASTERPAN		0x0100
#define MASTERVOLUME	0x0200
//
// defining bits for enabling sending certain commands to REAPER following X32 parsing
#define X32PAN			0x0001
#define X32FADER		0x0002
#define X32NAME			0x0004
#define X32MUTE			0x0008
#define X32SELECT		0x0010
#define X32SEND			0x0020
#define X32SOLO			0x0040
#define X32FX			0x0080
#define X32MPAN			0x0100
#define X32MFADER		0x0200
//
// Private functions
void X32UsrCtrlC();
void XGetPanelData();
void XGetPresetData();
void XSetPresetData();
void XUpdateBkCh();
void XManageTransport();
void XManageSetmk();
void XAllocChbank();
void XManageChbank();
void XRcvClean();
void Xlogf(char *header, char *buf, int len);
void X32_eqon(int Xb_i, int bank, int cnum, int cnum1);
void X32_eqfr(int Xb_i, int bank, int cnum, int cnum1);
void X32_eqgq(int Xb_i, int bank, int cnum, int cnum1, int index);
void X32_dynon(int Xb_i, int bank, int cnum, int cnum1);
void X32_dynthr(int Xb_i, int bank, int cnum, int cnum1, int index);
void X32_dynratio(int Xb_i, int bank, int cnum, int cnum1, int index);
void X32_dynattack(int Xb_i, int bank, int cnum, int cnum1, int index);
void X32_dynrelease(int Xb_i, int bank, int cnum, int cnum1, int index);
void X32_dynmix(int Xb_i, int bank, int cnum, int cnum1, int index);
void X32ParseReaperMessage();
void X32ParseX32Message();
int  X32Connect();
void XwritePresetFile(FILE* pre_file);
int  XreadPresetFile(FILE* pre_file);
//
// External calls used
extern int Xsprint(char *bd, int index, char format, void *bs);
extern int Xfprint(char *bd, int index, char* text, char format, void *bs);
extern int validateIP4Dotted(char *s);
//
//
// Communication macros
//
#define SEND_TOX(delay)													\
	do {																\
		if (Xverbose) Xlogf("->X", Xb_s, Xb_ls);						\
		if (sendto(Xfd, Xb_s, Xb_ls, 0, XX32IP_pt, XX32IP_len) < 0) {	\
			fprintf(log_file, errorX32);								\
			exit(EXIT_FAILURE);											\
		} 																\
		if (delay > 0) Sleep((delay));									\
	} while (0);
//
//
#define SEND_TOR()														\
	do {																\
		if (Xverbose) Xlogf("->R", Rb_s, Rb_ls);						\
		if (sendto(Rfd, Rb_s, Rb_ls, 0, RHstIP_pt, RHstIP_len) < 0) {	\
			fprintf(log_file, errorRea);								\
			exit(EXIT_FAILURE);											\
		} 																\
	} while (0);
//
//
// Global variables
//
//
// Windows calls & callbacks
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE hInstance = 0;
HACCEL haccel;
HWND hwnd, hwndX32IP, hwndconx, hwndHstIP, hwndRecPort, hwndSndPort;
HWND hwndtprt, hwndchbk, hwndsave, hwndrest, hwnbkupbt, hwnbkdnbt;
HWND hwndsetm, hwnbkmkbt, hwneqctrl;
HWND hwndcamin, hwnbusmin, hwntrkmin, hwnfxrmin, hwnauxmin;
HWND hwndcamax, hwnbusmax, hwntrkmax, hwnfxrmax, hwnauxmax;
HWND hwnbusoff, hwnbksoff, hwnrcamin[8], hwnrcamax[8];
HWND hwnbkup, hwnbkdn;
RECT Rect;
HFONT hfont, htmp;
HDC hdc, hdcMem;
PAINTSTRUCT ps;
HBITMAP hBmp;
BITMAP bmp;
MSG wMsg;
HANDLE hf;             		// file handle
OPENFILENAME ofn;       	// common dialog box structure
char XPresetFileName[LENFILE];	// preset or resource file name
char X32PreFilePath[LENPATH];	// preset file path
char Xlogpath[LENPATH];			// log file path
// resource, preset and log file handlers
FILE *log_file;			// log file
FILE *pre_file;			// preset or resource files
//
// Window size (defaults overwritten by actual size)
int wWidth = 395;
int wHeight = 350;
int wBigButtons;
//
// type cast union
union littlebig {
	char cc[4];
	int ii;
	float ff;
} endian;
//
// reserve communication buffers
int Xb_ls;
char Xb_s[XBsmax];
int Xb_lr;
char Xb_r[XBrmax];
int Rb_lr;
char Rb_r[RBrmax];
int Rb_ls;
char Rb_s[RBsmax];
//
char *errorX32 = "Error sending data to X32\n";
char *errorRea = "Error sending data to REAPER\n";
char *errsock[] = { "create", "reuse", "bind" };
int loop_toggle = 0x00; // toggles between 0x00 and 0x7f
//
// Windows UI related storage
char	S_SndPort[8], S_RecPort[8], S_X32_IP[20], S_Hst_IP[20];
char	S_DMIN_str[8], S_BMIN_str[8], S_AMIN_str[8], S_FMIN_str[8], S_TMIN_str[8];
char	S_DMAX_str[8], S_BMAX_str[8], S_AMAX_str[8], S_FMAX_str[8], S_TMAX_str[8];
char	S_BOFF_str[8], S_BKOF_str[8], S_BKUP_str[8], S_BKDN_str[8], S_MKBT_str[8];
char	R_DMIN_str[8][8], R_DMAX_str[8][8];
//
int zero = 0;
int one = 1;
int two = 2;
int three = 3;
int four = 4;
int six4 = 64;
float fone = 1.0;
int option = 1;
int play = 0;
int play_1 = 0;
// Misc. flags
int MainLoopOn = 1;		// main loop flag
int Xconnected = 0;		// 1 when communication is running
int Xverbose;			// verbose flag
int Xdelayb, Xdelayg;	// OSC delay for banks control and generic OSC delay
int Xtransport_on = 0;	// whether transport is enabled or not (bank C)
int XMkerbt_on = 0;		// Marker Button number on or not (bank C)
int Xchbank_on = 0;		// whether we use Channel bank select and not Loops in bank C
int	Xchbkof = 0;		// channel bank number;
int Xselected = 0;		// X32 channel currently selected
int Rselected = 1;		// REAPER track currently selected
int Xmaster_on = 1;		// whether master is enabled or not
int TrackSendOffset = 0;// offset to manage REAPER track sends logical numbering
int XbankCcol = 0;		// Bank C color (is set when reading .ini file)
int Xeqcmp_on = 0;		// Enable EQ and CMP UI Control (is set when reading .ini file)
int	REQindex = 1;		// default ReaEQ effect index (is set when reading .ini file)
int RCindex = 2;		// default ReaCmp effect index (is set when reading .ini file)
int XShift = 0;			// flag to indicate key is currently down-pressed.
//
int XMbankup;			// user "bank up" selected button [5..12]
int XMbankdn;			// user "bank down" selected button [5..12]
int XMkerbtn;			// Marker Button number
//
int Xtrk_min = 0;		// Input min track number for Reaper/X32
int Xtrk_max = 0;		// Input max track number for Reaper/X32
int Xaux_min = 0;		// Auxin min track number for Reaper/X32
int Xaux_max = 0;		// Auxin max track number for Reaper/X32
int Xfxr_min = 0;		// FXrtn min track number for Reaper/X32
int Xfxr_max = 0;		// FXrtn max track number for Reaper/X32
int Xbus_min = 0;		// Bus min track number for Reaper/X32
int Xbus_max = 0;		// Bus max track number for Reaper/X32
int Xdca_min = 0;		// DCA min track number for Reaper/X32
int Xdca_max = 0;		// DCA max track number for Reaper/X32
int Rdca_min[8] = {0, 0, 0, 0, 0, 0, 0, 0};	// REAPER 'dca' mins
int Rdca_max[8] = {0, 0, 0, 0, 0, 0, 0, 0};	// REAPER 'dca' maxs
int XXmask = 0; // bit mask for commands sent to X32 following REAPER parsing
int Xxsend = -1; // bits for enabling sending commands sent to X32
int XRmask = 0; // bit mask for commands sent to REAPER following X32 parsing
int Xrsend = -1; // bits for enabling sending commands sent to REAPER
//
struct ifaddrs *ifa;									// to get our own system's IP address
struct sockaddr_in XX32IP;								// X socket IP we send/receive
struct sockaddr *XX32IP_pt = (struct sockaddr*) &XX32IP;// X socket IP pointer we send/receive
struct sockaddr_in RHstIP;								// R socket IP we send to
struct sockaddr *RHstIP_pt = (struct sockaddr*) &RHstIP;// R socket IP pointer we send to
struct sockaddr_in RFrmIP;								// R socket IP we received from
struct sockaddr *RFrmIP_pt = (struct sockaddr*) &RFrmIP;// R socket IP pointer we received from
int Xfd, Rfd, Mfd;					// X32 and Reaper receive and send sockets, and temp socket
//
#ifdef __WIN32__
WSADATA wsa;
int XX32IP_len = sizeof(XX32IP);	// length of X32 address
int RHstIP_len = sizeof(RHstIP);	// length of Reaper send to address
int RFrmIP_len = sizeof(RFrmIP);	// length of Reaper received from address
#else
unsigned int XX32IP_len = sizeof(XX32IP);	// length of X32 address
unsigned int RHstIP_len = sizeof(RHstIP);	// length of Reaper send to address
unsigned int RFrmIP_len = sizeof(RFrmIP);	// length of Reaper received from address
#endif
//
fd_set 			fds;		// socket file descriptor
struct timeval	timeout;	// UDP non-blocking Read timeout
int p_status;				// UDP Read status flag
time_t before, now;			// timers for Xremote controls
//
// structure definition for REAPER data backup (used for REAPER bank switches)
// we'll allocate memory according to the number of channels declared in the
// .X32Reaper.ini resource file
typedef struct bkch {
	float	fader;			// volume
	float	pan;			// panoramic
	float	mixbus[16];		// sends values
	float	mute;			// mute
	float	solo;			// solo
	char	scribble[16];	// scribble text; keep only 12 chars for X32
	int		color;			// scribble color
	int		icon;			// scribble icon
	float	eq[16];			// eq params
	int		eqon;			// eq enabled
	float	cmp[16];		// cmp params
	int		cmpon;			// cmp enabled
} S_bkch;
//
S_bkch *XMbanktracks = NULL;	// Address to data array for saved channel banks data
int	bkchsz           = BNKSZ;	// size of channel banks (32 or less)
int	bkchsz_save      = BNKSZ;	// used as saved value of bkchsz
//
// Keyboard shortcuts translation table
ACCEL accel[10] = {
	{0,	'C', 1},	// Connect
	{0,	'S', 2},	// Save
	{0,	'L', 3},	// Load
	{0,	'T', 4},	// Transport (on/off)
	{0,	'B', 5},	// channel Bank select (on/off)
	{0,	'M', 6},	// Markers
	{0,	'F', 7},	// FX UI control
	{0,	'U', 8},	// Bank Up UI control
	{0,	'D', 9},	// Bank Down UI control
};
//
//-------------------------------------------------------------------------
//
// Window create and mainloop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		PWSTR lpCmdLine, int nCmdShow) {

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"X32Reaper";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"X32Reaper - 2-way communication X32 / REAPER",
			WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 100, 220, wWidth, (wBigButtons?wHeight+60:wHeight), 0, 0, hInstance, 0);
//
// Define shortcuts
	   haccel = CreateAcceleratorTableA(accel, 10);
//
// Entering main loop
	while (MainLoopOn) {
		// Consume WIndows Messages
		while (PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			if (!TranslateAccelerator(wMsg.hwnd, haccel, &wMsg)) {
				TranslateMessage(&wMsg);
				DispatchMessage(&wMsg);
			}
		}
		// If connected/running, Consume X32 and REAPER messages
		if (Xconnected) {
			now = time(NULL); 			// get time in seconds
			if (now > before + 9) { 	// need to keep xremote alive?
				Xb_ls = Xsprint(Xb_s, 0, 's', "/xremote");
				SEND_TOX(Xdelayg)
				before = now;
			}
//
// Update on the X32 or Reaper?
			FD_ZERO(&fds);
			FD_SET(Rfd, &fds);
			FD_SET(Xfd, &fds);
			Mfd = Rfd + 1;
			if (Xfd > Rfd) Mfd = Xfd + 1;
			if (select(Mfd, &fds, NULL, NULL, &timeout) > 0) {
				if (FD_ISSET(Rfd, &fds) > 0) {
					if ((Rb_lr = recvfrom(Rfd, Rb_r, RBrmax, 0, RFrmIP_pt, &RFrmIP_len)) > 0) {
// Parse Reaper Messages and send corresponding data to X32
// These can be simple or #bundle messages!
// Can result in several/many messages to X32
						if (Xverbose) Xlogf("R->", Rb_r, Rb_lr);
						X32ParseReaperMessage();
					}
				}
				if (FD_ISSET(Xfd, &fds) > 0) {
					if ((Xb_lr = recvfrom(Xfd, Xb_r, XBrmax, 0, XX32IP_pt, &XX32IP_len)) > 0) {
// X32 transmitted something
// Parse and send (if applicable) to Reaper
						if (Xverbose) Xlogf("X->", Xb_r, Xb_lr);
						X32ParseX32Message();
					}
				}
			}
		} else { // not connected/running
			Sleep(100); //wait 100ms before looping again
		}
	}
	return (int) wMsg.wParam;
}
//
// Windows callbacks
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int i;
	char str1[LENFILE];
	//
	switch (msg) {
	case WM_CREATE:
		//
		hwndX32IP = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 125, 35, 120, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndconx = CreateWindowW(L"button", L"Connect/Run",
				WS_VISIBLE | WS_CHILD, 255, 35, 120, 60, hwnd,
				(HMENU )1, NULL, NULL);
		//
		hwndHstIP = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 125, 75, 120, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndSndPort = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 125, 115, 120, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndRecPort = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 255, 115, 120, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwndsave = CreateWindowW(L"button", L"Save",
				WS_VISIBLE | WS_CHILD, 150, 185, 40, 20, hwnd,
				(HMENU )2, NULL, NULL);
		hwndrest = CreateWindowW(L"button", L"Load",
				WS_VISIBLE | WS_CHILD, 190, 185, 40, 20, hwnd,
				(HMENU )3, NULL, NULL);
		//
		hwntrkmin = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 55, 149, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwntrkmax = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 105, 149, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwnauxmin = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 55, 169, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwnauxmax = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 105, 169, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwnbusmin = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 55, 189, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwnbusmax = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 105, 189, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwnbusoff = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 210, 149, 25, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwnfxrmin = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 295, 149, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwnfxrmax = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 345, 149, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwndcamin = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 295, 169, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		hwndcamax = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 345, 169, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwndtprt = CreateWindowW(L"button", L"",
				BS_CHECKBOX | WS_VISIBLE | WS_CHILD, 100, 216, 15, 20, hwnd,
				(HMENU )4, NULL, NULL);
		//
		hwndchbk = CreateWindowW(L"button", L"",
				BS_CHECKBOX | WS_VISIBLE | WS_CHILD, 5, 237, 15, 20, hwnd,
				(HMENU )5, NULL, NULL);
		//
		hwnbkupbt = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 188, 236, 25, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwnbkdnbt = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 240, 236, 25, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwnbksoff = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 345, 236, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		hwndsetm = CreateWindowW(L"button", L"",
				BS_CHECKBOX | WS_VISIBLE | WS_CHILD, 220, 216, 15, 20, hwnd,
				(HMENU )6, NULL, NULL);
		//
		hwneqctrl = CreateWindowW(L"button", L"",
				BS_CHECKBOX | WS_VISIBLE | WS_CHILD, 355, 194, 15, 20, hwnd,
				(HMENU )7, NULL, NULL);
		//
		hwnbkmkbt = CreateWindowW(L"Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 345, 216, 30, 20, hwnd,
				(HMENU )0, NULL, NULL);
		//
		if (wBigButtons) {
			hwnbkup = CreateWindowW(L"Button", L"UP",
					WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 266, 180, 50, hwnd,
					(HMENU )8, NULL, NULL);
			hwnbkdn = CreateWindowW(L"Button", L"DOWN",
					WS_CHILD | WS_VISIBLE | WS_BORDER, 195, 266, 180, 50, hwnd,
					(HMENU )9, NULL, NULL);
		} else {
			hwnbkup = CreateWindowW(L"Button", L"up",
					WS_CHILD | WS_VISIBLE | WS_BORDER, 168, 236, 20, 20, hwnd,
					(HMENU )8, NULL, NULL);
			hwnbkdn = CreateWindowW(L"Button", L"dn",
					WS_CHILD | WS_VISIBLE | WS_BORDER, 220, 236, 20, 20, hwnd,
					(HMENU )9, NULL, NULL);
		}
		//
		for (i = 0; i < 8; i++) {
			hwnrcamin[i] = CreateWindowW(L"Edit", NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER, 65 + i * 40, (wBigButtons?324:264), 30, 20, hwnd,
					(HMENU )0, NULL, NULL);
			hwnrcamax[i] = CreateWindowW(L"Edit", NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER, 65 + i * 40, (wBigButtons?344:284), 30, 20, hwnd,
					(HMENU )0, NULL, NULL);
		}
		if ((hBmp = (HBITMAP) LoadImage(NULL, (LPCTSTR) "./.X32.bmp",
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_SHARED)) == NULL) {
			fprintf(log_file, "Pixel bitmap file no found\n");
		}
		//
		// If needed allocate memory (an init it) for banks management
		XAllocChbank();
		break;
//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (hBmp) {
			hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hBmp);
			BitBlt(hdc, 5, 2, 115, 140, hdcMem, 0, 0, SRCCOPY);
			DeleteDC(hdcMem);
		}
		SetBkMode(hdc, TRANSPARENT);
		MoveToEx(hdc, 5, 144, NULL);
		LineTo(hdc, wWidth - 12, 144);
		//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
		        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		        ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 125, 0, str1, wsprintf(str1, "X32Reaper - [c]2015 - Patrick-Gilles Maillot"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		hfont = CreateFont(14, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		        ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 127, 20, str1, wsprintf(str1, "Enter X32 IP below:"));
		TextOut(hdc, 127, 60, str1, wsprintf(str1, "REAPER/HOST IP:"));
		TextOut(hdc, 127, 100, str1, wsprintf(str1, "Device Port:"));
		TextOut(hdc, 257, 100, str1, wsprintf(str1, "Local Listen Port:"));
		TextOut(hdc, 325, 18, str1, wsprintf(str1, "ver. 2.86"));
		//
		MoveToEx(hdc, 150, 178, NULL);
		LineTo(hdc, 167, 178);
		MoveToEx(hdc, 213, 178, NULL);
		LineTo(hdc, 230, 178);
		TextOut(hdc, 170, 170, str1, wsprintf(str1, "Presets :"));
		//
		TextOut(hdc, 5, 152, str1, wsprintf(str1, "X32 IN:"));
		TextOut(hdc, 90, 152, str1, wsprintf(str1, "to:"));
		TextOut(hdc, 5, 172, str1, wsprintf(str1, "X32 Aux:"));
		TextOut(hdc, 90, 172, str1, wsprintf(str1, "to:"));
		TextOut(hdc, 5, 192, str1, wsprintf(str1, "X32 Bus:"));
		TextOut(hdc, 90, 192, str1, wsprintf(str1, "to:"));
		//
		TextOut(hdc, 150, 152, str1, wsprintf(str1, "RTS Offset"));
		TextOut(hdc, 245, 152, str1, wsprintf(str1, "X32 FX:"));
		TextOut(hdc, 330, 152, str1, wsprintf(str1, "to:"));
		TextOut(hdc, 245, 172, str1, wsprintf(str1, "X32 DCA:"));
		TextOut(hdc, 330, 172, str1, wsprintf(str1, "to:"));
		//
		MoveToEx(hdc, 5, 212, NULL);
		LineTo(hdc, 240, 212);
		LineTo(hdc, 240, 192);
		LineTo(hdc, wWidth - 12, 192);
		TextOut(hdc, 5, 218, str1, wsprintf(str1, "Bank C: Transport:"));
		TextOut(hdc, 130, 218, str1, wsprintf(str1, "REAPER Markers:"));
		TextOut(hdc, 250, 218, str1, wsprintf(str1, "Marker Insert Btn #:"));
		TextOut(hdc, 250, 196, str1, wsprintf(str1, "Enable FX UI Control:"));
		//
//		MoveToEx(hdc, 5, 240, NULL);
//		LineTo(hdc, wWidth - 12, 240);
		TextOut(hdc, 25, 239, str1, wsprintf(str1, "Channel Bank select"));
		TextOut(hdc, 140, 239, str1, wsprintf(str1, "Btn # UP:"));
		TextOut(hdc, 220, 239, str1, wsprintf(str1, "DN:"));
		TextOut(hdc, 275, 239, str1, wsprintf(str1, "Current Bank:"));
		//
		MoveToEx(hdc, 5, (wBigButtons?320:260), NULL);
		LineTo(hdc, wWidth - 12, (wBigButtons?320:260));
		//
		TextOut(hdc, 5, (wBigButtons?327:267), str1, wsprintf(str1, "RDCA min:"));
		TextOut(hdc, 5, (wBigButtons?347:287), str1, wsprintf(str1, "RDCA max:"));
		TextOut(hdc,  80, (wBigButtons?365:305), str1, wsprintf(str1, "1"));
		TextOut(hdc, 120, (wBigButtons?365:305), str1, wsprintf(str1, "2"));
		TextOut(hdc, 160, (wBigButtons?365:305), str1, wsprintf(str1, "3"));
		TextOut(hdc, 200, (wBigButtons?365:305), str1, wsprintf(str1, "4"));
		TextOut(hdc, 240, (wBigButtons?365:305), str1, wsprintf(str1, "5"));
		TextOut(hdc, 280, (wBigButtons?365:305), str1, wsprintf(str1, "6"));
		TextOut(hdc, 320, (wBigButtons?365:305), str1, wsprintf(str1, "7"));
		TextOut(hdc, 360, (wBigButtons?365:305), str1, wsprintf(str1, "8"));
		//
		DeleteObject(htmp);
		DeleteObject(hfont);
		EndPaint(hwnd, &ps);
		//
		SetWindowText(hwndX32IP, (LPSTR) S_X32_IP);
		SetWindowText(hwndHstIP, (LPSTR) S_Hst_IP);
		SetWindowText(hwndSndPort, (LPSTR) S_SndPort);
		SetWindowText(hwndRecPort, (LPSTR) S_RecPort);
		//
		XSetPresetData();
		break;
	case WM_COMMAND:
		if ((HIWORD(wParam) == BN_CLICKED) || (HIWORD(wParam) == KEY_EVENT)) {	// user action
			i = LOWORD(wParam);
			switch (i) {
			case 1:	// connect/run
				XGetPanelData();
				if (validateIP4Dotted(S_X32_IP) && validateIP4Dotted(S_Hst_IP)) {
					Xconnected = X32Connect();
					if (Xconnected) {
						SetWindowTextW(hwndconx, L"Running ...");
						SetWindowTextW(hwnd, L"X32Reaper: Running");
					} else {
						SetWindowTextW(hwndconx, L"Connect/Run");
						SetWindowTextW(hwnd, L"X32Reaper: Not connected");
					}
				}
				break;
			case 2:	// save preset
				SetWindowTextW(hwnd, L"X32Reaper: Save X32Reaper Preset");
				strcpy(X32PreFilePath, "extension '.xpr' will be added to filename");
				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = X32PreFilePath;
				ofn.nMaxFile = sizeof(X32PreFilePath);
				ofn.lpstrFilter = "X32Reaper Presets\0*.xpr\0\0";
				ofn.nFilterIndex = 0;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrTitle = (LPCTSTR)"Save X32Reaper Preset to filename below\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_EXPLORER | OFN_CREATEPROMPT |
							OFN_HIDEREADONLY | OFN_NOVALIDATE |
							OFN_PATHMUSTEXIST | OFN_READONLY;
				// Display the Open dialog box.
				if (GetSaveFileName(&ofn)) {
					if ((i = strlen(X32PreFilePath)) > 4) {
						if (strncmp(&X32PreFilePath[i - 4], ".xpr", 4) != 0) strcat(X32PreFilePath, ".xpr");
					}
					if (MessageBox(NULL, X32PreFilePath, "Save to file: ", MB_YESNO) == IDYES) {
						if ((pre_file = fopen(X32PreFilePath, "wb")) != 0) {
							XGetPresetData();
							XwritePresetFile(pre_file);
							fclose (pre_file);
						}
					}
				}
				break;
			case 3:	// Load preset
				SetWindowTextW(hwnd, L"X32Reaper: Load X32Reaper Preset");
				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = X32PreFilePath;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(X32PreFilePath);
				ofn.lpstrFilter = "X32Reaper Presets\0*.xpr\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = XPresetFileName;
				ofn.nMaxFileTitle = sizeof(XPresetFileName);;
				ofn.lpstrTitle = (LPCTSTR)"Select an X32Reaper Preset from the files below\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
					if ((pre_file = fopen(X32PreFilePath, "rb")) != 0) {
						if (XreadPresetFile(pre_file)) {
							XSetPresetData();
							// set/show preset name in window title bar.
							XPresetFileName[strlen(XPresetFileName) - 4] = 0;
							sprintf(X32PreFilePath, "X32Reaper - Preset: %s", XPresetFileName);
							if (SetWindowText(hwnd, X32PreFilePath) == FALSE) {};
						}
						fclose (pre_file);
					}
				}
				break;
			case 4:	// manage transport
				if (Xtransport_on ^= 1) {
					XMkerbt_on = 0;
				}
				if (Xtransport_on) SetWindowTextW(hwnd, L"X32Reaper: Bank C Transport Selected");
				else               SetWindowTextW(hwnd, L"X32Reaper: Bank C Transport Unselected");
				XManageTransport();
				XManageSetmk();
				if (Xconnected) X32UsrCtrlC();
				break;
			case 5:	// manage channel banks, vs. loops
				//
				// How to read all REAPER tracks for updating colors, scribbles etc...?
				// This happens automatically at REAPER startup, but if the Ch Bank data is completely
				// deleted and later recreated, All data will be at logical 0
				// Request confirmation...
				if (!Xchbank_on) {
					if (MessageBox(NULL, "this will set all scribbles to black!\ncontinue?", "Warning", MB_YESNO) == IDYES) {
						Xchbank_on ^= 1;
						XManageChbank();
						SetWindowTextW(hwnd, L"X32Reaper: Channel Bank select Selected");
						if (Xconnected) X32UsrCtrlC();
					}
				} else {
					Xchbank_on ^= 1;
					XManageChbank();
					SetWindowTextW(hwnd, L"X32Reaper: Channel Bank select Unselected");
					if (Xconnected) X32UsrCtrlC();
				}
				break;
			case 6:	// manage marker button
				if (XMkerbt_on ^= 1) {
					Xtransport_on = 0;
				}
				if (XMkerbt_on) SetWindowTextW(hwnd, L"X32Reaper: REAPER Markers Selected");
				else            SetWindowTextW(hwnd, L"X32Reaper: REAPER Markers Unselected");
				//
				// XMkerbtn (Marker Button) is exclusive from Xtransport_on (Transport)
				XManageTransport();
				XManageSetmk();
				if (Xconnected) X32UsrCtrlC();
				break;
			case 7:	// manage EQ & CMP UI Control
				Xeqcmp_on ^= 1;
				SendMessage(hwneqctrl, BM_SETCHECK, Xeqcmp_on? BST_CHECKED: BST_UNCHECKED, 0);
				if (Xeqcmp_on) SetWindowTextW(hwnd, L"X32Reaper: Enable FX UI Control Selected");
				else           SetWindowTextW(hwnd, L"X32Reaper: Enable FX UI Control Unselected");
				break;
			case 8:
				if (Xconnected && Xchbank_on) {
					// Channel Bank UP
					if (Xchbkof < ((Xtrk_max - Xtrk_min + 1) / bkchsz) - 1) {
						Xchbkof += 1; // ignore non zero values
						XUpdateBkCh();
					}
				}
				break;
			case 9:
				if (Xconnected && Xchbank_on) {
					// Channel Bank DOWN
					if (Xchbkof > 0) { // 0 is the lowest accepted value
						Xchbkof -= 1;
						XUpdateBkCh();
					}
				}
				break;
			}
		}
		break;
	case WM_DESTROY:
		// save data before quit
		XGetPanelData();
		if ((pre_file = fopen("./.X32Reaper.ini", "w")) != NULL) {
			GetWindowRect(hwnd, &Rect);
//			fprintf(pre_file, "%d %d %d %d %d %d %d %d\n",(int)(Rect.right - Rect.left), (int)(Rect.bottom - Rect.top), Xverbose, Xdelayb, Xdelayg, Xxsend, Xrsend, wBigButtons);
			fprintf(pre_file, "%d %d %d %d %d %d %d %d\n",wWidth, wHeight, Xverbose, Xdelayb, Xdelayg, Xxsend, Xrsend, wBigButtons);
			fprintf(pre_file, "%s\n", S_X32_IP);
			fprintf(pre_file, "%s\n", S_Hst_IP);
			fprintf(pre_file, "%s\n", S_SndPort);
			fprintf(pre_file, "%s\n", S_RecPort);
			XwritePresetFile(pre_file);
			fclose(pre_file);
		} else {
			fprintf(log_file, "Failed to save resource file\n");
		}
		MainLoopOn = 0;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//---------------------------------------------------------------------------------
//
//
// Private functions:
//
//
int X32Connect() {
	int i;
//
	if (Xconnected) {
//
// Signing OFF
		Xb_ls = Xsprint(Xb_s, 0, 's', "/unsubscribe");
		if (Xverbose)
			Xlogf("->X", Xb_s, Xb_ls);
		if (sendto(Xfd, Xb_s, Xb_ls, 0, XX32IP_pt, XX32IP_len) < 0)
			fprintf(log_file, errorX32);
		WSACleanup();
		return 0;
	} else {
//
// Initialize winsock / communication with X32 server at IP ip and PORT port
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			fprintf(log_file, "WSA Startup Error\n");
			exit(EXIT_FAILURE);
		}
//
// Load the X32 address we connect to; we're a client to X32, keep it simple.
		// Create the UDP socket
		if ((Xfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
			fprintf(log_file, "X32 socket creation error\n");
			WSACleanup();
			return 0; // Make sure we don't considered being connected
		} else {
			// Construct the server sockaddr_in structure
			memset(&XX32IP, 0, sizeof(XX32IP));				// Clear struct
			XX32IP.sin_family = AF_INET;					// Internet/IP
			XX32IP.sin_addr.s_addr = inet_addr(S_X32_IP);	// IP address
			XX32IP.sin_port = htons(atoi("10023"));			// X32 port
//
// Non blocking mode; Check for X32 connectivity
			timeout.tv_sec = 0;
			timeout.tv_usec = 100000; //Set timeout for non blocking recvfrom(): 100ms
			Xb_ls = Xsprint(Xb_s, 0, 's', "/info");
			if (Xverbose)
				Xlogf("->X", Xb_s, Xb_ls);
			if (sendto(Xfd, Xb_s, Xb_ls, 0, XX32IP_pt, XX32IP_len) < 0) {
				fprintf(log_file, errorX32);
				WSACleanup();
				return 0; // Make sure we don't considered being connected
			} else {
				FD_ZERO(&fds);
				FD_SET(Xfd, &fds);
				p_status = select(Xfd + 1, &fds, NULL, NULL, &timeout);
				if (p_status < 0) {
					fprintf(log_file, "Polling for data failed\n");
					WSACleanup();
					return 0; // Make sure we don't considered being connected
				} else if (p_status > 0) {
					// We have received data - process it!
					Xb_lr = recvfrom(Xfd, Xb_r, BSIZE, 0, 0, 0);
					if (Xverbose)
						Xlogf("X->", Xb_r, Xb_lr);
					if (strcmp(Xb_r, "/info") != 0) {
						fprintf(log_file, "Unexpected answer from X32\n");
						WSACleanup();
						return 0;
					}
				} else {
					// time out... Not connected or Not an X32
					fprintf(log_file, "X32 reception timeout\n");
					WSACleanup();
					return 0; // Make sure we don't considered being connected
				}
				// Connected! Get the X32 channel ID that's currently selected
				// to init the Xselected global variable.
				// This is likely to be overwritten when loading REAPER template
				Xb_ls = Xsprint(Xb_s, 0, 's', "/-stat/selidx");
				SEND_TOX(Xdelayg)
				// get data back
				FD_ZERO(&fds);
				FD_SET(Xfd, &fds);
				if ((p_status = select(Xfd + 1, &fds, NULL, NULL, &timeout)) > 0) {
					Xb_lr = recvfrom(Xfd, Xb_r, BSIZE, 0, 0, 0);
					if (strcmp(Xb_r, "/-stat/selidx") == 0) {	//
						for (i = 0; i < 4; i++) endian.cc[i] = Xb_r[23-i];
						Xselected = endian.ii;
						Rselected = Xselected + 1;
					}
				} // ignore errors or timeout (leave Xselected = 0, Rselected = 1)
			}
		}
	}
//
// X32 connectivity OK. Set Connection with REAPER as Hst (HOST)
// Connect / Bind HOST
	i = 0;
	if ((Rfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0) {
		i = 1;
		if (setsockopt(Rfd, SOL_SOCKET, SO_REUSEADDR, (char*) &option,
				sizeof(option)) >= 0) {
			// Construct the server sockaddr_in structure
			memset(&RHstIP, 0, sizeof(RHstIP)); /* Clear struct */
			RHstIP.sin_family = AF_INET; /* Internet/IP */
			RHstIP.sin_addr.s_addr = inet_addr(S_Hst_IP); /* Reaper IP address */
			RHstIP.sin_port = htons(atoi(S_RecPort)); /* The Reaper port we send to */
//
			memset(&RFrmIP, 0, sizeof(RFrmIP)); /* Clear struct */
			RFrmIP.sin_family = AF_INET; /* Internet/IP */
			RFrmIP.sin_addr.s_addr = htons(atoi(INADDR_ANY)); /* Reaper IP address */
			RFrmIP.sin_port = htons(atoi(S_SndPort)); /* The Reaper port we receive from */
			i = 2;
			if (bind(Rfd, RFrmIP_pt, sizeof(RFrmIP)) != SOCKET_ERROR) {
				timeout.tv_sec = 0;
				timeout.tv_usec = 1000; //Set timeout for non blocking recvfrom(): 1ms for Reaper/X32 comm
//
// Cleanup X32 buffers if needed
				XRcvClean();
//
// Init CH bank & UserCtrl bank C
				X32UsrCtrlC();
// Force REAPER track refresh to ensure sync
				Rb_ls = Xsprint(Rb_s, 0, 's', "/action/41743");
				SEND_TOR()
				return 1; // We are connected!
			}
		}
	}
	// If we're here, there was an error
	fprintf(log_file, "Reaper socket %s error\n", errsock[i]);
	WSACleanup();
	return 0; // Make sure we don't considered being connected
}
//
// Empty any pending messages from X32 function
//
void XRcvClean() {
//
	if (Xverbose)
		fprintf(log_file, "X32 receive buffer cleanup if needed\n");
	do {
		FD_ZERO(&fds);
		FD_SET(Xfd, &fds);
		if ((p_status = select(Xfd + 1, &fds, NULL, NULL, &timeout)) > 0) {
			if ((Xb_lr = recvfrom(Xfd, Xb_r, BSIZE, 0, 0, 0)) > 0) {
				if (Xverbose)
					Xlogf("X->", Xb_r, Xb_lr);
			}
		}
	} while (p_status > 0);	// read and ignore X32 incoming data until
	return;					// first timeout or error
}
//
//
//
int main(int argc, char **argv) {
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmdShow = 0;
//
	Xverbose = Xdelayg = Xchbkof = XMkerbtn = 0;
//
	strcpy(S_X32_IP, "");
	strcpy(S_Hst_IP, "");
	strcpy(Xlogpath, ".");
	if ((p_status = CreateDirectory(Xlogpath, NULL)) == 0) {
		if (ERROR_ALREADY_EXISTS != GetLastError()) {
			printf("Cannot create directory\n");
			Sleep(10000);
			return (-1);
		}
	}
	wBigButtons = 0;
	strcpy(Xb_r, Xlogpath);
	strcat(Xb_r, "/.X32Reaper.log");
	//
	// create logfile and run program
	if ((log_file = fopen(Xb_r, "w")) != NULL) {
		fprintf(log_file, "*\n*\n");
		fprintf(log_file, "*    X32Reaper Log data - [c]2015-2024 - Patrick-Gilles Maillot\n");
		fprintf(log_file, "*\n*\n");
		// load resource file
		if ((pre_file = fopen("./.X32Reaper.ini", "r")) != NULL) {
			//
			fscanf(pre_file, "%d %d %d %d %d %d %d %d\n", &wWidth, &wHeight, &Xverbose, &Xdelayb, &Xdelayg, &Xxsend, &Xrsend, &wBigButtons);
			fgets(S_X32_IP, sizeof(S_X32_IP), pre_file);
			S_X32_IP[strlen(S_X32_IP) - 1] = 0;
			fgets(S_Hst_IP, sizeof(S_Hst_IP), pre_file);
			S_Hst_IP[strlen(S_Hst_IP) - 1] = 0;
			fgets(S_SndPort, sizeof(S_SndPort), pre_file);
			S_SndPort[strlen(S_SndPort) - 1] = 0;
			fgets(S_RecPort, sizeof(S_RecPort), pre_file);
			S_RecPort[strlen(S_RecPort) - 1] = 0;
			if (XreadPresetFile(pre_file) == 0) {
				printf("Error in reading resource file\n");
			}
			fclose(pre_file);
			ShowWindow(GetConsoleWindow(), SW_HIDE); 		// Hide console window
			wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
			WSACleanup();
			if (XMbanktracks) {
				free(XMbanktracks);
				XMbanktracks = NULL;
			}
		} else {
			fprintf(log_file, "couldn't open .X32Reaper.ini file\n");
		}
	} else {
		printf("Cannot create log file\n");
		Sleep(5000);
		return (-3);
	}
	if (log_file)
		fclose(log_file);
	return 0;
}
//
//
void Xlogf(char *header, char *buf, int len) {
	int i, k, n, j, l, comma = 0, data = 0, dtc = 0;
	unsigned char c;
	//
	fprintf(log_file, "%s, %4d B: ", header, len);
	for (i = 0; i < len; i++) {
		c = (unsigned char) buf[i];
		if (c < 32 || c == 127 || c == 255)
			c = '~'; // Manage unprintable chars
		fprintf(log_file, "%c", c);
		if (c == ',') {
			comma = i;
			dtc = 1;
		}
		if (dtc && (buf[i] == 0)) {
			data = (i + 4) & ~3;
			for (dtc = i + 1; dtc < data; dtc++) {
				if (dtc < len) {
					fprintf(log_file, "~");
				}
			}
			dtc = 0;
			l = data;
			while (++comma < l && data < len) {
				switch (buf[comma]) {
				case 's':
					k = (strlen((char*) (buf + data)) + 4) & ~3;
					for (j = 0; j < k; j++) {
						if (data < len) {
							c = (unsigned char) buf[data++];
							if (c < 32 || c == 127 || c == 255)
								c = '~'; // Manage unprintable chars
							fprintf(log_file, "%c", c);
						}
					}
					break;
				case 'i':
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					fprintf(log_file, "[%6d]", endian.ii);
					break;
				case 'f':
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					if (endian.ff < 10.)
						fprintf(log_file, "[%06.4f]", endian.ff);
					else if (endian.ff < 100.)
						fprintf(log_file, "[%06.3f]", endian.ff);
					else if (endian.ff < 1000.)
						fprintf(log_file, "[%06.2f]", endian.ff);
					else if (endian.ff < 10000.)
						fprintf(log_file, "[%06.1f]", endian.ff);
					break;
				case 'b':
					// Get the number of bytes
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					n = endian.ii;
					// Get the number of data (floats or ints ???) in little-endian format
					for (k = 0; k < 4; endian.cc[k++] = buf[data++]);
					if (n == endian.ii) {
						// Display blob as string
						fprintf(log_file, "%d chrs: ", n);
						for (j = 0; j < n; j++) fprintf(log_file, "%c ", buf[data++]);
					} else {
						// Display blob as floats
						n = endian.ii;
						fprintf(log_file, "%d flts: ", n);
						for (j = 0; j < n; j++) {
							//floats are little-endian format
							for (k = 0; k < 4; endian.cc[k++] = buf[data++]);
							fprintf(log_file, "%06.2f ", endian.ff);
						}
					}
					break;
				default:
					break;
				}
			}
			i = data - 1;
		}
	}
	fprintf(log_file, "\n");
}
//
//
void XwritePresetFile(FILE* pre_file) {
	int i;
//
	fprintf(pre_file, "%d %d %d %d %d %d %d %d\n", Xtransport_on, Xchbank_on, XMkerbt_on, XbankCcol, Xeqcmp_on, Xmaster_on, REQindex, RCindex);
	fprintf(pre_file, "%d %d %d %d %d %d %d %d %d %d %d\n",
		Xtrk_min, Xtrk_max, Xaux_min, Xaux_max,
		Xfxr_min, Xfxr_max, Xbus_min, Xbus_max,
		Xdca_min, Xdca_max, TrackSendOffset);
	for (i = 0; i < 8; i++) fprintf(pre_file, "%d %d\n", Rdca_min[i], Rdca_max[i]);
	fprintf(pre_file, "%d %d %d %d %d\n", XMbankup, XMbankdn, XMkerbtn, 0, bkchsz_save);
}
//
//
int XreadPresetFile(FILE* pre_file) {
	int i;
//
	if (fgets(Rb_s, BSIZE, pre_file) != 0) {
		sscanf(Rb_s, "%d %d %d %d %d %d %d %d", &Xtransport_on, &Xchbank_on, &XMkerbt_on, &XbankCcol, &Xeqcmp_on, &Xmaster_on, &REQindex, &RCindex);
		fscanf(pre_file, "%d %d %d %d %d %d %d %d %d %d %d\n",
			&Xtrk_min, &Xtrk_max, &Xaux_min, &Xaux_max,
			&Xfxr_min, &Xfxr_max, &Xbus_min, &Xbus_max,
			&Xdca_min, &Xdca_max, &TrackSendOffset);
		for (i = 0; i < 8; i++) fscanf(pre_file, "%d %d\n", &Rdca_min[i], &Rdca_max[i]);
		fscanf(pre_file, "%d %d %d %d %d\n", &XMbankup, &XMbankdn, &XMkerbtn, &Xchbkof, &bkchsz_save);
		if (Xchbank_on == 0) bkchsz = BNKSZ;
		else                 bkchsz = bkchsz_save;
		return 1;
	}
	return 0;
}
//
//
void XGetPanelData() {
	// Read  and save panel data
	GetWindowText(hwndX32IP, S_X32_IP, GetWindowTextLength(hwndX32IP) + 1);
	GetWindowText(hwndHstIP, S_Hst_IP, GetWindowTextLength(hwndHstIP) + 1);
	GetWindowText(hwndSndPort, S_SndPort, GetWindowTextLength(hwndSndPort) + 1);
	GetWindowText(hwndRecPort, S_RecPort, GetWindowTextLength(hwndRecPort) + 1);
	XGetPresetData();
	return;
}
//
//
void XGetPresetData() {
	int i;
	// Read  and set Preset data
	GetWindowText(hwntrkmin, S_TMIN_str, GetWindowTextLength(hwntrkmin) + 1);
	sscanf(S_TMIN_str, "%d", &Xtrk_min);
	GetWindowText(hwntrkmax, S_TMAX_str, GetWindowTextLength(hwntrkmax) + 1);
	sscanf(S_TMAX_str, "%d", &Xtrk_max);
	//
	GetWindowText(hwnauxmin, S_AMIN_str, GetWindowTextLength(hwnauxmin) + 1);
	sscanf(S_AMIN_str, "%d", &Xaux_min);
	GetWindowText(hwnauxmax, S_AMAX_str, GetWindowTextLength(hwnauxmax) + 1);
	sscanf(S_AMAX_str, "%d", &Xaux_max);
	//
	GetWindowText(hwnfxrmin, S_FMIN_str, GetWindowTextLength(hwnfxrmin) + 1);
	sscanf(S_FMIN_str, "%d", &Xfxr_min);
	GetWindowText(hwnfxrmax, S_FMAX_str, GetWindowTextLength(hwnfxrmax) + 1);
	sscanf(S_FMAX_str, "%d", &Xfxr_max);
	//
	GetWindowText(hwnbusmin, S_BMIN_str, GetWindowTextLength(hwnbusmin) + 1);
	sscanf(S_BMIN_str, "%d", &Xbus_min);
	GetWindowText(hwnbusmax, S_BMAX_str, GetWindowTextLength(hwnbusmax) + 1);
	sscanf(S_BMAX_str, "%d", &Xbus_max);
	GetWindowText(hwnbusoff, S_BOFF_str, GetWindowTextLength(hwnbusoff) + 1);
	sscanf(S_BOFF_str, "%d", &TrackSendOffset);
	//
	GetWindowText(hwndcamin, S_DMIN_str, GetWindowTextLength(hwndcamin) + 1);
	sscanf(S_DMIN_str, "%d", &Xdca_min);
	GetWindowText(hwndcamax, S_DMAX_str, GetWindowTextLength(hwndcamax) + 1);
	sscanf(S_DMAX_str, "%d", &Xdca_max);
	//
	GetWindowText(hwnbksoff, S_BKOF_str, GetWindowTextLength(hwnbksoff) + 1);
	sscanf(S_BKOF_str, "%d", &Xchbkof);
	// validate the value of Xchbkof. Force to 0 in case of out-of-range data
	if ((Xchbkof > (Xtrk_max - 1) / bkchsz) || (Xchbkof < 0)) {
		Xchbkof = 0;
		sprintf(S_BKOF_str, "%d", Xchbkof);
		SetWindowText(hwnbksoff, (LPSTR) S_BKOF_str);
	}
	//
	GetWindowText(hwnbkmkbt, S_MKBT_str, GetWindowTextLength(hwnbkmkbt) + 1);
	sscanf(S_MKBT_str, "%d", &XMkerbtn);
	//
	GetWindowText(hwnbkupbt, S_BKUP_str, GetWindowTextLength(hwnbkupbt) + 1);
	sscanf(S_BKUP_str, "%d", &XMbankup);
	GetWindowText(hwnbkdnbt, S_BKDN_str, GetWindowTextLength(hwnbkdnbt) + 1);
	sscanf(S_BKDN_str, "%d", &XMbankdn);
	//
	for (i = 0; i < 8; i++) {
		GetWindowText(hwnrcamin[i], R_DMIN_str[i], GetWindowTextLength(hwnrcamin[i]) + 1);
		sscanf(R_DMIN_str[i], "%d", &Rdca_min[i]);
		GetWindowText(hwnrcamax[i], R_DMAX_str[i], GetWindowTextLength(hwnrcamax[i]) + 1);
		sscanf(R_DMAX_str[i], "%d", &Rdca_max[i]);
	}
	return;
}
//
//
void XSetPresetData() {
	int i;
	//
	sprintf(S_TMIN_str, "%d", Xtrk_min);
	SetWindowText(hwntrkmin, (LPSTR) S_TMIN_str);
	sprintf(S_TMAX_str, "%d", Xtrk_max);
	SetWindowText(hwntrkmax, (LPSTR) S_TMAX_str);
	//
	sprintf(S_AMIN_str, "%d", Xaux_min);
	SetWindowText(hwnauxmin, (LPSTR) S_AMIN_str);
	sprintf(S_AMAX_str, "%d", Xaux_max);
	SetWindowText(hwnauxmax, (LPSTR) S_AMAX_str);
	//
	sprintf(S_FMIN_str, "%d", Xfxr_min);
	SetWindowText(hwnfxrmin, (LPSTR) S_FMIN_str);
	sprintf(S_FMAX_str, "%d", Xfxr_max);
	SetWindowText(hwnfxrmax, (LPSTR) S_FMAX_str);
	//
	sprintf(S_BMIN_str, "%d", Xbus_min);
	SetWindowText(hwnbusmin, (LPSTR) S_BMIN_str);
	sprintf(S_BMIN_str, "%d", Xbus_max);
	SetWindowText(hwnbusmax, (LPSTR) S_BMIN_str);
	sprintf(S_BOFF_str, "%d", TrackSendOffset);
	SetWindowText(hwnbusoff, (LPSTR) S_BOFF_str);
	//
	sprintf(S_DMIN_str, "%d", Xdca_min);
	SetWindowText(hwndcamin, (LPSTR) S_DMIN_str);
	sprintf(S_DMAX_str, "%d", Xdca_max);
	SetWindowText(hwndcamax, (LPSTR) S_DMAX_str);
	//
	sprintf(S_BKOF_str, "%d", Xchbkof);
	SetWindowText(hwnbksoff, (LPSTR) S_BKOF_str);
	//
	sprintf(S_MKBT_str, "%d", XMkerbtn);
	SetWindowText(hwnbkmkbt, (LPSTR) S_MKBT_str);
	//
	sprintf(S_BKUP_str, "%d", XMbankup);
	SetWindowText(hwnbkupbt, (LPSTR) S_BKUP_str);
	sprintf(S_BKDN_str, "%d", XMbankdn);
	SetWindowText(hwnbkdnbt, (LPSTR) S_BKDN_str);
	//
	for (i = 0; i < 8; i++) {
		sprintf(R_DMIN_str[i], "%d", Rdca_min[i]);
		SetWindowText(hwnrcamin[i], (LPSTR) R_DMIN_str[i]);
		sprintf(R_DMAX_str[i], "%d", Rdca_max[i]);
		SetWindowText(hwnrcamax[i], (LPSTR) R_DMAX_str[i]);
	}
	// re-allocate memory for REAPER tracks bank data
	// WE make the choice to allocate by blocks of bkchsz, ensuring we cover all input tracks
	// between Xtrk_max and Xtrk_min
	XManageTransport();
	XManageSetmk();
	XManageChbank();
	SendMessage(hwneqctrl, BM_SETCHECK, Xeqcmp_on? BST_CHECKED: BST_UNCHECKED, 0);
	if (Xconnected) X32UsrCtrlC();
	return;
}
//
//
void XManageTransport() {
	SendMessage(hwndtprt, BM_SETCHECK, Xtransport_on? BST_CHECKED: BST_UNCHECKED, 0);
}
//
//
void XManageSetmk() {
	SendMessage(hwndsetm, BM_SETCHECK, XMkerbt_on? BST_CHECKED: BST_UNCHECKED, 0);
	GetWindowText(hwnbkmkbt, S_MKBT_str, GetWindowTextLength(hwnbkmkbt) + 1);
	sscanf(S_MKBT_str, "%d", &XMkerbtn);
	//
	GetWindowText(hwnbkupbt, S_BKUP_str, GetWindowTextLength(hwnbkupbt) + 1);
	sscanf(S_BKUP_str, "%d", &XMbankup);
	GetWindowText(hwnbkdnbt, S_BKDN_str, GetWindowTextLength(hwnbkdnbt) + 1);
	sscanf(S_BKDN_str, "%d", &XMbankdn);
	if ((XMkerbtn == XMbankup) || (XMkerbtn == XMbankdn)) {
		MessageBox(NULL, "Btn # UP and DN must differ\nfrom Marker Insert Btn #", NULL, MB_OKCANCEL);
	}
}
//
//
void XManageChbank() {
	//
	SendMessage(hwndchbk, BM_SETCHECK, Xchbank_on? BST_CHECKED: BST_UNCHECKED, 0);
	XAllocChbank();
}
//
//
void XAllocChbank() {
	int i, j;
	//
	if (Xchbank_on) {

		//
		// allocate memory for maintaining REAPER data between banks
		// WE make the choice to allocate by blocks of bkchsz, ensuring we cover all input tracks
		// between Xtrk_max and Xtrk_min
		bkchsz = bkchsz_save;
		if (XMbanktracks) free(XMbanktracks);
		if ((XMbanktracks = (S_bkch*)malloc(((Xtrk_max - Xtrk_min + 1 + bkchsz - 1) / bkchsz) * bkchsz * sizeof(S_bkch))) == NULL) exit(-1);
		//
		// make sure it's all clean.
		for (i = 0; i < Xtrk_max - Xtrk_min + 1; i++) {
			XMbanktracks[i].fader = 0.0;
			XMbanktracks[i].pan = 0.5;
			XMbanktracks[i].mute = 0.0;
			XMbanktracks[i].solo = 0.0;
			XMbanktracks[i].color = 0;
			XMbanktracks[i].icon = 1;
			for (j = 0; j < 16; j++) {
				XMbanktracks[i].scribble[j] = 0;				// scribbles are 16 chars
				XMbanktracks[i].mixbus[j] = 0.0;				// 16 mixbus per track
			}
			if (Xeqcmp_on) {
				XMbanktracks[i].eqon = 0;
				XMbanktracks[i].cmpon = 0;
				for (j = 0; j < 16; j++) {
					XMbanktracks[i].eq[j] = 0.0;		// 16 eq params per track
					XMbanktracks[i].cmp[j] = 0.0;		// 16 cmp params per track
				}
				//eq defaults
				XMbanktracks[i].eq[1] = 0.264131;		// 120Hz
				XMbanktracks[i].eq[2] = 0.5;			// 0dB
				XMbanktracks[i].eq[3] = 0.5;			// Q=2
				XMbanktracks[i].eq[4] = 0.464017;		// 496Hz
				XMbanktracks[i].eq[5] = 0.5;			// 0dB
				XMbanktracks[i].eq[6] = 0.5;			// Q=2
				XMbanktracks[i].eq[7] = 0.664479;		// 1970Hz
				XMbanktracks[i].eq[8] = 0.5;			// 0dB
				XMbanktracks[i].eq[9] = 0.5;			// Q=2
				XMbanktracks[i].eq[10] = 0.899657;		// 10kHz
				XMbanktracks[i].eq[11] = 0.5;			// 0dB
				XMbanktracks[i].eq[12] = 0.5;			// Q=2
				//comp defaults
				XMbanktracks[i].cmp[1] = 1.0;			// default threshold (0dB)
				XMbanktracks[i].cmp[3] = 0.025119;		// default attack (3ms)
				XMbanktracks[i].cmp[4] = 0.447422;		// default release (100ms)
				XMbanktracks[i].cmp[12] = 1.0;			// default mix/wet (0dB)
			}
		}
	} else {
		if (XMbanktracks) free(XMbanktracks);
		XMbanktracks = NULL;
		bkchsz = BNKSZ;
	}
}
//
// XUpdateBkCh():
// This function is called only when switching REAPER channel banks. It maps the values stored
// in the XMbanktracks structure array to the X32 channels 1 to 32(max) to reflect the values of the
// selected REAPER bank of 32(max) tracks
void XUpdateBkCh() {
	int i, j, src;
	char tmp[64];
	//
	// manage channel select, only for strips 1...32
	Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40297"); // unselect all REAPER tracks
	SEND_TOR()
	if (Xselected < bkchsz && Xtrk_max > 0) {
//		i = Xselected;
//		Xb_ls = Xfprint(Xb_s, 0, "/-stat/selidx", 'i', &i);	//set X32 selected channel
//		SEND_TOX(Xdelayg)
		Rselected = Xselected + Xchbkof * bkchsz + Xtrk_min;
	} else if (Xselected < 32) {
		Rselected = Xselected + Xtrk_min;
	} else if (Xselected < 40) {
		// Auxin section selected
		Rselected = Xselected - 32 + Xaux_min;
	} else if (Xselected < 48) {
		// Fxrtn section selected
		Rselected = Xselected - 40 + Xfxr_min;
	} else if (Xselected < 64) {
		// Mixbus section selected
		Rselected = Xselected - 48 + Xbus_min;
	}
	sprintf(tmp, "/track/%d/select", Rselected);
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &fone);	// REAPER track select
	SEND_TOR()
	//
	sprintf(S_BKOF_str, "%d", Xchbkof);
	SetWindowText(hwnbksoff, (LPSTR)S_BKOF_str);
	for (i = 1; i < bkchsz+1; i++) {
		sprintf(tmp, "/ch/%02d/", i);
// update the bkchsz channels of X32 upon REAPER bank change requested from X32
		src = i - 1 + Xchbkof * bkchsz;	// XMbanktracks index start at 0,channel and tracks start at index 1
		strcpy(tmp + 7, "mix/fader");
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].fader);
//		SEND_TOX(Xdelayb)
		SEND_TOX(0)
		//
		strcpy(tmp + 11, "pan");	// pan
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].pan);
//		SEND_TOX(Xdelayb)
		SEND_TOX(0)		//
		strcpy(tmp + 11, "on");		// mute
		j = 1;
		if (XMbanktracks[src].mute > 0.5) j = 0;
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &j);
//		SEND_TOX(Xdelayb)
		SEND_TOX(0)		//
		strcpy(tmp + 11, "00/level");			// sends
		for (j = 1; j < 17; j++) {				// 16 mixbus
			tmp[11] = j / 10 + '0';
			tmp[12] = j - ((j / 10) * 10) + '0';
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].mixbus[j - 1]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)		}
		//
		strcpy(tmp + 7, "config/name");// scribble names
		Xb_ls = Xfprint(Xb_s, 0, tmp, 's', XMbanktracks[src].scribble);
		SEND_TOX(Xdelayb)
		//
		strcpy(tmp + 14, "color");// scribble colors
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &XMbanktracks[src].color);
//		SEND_TOX(Xdelayb)
		SEND_TOX(0)		//
		strcpy(tmp + 14, "icon");// scribble icons
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &XMbanktracks[src].icon);
//		SEND_TOX(Xdelayb)
		SEND_TOX(0)
		//
		if (Xeqcmp_on) {
			strcpy(tmp + 7, "eq/1/f");	// eq/1/f
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[0]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			tmp[12] = 'g';	// eq/1/g
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[1]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			tmp[12] = 'q';	// eq/1/q
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[2]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)

			strcpy(tmp + 10, "2/f");	// eq/2/f
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[3]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			tmp[12] = 'g';	// eq/2/g
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[4]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			tmp[12] = 'q';	// eq/2/q
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[5]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)

			strcpy(tmp + 10, "3/f");	// eq/3/f
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[6]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			tmp[12] = 'g';	// eq/3/g
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[7]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			tmp[12] = 'q';	// eq/3/q
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[8]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)

			strcpy(tmp + 10, "4/f");	// eq/4/f
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[9]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			tmp[12] = 'g';	// eq/4/g
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[10]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			tmp[12] = 'q';	// eq/4/q
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].eq[11]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)

			strcpy(tmp + 7, "eq/on");	// eq/on
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &XMbanktracks[src].eqon);
			SEND_TOX(Xdelayb)


			strcpy(tmp + 7, "dyn/thr");	// dyn/thr
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].cmp[1]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			strcpy(tmp + 7, "dyn/ratio");	// dyn/ratio
			endian.ff = XMbanktracks[src].cmp[2];
			if (endian.ff < .003) endian.ii = 0;
			else if(endian.ff < .003) endian.ii = 0;
			else if(endian.ff < .005) endian.ii = 1;
			else if(endian.ff < .0101) endian.ii = 2;
			else if(endian.ff < .0151) endian.ii = 3;
			else if(endian.ff < .0202) endian.ii = 4;
			else if(endian.ff < .0303) endian.ii = 5;
			else if(endian.ff < .0404) endian.ii = 6;
			else if(endian.ff < .0606) endian.ii = 7;
			else if(endian.ff < .0909) endian.ii = 8;
			else if(endian.ff < .1919) endian.ii = 9;
			else if(endian.ff < .90) endian.ii = 10;
			else endian.ii = 11;
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', endian.cc);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			strcpy(tmp + 7, "dyn/attack");	// dyn/attack
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].cmp[3]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			strcpy(tmp + 7, "dyn/release");	// dyn/release
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].cmp[4]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
			strcpy(tmp + 7, "dyn/mix");	// dyn/mix
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &XMbanktracks[src].cmp[12]);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)

			strcpy(tmp + 7, "dyn/on");	// dyn/on
			Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &XMbanktracks[src].cmpon);
//			SEND_TOX(Xdelayb)
			SEND_TOX(0)
		}
		//
		j = 0;
		if (XMbanktracks[src].solo > 0.5) j = 1;
		sprintf(tmp, "/-stat/solosw/%02d", i);	// solo
		Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &j);
		SEND_TOX(Xdelayg)
	}
}
//
// This function initializes User Assign section C either with all transport options, or
// if channel banks are enabled, it sets two of the transport buttons to manage up/down.
// If transport is off and channel banks are enable, two user chosen buttons are used
// from User Assign section C to manage bank up/down
void X32UsrCtrlC() {
	int i;
	char* MP[4] = { "MP13000", "MP14000", "MP15000", "MP16000" };
	char* MN[8] = { "MN16000", "MN16001", "MN16002", "MN16003",
					"MN16004", "MN16005", "MN16006", "MN16007" };
	//
	if (Xtransport_on) {
		//
		// Encoders
		for (i = 1; i < 5; i++) {
			sprintf(Xb_r, "/config/userctrl/C/enc/%d", i);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 's', MP[i - 1]);
			SEND_TOX(Xdelayg)
		}
		//
		// Buttons
		for (i = 5; i < 13; i++) {
			sprintf(Xb_r, "/config/userctrl/C/btn/%d", i);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 's', MN[i - 5]);
			SEND_TOX(Xdelayg)
		}
		//
		// Set X32 Bank C Encoders  to center "64" value
		for (i = 33; i < 37; i++) {
			sprintf(Xb_r, "/-stat/userpar/%2d/value", i);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 'i', &six4);
			SEND_TOX(Xdelayg)
		}
		//
		// Set X32 Bank C buttons  to "0" value
		for (i = 17; i < 25; i++) {
			sprintf(Xb_r, "/-stat/userpar/%2d/value", i);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 'i', &zero);
			SEND_TOX(Xdelayg)
		}
	} else {
		if (XMkerbt_on) {
			//
			// update/change REAPER Marker button.
			//
			sprintf(Xb_r, "/config/userctrl/C/btn/%d", XMkerbtn);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 's', MN[XMkerbtn - 5]);
			SEND_TOX(Xdelayg)
			sprintf(Xb_r, "/-stat/userpar/%2d/value", 12 + XMkerbtn);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 'i', &zero);
			SEND_TOX(Xdelayg)
		}
		//
		if(Xchbank_on) {
			//
			// Only update/change bank up and bank down buttons.
			//
			// bank up
			sprintf(Xb_r, "/config/userctrl/C/btn/%d", XMbankup);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 's', MN[XMbankup - 5]);
			SEND_TOX(Xdelayg)
			sprintf(Xb_r, "/-stat/userpar/%2d/value", 12 + XMbankup);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 'i', &zero);
			SEND_TOX(Xdelayg)
			//
			// bank down
			sprintf(Xb_r, "/config/userctrl/C/btn/%d", XMbankdn);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 's', MN[XMbankdn - 5]);
			SEND_TOX(Xdelayg)
			sprintf(Xb_r, "/-stat/userpar/%2d/value", 12 + XMbankdn);
			Xb_ls = Xfprint(Xb_s, 0, Xb_r, 'i', &zero);
			SEND_TOX(Xdelayg)
		}
	}
	if (Xtransport_on || XMkerbt_on || Xchbank_on) {
		// Color : XbankCcol
		Xb_ls = Xfprint(Xb_s, 0, "/config/userctrl/C/color", 'i', &XbankCcol);
		SEND_TOX(Xdelayg)
		// Select X32 UserAssign Bank C
		Xb_ls = Xfprint(Xb_s, 0, "/-stat/userbank", 'i', &two);
		SEND_TOX(Xdelayg)
	}
	//
	// Finally, if we're connected and the CH bank flag is set,
	// set current bank values to X32
	if (Xchbank_on) XUpdateBkCh();
	return;
}
// Manage X32 ...eq/on messages
void X32_eqon(int Xb_i, int bank, int cnum, int cnum1) {
	int i;
	char tmp[48];
	//
	Xb_i += 5; //skip "/eq/on" string
	//	/xxx/%02d/eq/on..,i..[0|1]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	sprintf(tmp, "/track/%d/fx/%1d/bypass", cnum1, REQindex);
	if (bank) XMbanktracks[cnum - 1].eqon = endian.ii;
	endian.ff = (float)endian.ii;
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
//
// Manage X32 ...eq/x/f messages
void X32_eqfr(int Xb_i, int bank, int cnum, int cnum1) {
	int i, j;
	char tmp[64];
	// get eq#
	j = (int)(Xb_r[Xb_i + 3] - '1') * 3;
	Xb_i += 5; //skip "/eq/x/f" string
	//	/ch/%02d/eq/*/f..,f..[float]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	// get Behringer freq value
	endian.ff = exp(endian.ff*6.907755279 + 2.995732274);
	// set REAPER float value
	endian.ff = log((endian.ff - 20.)*400./(23980.) + 1) / 5.991464547;
	sprintf(tmp, "/track/%d/fx/%1d/fxparam/%d/value", cnum1, REQindex, j+1);
	if (bank) XMbanktracks[cnum - 1].eq[j] = endian.ff;
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
//
// Manage X32 ...eq/x/g or ...eq/x/q messages
void X32_eqgq(int Xb_i, int bank, int cnum, int cnum1, int index) {
	int i, j;
	char tmp[64];
	// get eq#
	j = (int)(Xb_r[Xb_i + 3] - '1') * 3;
	Xb_i += 5; //skip "/eq/x/g" string
	//	/.../%02d/eq/*/g|q..,f..[float]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	sprintf(tmp, "/track/%d/fx/%1d/fxparam/%d/value", cnum1, REQindex, j + index);
	if (bank) XMbanktracks[cnum - 1].eq[j] = endian.ff;
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
// Manage X32 ...dyn/on messages
void X32_dynon(int Xb_i, int bank, int cnum, int cnum1) {
	int i;
	char tmp[48];
	//
	Xb_i += 5; //skip "/dyn/on" string
	//	/xxx/%02d/dyn/on..,i..[0|1]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	sprintf(tmp, "/track/%d/fx/%1d/bypass", cnum1, RCindex);
	if (bank) XMbanktracks[cnum - 1].cmpon = endian.ii;
	endian.ff = (float)endian.ii;
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
// Manage X32 ...dyn/xxx messages for floats other than dyn/on
void X32_dyn(int Xb_i, int bank, int cnum, int cnum1, int index) {
	int i;
	char tmp[48];
	//
	Xb_i += 5; //skip "/dyn/xxx" string
	//	/xxx/%02d/dyn/xxx..,f..[float]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	sprintf(tmp, "/track/%d/fx/%1d/fxparam/%d/value", cnum1, RCindex, index);
	//printf("valuepre: %f\n", endian.ff);
	endian.ff = exp((endian.ff * (-0.22314 + 6.90775)) -  6.90775);
	if (bank) XMbanktracks[cnum - 1].cmp[index] = endian.ff;
	//printf("valuepost: %f\n", endian.ff);
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
// Manage X32 ...dyn/thr messages for floats
void X32_dynthr(int Xb_i, int bank, int cnum, int cnum1, int index) {
	int i;
	char tmp[48];
	//
	Xb_i += 5; //skip "/dyn/thr" string
	//	/xxx/%02d/dyn/thr..,f..[float]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	sprintf(tmp, "/track/%d/fx/%1d/fxparam/%d/value", cnum1, RCindex, index);
	//printf("valuepre: %f\n", endian.ff);
	endian.ff = exp((1. - endian.ff) * (-7.600903 + 0.693147) - 0.693147);
	if (bank) XMbanktracks[cnum - 1].cmp[index] = endian.ff;
	//printf("valuepost: %f\n", endian.ff);
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
// Manage X32 ...dyn/attack messages for floats
void X32_dynattack(int Xb_i, int bank, int cnum, int cnum1, int index) {
	int i;
	char tmp[48];
	//
	Xb_i += 5; //skip "/dyn/attack" string
	//	/xxx/%02d/dyn/attack..,f..[float]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	sprintf(tmp, "/track/%d/fx/%1d/fxparam/%d/value", cnum1, RCindex, index);
	//printf("valuepre: %f\n", endian.ff);
	endian.ff = endian.ff * 0.24;
	if (bank) XMbanktracks[cnum - 1].cmp[index] = endian.ff;
	//printf("valuepost: %f\n", endian.ff);
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
// Manage X32 ...dyn/ratio messages
void X32_dynratio(int Xb_i, int bank, int cnum, int cnum1, int index) {
	int i;
	char tmp[48];
	//
	Xb_i += 5; //skip "/dyn/ratio" string
	//	/xxx/%02d/dyn/ratio..,i..[int]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	switch (endian.ii) {
		case 0:  endian.ff = 0.;		break;
		case 1:  endian.ff = .003;		break;
		case 2:  endian.ff = .005;		break;
		case 3:  endian.ff = .0101;		break;
		case 4:  endian.ff = .0151;		break;
		case 5:  endian.ff = .0202;		break;
		case 6:  endian.ff = .0303;		break;
		case 7:  endian.ff = .0404;		break;
		case 8:  endian.ff = .0606;		break;
		case 9:  endian.ff = .0909;		break;
		case 10: endian.ff = .1919;		break;
		case 11: endian.ff = 1.;		break;
		default:						break;
	}
	sprintf(tmp, "/track/%d/fx/%1d/fxparam/%d/value", cnum1, RCindex, index);
	if (bank) XMbanktracks[cnum - 1].cmp[index] = endian.ff;
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
// Manage X32 ...dyn/release messages
void X32_dynrelease(int Xb_i, int bank, int cnum, int cnum1, int index) {
	int i;
	char tmp[48];
	//
	Xb_i += 5; //skip "/dyn/release" string
	//	/xxx/%02d/dyn/release..,f..[float]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	sprintf(tmp, "/track/%d/fx/%1d/fxparam/%d/value", cnum1, RCindex, index);
	//printf("valuepre: %f\n", endian.ff);
	endian.ff = exp((endian.ff * (-0.22314 + 6.90775)) -  6.90775);
	if (bank) XMbanktracks[cnum - 1].cmp[index] = endian.ff;
	//printf("valuepost: %f\n", endian.ff);
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
// Manage X32 ...dyn/release messages
void X32_dynmix(int Xb_i, int bank, int cnum, int cnum1, int index) {
	int i;
	char tmp[48];
	//
	Xb_i += 5; //skip "/dyn/mix" string
	//	/xxx/%02d/dyn/mix..,f..[float]
	XRmask = X32FX;
	while (Xb_r[Xb_i] != ',') Xb_i += 1;
	Xb_i += 4;
	for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
	sprintf(tmp, "/track/%d/fx/%1d/fxparam/%d/value", cnum1, RCindex, index);
	//printf("valuepre: %f\n", endian.ff);
	endian.ff = exp((endian.ff * (11.512925 - 0.693127)) - 11.512925) - .00001;
	if (bank) XMbanktracks[cnum - 1].cmp[index] = endian.ff;
	//printf("valuepost: %f\n", endian.ff);
	Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
	return;
}
//--------------------------------------------------------------------
// X32 Messages data parsing
//
// Analysis of incoming X32 data, and sending respective commands to REAPER in order
// to synchronize X32 and REAPER. Data controlled includes fader, pan, mute, solo,
// select, scribble, icon, color, bus sends fader and pan.
//
void X32ParseX32Message() {
	int Xb_i = 0;
	int Xb_ls = 0;
	int cnum, bus, dca, i;
	int cnum1;
	char tmp[32];
//
// What is the X32 message made of?
// X32 format is:
// /ch/%02d/mix/pan......,f..[float] %02d = 01..bkchsz
// /ch/%02d/mix/fader....,f..[float] %02d = 01..bkchsz
// /ch/%02d/mix/on.......,i..[0/1] %02d = 01..bkchsz
// /ch/%02d/config/name..,s..[string\0] %02d = 01..bkchsz
// /ch/%02d/mix/%02d/level...,f..[float] %02d = 01..bkchsz / %02d = 01..16
// /ch/%02d/eq/on........,i..[0/1] %02d = 01..bkchsz
// /ch/%02d/eq/x/f.......,f..[float] %02d = 01..bkchsz, x:1..4
// /ch/%02d/eq/x/g.......,f..[float] %02d = 01..bkchsz, x:1..4
// /ch/%02d/eq/x/q.......,f..[float] %02d = 01..bkchsz, x:1..4
// /ch/%02d/dyn/on.......,i..[0/1] %02d = 01..bkchsz
// /ch/%02d/dyn/thr......,f..[float] %02d = 01..bkchsz
// /ch/%02d/dyn/ratio....,i..[0..11] %02d = 01..bkchsz
// /ch/%02d/dyn/attack...,f..[float] %02d = 01..bkchsz
// /ch/%02d/dyn/release..,f..[float] %02d = 01..bkchsz
// /ch/%02d/dyn/mix......,f..[float] %02d = 01..bkchsz
//
// Same applies to /auxin and /fxrtn as for /ch above
//
// /-stat/selidx.........,i..[%d]
// /-stat/solosw/%02d....,i..[0/1]
//
// /main/st/mix/fader....,f..[float]
// /main/st/mix/pan......,f..[float]
//
// /bus/%02d/mix/pan......,f..[float] %02d = 01..16
// /bus/%02d/mix/fader....,f..[float] %02d = 01..16
// /bus/%02d/mix/on.......,i..[0/1] %02d = 01..16
// /bus/%02d/config/name..,s..[string\0] %02d = 01..16
// /bus/%02d/dyn/on.......,i..[0/1] %02d = 01..16
// /bus/%02d/dyn/thr......,f..[float] %02d = 01..16
// /bus/%02d/dyn/ratio....,i..[0..11] %02d = 01..16
// /bus/%02d/dyn/attack...,f..[float] %02d = 01..16
// /bus/%02d/dyn/release..,f..[float] %02d = 01..16
// /bus/%02d/dyn/mix......,f..[float] %02d = 01..16
//
// /dca/1..8/on...........,i..[0/1]
// /dca/1..8/fader........,f..[0..1.0]
// /dca/1..8/config/name..,s..[string\0]
//
// if transport is on:
// Bank C Actuators
// 33 34 35 36
//   Beat   Measure  Marker  Item
//
// Bank C Buttons
// /-stat/userpar/%2d/value~,i~~[0 or 127] %2d = 17..24
//
// 17 18 19 20
// REW PLAY PAUSE FF
//
// 21 22 23 24
// S/S loop  Repeat  STOP    REC
//
//   or if chbank is on:
//
// 21       22       23 24
// Bank UP  Bank Down  STOP  REC
//
// or if transport is OFF and chbank is on:
// 2 buttons chosen by the user
//
// if transport is OFF : 1 button for REAPER Marker setting, chosen by the user
//
	Rb_ls = 0;
	if (strncmp(Xb_r, "/ch/", 4) == 0) {
		// /ch/ cases : get channel number
		Xb_i = 4;
		cnum = (int) Xb_r[Xb_i++] - (int) '0';
		cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
		//
		// manage bank offset if the user selected that option
		if (cnum <= bkchsz) {
			if (Xchbank_on) {
				// Set actual channel number to match Channel Bank
				cnum = Xchbkof * bkchsz + cnum;
			}
			cnum1 = cnum + Xtrk_min - 1;
			if ((Xtrk_max > 0) && (cnum1 <= Xtrk_max)) {
				Xb_i += 1; // skip '/'
				if (Xb_r[Xb_i] == 'm') {
					Xb_i += 4; //skip "/mix/" string
					if (Xb_r[Xb_i] == 'p') {
						// /ch/%02d/mix/pan......,f..[float]
						XRmask = X32PAN;
						while (Xb_r[Xb_i] != ',') Xb_i += 1;
						Xb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
						sprintf(tmp, "/track/%d/pan", cnum1);
						if (Xchbank_on)
							XMbanktracks[cnum].pan = endian.ff;
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					} else if (Xb_r[Xb_i] == 'f') {
						// /ch/%02d/mix/fader....,f..[float]
						XRmask = X32FADER;
						while (Xb_r[Xb_i] != ',') Xb_i += 1;
						Xb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
						sprintf(tmp, "/track/%d/volume", cnum1);
						if (Xchbank_on) XMbanktracks[cnum - 1].fader = endian.ff;
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					} else if (Xb_r[Xb_i] == 'o') {
						// /ch/%02d/mix/on.......,i..[0/1]
						XRmask = X32MUTE;
						while (Xb_r[Xb_i] != ',') Xb_i += 1;
						Xb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
						if (endian.ii == 1) endian.ff = 0.0;
						else                endian.ff = 1.0;
						sprintf(tmp, "/track/%d/mute", cnum1);
						if (Xchbank_on) {
							XMbanktracks[cnum - 1].mute = endian.ff;
						}
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					} else if ((Xb_r[Xb_i] == '0') || (Xb_r[Xb_i] == '1')) {
						// "/mix/" is followed by a Bus send number
						// get bus number
						XRmask = X32SEND;
						bus = (int) Xb_r[Xb_i++] - (int) '0';
						bus = bus * 10 + (int) Xb_r[Xb_i++] - (int) '0';
						bus += TrackSendOffset;
						Xb_i += 1; // skip '/'
						if (Xb_r[Xb_i] == 'l') {
							// /ch/%02d/mix/%02d/level....,f..[float]
							while (Xb_r[Xb_i] != ',') Xb_i += 1;
							Xb_i += 4;
							for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
							sprintf(tmp, "/track/%d/send/%d/volume", cnum1, bus);
							if (Xchbank_on) XMbanktracks[cnum - 1].mixbus[bus - 1] = endian.ff;
							Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
						}
					}
				} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')) {
					Xb_i += 11; //skip "/config/name" string
					// /ch/%02d/config/name..,s..[string\0]
					XRmask = X32NAME;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					sprintf(tmp, "/track/%d/name", cnum);
					if (Xchbank_on) strncpy(XMbanktracks[cnum - 1].scribble, Xb_r + Xb_i, 12);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
					// /ch/%02d/eq/on........,i..[0/1] %02d = 01..bkchsz
					// /ch/%02d/eq/x/f.......,f..[float] %02d = 01..bkchsz, x:1..4
					// /ch/%02d/eq/x/g.......,f..[float] %02d = 01..bkchsz, x:1..4
					// /ch/%02d/eq/x/q.......,f..[float] %02d = 01..bkchsz, x:1..4
				} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 3] == 'o')) {
					X32_eqon(Xb_i, Xchbank_on, cnum, cnum1);
				} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'f')) {
					X32_eqfr(Xb_i, Xchbank_on, cnum, cnum1);
				} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'g')) {
					X32_eqgq(Xb_i, Xchbank_on, cnum, cnum1, 2);
				} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'q')) {
					X32_eqgq(Xb_i, Xchbank_on, cnum, cnum1, 3);
					// /ch/%02d/dyn/on.......,i..[0/1] %02d = 01..bkchsz
					// /ch/%02d/dyn/thr......,f..[float] %02d = 01..bkchsz
					// /ch/%02d/dyn/ratio....,i..[0..11] %02d = 01..bkchsz
					// /ch/%02d/dyn/attack...,f..[float] %02d = 01..bkchsz
					// /ch/%02d/dyn/release..,f..[float] %02d = 01..bkchsz
					// /ch/%02d/dyn/mix......,f..[float] %02d = 01..bkchsz
				} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'o')) {
					X32_dynon(Xb_i, Xchbank_on, cnum, cnum1);
				} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 't')) {
					X32_dynthr(Xb_i, Xchbank_on, cnum, cnum1, 1);
				} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'r') && (Xb_r[Xb_i + 5] == 'a')) {
					X32_dynratio(Xb_i, Xchbank_on, cnum, cnum1, 2);
				} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'a')) {
					X32_dynattack(Xb_i, Xchbank_on, cnum, cnum1, 3);
				} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'r') && (Xb_r[Xb_i + 5] == 'e')) {
					X32_dynrelease(Xb_i, Xchbank_on, cnum, cnum1, 4);
				} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'm')) {
					X32_dynmix(Xb_i, Xchbank_on, cnum, cnum1, 12);
				}
			}
		}
	} else if (strncmp(Xb_r, "/auxin/", 7) == 0) {
		// /auxin/ cases : get channel number
		Xb_i = 7;
		cnum = (int) Xb_r[Xb_i++] - (int) '0';
		cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
		cnum1 = cnum + Xaux_min - 1;
		if ((Xaux_max > 0) && (cnum1 <= Xaux_max)) {
			Xb_i += 1; // skip '/'
			if (Xb_r[Xb_i] == 'm') {
				Xb_i += 4; //skip "/mix/" string
				if (Xb_r[Xb_i] == 'p') {
					// /auxin/%02d/mix/pan......,f..[float]
					XRmask = X32PAN;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/pan", cnum1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'f') {
					// /aunxin/%02d/mix/fader....,f..[float]
					XRmask = X32FADER;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/volume", cnum1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'o') {
					// /auxin/%02d/mix/on.......,i..[0/1]
					XRmask = X32MUTE;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					if (endian.ii == 1) endian.ff = 0.0;
					else                endian.ff = 1.0;
					sprintf(tmp, "/track/%d/mute", cnum1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if ((Xb_r[Xb_i] == '0') || (Xb_r[Xb_i] == '1')) {
					// "/mix/" is followed by a Bus send number
					// get bus number
					XRmask = X32SEND;
					bus = (int) Xb_r[Xb_i++] - (int) '0';
					bus = bus * 10 + (int) Xb_r[Xb_i++] - (int) '0';
					bus += TrackSendOffset;
					Xb_i += 1; // skip '/'
					if (Xb_r[Xb_i] == 'l') {
						// /auxin/%02d/mix/%02d/level....,f..[float]
						while (Xb_r[Xb_i] != ',') Xb_i += 1;
						Xb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
						sprintf(tmp, "/track/%d/send/%d/volume", cnum1, bus);
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					}
				}
			} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')) {
				Xb_i += 11; //skip "/config/name" string
				// /auxin/%02d/config/name..,s..[string\0]
				XRmask = X32NAME;
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				sprintf(tmp, "/track/%d/name", cnum1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
			} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 3] == 'o')) {
				X32_eqon(Xb_i, 0, cnum, cnum1);
			} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'f')) {
				X32_eqfr(Xb_i, 0, cnum, cnum1);
			} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'g')) {
				X32_eqgq(Xb_i, 0, cnum, cnum1, 2);
			} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'q')) {
				X32_eqgq(Xb_i, 0, cnum, cnum1, 3);
			}
		}
	} else if (strncmp(Xb_r, "/fxrtn/", 7) == 0) {
		// /fxrtn/ cases : get channel number
		Xb_i = 7;
		cnum = (int) Xb_r[Xb_i++] - (int) '0';
		cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
		cnum1 = cnum + Xfxr_min - 1;
		if ((Xfxr_max > 0) && (cnum1 <= Xfxr_max)) {
			Xb_i += 1; // skip '/'
			if (Xb_r[Xb_i] == 'm') {
				Xb_i += 4; //skip "/mix/" string
				if (Xb_r[Xb_i] == 'p') {
					// /fxrtn/%02d/mix/pan......,f..[float]
					XRmask = X32PAN;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/pan", cnum1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'f') {
					// /fxrtn/%02d/mix/fader....,f..[float]
					XRmask = X32FADER;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/volume", cnum1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'o') {
					// /fxrtn/%02d/mix/on.......,i..[0/1]
					XRmask = X32MUTE;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					if (endian.ii == 1) endian.ff = 0.0;
					else                endian.ff = 1.0;
					sprintf(tmp, "/track/%d/mute", cnum1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if ((Xb_r[Xb_i] == '0') || (Xb_r[Xb_i] == '1')) {
					// "/mix/" is followed by a Bus send number
					// get bus number
					XRmask = X32SEND;
					bus = (int) Xb_r[Xb_i++] - (int) '0';
					bus = bus * 10 + (int) Xb_r[Xb_i++] - (int) '0';
					bus += TrackSendOffset;
					Xb_i += 1; // skip '/'
					if (Xb_r[Xb_i] == 'l') {
						// /fxrtn/%02d/mix/%02d/level....,f..[float]
						while (Xb_r[Xb_i] != ',') Xb_i += 1;
						Xb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
						sprintf(tmp, "/track/%d/send/%d/volume", cnum1, bus);
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					}
				}
			} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')) {
				Xb_i += 11; //skip "/config/name" string
				// /fxrtn/%02d/config/name..,s..[string\0]
				XRmask = X32NAME;
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				sprintf(tmp, "/track/%d/name", cnum1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
			} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 3] == 'o')) {
				X32_eqon(Xb_i, 0, cnum, cnum1);
			} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'f')) {
				X32_eqfr(Xb_i, 0, cnum, cnum1);
			} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'g')) {
				X32_eqgq(Xb_i, 0, cnum, cnum1, 2);
			} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'q')) {
				X32_eqgq(Xb_i, 0, cnum, cnum1, 3);
			}
		}
	} else if (strncmp(Xb_r, "/bus/", 5) == 0) {
		// /bus/%02d/mix/fader....,f..[float] %02d = 01..16
		// /bus/%02d/mix/on.......,i..[0/1] %02d = 01..16
		// /bus/%02d/config/name..,s..[string\0] %02d = 01..16
		// /bus/%02d/dyn/on.......,i..[0/1] %02d = 01..16
		// /bus/%02d/dyn/thr......,f..[float] %02d = 01..16
		// /bus/%02d/dyn/ratio....,i..[0..11] %02d = 01..16
		// /bus/%02d/dyn/attack...,f..[float] %02d = 01..16
		// /bus/%02d/dyn/release..,f..[float] %02d = 01..16
		// /bus/%02d/dyn/mix......,f..[float] %02d = 01..16
		Xb_i = 5;
		bus = (int) Xb_r[Xb_i++] - (int) '0';
		bus = bus * 10 + (int) Xb_r[Xb_i++] - (int) '0';
		cnum1 = bus + Xbus_min - 1;
		if ((Xbus_max > 0) && (cnum1 <= Xbus_max)) {
			Xb_i += 1; // skip '/'
			if (Xb_r[Xb_i] == 'm') {
				Xb_i += 4; // skip "mix/"
				if (Xb_r[Xb_i] == 'p') {
					// /bus/%02d/mix/pan
					XRmask = X32PAN;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/pan", cnum1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'f') {
					// /bus/%02d/mix/fader
					XRmask = X32FADER;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					sprintf(tmp, "/track/%d/volume", cnum1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				} else if (Xb_r[Xb_i] == 'o') {
					// /bus/%02d/mix/on
					XRmask = X32MUTE;
					while (Xb_r[Xb_i] != ',') Xb_i += 1;
					Xb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
					if (endian.ii == 1) endian.ff = 0.0;
					else                endian.ff = 1.0;
					sprintf(tmp, "/track/%d/mute", cnum1);
					Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				}
			} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')) {
				// /bus/%02d/config/name
				XRmask = X32NAME;
				Xb_i += 11; //skip "/config/name" string
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				sprintf(tmp, "/track/%d/name", cnum1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
				//
				// At this time, no ReaEQ management Reaper ReaEQ <-> X32 EQ for bus tracks
				// as REAPER uses a max of 16 parameters for its effects and X32 bus EQ has 6 bands (18 parameters)
				// It is therefore not practically usable, unless one agrees to sacrifice a band from X32... but which one?
			// } else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 3] == 'o')) {
			// X32_eqon(Xb_i, 0, bus, cnum1);
			// } else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'f')) {
			// X32_eqfr(Xb_i, 0, bus, cnum1);
			// } else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'g')) {
			// X32_eqgq(Xb_i, 0, bus, cnum1, 2);
			// } else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 5] == 'q')) {
			// X32_eqgq(Xb_i, 0, bus, cnum1, 3);
			} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'o')) {
				X32_dynon(Xb_i, 0, bus, cnum1);
			} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 't')) {
				X32_dynthr(Xb_i, 0, bus, cnum1, 1);
			} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'r') && (Xb_r[Xb_i + 5] == 'a')) {
				X32_dynratio(Xb_i, 0, bus, cnum1, 2);
			} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'a')) {
				X32_dynattack(Xb_i, 0, bus, cnum1, 3);
			} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'r') && (Xb_r[Xb_i + 5] == 'e')) {
				X32_dynrelease(Xb_i, 0, bus, cnum1, 4);
			} else if ((Xb_r[Xb_i] == 'd') && (Xb_r[Xb_i + 4] == 'm')) {
				X32_dynmix(Xb_i, 0, bus, cnum1, 12);
			}
		}
	} else if (strncmp(Xb_r, "/dca/", 5) == 0) {
		// /dca/1..8/on...........,i..[0/1]
		// /dca/1..8/fader........,f..[0..1.0]
		// /dca/1..8/config/name..,s..[string\0]
		Xb_i = 5;
		dca = (int) Xb_r[Xb_i++] - (int) '0';
		cnum1 = dca + Xdca_min - 1;
		if ((Xdca_max > 0) && (cnum1 <= Xdca_max)) {
			Xb_i += 1; // skip '/'
			if (Xb_r[Xb_i] == 'f') {
				// /dca/1..8/fader
				XRmask = X32FADER;
				Xb_i += 5; // skip "fader"
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				sprintf(tmp, "/track/%d/volume", cnum1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				if ((Rdca_min[dca - 1] > 0) && (Rdca_max[dca - 1] >= Rdca_min[dca - 1])) {
					// There are REAPER 'dca' tracks to manage
					for (i = Rdca_min[dca - 1]; i <= Rdca_max[dca - 1]; i++) {
						if (XRmask & Xrsend) SEND_TOR()
						sprintf(tmp, "/track/%d/volume", i);
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					}
				}
			} else if (Xb_r[Xb_i] == 'o') {
				// /dca/1..8/on
				XRmask = X32MUTE;
				Xb_i += 2; // skip "on"
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				if (endian.ii == 1) endian.ff = 0.0;
				else                endian.ff = 1.0;
				sprintf(tmp, "/track/%d/mute", cnum1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
				if ((Rdca_min[dca - 1] > 0) && (Rdca_max[dca - 1] >= Rdca_min[dca - 1])) {
					// There are REAPER 'dca' tracks to manage
					for (i = Rdca_min[dca - 1]; i <= Rdca_max[dca - 1]; i++) {
						if (XRmask & Xrsend) SEND_TOR()
						sprintf(tmp, "/track/%d/mute", i);
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
					}
				}
			} else if ((Xb_r[Xb_i] == 'c') && (Xb_r[Xb_i + 7] == 'n')) {
				// /dca/1..8/config/name
				XRmask = X32NAME;
				Xb_i += 11; //skip "/config/name" string
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				sprintf(tmp, "/track/%d/name", cnum1);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 's', Xb_r + Xb_i);
			}
		}
	} else if (strncmp(Xb_r, "/-stat/", 7) == 0) {
		// /-stat/ cases
		Xb_i = 10;
		if (Xb_r[Xb_i] == 'i') { // test on 'i' for selidx
			XRmask = X32SELECT;
			Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40297"); // unselect all REAPER tracks
			if (XRmask & Xrsend) SEND_TOR()
			Rb_ls = 0;
			// /-stat/selidx.........,i..[%d]
			while (Xb_r[Xb_i] != ',') Xb_i += 1;
			Xb_i += 4;
			for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]); // get track number
			cnum = -2;
			Xselected = endian.ii;
			if ((endian.ii < bkchsz) && (Xtrk_max > 0)) {
				if (Xchbank_on) {
					// Set actual channel number to match Channel Bank
					cnum = Xselected + Xchbkof * bkchsz + Xtrk_min;
				}
			} else if ((endian.ii < 32) && (Xtrk_max > 0)) cnum = -2;
			else if ((endian.ii < 40) && (Xaux_max > 0)) cnum = endian.ii + Xaux_min - 32;
			else if ((endian.ii < 48) && (Xfxr_max > 0)) cnum = endian.ii + Xfxr_min - 40;
			else if ((endian.ii < 64) && (Xbus_max > 0)) cnum = endian.ii + Xbus_min - 48;
			// else if (endian.ii == 70) cnum = -1; // set flag for master track...
			// select requested track
			if (cnum > -2) {
				// if (cnum == -1) {
				// Rb_ls = Xsprint(Rb_s, 0, 's', "/action/53808"); // seems this does not exist anymore
				// } else {
				Rselected = cnum;
				sprintf(tmp, "/track/%d/select", cnum);
				Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &fone);
				// }
			}
		} else if ((Xb_r[Xb_i] == 'e') && (Xb_r[Xb_i + 6] == 'A')) {
			if (Xeqcmp_on) {
				//           v     v
				// /-stat/screen/CHAN/page...,i..[4]
				// In that case we open the REAPER FX UI respective of current channel/track
				XRmask = X32FX;
				Xb_i += 12;
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				if (endian.ii == 4) {
					// interested in input, aux, bus, and fx tracks only
					cnum = Xselected + 1;
					i = 0;
					if ((cnum < bkchsz + 1) && (Xtrk_max > 0)) {
						i = cnum + Xtrk_min - 1;
						if (Xchbank_on) {
							// Set actual channel number to match Channel Bank
							i = Xchbkof * bkchsz + i;
						}
					} else if ((cnum < 41) && (Xaux_max > 0)) i = cnum + Xaux_min - 33;
					else if ((cnum < 49) && (Xfxr_max > 0)) i = cnum + Xfxr_min - 41;
					// else if ((cnum < 65) && (Xbus_max > 0)) i = cnum + Xbus_min - 49;
					else
						i = -1;
					if (i >= 0) {
						sprintf(tmp, "/track/%d/fx/%1d/openui", i, REQindex);
						Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &fone);
					}
				}
			}
		} else if ((Xb_r[Xb_i] == 'o') && (Xb_r[Xb_i + 1] == 's')) {
			// /-stat/solosw/%02d....,i..[0/1]
			XRmask = X32SOLO;
			Xb_i += 4;
			cnum = (int) Xb_r[Xb_i++] - (int) '0';
			cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
			while (Xb_r[Xb_i] != ',') Xb_i += 1;
			Xb_i += 4;
			for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
			if (endian.ii == 1) endian.ff = 1.0;
			else                endian.ff = 0.0;
			i = 0;
			if ((cnum < bkchsz + 1) && (Xtrk_max > 0)) {
				i = cnum + Xtrk_min - 1;
				if (Xchbank_on) {
					// Set actual channel number to match Channel Bank
					i = Xchbkof * bkchsz + i;
					XMbanktracks[i - 1].solo = endian.ff;
				}
			} else if ((cnum < 41) && (Xaux_max > 0)) i = cnum + Xaux_min - 33;
			else if ((cnum < 49) && (Xfxr_max > 0)) i = cnum + Xfxr_min - 41;
			else if ((cnum < 65) && (Xbus_max > 0)) i = cnum + Xbus_min - 49;
			else if ((cnum > 72) && (cnum < 81) && (Xdca_max > 0)) i = cnum + Xdca_min - 73;
			// !! TODO find Master track REAPER numbering
			sprintf(tmp, "/track/%d/solo", i);
			Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
		} else if ((Xb_r[Xb_i] == 'r') && (Xb_r[Xb_i + 1] == 'p')) {
			// /-stat/userpar/XX/value.,i..[int]
			// /-stat/userbank.,i..[int]
			// /-stat/xcardtype....,i..[int]
			// /-stat/xcardsync....,i..[int]
			Xb_i += 5; // get encoder/button value
			cnum = (int) Xb_r[Xb_i++] - (int) '0';
			cnum = cnum * 10 + (int) Xb_r[Xb_i++] - (int) '0';
			Xb_i += 11; // get value [0...127] in endian;
			for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
			if (Xtransport_on) {
				switch (cnum) { // ignore non-bank C values
				case 17: // bank C button 5
					//user pressed "REW" / Go to Home
					if (endian.ii == 0) { // ignore non 0 value
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40042");
					}
					break;
				case 18: // bank C button 6
					//user pressed "PLAY"
					if (endian.ii == 0) { // ignore non 0 value
						Rb_ls = Xfprint(Rb_s, 0, "/play", 'f', &fone);
					}
					break;
				case 19: // bank C button 7
					//user pressed "PAUSE" - toggle function
					if (endian.ii == 0) { // ignore non 0 value
						Rb_ls = Xfprint(Rb_s, 0, "/pause", 'f', &fone);
					}
					break;
				case 20: // bank C button 8
					//user pressed "FF" / Go to End of Project
					if (endian.ii == 0) { // ignore non 0 value
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40043");
					}
					break;
				case 21: // bank C button 9
					if (endian.ii == 0) { // Take into account only the button up transition
						if (XShift == 1) { // active only if no other key was pressed while key was on
							if (Xchbank_on) {
								// Channel Bank UP
								if (Xchbkof < ((Xtrk_max - Xtrk_min + 1) / bkchsz) - 1) {
									Xchbkof += 1; // ignore non zero values
									XUpdateBkCh();
								}
							} else {
								// user pressed "Loop" (start/stop toggle)
								if (loop_toggle) {
									Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40223"); // end loop
								} else {
									Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40222"); // start loop
								}
								loop_toggle ^= 0x7f; // set Loop start/stop indicator on X32
								Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/21/value", 'i', &loop_toggle);
								SEND_TOX(Xdelayg)
							}
						}
					} else {
						XShift = 1;
					}
					break;
				case 22: // bank C button 10
					if (endian.ii == 0) { // Take into account only the button up transition
						if (XShift) {
							// If Shift is on: special action to toggle record arming
							Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40294"); // toggle arm on last track touched
							XShift = 0;
						} else {
							if (Xchbank_on) {
								// Channel Bank DOWN
								if (Xchbkof > 0) { // 0 is the lowest accepted value
									Xchbkof -= 1;
									XUpdateBkCh();
								}
							} else {
								// user pressed "Toggle Repeat"
								Rb_ls = Xfprint(Rb_s, 0, "/repeat", 'f', &fone);
							}
						}
					}
					break;
				case 23: // bank C button 11
					//user pressed "STOP"
					if (endian.ii == 0) { // ignore non 0 value
						Rb_ls = Xfprint(Rb_s, 0, "/stop", 'f', &fone);
					}
					break;
				case 24: // bank C button 12
					//user pressed "REC"
					if (endian.ii == 0) { // ignore non 0 value
						Rb_ls = Xfprint(Rb_s, 0, "/record", 'f', &fone);
					}
					break;
				case 33: // bank C encoder 1 - Infinite rotation,
					// Move cursor to previous or next Beat depending on value change
					if (play && !play_1) {
						// stop play so we can move cursor (scrubb)
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 1; // remember we changed the state; "play" may be modified
						SEND_TOR()
					}
					if (endian.ii > six4) {
						//Move Right
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40841");
					} else {
						//Move Left
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40842");
					}
					SEND_TOR()
					if (play_1) {
						// restart play after we moved cursor
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 0;
						SEND_TOR()
					}
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/33/value", 'i', &six4);
					SEND_TOX(Xdelayg)
					Rb_ls = 0;
					break;
				case 34: // bank C encoder 2 - Infinite rotation,
					// Move cursor to previous or next Measure depending on value change
					if (play && !play_1) {
						// stop play so we can move cursor (scrubb)
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 1; // remember we changed the state; "play" may be modified
						SEND_TOR()
					}
					if (endian.ii > six4) {
						//Next Measure
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40839");
					} else {
						//Previous Measure
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40840");
					}
					SEND_TOR()
					if (play_1) {
						// restart play after we moved cursor
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 0;
						SEND_TOR()
					}
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/34/value", 'i', &six4);
					SEND_TOX(Xdelayg)
					Rb_ls = 0;
					break;
				case 35: // bank C encoder 3 - Infinite rotation,
					// Jump to previous or next Marker depending on value change
					if (play && !play_1) {
						// stop play so we can move cursor (scrubb)
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 1; // remember we changed the state; "play" may be modified
						SEND_TOR()
					}
					if (endian.ii > six4) {
						// Next marker
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40173");
					} else {
						// Previous marker
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40172");
					}
					SEND_TOR()
					if (play_1) {
						// restart play after we moved cursor
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 0;
						SEND_TOR()
					}
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/35/value", 'i', &six4);
					SEND_TOX(Xdelayg)
					Rb_ls = 0;
					break;
				case 36: // bank C encoder 4 - Infinite rotation,
					// Move cursor to next Item left or right depending on value change
					if (play && !play_1) {
						// stop play so we can move cursor (scrubb)
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 1; // remember we changed the state; "play" may be modified
						SEND_TOR()
					}
					if (endian.ii > six4) {
						// Move Right
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40319");
					} else {
						// Move Left
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40318");
					}
					if (play_1) {
						// restart play after we moved cursor
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40073");
						play_1 = 0;
						SEND_TOR()
					}
					Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/36/value", 'i', &six4);
					SEND_TOX(Xdelayg)
					Rb_ls = 0;
					break;
				}
			} else {
				if (endian.ii == 0) { // Take into account only button UP transition
					// REAPER Marker Button?
					cnum -= 12; // Bank C buttons
					if (cnum == XMkerbtn) {
						// Set REAPER Marker at current REAPER cursor position
						Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40157");
						SEND_TOR()
						Rb_ls = 0;
					}
					if (Xchbank_on) { // Transport is OFF
						//
						// ChBank buttons are two Bank C numbers between 5 to 12, OSC # between 17 to 24
						if (cnum == XMbankup) {
							// Channel Bank UP
							if (Xchbkof < ((Xtrk_max - Xtrk_min + 1) / bkchsz) - 1) {
								Xchbkof += 1; // ignore non zero values
								XUpdateBkCh();
							}
						} else if (cnum == XMbankdn) {
							// Channel Bank DOWN
							if (Xchbkof > 0) { // 0 is the lowest accepted value
								Xchbkof -= 1;
								XUpdateBkCh();
							}
						}
					}
				}
			}
		}
	} else if (strncmp(Xb_r, "/main/st/mix/", 13) == 0) {
		if (Xmaster_on) {
			// /main cases
			Xb_i += 13;
			if (Xb_r[Xb_i] == 'f') {
				// /main/st/mix/fader....,f..[float]
				XRmask = X32MFADER;
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				Rb_ls = Xfprint(Rb_s, 0, "/master/volume", 'f', &endian.ff);
			} else if (Xb_r[Xb_i] == 'p') {
				// /main/st/mix/pan......,f..[float]
				XRmask = X32MPAN;
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				Rb_ls = Xfprint(Rb_s, 0, "/master/pan", 'f', &endian.ff);
			} else if (Xb_r[Xb_i] == 'o') {
				// unselect all REAPER tracks and select Master track
				XRmask = X32SELECT;
				Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40297");
				if (XRmask & Xrsend) SEND_TOR()
					// echo Master track selected on X32
				i = 70; // master track on X32
				Xb_ls = Xfprint(Xb_s, 0, "/-stat/selidx", 'i', &i);
				if (XRmask & Xrsend)
					SEND_TOX(Xdelayg)
					// /main/st/mix/on....,i..0/1
				while (Xb_r[Xb_i] != ',') Xb_i += 1;
				Xb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Xb_r[Xb_i++]);
				if (endian.ii == 1)
					Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40731");
				else
					Rb_ls = Xsprint(Rb_s, 0, 's', "/action/40730");
			}
		}
	}
	if (Rb_ls) {
		if (XRmask & Xrsend)
			SEND_TOR()
		Rb_ls = 0; // REAPER message has been sent
	}
	return;
}
//--------------------------------------------------------------------
// REAPER Messages data parsing
//
// X32Reaper should be running when the user launches REAPER; this way, REAPER initial values are loaded
// into the program (twice in fact), ensuring all values are correctly set when the user starts using REAPER
// I noted it is better to have a 2 to 5ms delay in REAPER sending 1KB buffers when managing more than 32
// channels, otherwise, some track names are not transmitted correctly (buffer overflow?)
//
// Analysis of incoming REAPER data, and sending respective commands to X32 in order
// to synchronize REAPER and X32. Data controlled includes fader, pan, mute, solo,
// select, scribble, icon, color, bus sends fader and pan.
// FX ReaEQ can be added per track, for tracks that correspond to X32 input, mixbus, aux, and fxrtn
//   An X32 preset should be set and used to set ReaEQ to match the type and default freqs found on X32
//   The basic, 4 bands ReaEQ can fit the 3bands + high shelf X32 EQ. This will not cover for X32 loCut
//
void X32ParseReaperMessage() {

	int Rb_i;
	int Xb_ls;
	int bundle;
	int Rb_nm;
	int mes_len, tnum, fpnum, bus;
	int i;
	char tmp[32];
	//
	// is the message a bundle ?
	Rb_i = bundle = Xb_ls = i = 0;
	mes_len = Rb_nm = Rb_lr;
	//
	// REAPER bundle message?
	if (strncmp(Rb_r, "#bundle", 7) == 0) {
		bundle = 1;
		Rb_i = 16;
	}
	// Xlogf("Reaper: ", Rb_r, Rb_lr) ; fflush(log_file);
	do {
		if (bundle) {
			mes_len = (int) Rb_r[Rb_i + 2] * 256 + (int) Rb_r[Rb_i + 3]; // sub-message length fits on 2 bytes max
			Rb_i += 4;
			// prepare index for nex message (used later) and check for bundle continuity
			if ((Rb_nm = (Rb_i + mes_len)) >= Rb_lr) {
				bundle = 0; // Rb_nm is used later!
			}
		}
		// Parse message or message parts
		Xb_ls = 0;
		if (strncmp(Rb_r + Rb_i, "/track/", 7) == 0) {
			Rb_i += 7;
			// build track number...
			tnum = (int) Rb_r[Rb_i++] - (int) '0';
			while (Rb_r[Rb_i] != '/')
				tnum = tnum * 10 + (int) Rb_r[Rb_i++] - (int) '0';
			Rb_i++; // skip '/'
			// Got track #
			// Known: /fx, /pan, /volume, /name, /mute, /select, /solo, /send
			if (Rb_r[Rb_i] == 'f') { // /track/<tnum>/fx...
				if (Xeqcmp_on) {
					XXmask = TRACKFX;
					// .../fx/n/<type>/.... Type can be number, name, enable, preset, openui, fxparam
					// wetdry not needed
					if (Rb_r[Rb_i + 3] == 48 + REQindex) {
						// We only track fx/[1-9] which we expect to be Cokos ReaEQ with 4 bands
						// or Cokos ReaComp with a specific subset of parameters
						if (Rb_r[Rb_i + 5] == 'n') {
							// /track/<tnum>/fx/[1-9]/name or /track/<tnum>/fx/[1-9]/number
							// ...ignored....
						} else if (Rb_r[Rb_i + 5] == 'b') {
							// /track/<tnum>/fx/[1-9]/bypass
							while (Rb_r[Rb_i] != ',')
								Rb_i++;
							Rb_i += 4;
							for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
							// prepare X32 command
							if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
								if (Xchbank_on) {
									XMbanktracks[tnum - Xtrk_min].eqon = (int) endian.ff;
									// Set actual channel number to match Channel Bank
									tnum = tnum - Xchbkof * bkchsz;
								}
								if (((tnum - Xtrk_min) >= 0) && ((tnum - Xtrk_min) < bkchsz)) {

									sprintf(tmp, "/ch/%02d/eq/on", tnum);
								} else {
									tnum = -1;
								}
							} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))
								sprintf(tmp, "/auxin/%02d/eq/on",
										tnum - Xaux_min + 1);
							else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))
									sprintf(tmp, "/fxrtn/%02d/eq/on",
											tnum - Xfxr_min + 1);
	//
	// At this time, no ReaEQ management Reaper ReaEQ <-> X32 EQ for bus tracks
	// as REAPER uses a max of 16 parameters for its effects and X32 bus EQ has 6 bands (18 parameters)
	// It is therefore not practically usable, unless one agrees to sacrifice a band from X32... but which one?
	// else if ((tnum >= Xbus_min) && (tnum <= Xbus_max)) sprintf(tmp, "/bus/%02d/eq/on", tnum - Xbus_min + 1);
							else
								tnum = -1;
							if (tnum > 0) {
								if (endian.ff > 0.0)
									Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &one);
								else
									Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &zero);
							}
						} else if (Rb_r[Rb_i + 5] == 'p') {
							// /track/<tnum>/fx/[1-9]/preset
						} else if (Rb_r[Rb_i + 5] == 'o') {
							// /track/<tnum>/fx/[1-9]/openui
							// we then ask X32 to switch to EQ screen : /-stat/screen/CHAN/page ,i 4, if value == 1.0
							// tnum must not be a dca track num nor n RDCA track
							if ((tnum < Xdca_min) || (tnum > Xdca_max)) {
								for (i = 0; i < 8; i++) {
									if (tnum >= Rdca_min[i] && tnum <= Rdca_max[i]) {
										tnum = -1;
										break;
									}
								}
							} else {
								tnum = -1;
							}
							if (tnum > 0) {
								while (Rb_r[Rb_i] != ',') Rb_i++;
								Rb_i += 4;
								for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
								// prepare X32 command
								if (endian.ff > 0.0) Xb_ls = Xfprint(Xb_s, 0, "/-stat/screen/CHAN/page", 'i', &four);
							}
						} else if (Rb_r[Rb_i + 5] == 'f') {
							// /track/<tnum>/fx/[1-9]/fxparam/<fpnum>/value ,f <endian>
							Rb_i += 13;   //          ^
							fpnum = (int) Rb_r[Rb_i++] - (int) '0';
							while (Rb_r[Rb_i] != '/')
								fpnum = fpnum * 10 + (int) Rb_r[Rb_i++] - (int) '0';
							// ReaEQ has 16 fxparam values; only values 1-12 are used here;
							// 13(enable) is covered by bypass and 14(wetdry) is not used
							if (fpnum > 0 && fpnum < 13) {
								Rb_i++; // skip '/'
								if (Rb_r[Rb_i] == 'v') {
									// /track/<tnum>/fx/[1-9]/fxparam/<fpnum>/value
									while (Rb_r[Rb_i] != ',') Rb_i++;
									Rb_i += 4;
									for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
									// prepare X32 command
									if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
										if (Xchbank_on) {
											// Set actual channel number to match Channel Bank - for frequencies, see conversion chart later in this file
											if (fpnum == 1 || fpnum == 4 || fpnum == 7 || fpnum == 10) {
												XMbanktracks[tnum - Xtrk_min].eq[fpnum - 1] = log(2.9975 * exp(5.993961427 * endian.ff) - 1.9975) / 6.907755279;
											} else {
												XMbanktracks[tnum - Xtrk_min].eq[fpnum - 1] = endian.ff;
											}
											tnum = tnum - Xchbkof * bkchsz;
										}
										if (((tnum - Xtrk_min) >= 0) && ((tnum - Xtrk_min) < bkchsz)) {
											sprintf(tmp, "/ch/%02d/eq", tnum);
										} else {
											tnum = -1;
										}
									} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))
										sprintf(tmp, "/auxin/%02d/eq", tnum - Xaux_min + 1);
									else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))
										sprintf(tmp, "/fxrtn/%02d/eq", tnum - Xfxr_min + 1);
	//
	// At this time, no ReaEQ management Reaper ReaEQ <-> X32 EQ for bus tracks
	// as REAPER uses a max of 16 parameters for its effects and X32 bus EQ has 6 bands (18 parameters)
	// It is therefore not practically usable, unless one agrees to sacrifice a band from X32... but which one?
	// else if ((tnum >= Xbus_min) && (tnum <= Xbus_max)) sprintf(tmp, "/bus/%02d/eq", tnum - Xbus_min + 1);
									else
										tnum = -1;
									if (tnum > 0) {
										switch (fpnum) {
										case 1: // band 1 freq [20..24000] Hz (100)
											strcat(tmp, "/1/f");
	// convert REAPER freq log curve to X32; Also, REAPER is on 20..24000 and X32 20..20000
	// float curve = (exp(log(401)*x) - 1) * 0.0025; with x in [0..1] for freq in [20...24000]
	// float freq = (24000 - 20) * curve + 20;
	// float X32val = log(freq / 20.) / log(20000. / 20.);
	// if (X32val > 1.0) X32val = 1.0;
	// if (X32val < 0.0) X32val = 0.0;
	// X32val is the float [0..1.] to be sent to X32
											endian.ff = log(2.9975 * exp(5.993961427 * endian.ff) - 1.9975) / 6.907755279;
											//printf("f1:%f\n", endian.ff);
											break;
										case 2: // band 1 gain [-oo, 10] db (0)
											//printf("g1:%f\n", endian.ff);
											strcat(tmp, "/1/g");
											break;
										case 3: // band 1 bandwidth [0..4] octave (2)
											//printf("Q1:%f\n", endian.ff);
											strcat(tmp, "/1/q");
											break;
										case 4: // band 2 freq [20..24000] Hz (300)
											strcat(tmp, "/2/f");
											endian.ff = log(2.9975 * exp(5.993961427 * endian.ff) - 1.9975) / 6.907755279;
											//printf("f2:%f\n", endian.ff);
											break;
										case 5: // band 2 gain [-oo, 10] db (0)
											strcat(tmp, "/2/g");
											break;
										case 6: // band 2 bandwidth [0..4] octave (2)
											strcat(tmp, "/2/q");
											break;
										case 7: // band 3 freq [20..24000] Hz (1000)
											strcat(tmp, "/3/f");
											endian.ff = log(2.9975 * exp(5.993961427 * endian.ff) - 1.9975) / 6.907755279;
											//printf("f3:%f\n", endian.ff);
											break;
										case 8: // band 3 gain [-oo, 10] db (0)
											strcat(tmp, "/3/g");
											break;
										case 9: // band 3 bandwidth [0..4] octave (2)
											strcat(tmp, "/3/q");
											break;
										case 10: // band 4 freq [20..24000] Hz (5000)
											strcat(tmp, "/4/f");
											endian.ff = log(2.9975 * exp(5.993961427 * endian.ff) - 1.9975) / 6.907755279;
											//printf("f4:%f\n", endian.ff);
											break;
										case 11: // band 4 gain [-oo, 10] db (0)
											strcat(tmp, "/4/g");
											break;
										case 12: // band 4 bandwidth [0..4] octave (2)
											strcat(tmp, "/4/q");
											break;
										default:
											break;
										}
	// At this time, fxparams 13-16 are never considered
	// REAPER uses a max of 16 parameters for its effects and X32 bus EQ has 6 bands (18 parameters)
	// Maybe one more X32 band could be considered, but a 6th one cannot.
										Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
									}
								}
							}
						}
					}
					else
					// .../fx/n/<type>/.... Type can be number, name, enable, preset, openui, fxparam
					// wetdry not needed
					if (Rb_r[Rb_i + 3] == 48 + RCindex) {
						// We only track fx/[1-9] which we expect to be Cokos ReaEQ with 4 bands
						// or Cokos ReaComp with a specific subset of parameters
						if (Rb_r[Rb_i + 5] == 'n') {
							// /track/<tnum>/fx/[1-9]/name or /track/<tnum>/fx/[1-9]/number
							// ...ignored....
						} else if (Rb_r[Rb_i + 5] == 'b') {
							// /track/<tnum>/fx/[1-9]/bypass
							while (Rb_r[Rb_i] != ',')
								Rb_i++;
							Rb_i += 4;
							for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
							// prepare X32 command; CMP only applies to ch and bus
							if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
								if (Xchbank_on) {
									XMbanktracks[tnum - Xtrk_min].cmpon = (int) endian.ff;
									// Set actual channel number to match Channel Bank
									tnum = tnum - Xchbkof * bkchsz;
								}
								if (((tnum - Xtrk_min) >= 0) && ((tnum - Xtrk_min) < bkchsz)) {
									sprintf(tmp, "/ch/%02d/dyn/on", tnum);
								} else {
									tnum = -1;
								}
							} else  if ((tnum >= Xbus_min) && (tnum <= Xbus_max)) {
									sprintf(tmp, "/bus/%02d/dyn/on", tnum - Xbus_min + 1);
							} else {
								tnum = -1;
							}
							if (tnum > 0) {
								if (endian.ff > 0.0)
									Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &one);
								else
									Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &zero);
							}
						} else if (Rb_r[Rb_i + 5] == 'p') {
							// /track/<tnum>/fx/[1-9]/preset
							// ignore presets
						} else if (Rb_r[Rb_i + 5] == 'o') {
							// /track/<tnum>/fx/[1-9]/openui
							// we then ask X32 to switch to DYN screen : /-stat/screen/CHAN/page ,i 3, if value == 1.0
							// tnum must not be a dca track num nor an RDCA track
							if ((tnum < Xdca_min) || (tnum > Xdca_max)) {
								for (i = 0; i < 8; i++) {
									if (tnum >= Rdca_min[i] && tnum <= Rdca_max[i]) {
										tnum = -1;
										break;
									}
								}
							} else {
								tnum = -1;
							}
							if (tnum > 0) {
								while (Rb_r[Rb_i] != ',') Rb_i++;
								Rb_i += 4;
								for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
								// prepare X32 command
								if (endian.ff > 0.0) Xb_ls = Xfprint(Xb_s, 0, "/-stat/screen/CHAN/page", 'i', &three);
							}
						} else if (Rb_r[Rb_i + 5] == 'f') {
							// /track/<tnum>/fx/[1-9]/fxparam/<fpnum>/value ,f <endian>
							Rb_i += 13;   //          ^
							fpnum = (int) Rb_r[Rb_i++] - (int) '0';
							while (Rb_r[Rb_i] != '/')
								fpnum = fpnum * 10 + (int) Rb_r[Rb_i++] - (int) '0';
							// ReaCOMP has many fxparam values; only values 1-4 and 12 (wet/gain)are used here;
							//printf("FX par# %d\n", fpnum);
							if ((fpnum > 0 && fpnum < 5) || fpnum == 12) {
								Rb_i++; // skip '/'
								if (Rb_r[Rb_i] == 'v') {
									// /track/<tnum>/fx/[1-9]/fxparam/<fpnum>/value
									while (Rb_r[Rb_i] != ',') Rb_i++;
									Rb_i += 4;
									for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
									// prepare X32 command
									if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
										if (Xchbank_on) {
											// For now, just pass the float value.
											// Set actual channel number to match Channel Bank - for log data, see conversion chart later in this file
//											if (fpnum == 1 || fpnum == 4 || fpnum == 7 || fpnum == 10) {
//												XMbanktracks[tnum - Xtrk_min].cmp[fpnum - 1] = log(2.9975 * exp(5.993961427 * endian.ff) - 1.9975) / 6.907755279;
//											} else {
												XMbanktracks[tnum - Xtrk_min].cmp[fpnum - 1] = endian.ff;
//											}
											tnum = tnum - Xchbkof * bkchsz;
										}
										if (((tnum - Xtrk_min) >= 0) && ((tnum - Xtrk_min) < bkchsz)) {
											sprintf(tmp, "/ch/%02d/dyn", tnum);
										} else {
											tnum = -1;
										}
									} else  if ((tnum >= Xbus_min) && (tnum <= Xbus_max)) {
										sprintf(tmp, "/bus/%02d/dyn", tnum - Xbus_min + 1);
									} else {
										tnum = -1;
									}
									if (tnum > 0) {
										// if needed convert REAPER log curve to X32 400 steps log data;
										// Also, REAPER is on 20..24000 and X32 20..20000
										// float curve = (exp(log(401)*x) - 1) * 0.0025; with x in [0..1] for freq in [20...24000]
										// float freq = (24000 - 20) * curve + 20;
										// float X32val = log(freq / 20.) / log(20000. / 20.);
										// if (X32val > 1.0) X32val = 1.0;
										// if (X32val < 0.0) X32val = 0.0;
										// X32val is the float [0..1.] to be sent to X32
										switch (fpnum) {
										case 1:
											// X32 threshold is linf -60->0
											// REAPER is log -oo to +10 [-60 = 0.0005, 0 = 0.5]
											if (endian.ff > 0.5) endian.ff = 0.5;
											if (endian.ff < 0.0005) endian.ff = 0.0005;
											//printf("valueRpre=%f\n", endian.ff);
											endian.ff = 1. - (log(endian.ff) + 0.693147) / (-7.600903 + 0.693147);
											//printf("valueRpost=%f\n", endian.ff);
											strcat(tmp, "/thr");
											Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
											break;
										case 2:
											// X32 ratio is an int [0..11] for values 1:1 to 100:1
											if (endian.ff < .003) endian.ii = 0;
											else if(endian.ff < .003) endian.ii = 0;
											else if(endian.ff < .005) endian.ii = 1;
											else if(endian.ff < .0101) endian.ii = 2;
											else if(endian.ff < .0151) endian.ii = 3;
											else if(endian.ff < .0202) endian.ii = 4;
											else if(endian.ff < .0303) endian.ii = 5;
											else if(endian.ff < .0404) endian.ii = 6;
											else if(endian.ff < .0606) endian.ii = 7;
											else if(endian.ff < .0909) endian.ii = 8;
											else if(endian.ff < .1919) endian.ii = 9;
											else if(endian.ff < .90) endian.ii = 10;
											else endian.ii = 11;
											strcat(tmp, "/ratio");
											Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &endian.ii);
											break;
										case 3:
											// X32 attack is linf 0...120 ms; REAPER is log 0-500, with 120 == 0.24
											if (endian.ff > 0.24) endian.ff = 0.24;
											//printf("valueRpre=%f\n", endian.ff);
											endian.ff = endian.ff / 0.24;
											strcat(tmp, "/attack");
											//printf("valueRpost=%f\n", endian.ff);
											Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
											break;
										case 4:
											// X32 release is logf 5...4000 ms; REAPER is log 0-5000, with 100 == 0.019903
											if (endian.ff < 0.001) endian.ff = 0.001;
											if (endian.ff > 0.8) endian.ff = 0.8;
											//printf("valueRpre=%f\n", endian.ff);
											endian.ff = (log(endian.ff) + 6.90775) / (-0.22314 + 6.90775);
											strcat(tmp, "/release");
											//printf("valueRpost=%f\n", endian.ff);
											Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
											break;
										case 12:
											// X32 Mix is linf 0..100%; REAPER is log -oo..10dB, with 0dB == 0.5
											if (endian.ff > 0.5) endian.ff = 0.5;
											// printf("valueRpre=%f\n", endian.ff);
											endian.ff = (log(endian.ff + 0.00001) + 11.512925) / (11.512925 - 0.693127) ;
											strcat(tmp, "/mix");
											//printf("valueRpost=%f\n", endian.ff);
											Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
											break;
										default:
											// At this time, all other fxparams 5-11 & 13-16 are never considered
											break;
										}
									}
								}
							}
						}
					}
				}
			} else if (Rb_r[Rb_i] == 'p') { // /track/pan
				XXmask = TRACKPAN;
				while (Rb_r[Rb_i] != ',') Rb_i++;
				Rb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				// /xxxx/[01-32]/mix/pan ,f -100 100
				if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
					if (Xchbank_on) {
						XMbanktracks[tnum - Xtrk_min].pan = endian.ff;
						// Set actual channel number to match Channel Bank
						tnum = tnum - Xchbkof * bkchsz;
					}
					if (((tnum - Xtrk_min) >= 0) && ((tnum - Xtrk_min) < bkchsz)) {
						sprintf(tmp, "/ch/%02d/mix/pan", tnum);
					} else {
						tnum = -1;
					}
				} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))
					sprintf(tmp, "/auxin/%02d/mix/pan", tnum - Xaux_min + 1);
				else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))
					sprintf(tmp, "/fxrtn/%02d/mix/pan", tnum - Xfxr_min + 1);
				else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))
					sprintf(tmp, "/bus/%02d/mix/pan", tnum - Xbus_min + 1);
				else
					tnum = -1;
				if (tnum > 0)
					Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
			} else if (Rb_r[Rb_i] == 'v') { // /track/volume
				XXmask = TRACKFADER;
				while (Rb_r[Rb_i] != ',') Rb_i++;
				Rb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				//
				// Make REAPER stick to X32 known values to avoid fader kick-backs
				endian.ff = (int) (endian.ff * 1023.5) / 1023.0;
				// /xxxx/[01-32]/mix/fader ,f 0..1
				if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
					if (Xchbank_on) {
						XMbanktracks[tnum - Xtrk_min].fader = endian.ff;
						// Set actual channel number to match Channel Bank
						tnum = tnum - Xchbkof * bkchsz;
					}
					if (((tnum - Xtrk_min) >= 0) && ((tnum - Xtrk_min) < bkchsz)) {
						sprintf(tmp, "/ch/%02d/mix/fader", tnum);
					} else {
						tnum = -1;
					}
				} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))
					sprintf(tmp, "/auxin/%02d/mix/fader", tnum - Xaux_min + 1);
				else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))
					sprintf(tmp, "/fxrtn/%02d/mix/fader", tnum - Xfxr_min + 1);
				else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))
					sprintf(tmp, "/bus/%02d/mix/fader", tnum - Xbus_min + 1);
				else if ((tnum >= Xdca_min) && (tnum <= Xdca_max)) {
					if ((Rdca_min[tnum - Xdca_min] > 0) && (Rdca_max[tnum - Xdca_min] >= Rdca_min[tnum - Xdca_min])) {
						for (i = Rdca_min[tnum - Xdca_min]; i <= Rdca_max[tnum - Xdca_min]; i++) {
							// update all REAPER DCA tracks to same values
							sprintf(tmp, "/track/%d/volume", i);
							Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
							SEND_TOR()
						}
					}
					sprintf(tmp, "/dca/%1d/fader", tnum - Xdca_min + 1);
				} else {
					// Do we have a REAPER DCA assigned channel?
					for (i = 0; i < 8; i++) {
						if (tnum >= Rdca_min[i] && tnum <= Rdca_max[i]) {
							for (tnum = Rdca_min[i]; tnum <= Rdca_max[i]; tnum++) {
								// update all REAPER DCA tracks to same values
								sprintf(tmp, "/track/%d/volume", tnum);
								Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
								SEND_TOR()
							}
							tnum = i + 1;
							// also update REAPER DCA fader
							if ((Xdca_max > 0)
									&& (i <= (Xdca_max - Xdca_min + 1))) {
								sprintf(tmp, "/track/%d/volume", Xdca_min + i);
								Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
								SEND_TOR()
							}
							sprintf(tmp, "/dca/%1d/fader", i + 1);
							break;
						}
					}
					if (i >= 8)
						tnum = -1;
				}
				if (tnum > 0)
					Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
			} else if (Rb_r[Rb_i] == 'n') { // /track/name
				XXmask = TRACKNAME;
				while (Rb_r[Rb_i] != ',') Rb_i++;
				Rb_i += 4;
				// Track name starts at Rb_i index
				// We can set name, but also icon and color using the following format:
				// <name>[%icon[%color]] (optional spaces; '%' as a delimiter)
				//         icon and color as integers
				int i_icon = 0;
				int i_color = -1;
				int length = strlen(Rb_r + Rb_i);
				for (i = 0; i < length; i++) {
					if (Rb_r[Rb_i + i] == '%') {
						if (i_icon) {
							i_color = i + 1;
						} else {
							i_icon = i + 1;
						}
						Rb_r[Rb_i + i] = 0;
					}
				}
				if (i_icon)
					sscanf(Rb_r + Rb_i + i_icon, "%d", &i_icon);
				if (i_color > -1)
					sscanf(Rb_r + Rb_i + i_color, "%d", &i_color);
				// /xxxx/[01-32]/config/name ,s string
				if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
					if (Xchbank_on)
						strncpy(XMbanktracks[tnum - Xtrk_min].scribble, Rb_r + Rb_i, 12);
					if (i_icon) {
						if ((tnum - Xchbkof * bkchsz - Xtrk_min) < bkchsz) {
							sprintf(tmp, "/ch/%02d/config/icon", tnum - Xchbkof * bkchsz - Xtrk_min + 1);							Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
							if (XXmask & Xxsend) SEND_TOX(Xdelayg)
						}
						if (Xchbank_on) XMbanktracks[tnum - Xtrk_min].icon = i_icon;
					}
					if (i_color > -1) {
						if ((tnum - Xchbkof * bkchsz - Xtrk_min) < bkchsz) {
							sprintf(tmp, "/ch/%02d/config/color", tnum - Xchbkof * bkchsz - Xtrk_min + 1);							Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
							Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
							if (XXmask & Xxsend) SEND_TOX(Xdelayg)
						}
						if (Xchbank_on)
							XMbanktracks[tnum - Xtrk_min].color = i_color;
					}
					if (Xchbank_on) {
						// Set actual channel number to match Channel Bank
						tnum = tnum - Xchbkof * bkchsz;
					}
					if ((tnum - Xtrk_min) < bkchsz) {
						sprintf(tmp, "/ch/%02d/config/name", tnum - Xtrk_min + 1);
					} else {
						tnum = -1;
					}
				} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max)) {
					if (i_icon) {
						sprintf(tmp, "/auxin/%02d/config/icon", tnum - Xaux_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
						if (XXmask & Xxsend) SEND_TOX(Xdelayg)
					}
					if (i_color > -1) {
						sprintf(tmp, "/auxin/%02d/config/color", tnum - Xaux_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
						if (XXmask & Xxsend) SEND_TOX(Xdelayg)
					}
					sprintf(tmp, "/auxin/%02d/config/name", tnum - Xaux_min + 1);
				} else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max)) {
					if (i_icon) {
						sprintf(tmp, "/fxrtn/%02d/config/icon", tnum - Xfxr_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
						if (XXmask & Xxsend) SEND_TOX(Xdelayg)
					}
					if (i_color > -1) {
						sprintf(tmp, "/fxrtn/%02d/config/color", tnum - Xfxr_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
						if (XXmask & Xxsend) SEND_TOX(Xdelayg)
					}
					sprintf(tmp, "/fxrtn/%02d/config/name", tnum - Xfxr_min + 1);
				} else if ((tnum >= Xbus_min) && (tnum <= Xbus_max)) {
					if (i_icon) {
						sprintf(tmp, "/bus/%02d/config/icon", tnum - Xbus_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
						if (XXmask & Xxsend) SEND_TOX(Xdelayg)
					}
					if (i_color > -1) {
						sprintf(tmp, "/bus/%02d/config/color", tnum - Xbus_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
						if (XXmask & Xxsend) SEND_TOX(Xdelayg)
					}
					sprintf(tmp, "/bus/%02d/config/name", tnum - Xbus_min + 1);
				} else if ((tnum >= Xdca_min) && (tnum <= Xdca_max)) {
					if (i_icon) {
						sprintf(tmp, "/dca/%1d/config/icon", tnum - Xdca_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_icon);
						if (XXmask & Xxsend) SEND_TOX(Xdelayg)
					}
					if (i_color > -1) {
						sprintf(tmp, "/dca/%1d/config/color", tnum - Xdca_min + 1);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i_color);
						if (XXmask & Xxsend) SEND_TOX(Xdelayg)
					}
					sprintf(tmp, "/dca/%1d/config/name", tnum - Xdca_min + 1);
				} else
					tnum = -1;
				if (tnum > 0) {
					Xb_ls = Xfprint(Xb_s, 0, tmp, 's', Rb_r + Rb_i);
				}
			} else if (Rb_r[Rb_i] == 'm') { // /track/mute
				XXmask = TRACKMUTE;
				while (Rb_r[Rb_i] != ',') Rb_i++;
				Rb_i += 4;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++])
					;
				// /xxxx/[01-32]/mix/on ,i 0/1
				if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
					if (Xchbank_on) {
						XMbanktracks[tnum - Xtrk_min].mute = endian.ff;
						// Set actual channel number to match Channel Bank
						tnum = tnum - Xchbkof * bkchsz;
					}
					if (((tnum - Xtrk_min) >= 0) && ((tnum - Xtrk_min) < bkchsz)) {
						sprintf(tmp, "/ch/%02d/mix/on", tnum - Xtrk_min + 1);
					} else {
						tnum = -1;
					}
				} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))
					sprintf(tmp, "/auxin/%02d/mix/on", tnum - Xaux_min + 1);
				else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))
					sprintf(tmp, "/fxrtn/%02d/mix/on", tnum - Xfxr_min + 1);
				else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))
					sprintf(tmp, "/bus/%02d/mix/on", tnum - Xbus_min + 1);
				else if ((tnum >= Xdca_min) && (tnum <= Xdca_max)) {
					if ((Rdca_min[tnum - Xdca_min] > 0) && (Rdca_max[tnum - Xdca_min] >= Rdca_min[tnum - Xdca_min])) {
						for (i = Rdca_min[tnum - Xdca_min]; i <= Rdca_max[tnum - Xdca_min]; i++) {
							// update all REAPER DCA tracks to same values
							sprintf(tmp, "/track/%d/mute", i);
							Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
							SEND_TOR()
						}
					}
					sprintf(tmp, "/dca/%1d/on", tnum - Xdca_min + 1);
				} else {
					// Do we have a REAPER DCA assigned channel?
					for (i = 0; i < 8; i++) {
						if (tnum >= Rdca_min[i] && tnum <= Rdca_max[i]) {
							for (tnum = Rdca_min[i]; tnum <= Rdca_max[i]; tnum++) {
								// update all REAPER DCA tracks to same values
								sprintf(tmp, "/track/%d/mute", tnum);
								Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
								SEND_TOR()
							}
							tnum = i + 1;
							// also update REAPER DCA fader
							if ((Xdca_max > 0) && (i <= (Xdca_max - Xdca_min + 1))) {
								sprintf(tmp, "/track/%d/mute", Xdca_min + i);
								Rb_ls = Xfprint(Rb_s, 0, tmp, 'f', &endian.ff);
								SEND_TOR()
							}
							sprintf(tmp, "/dca/%1d/on", i + 1);
							break;
						}
					}
					if (i >= 8) tnum = -1;
				}
				if (tnum > 0) {
					if (endian.ff > 0.0)
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &zero);
					else
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &one);
				}
			} else if (Rb_r[Rb_i] == 's') { // /track/select, /track/send or /track/solo
				Rb_i++;
				if ((Rb_r[Rb_i] == 'e') && (Rb_r[Rb_i + 1] == 'l')) { // /track/select
					XXmask = TRACKSELECT;
					while (Rb_r[Rb_i] != ',') Rb_i++;
					Rb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					if ((int) endian.ff == 1) {
						// set channel # based on track num.
						Rselected = tnum;
						if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
							tnum = tnum - Xtrk_min;
							if (Xchbank_on) {
								// Set actual channel number to match Channel Bank
								tnum = tnum - Xchbkof * bkchsz;
							}
							if (tnum > bkchsz - 1) tnum = -1; // do not "touch" ch > bkchsz
						} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))
							tnum = tnum - Xaux_min + 32;
						else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))
							tnum = tnum - Xfxr_min + 40;
						else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))
							tnum = tnum - Xbus_min + 48;
						else
							tnum = -1; // TODO: How to select Master from REAPER???
						// X32: selecting one track automatically unselects others
						if (tnum >= 0) {
							Xselected = tnum;
							Xb_ls = Xfprint(Xb_s, 0, "/-stat/selidx", 'i', &tnum);
						}
					}
				} else if ((Rb_r[Rb_i] == 'e') && (Rb_r[Rb_i + 1] == 'n')) { // /track/send
					// example: /track/6/send/2/volume\0\0,f\0\0?7K� (track 6 sending to 2nd bus)
					XXmask = TRACKSEND;
					Rb_i += 4; // skip "....send/"
					// build bus track number
					bus = (int) Rb_r[Rb_i++] - (int) '0';
					while (Rb_r[Rb_i] != '/') bus = bus * 10 + (int) Rb_r[Rb_i++] - (int) '0';
					bus -= TrackSendOffset;
					Rb_i++; // skip '/'
					if (Rb_r[Rb_i] == 'v') { // volume <float>
						while (Rb_r[Rb_i] != ',') Rb_i++;
						Rb_i += 4;
						for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
						// /xxxx/<tnum>/mix/<bus>/level ,f 0...1.
						if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
							if (Xchbank_on) {
								XMbanktracks[tnum - Xtrk_min].mixbus[bus - 1] = endian.ff;
								// Set actual channel number to match Channel Bank
								tnum = tnum - Xchbkof * bkchsz;
							}
							if ((tnum - Xtrk_min + 1) < bkchsz + 1) {
								sprintf(tmp, "/ch/%02d/mix/%02d/level", tnum - Xtrk_min + 1, bus);
							} else {
								tnum = -1;
							}
						} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))
							sprintf(tmp, "/auxin/%02d/mix/%02d/level", tnum - Xaux_min + 1, bus);
						else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))
							sprintf(tmp, "/fxrtn/%02d/mix/%02d/level", tnum - Xfxr_min + 1, bus);
						else
							tnum = -1;
						if (tnum > 0)
							Xb_ls = Xfprint(Xb_s, 0, tmp, 'f', &endian.ff);
					}
				} else if (Rb_r[Rb_i] == 'o') { // /track/solo
					XXmask = TRACKSOLO;
					while (Rb_r[Rb_i] != ',') Rb_i++;
					Rb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					i = (int) endian.ff;
					// set track X32 Channel number based on track
					if ((tnum >= Xtrk_min) && (tnum <= Xtrk_max)) {
						tnum = tnum - Xtrk_min + 1;
						if (Xchbank_on) {
							XMbanktracks[tnum - 1].solo = endian.ff;
							// Set actual channel number to match Channel Bank
							tnum = tnum - Xchbkof * bkchsz;
						}
						if (tnum > bkchsz)
							tnum = -1; // do not "touch" ch > bkchsz
					} else if ((tnum >= Xaux_min) && (tnum <= Xaux_max))
						tnum = tnum - Xaux_min + 32 + 1;
					else if ((tnum >= Xfxr_min) && (tnum <= Xfxr_max))
						tnum = tnum - Xfxr_min + 40 + 1;
					else if ((tnum >= Xbus_min) && (tnum <= Xbus_max))
						tnum = tnum - Xbus_min + 48 + 1;
					else if ((tnum >= Xdca_min) && (tnum <= Xdca_max))
						tnum = tnum - Xdca_min + 72 + 1;
					else
						tnum = -1;
					if (tnum > 0) { // TODO: How to select Master from REAPER???
						sprintf(tmp, "/-stat/solosw/%02d", tnum);
						Xb_ls = Xfprint(Xb_s, 0, tmp, 'i', &i);
					}
				}
			}
		} else if (strncmp(Rb_r + Rb_i, "/master/", 8) == 0) {
			if (Xmaster_on) {
				// MASTER
				Rb_i += 8;
				// Known:/master/pan, /master/volume
				// TODO: Not possible today: Select, Solo, Mute
				if (Rb_r[Rb_i] == 'p') { // pan
					XXmask = MASTERPAN;
					while (Rb_r[Rb_i] != ',') Rb_i++;
					Rb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					Xb_ls = Xfprint(Xb_s, 0, "/main/st/mix/pan", 'f', &endian.ff);
				} else if (Rb_r[Rb_i] == 'v') { // volume
					XXmask = MASTERVOLUME;
					while (Rb_r[Rb_i] != ',') Rb_i++;
					Rb_i += 4;
					for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
					Xb_ls = Xfprint(Xb_s, 0, "/main/st/mix/fader", 'f', &endian.ff);
				}
			}
		} else if (Xtransport_on) {
			if (strncmp(Rb_r + Rb_i, "/repeat", 7) == 0) {
				Rb_i += 12;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				if (endian.ff == 1.0) endian.ii = 0x7F; // otherwise, endian.ii will be all 0
				Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/22/value", 'i', &endian.ii);
			} else if (strncmp(Rb_r + Rb_i, "/record", 7) == 0) {
				Rb_i += 12;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				if (endian.ff == 1.0) endian.ii = 0x7F; // otherwise, endian.ii will be all 0
				Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/24/value", 'i', &endian.ii);
			} else if (strncmp(Rb_r + Rb_i, "/play", 5) == 0) {
				Rb_i += 12;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				if (endian.ff == 1.0) {
					endian.ii = 0x7F; // otherwise, endian.ii will be all 0
					play = 1;
				} else {
					play = 0;
				}
				Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/18/value", 'i', &endian.ii);
			} else if (strncmp(Rb_r + Rb_i, "/pause", 6) == 0) {
				Rb_i += 12;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				if (endian.ff == 1.0) endian.ii = 0x7F; // otherwise, endian.ii will be all 0
				Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/19/value", 'i', &endian.ii);

			} else if (strncmp(Rb_r + Rb_i, "/stop", 5) == 0) {
				Rb_i += 12;
				for (i = 4; i > 0; endian.cc[--i] = Rb_r[Rb_i++]);
				if (endian.ff == 1.0)
					endian.ii = 0x7F; // otherwise, endian.ii will be all 0
				Xb_ls = Xfprint(Xb_s, 0, "/-stat/userpar/23/value", 'i', &endian.ii);
			}
		}
		if (Xb_ls) {
			if (XXmask & Xxsend)
				SEND_TOX(Xdelayg)
			Xb_ls = 0;
		}
		Rb_i = Rb_nm; // Set Rb_i pointing to next message (at index Rb_nm) in Reaper bundle
	} while (bundle);
	return;
}
