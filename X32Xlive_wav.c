/*
 * X32Xlive_Wav.c
 *
 *  Created on: Jan 24, 2018
 *
 * ©2018 - Patrick-Gilles Maillot
 *
 *
 * X32Xlive_Wav - Command-line / Windows application for exploding X-Live! multi-channel
 * 				WAV files into standard (mono-channel) WAV files
 *
 * Working principles:
 * 		-1- user chooses a source directory with an XLive! recording session [mandatory]
 * 		-2- user decides on a destination directory where files will be saved [default is ./]
 * 		Sessions can have several 4GB (up to) files. Each is a 8 to 32 multi-channel wav file
 * 		The actual destination file names is set by the user and individual names can be
 * 		assigned to them, rather than ch_1...ch_32
 *		-3- user decides on how many of these channels are extracted, and
 *		-4- user names the respective destination files
 * 		There are two ways to set names: either 1 by 1, using the GUI "Set" button, or
 * 		by selecting an X32 scene file where names are automatically extracted from the
 * 		scene X32 channels attributes.
 *		-5- click "explode"
 * 		Once set, a name can be modified using the GUI "Get" function, editing the name and
 * 		using the "Set" button to save modifications. Names are limited to 12 characters.
 *
 * 		Session files ar typically 24bit samples in 32bit containers. The user can chose
 * 		to keep the 24bit format, to downgrade to 8bit, 16bit or upgrade to 32bit.
 * 		Files are exploded following each 4GB session file; if there are for ex. 2 wav
 * 		files in the session (on 4BG and the next one < 4GB), and the user called for 2 channels
 * 		to be extracted, naming them "AA" and "BB", this will result in the following file
 * 		set after extraction:
 * 			AA.wav
 * 			BB.wav
 *		with AA.wav and BB.wav accumulating the single wave file sections of the two section
 *		files used as source.
 *
 * In case of inconsistencies or errors, messages are reported to the GUI level.
 * Upon termination, the time needed to explode files is provided. Be patient...
 * Exploding multi-channel wav files takes time, even with optimizations
 * 		for ex ~22s for extracting 5 24bit channels out of a 5.2GB 8-channel XLive! Session
 *
 *
 * Command-line case explanation: see -h option in the code below for explanations
 *
 *
 * Revision history
 *	ver. 0.10: first release, including optimizations for exploding files
 *	ver. 0.20: added command line version (using ifdefs), keeping most options active
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/timeb.h>	// precise timing functions
#ifdef _WIN32
#include <winsock2.h>	// Windows functions for std GUI & sockets
#include <Shlobj.h>		// Windows shell functions
#define MESSAGE(s1,s2)	\
			MessageBox(NULL, s1, s2, MB_OK);
#else
#define MAX_PATH 256		// file name/path size
#define MESSAGE(s1,s2)	\
			printf("%s - %s\n",s2, s1);
#endif
//
#define NAMSIZ 16		// name string size
#define MAXLR 512		// max size for reading scene lines
#define OP2 64			// max optimization value [must be a power of 2]
//
typedef union {
	char			s[4];
	unsigned int	i;
} i4chr;
//
i4chr	uRIFF, uriff;
i4chr	uFMT, ufmt;
i4chr	uWAVE, uwave;
i4chr	uDATA, udata;
//
// Private functions
int	ExplodeWavFile();
int	SetNamesFromScene();
//
#ifdef _WIN32
// Windows Declarations
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE		hInstance = 0;
HWND				hwndInDir, hwndOutDir, hwndSource, hwndDestin, hwndsample;
HWND				hwndNbChan, hwndChannels, hwndSetCh, hwndGetCh, hwndChNum;
HWND				hwndChName, hwndInScn, hwndScene;
HWND				hwndMerge;
HFONT			hfont, htmp;
HDC				hdc, hdcMem;
HBITMAP			hBmp;
BITMAP			bmp;
PAINTSTRUCT		ps;
MSG				wMsg;				// Windows msg event
OPENFILENAME		ofn;       			// common dialog box structure
BROWSEINFO 		bi;					// Windows Shell structure
ITEMIDLIST 		*pidl;				// dir item list
#endif

char			Xspath[MAX_PATH];		// file path, used for source directory where to source wav files
char			Xdpath[MAX_PATH];		// file path, used for destination directory where to save wav files
char			Spath[MAX_PATH];			// file path, used for scene file name
//
struct timeb	start,end;				// precise timers (well... to the ms)
//
int			keep_running;			// Mainloop flag
int			wWidth = 550;			// Default window size
int			wHeight = 178;			//
int			nbchans;					// number of channels [1 to 32]
int			chan_id;					// current channel number [1 to 32]
int			sampsel;					// sample selection factor
int			dlen, slen;				// string lengths used in exploding function
char			str0[8];					// used for Windows strings conversions
char			str1[MAX_PATH];			// used for Windows strings conversions
char			*ChNamTable;				// Channel name table pointer (Table is n x 16 chars)
FILE			*Wfile[32];				// output wavefile handles
//
struct {
	unsigned int 	Riff;				// = "Riff";
	unsigned int 	wavsize; 			// size of WAVE sub-chunk = Dbytes + 36
	unsigned int 	Wave; 				// = "WAVE"
	unsigned int	 	fmt;					// = "fmt ";
	unsigned int	 	Sixteen;				// = 16;
	unsigned short	One;					// = 1;
	unsigned short	num_channels;		// = 1; number of channels in multichannel data (8, 16 or 32)
	unsigned int	 	audio_samprate;		// sample rate (48000 or 44100)
	unsigned int	 	ByteRate;         	// average bytes per second
	unsigned short	wBlockAlign;			// alignment
	unsigned short	SampleBits;			// = 32;
	unsigned int 	Data;				// = "data" or "JUNK";
	unsigned int 	Dbytes;				// = data size or if JUNK: 32716, or (1024*32) - 52
} wheader, rheader, theader;

char			*sampsizes[] = {"8", "16", "24", "32","\0"};
//
#ifdef _WIN32
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmWfile) {
//
	WNDCLASSW wc = {0};
	wc.lpszClassName = L"X32Wav_Xlive";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
		L"X32Xlive_Wav - Explode X-Live! multichannel WAV files",
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
		hwndMerge = CreateWindow("button", "Explode",
				WS_VISIBLE | WS_CHILD, 432, 74, 109, 71, hwnd, (HMENU )1, NULL, NULL);

		hwndInDir = CreateWindow("button", "Session Directory",
				WS_VISIBLE | WS_CHILD, 128, 25, 150, 20, hwnd, (HMENU )2, NULL, NULL);
		hwndSource = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 285, 25, 255, 20, hwnd, (HMENU )0, NULL, NULL);

		hwndOutDir = CreateWindow("button", "Destination Directory",
				WS_VISIBLE | WS_CHILD, 128, 50, 150, 20, hwnd, (HMENU )3, NULL, NULL);
		hwndDestin = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 285, 50, 255, 20, hwnd, (HMENU )0, NULL, NULL);

		hwndNbChan = CreateWindow("button", "Nb of Channels",
				WS_VISIBLE | WS_CHILD, 128, 75, 120, 20, hwnd, (HMENU )4, NULL, NULL);
		hwndChannels = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 255, 75, 23, 20, hwnd, (HMENU )0, NULL, NULL);
		hwndsample = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
				389, 73, 40, 80, hwnd, (HMENU)0, NULL, NULL);
		for (i = 0; i < 4; i++) {
			SendMessage(hwndsample, CB_ADDSTRING, (WPARAM)0, (LPARAM)sampsizes[i]);
		}
		// display initial item in the selection field to 24 bits/sample
		SendMessage(hwndsample, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

		hwndSetCh = CreateWindow("button", "Set",
				WS_VISIBLE | WS_CHILD, 128, 100, 30, 20, hwnd, (HMENU )5, NULL, NULL);
		hwndGetCh = CreateWindow("button", "Get",
				WS_VISIBLE | WS_CHILD, 163, 100, 30, 20, hwnd, (HMENU )6, NULL, NULL);
		hwndChNum = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 220, 100, 23, 20, hwnd, (HMENU )0, NULL, NULL);
		hwndChName = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 293, 100, 135, 20, hwnd, (HMENU )0, NULL, NULL);

		hwndInScn = CreateWindow("button", "Scene File",
				WS_VISIBLE | WS_CHILD, 128, 125, 75, 20, hwnd, (HMENU )7, NULL, NULL);
		hwndScene = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 208, 125, 220, 20, hwnd, (HMENU )0, NULL, NULL);

		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./sdcard1.bmp", IMAGE_BITMAP, 0, 0,
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
		BitBlt(hdc, 3, 3, 240, 260, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);

		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 128, 3, str1, wsprintf(str1, "X32Xlive_Wav - ver 0.20 - ©2018 - Patrick-Gilles Maillot"));

		DeleteObject(htmp);
		DeleteObject(hfont);

		hfont = CreateFont(16, 0, 0, 0, FW_BOLD, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 285, 76, str1, wsprintf(str1, "Output Samples"));
		TextOut(hdc, 197, 102, str1, wsprintf(str1, "Ch:"));
		TextOut(hdc, 250, 102, str1, wsprintf(str1, "Name:"));

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
				// Explode button clicked!
				// get output sample size (16, 24, 32 bits)
				sampsel = SendMessage(hwndsample, CB_GETCURSEL,(WPARAM) 0, (LPARAM) 0);
				sampsel += 1;
				//
				if (Xspath[0]) {
					if (Xdpath[0]) {
						if (nbchans) {
							//
							// At this point, we're ready to unpack/explode the session files
							// into separate wav files. Call the dedicated function.
							if (ExplodeWavFile()) {
								MessageBox(NULL, "Something went wrong", NULL, MB_OK);
							}
						} else {
							MessageBox(NULL, "No channels!", NULL, MB_OK);
						}
					} else {
						MessageBox(NULL, "Select Destination dir. first!", NULL, MB_OK);
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
					if ((i = strlen(Xspath)) > 31) {
						strncpy(str1, Xspath, 14);
						strcpy(str1 + 14, " ... ");
						strcpy(str1 + 19, Xspath + i - 15);
					} else {
						strcpy(str1, Xspath);
					}
					SetWindowText(hwndSource, (LPSTR)str1);
					slen = strlen(Xspath);
					Xspath[slen++] =  '\\';
				}
				break;
			case 3:
				// Select Destination Directory (must contain session files)
				Xdpath[0] = 0; // no/empty directory name
				bi.hwndOwner = hwnd;
				bi.pidlRoot = 0;
				bi.pszDisplayName = Xdpath;
				bi.lpszTitle = "Select destination directory";
				bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
				bi.lpfn = 0;
				bi.lParam = 0;
				bi.iImage = 0;
				pidl = SHBrowseForFolder(&bi);
				if (SHGetPathFromIDList(pidl, Xdpath) == TRUE) {
					if ((i = strlen(Xdpath)) > 31) {
						strncpy(str1, Xdpath, 14);
						strcpy(str1 + 14, " ... ");
						strcpy(str1 + 19, Xdpath + i - 15);
					} else {
						strcpy(str1, Xdpath);
					}
					SetWindowText(hwndDestin, (LPSTR)str1);
					dlen = strlen(Xdpath);
					Xdpath[dlen++] =  '\\';
				}
				break;
			case 4:
				// Set the number of channels
				GetWindowText(hwndChannels, str1, GetWindowTextLength(hwndChannels) + 1);
				sscanf(str1,"%d", &nbchans);
				if ((nbchans < 1) || (nbchans > 32)) {
					MessageBox(NULL, "Invalid channel count!", NULL, MB_OK);
					nbchans = 0;
				} else {
					if (ChNamTable) free(ChNamTable);
					if ((ChNamTable = malloc(nbchans * NAMSIZ * sizeof(char))) == 0) {
						MessageBox(NULL, "Memory allocation error!", NULL, MB_OK);
						nbchans = 0;
					}
				}
				break;
			case 5:
				// Set channel characteristics
				GetWindowText(hwndChNum, str0, GetWindowTextLength(hwndChNum) + 1);
				sscanf(str0,"%d", &chan_id);
				if ((chan_id < 1) || (chan_id > nbchans)) {
					MessageBox(NULL, "Invalid channel!", NULL, MB_OK);
				} else {
					GetWindowText(hwndChName, str1, GetWindowTextLength(hwndChName) + 1);
					if (strlen(str1) > 12)  str1[12] = '\0';
					strcpy(ChNamTable + (chan_id - 1) * NAMSIZ, str1);
				}
				break;
			case 6:
				// Get channel characteristics
				GetWindowText(hwndChNum, str0, GetWindowTextLength(hwndChNum) + 1);
				sscanf(str0,"%d", &chan_id);
				if ((chan_id < 1) || (chan_id > nbchans)) {
					MessageBox(NULL, "Invalid channel!", NULL, MB_OK);
				} else {
					SetWindowText(hwndChName, (LPSTR)(ChNamTable + (chan_id - 1) * NAMSIZ));
				}
				break;
			case 7:
				// Select Source Directory (must contain .wav files)
				Spath[0] = 0; // no/empty file name
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = Spath;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(Spath);
				ofn.lpstrFilter = "Scene file\0*.scn\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrTitle = (LPCTSTR) "Select a Scene file\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST;
				//
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
					// remove filename from returned path so we can either save to
					// existing directory or newly created one
					if ((i = strlen(Spath)) > 31) {
						strncpy(str1, Spath, 11);
						strcpy(str1 + 11, " ... ");
						strcpy(str1 + 16, Spath + i - 17);
					} else {
						strcpy(str1, Spath);
					}
					SetWindowText(hwndScene, (LPSTR)str1);
				}
				if (SetNamesFromScene()) {
					MessageBox(NULL, "Error while parsing Scene file!", NULL, MB_OK);
				}
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		// Quit
		keep_running = 0;
		PostQuitMessage(0);
		for (i = 0; i < 32; i ++) if (Wfile[i]) fclose (Wfile[i]);
		if (ChNamTable) free(ChNamTable);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
#else
#endif
//
//
// Main()
//
int main(int argc, char **argv) {
#ifdef _WIN32
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmWfile = 0;
#else
	int input_intch;
#endif
	int i;
	//
	// Init structures
	strncpy(uriff.s, "riff",4);
	strncpy(uRIFF.s, "RIFF",4);
	strncpy(uwave.s, "wave",4);
	strncpy(uWAVE.s, "WAVE",4);
	strncpy(ufmt.s, "fmt ",4);
	strncpy(uFMT.s, "FMT ",4);
	strncpy(udata.s, "data",4);
	strncpy(uDATA.s, "DATA",4);
	//
	keep_running = 1;		// mainloop flag
	sampsel = 3;				// 1: 8 bits, 2: 16 bits, 3: 24 bits (default), 4: 32 bits
	ChNamTable = 0;			// table of names
	nbchans = 0;				// nb of channels to consider
	Spath[0] = 0;			// no scene
	strcpy(Xdpath, "./");	// default destination = here
	Xspath[0] = 0;			// no default source
	dlen = 2;				// string lengths for paths are global
	slen = 0;				// so they can be reused in sequential calls to "Explode"
	//
	wheader.Riff = uRIFF.i;
	wheader.wavsize = 0;
	wheader.Wave = uWAVE.i;
	wheader.fmt = ufmt.i;
	wheader.Sixteen = 16;
	wheader.One = 1;
	wheader.num_channels = 1;
	wheader.audio_samprate = 0;
	wheader.ByteRate = 0;
	wheader.wBlockAlign = 0;
	wheader.SampleBits =
	wheader.Data = udata.i;
	wheader.Dbytes= 0;
	//
	for (i = 0; i < 32; i ++) Wfile[i] = 0;
	//
#ifdef _WIN32
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmWfile);
#else
	// manage command-line parameters
	while ((input_intch = getopt(argc, argv, "d:n:c:s:w:h")) != -1) {
		switch ((char)input_intch) {
			case 'd':
				strcpy(Xdpath, optarg );
				dlen = strlen(Xdpath);
				Xdpath[dlen++] =  '/';
				break;
			case 'n':
				sscanf(optarg, "%d", &nbchans);
				if (ChNamTable) free(ChNamTable);
				if ((ChNamTable = malloc(nbchans * NAMSIZ * sizeof(char))) == 0) {
					MESSAGE("Memory allocation error!", NULL);
					nbchans = 0;
				}
				break;
			case 'c':
				if (strcmp(optarg, "8") == 0) sampsel = 1;
				if (strcmp(optarg, "16") == 0) sampsel = 2;
				if (strcmp(optarg, "32") == 0) sampsel = 4;
				break;
			case 's':
				strcpy(Spath, optarg );
				break;
			case 'w':
				sscanf(optarg, "%d,%s", &i, str1);
				if ((i < 1) || (i > 32)) {
					printf ("Invalid channel number\n");
					return -1;
				}
				i -= 1;
				if (strlen(str1) > 12) str1[12] = 0;
				strcpy(ChNamTable + i * NAMSIZ, str1);
				break;
			default:
			case 'h':
				printf("X32Xlive_Wav - ver 0.20 - ©2018 - Patrick-Gilles Maillot\n\n");
				printf("usage: X32Xlive_wav [-d dir [./]: Mono wave files path]\n");
				printf("                    [-n 1..32 [0]: number of channels to explode to mono wave files]\n");
				printf("                    [-c 8/16/24/32 [24]: sample size]\n");
				printf("                    [-s file []: optional scene file]\n");
				printf("                    [-w #,name, [,]: ch. number ',' followed by respective wave file name]\n");
				printf("                    Xlive! Session\n\n");
				printf("       X32Xlive_wav will take into account all command-line parameter and run its\n");
				printf("       'magic', generating mono-wave files from the XLive! session given as input.\n");
				printf("       Sample size conversion may take place depending on the -c option.\n");
				printf("       Channel/Wave or file names can be set all at once if a scene file is provided\n");
				printf("       using the -f parameter, or set one at a time or edited if parameters -1...-32\n");
				printf("       are used with appropriate names\n\n");
				printf("       Example:\n");
				printf("       X32Xlive_wav -n 3 -d ~ -c 16 -s ~/myscene -w 3,new_name ~/ABCD12345678\n");
				printf("       will extract as 16bit samples the first 3 channels contained in XLive! session\n");
				printf("       ABCD12345678 in the home directory, into 3 separate wave files placed in the home\n");
				printf("       directory with names taken from the X32 scene file 'myscene', and setting or overriding\n");
				printf("       the 3rd wave file name with 'new_name'\n\n");
				return(0);
			break;
		}
	}
	// run the program
	if (nbchans) {
		if (Spath[0]) {
			if (	SetNamesFromScene()) return -1;
		}
		// no confirmation, no warning, just go ahead... the magic of CLI :)
		for (i = 0; i < nbchans; i++) printf("ch: %d, filename: %s\n", i, ChNamTable + i * NAMSIZ);
		if (argv[optind]) {
			strcpy(Xspath, argv[optind]);
			return (ExplodeWavFile());
		} else {
			printf("no session directory!\n");
		}
	} else {
		printf("no-op\n");
	}
#endif
	//
	return 0;
}
//
//
//
// Read Scene, Parse File and set Channel Names from it
//
int	SetNamesFromScene() {
//
int				i, j, k;
char			line[MAXLR];
char			*rs;
FILE			*Sfile;
//
	// scene file is in Spath[]
	if ((Sfile = fopen(Spath, "r")) == NULL) {
		MESSAGE(Spath, "Error opening Scene File");
		return 1;
	}
	for (i = 0; i < nbchans; i++) {
		line[0] = 0;
		sprintf(str1, "/ch/%02d/confi", i + 1);
		// read to the next scene item of interest
		while ((strncmp(line, str1, 12)) != 0) {
			if ((rs = fgets(line, MAXLR, Sfile)) == NULL) {
				MESSAGE(Spath, "Error reading Scene File");
				fclose (Sfile);
				return 1;
			}
		}
		// we're now pointing to the right item
		// i.e. /ch/xx/config "sssss" d SS d
		// extract the sssss portion of the line and save it at the right place
		k = 15;
		// limit string length to 12 chars (per X32 limits)
		if ((j = strlen(line)) > 15 + 12) j = 15 + 12;
		while ((k < j) && (line[k] != '"')) k++;
		line[k] = 0;
		strcpy(ChNamTable + i * NAMSIZ, line + 15);
	}
	fclose (Sfile);
	return 0;
}
//
//
// Explode function
//
//
int	ExplodeWavFile() {
//
int				i, k, l, m, n, op;
int				fnum, n8_16_32;
unsigned int		src_samples;			// nb of audio samples in src file
i4chr			sdata[OP2 * 32];		// to accommodate to up to multiple of OP2
unsigned char	ob1[OP2 * 3];		// 16 and 24bit temp buffer
unsigned int		obuf[OP2];			// 32bit temp buffer
unsigned int		totalSize;			// final chuncksizze bytes (per single file), max = 4GB
unsigned int		totalBytes;			// final number of bytes (per single file), max = 4GB
FILE				*Sfile;

	//
	// prepare data elements for output files; preserve lengths from one
	// call to the next
	fnum = 1;
	strcpy(Xspath + slen, "00000001.wav");
	if ((Sfile = fopen(Xspath, "rb")) == NULL) {
		MESSAGE(Xspath, "Error opening session File");
		return 1;
	}
	// open first source wave files as needed
	fread(&rheader, sizeof(rheader), 1, Sfile);
	wheader.audio_samprate = rheader.audio_samprate;
	wheader.ByteRate       = rheader.audio_samprate * sampsel;
	n8_16_32               = rheader.num_channels;
	// skip the input file JUNK section and set data length to be read
	for (k = 0; k < rheader.Dbytes / 4; k++) {
		fread(sdata, 4, 1, Sfile);
	}
	ftime (&start);
	//
	wheader.wBlockAlign = sampsel;
	wheader.SampleBits = 8 * sampsel;
	wheader.wavsize = 0;				// updated at the end of the process
	wheader.Dbytes  = 0;				// updated at the end of the process
	// create output files in directory Xdpath[]
	for (i = 0; i < nbchans; i++) {
		strcpy(Xdpath + dlen, ChNamTable + NAMSIZ * i);
		strcpy(Xdpath + strlen(Xdpath), ".wav");
		if ((Wfile[i] = fopen(Xdpath, "wb")) == NULL) {
			MESSAGE(Xdpath, "Error creating File");
			return 1;
		}
		// output files are ready... fill them out!
		// Read input file while there are data to read and write to
		// separate destination files
		// write output wav file header
		fwrite(&wheader, sizeof(wheader), 1, Wfile[i]);
	}
	//init global data counters (unsigned 32bits = 4GB max / standard .wav file format)
	totalBytes = totalSize = 0;
	do {
		//
		// we loop through this do... while loop as long as there are multi-channel wave files
		// available to be processed for the session.
		// note: No provision/test is made for the case where resulting file sizes would
		//       exceed 4GB or 8 hours or audio at 48kHz/24bits
		//
		// next chunk should be "data" == 0x61746164
		fread(sdata, 4, 1, Sfile);
		if (sdata[0].i != 0x61746164) {
			MESSAGE("Error reading source file!", NULL);
			return 1;
		}
		// read data length
		fread(&src_samples, 4, 1, Sfile);
		// evaluate the chunk of data that can be read for 1 sample, all channels
		src_samples = src_samples / n8_16_32 / 4; // always reading 32bit samples
		//
		// update total number of bytes
		totalBytes += src_samples * sampsel;
		//
		// Optimizations
		// this section [for(){for(){}}] optimizes operations in limiting the number of calls
		// to fwrite() by keeping records in memory and writing them in chunks of op samples
		// rather than one at a time.
		//       it seems that OP2 = 8 or 16 is already quite effective. no better results
		//       with OP2 greater than these values
		op = OP2;
		while ((src_samples & (op - 1)) != 0) op /= 2;
		// manage multiple (OP2) 32bit samples at a time depending on the sample size
		switch (sampsel) {
		case 4:									// generate 32bit samples
			n = 4 * op;
			for (k = 0; k < src_samples; k += op) {
				// read op 32bit source samples at a time
				fread(sdata, n8_16_32 * n, 1, Sfile);
				// write requested samples to output file(s)
				for (l = 0; l < nbchans; l++) {
					for (m = 0; m < op; m++) {
						obuf[m] = sdata[n8_16_32 * m + l].i;
					}
					fwrite(obuf, n, 1, Wfile[l]);
				}
			}
			break;
		case 3:									// generate 24bit samples
			for (k = 0; k < src_samples; k += op) {
				// read op 32bit source samples at a time
				fread(sdata, n8_16_32 * 4 * op, 1, Sfile);
				// write requested samples to output file(s)
				for (l = 0; l < nbchans; l++) {
					for (m = 0, n= 0; m < op; m++) {
						ob1[n++] = sdata[n8_16_32 * m + l].s[1];
						ob1[n++] = sdata[n8_16_32 * m + l].s[2];
						ob1[n++] = sdata[n8_16_32 * m + l].s[3];
					}
					fwrite(ob1, n, 1, Wfile[l]);
				}
			}
			break;
		case 2:									// generate 16bit samples
			for (k = 0; k < src_samples; k += op) {
				// read op 32bit source samples at a time
				fread(sdata, n8_16_32 * 4 * op, 1, Sfile);
				// write requested samples to output file(s)
				for (l = 0; l < nbchans; l++) {
					for (m = 0, n= 0; m < op; m++) {
						ob1[n++] = sdata[n8_16_32 * m + l].s[2];
						ob1[n++] = sdata[n8_16_32 * m + l].s[3];
					}
					fwrite(ob1, n, 1, Wfile[l]);
				}
			}
			break;
		case 1:									// generate 8bit samples
			// want to know how bad it is on 8 bits? :)
			for (k = 0; k < src_samples; k += op) {
				// read op 32bit source samples at a time
				fread(sdata, n8_16_32 * 4 * op, 1, Sfile);
				// write requested samples to output file(s)
				for (l = 0; l < nbchans; l++) {
					for (m = 0, n= 0; m < op; m++) {
						ob1[n++] = sdata[n8_16_32 * m + l].s[3];
					}
					fwrite(ob1, n, 1, Wfile[l]);
				}
			}
			break;
		}

// all case code - no optimization
//			for (k = 0; k < src_samples; k++) {
//				// read 1 32bit source sample at a time
//				fread(sdata, n8_16_32 * 4, 1, Sfile);
//				// write requested samples to output file(s)
//				for (l = 0; l < nbchans; l++) {
//					fwrite(&(sdata[l].s[4 - sampsel]), sampsel, 1, Wfile[l]);
//				}
//			}
		//
		// done reading data bytes from current source file
		fclose (Sfile);
		// another to read?
		fnum += 1;
		sprintf(Xspath + slen, "%08x.wav", fnum);
		if ((Sfile = fopen(Xspath, "rb")) != NULL){
			fread(&theader, sizeof(theader), 1, Sfile);
			// Pass the JUNK section of the input file and set data length to be read
			for (k = 0; k < theader.Dbytes / 4; k++) {
				fread(sdata, 4, 1, Sfile);
			}
		}
	} while (Sfile);
	// close Wfiles
	totalSize = totalBytes + 36;
	for (i = 0; i < nbchans; i++) {
		// update destination files' headers
		fflush(Wfile[i]);
		if (fseek(Wfile[i], 4, SEEK_SET) != 0) {
			MESSAGE("Error Seek 4!", NULL);
			return 1;
		}
		fwrite(&totalSize, 4, 1, Wfile[i]);
		fflush(Wfile[i]);
		if (fseek(Wfile[i], 40, SEEK_SET) != 0) {
			MESSAGE("Error Seek 40!", NULL);
			return 1;
		}
		fwrite(&totalBytes, 4, 1, Wfile[i]);
		fflush(Wfile[i]);
		fclose(Wfile[i]);
	}
	//
	// Done!, calculate and display elapsed time
	ftime (&end);
	k = (int)(1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
//	sprintf(str1, "Elapsed time: %dms, op level: %d", k, op);
	sprintf(str1, "Elapsed time: %dms", k);
	MESSAGE(str1, "Done!");
	return 0;
}

