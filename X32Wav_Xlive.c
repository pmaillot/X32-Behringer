/*
 * X32Wav_Xlive.c
 *
 *  Created on: Sep 13, 2017
 *
 * ©2017 - Patrick-Gilles Maillot
 *
 * X32Wav_Xlive - a Windows application for merging single WAV files into X-Live! compatible
 * 			   multi-channel WAV files
 *
 * 			   input WAV files must be all the same size, with contiguous names ch_1 to ch_32,
 * 			   all 24bits, and all with the same sample rate (44100 or 48000)
 *
 * 			   A directory based on the creation time-stamp is created, and a number of
 * 			   multi-channel 32bits PCM WAV files 00000001.wav to 00000128.wav are created
 * 			   each 4GBytes max, along with a description file named se_log.bin
 *
 *	ver 0.80: initial release - non Windows, no particular optimization
 *	ver 0.90: initial release - Windows
 *	ver 0.91: fixes in different areas: date-code, and markers
 *	ver 0.92: got away from large memory block allocation; slower but can handle larger files
 *	ver 0.93: optimizations in fwrite use
 *	ver 0.94: initialization of a variable (fill_chls) was forgotten
 *	ver 0.95: limiting session name to 19 chrs + \0
 *	ver 0.96: .wav files in X-Live! sessions are base16 naming based: ..09, 0A, 0B..0F, 10,..
 *	ver 0.97: added call to SetCurrentDirectory to ensure the current directory is set
 *	ver 0.98: listed Wave files in File open dialog
 *	ver 0.99: fixed remaining size of data for last session wav file (typically not 4GB)
 *	ver 1.00: Better GUI interface (directory & marker select) and small bug in logging markers
 *	ver 1.10: Added command line interface
 *	ver 1.20: removed malloc support (not used) / accepts Pro-tools generated files
 *	ver 1.21: enabled no session name (so defaults to time stamp, as on X32)
 *	ver 1.22: Improved a few calls to write()
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>	// more precise timing functions
#ifdef _WIN32
#include <winsock2.h>	// Windows functions for std GUI & sockets
#include <Shlobj.h>		// Windows shell functions
#define MESSAGE(s1,s2)	\
			MessageBox(NULL, s1, s2, MB_OK);
#define zeromem(a1, a2) \
		ZeroMemory(a1, a2);
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MAX_PATH 256	// file name/path size
#define MESSAGE(s1,s2)	\
			printf("%s - %s\n",s2, s1);
#define zeromem(a1, a2) \
		memset((void *)a1, 0, a2);
#define min(a,b) 		\
			(((a)<(b))?(a):(b))
#endif
//
#define riff 0x66666972
#define RIFF 0x46464952
#define wave 0x65766177
#define WAVE 0x45564157
#define fmt  0x20746D66
#define FMT  0x20544D46
#define junk 0x6B6E756A
#define JUNK 0x4B4E554A
#define data 0x61746164
#define DATA 0x41544144
#define bext 0x74786562
#define BEXT 0x54584542
#define minf 0x666E696D
#define MINF 0x464E494D
#define elm1 0x316D6C65
#define ELM1 0x314D4C45
#define BLNK 0x20202020
//
#define FOUR 4 // sizeof(unsinged int), for 4 bytes
//
typedef union {
	char			s[FOUR];
	unsigned int	i;
} i4chr;
//
// Private functions
int	MergeWavFiles(int num_markers, float* markers);
//
#ifdef _WIN32
// Windows Declarations
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE		hInstance = 0;
HWND			hwndInDir, hwndSource, hwndSession, hwndMarkers, hwndMerge;
HWND			hwndPathFile, hwndPathList;
HFONT			hfont, htmp;
HDC				hdc, hdcMem;
HBITMAP			hBmp;
BITMAP			bmp;
PAINTSTRUCT		ps;
MSG				wMsg;
OPENFILENAME	ofn;       				// common dialog box structure
BROWSEINFO 		bi;						// Windows Shell structure
ITEMIDLIST 		*pidl;					// dir item list
#endif
char			Xspath[MAX_PATH];		// file path, used for directory where to source wav files
char			Mpath[MAX_PATH];		// Marker file path, or markers
//
#define 	max_take_size  	4294901760	// 4GB - 32KB - 32KB header (0xFFFF0000)
FILE			*Xin[32];				// input files
FILE			*Xout;					// output file
//
i4chr			sample;					// wav sample from 24 bits to 32 bits
int				slen;					// string length used with directory Xspath[]
int				keep_running = 1;		// main loop control
int				wWidth = 550;			// Default window size
int				wHeight = 158;			//
char			str1[64];				// used for Windows strings conversions
char			str2[1024];				// used for Windows strings conversions
//
float			marker_vec[101];		// array for markers (set from user)
unsigned int	take_size[256];			// max 256 takes; this is their individual size
unsigned int	imarker_vec[101];		// array for markers as ints (set from user * by sample rate)
unsigned int	file_size[32];			// size of each source wav file
unsigned int	data_size[32];			// size of the data payload of each wav file
unsigned int	wav_samp_rate[32];		// sample rate of each wav file
unsigned int	session_name;			// session name built from yymmhhmm in hex form (on 32 bits)
unsigned int	audio_len;				// data size; ensuring a single value
unsigned int	fmt_size;				// size of "fmt " sub chunk
unsigned int	dwAvgBytesPerSec;		// average bytes per second (all channels)
unsigned int	samp_rate;				// sample rate (48000 or 44100)
unsigned int	TmpData;				// used for fast char[4] comparisons (using a 32bit word)
unsigned short	wav_format;				// WAVE format (expected to be PCM = 1)
unsigned short	wBlockAlign;			// block alignment
unsigned short	bits_per_samp;			// bits per sample (24)
unsigned short	wav_chs;				// numb of channels os source file (expected to be 1)
unsigned int 	Chunk;					// 32 bits just read
unsigned int	JunkSize;				// size in bytes of Junk chunk
//
time_t			now;					// time
struct tm 		*current_time;			// year...second structure
struct timeb	start,end;				// precise timers (well... to the ms)
int				nb_takes;				// number of takes (1...256); sizes in take_size[i]
int				no_markers;				// number of Markers
unsigned int	total_length;			// length in # of samples
unsigned int	x_size;					// size of unknown sub-chunk
//
struct {								// 32k header (0x8000 in size)
	unsigned int	Riff;				// = "Riff";
	unsigned int	wavsize; 			// size of WAVE sub-chunk
	unsigned int	Wave; 				// = "WAVE"
	unsigned int	ufmt;				// = "fmt ";
	unsigned int	Sixteen;			// = 16;
	unsigned short	One;				// = 1;
	unsigned short	num_channels;		// number of channels in multichannel data (8, 16 or 32)
	unsigned int	audio_samprate;		// sample rate
	unsigned int	dwAvgBytesPerSec;	// average bytes per second
	unsigned short	wBlockAlign;		// alignment
	unsigned short	Thirtytwo;			// = 32;
	unsigned int	Junk;				// = "JUNK";
	unsigned int	Junk_bytes;			// = 32716, or (1024*32) - 52
} wheader;
//
//unsigned int 	wavsize;				// size of WAVE sub-chunk
int				MList_File;				// Default (1) is "List of Markers"; (0) is "File Path"
unsigned int	Zero[30] = {0};			// 0 (30 times)
unsigned int	Data = data;			// "data" on one unsigned int
unsigned int	Blnk = BLNK;			// "    " on one unsigned int
//
long long		audio_bytes;			// size of audio data (multi channels)
unsigned int	r_audio_bytes;			// audio data remainder on 32bits for file writing
#ifdef _WIN32
//
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdFile) {
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
		L"X32Wav_Xlive - Merge WAV files into X-Live! multichannel WAV files",
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
			Sleep(10);					// avoid too high a processor use when nothing to do
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
	int		i, j;
	//
	switch (msg) {
	case WM_CREATE:
		//
		hwndMerge = CreateWindow("button", "MERGE",
				WS_VISIBLE | WS_CHILD, 432, 55, 110, 45, hwnd, (HMENU )1, NULL, NULL);

		hwndInDir = CreateWindow("button", "Session Directory",
				WS_VISIBLE | WS_CHILD, 128, 25, 130, 20, hwnd, (HMENU )2, NULL, NULL);
		hwndSource = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 265, 25, 277, 20, hwnd, (HMENU )0, NULL, NULL);

		hwndPathList = CreateWindow("button", "List of Markers",
				WS_VISIBLE | WS_CHILD, 218, 80, 110, 20, hwnd, (HMENU )3, NULL, NULL);

		hwndPathFile = CreateWindow("button", "Marker File",
				WS_VISIBLE | WS_CHILD, 333, 80, 95, 20, hwnd, (HMENU )4, NULL, NULL);

		hwndSession = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 218, 55, 210, 20, hwnd,
				(HMENU )0, NULL, NULL);

		hwndMarkers = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 128, 105, 413, 20, hwnd,
				(HMENU )0, NULL, NULL);

		hBmp = (HBITMAP)LoadImage(NULL,(LPCTSTR)"./sdcard.bmp", IMAGE_BITMAP, 0, 0,
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
		BitBlt(hdc, 2, 3, 240, 240, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);

		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 128, 3, str1, wsprintf(str1, "X32Wav_Xlive - ver 1.22 - ©2017-18 - Patrick-Gilles Maillot"));
		TextOut(hdc, 128, 57, str1, wsprintf(str1, "Session Name:"));
		TextOut(hdc, 128, 90, str1, wsprintf(str1, "Markers:"));
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
				// MERGE button clicked!
				if (Xspath[0]) {
					// read Session name
					GetWindowText(hwndSession, str1, GetWindowTextLength(hwndSession) + 1);
					// limit session name to 16 chars
					str1[min(16, strlen(str1))] = '\0';
					// manage markers
					GetWindowText(hwndMarkers, str2, GetWindowTextLength(hwndMarkers) + 1);
					if (MList_File) {
						// Markers are given as a list of floats
						j = 0;
						for (no_markers = 0; no_markers < 100; no_markers++) {
							if ((i = (sscanf(str2 + j,"%f", &(marker_vec[no_markers])))) != EOF) {
								while ((str2[j] == ' ') || (str2[j] == '.') || ((str2[j] > '/') && (str2[j] < ':'))) j++;
								j++;
							} else break;
						}
					} else {
						// Markers to be read from file containing floats
						if ((Xout = fopen(str2, "r")) != NULL) {
							while ((i = (fscanf(Xout,"%f", &(marker_vec[no_markers])))) != EOF) {
								no_markers += 1;
							}
							fclose(Xout);
						} else {
							no_markers = 0;
							MESSAGE("Can't find Markers File", NULL);
						}
					}
					marker_vec[no_markers] = 0.0;	// the last marker must be 0.0
					//
					// launch Merge!
					if ((i = MergeWavFiles(no_markers, marker_vec)) > 0) {
						sprintf(str1, "Elapsed time: %dms", i);
						MESSAGE(str1, "Done!");
					} else {
						MESSAGE("Something went wrong!", NULL);
					}
				}
				break;
			case 2:
				// Select Source Directory (must contain .wav files)
				// Select Source Directory (must contain session files)
				ZeroMemory(Xspath, sizeof(Xspath));
				ZeroMemory(&bi, sizeof(bi));
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
				// Set markers to list (text in the reserved area)
				MList_File = 1;
				break;
			case 4:
				// Select marker file
				ZeroMemory(Mpath, sizeof(Mpath));
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = Mpath;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(Mpath);
				ofn.lpstrFilter = "Marker file\0*.txt\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrTitle = (LPCTSTR) "Select a Marker file\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST;
				//
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
					// remove filename from returned path so we can either save to
					// existing directory or newly created one
					if ((i = strlen(Mpath)) > 55) {
						strncpy(str1, Mpath, 20);
						strcpy(str1 + 20, " ... ");
						strcpy(str1 + 25, Mpath + i - 35);
					} else {
						strcpy(str1, Mpath);
					}
					SetWindowText(hwndMarkers, (LPSTR)str1);
					// Set markers to file
					MList_File = 0;
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
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
#endif
//
//
int main(int argc, char **argv) {
#ifdef _WIN32
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmdFile = 0;
#else
	int input_intch, i;
#endif
	zeromem(take_size, sizeof(take_size));
	zeromem(imarker_vec, sizeof(imarker_vec));
	zeromem(str1, sizeof(str1));
	//
	// Init structures
	// some initializations needed
	MList_File = 1;						// Default is "List of Markers"
	no_markers = 0;
	wheader.Riff = RIFF;
	wheader.wavsize = 0;
	wheader.Wave = WAVE;
	wheader.ufmt = fmt;
	wheader.Sixteen = 16;
	wheader.One = 1;
	wheader.num_channels = 0;
	wheader.audio_samprate = 0;
	wheader.dwAvgBytesPerSec = 0;
	wheader.wBlockAlign = 0;
	wheader.Thirtytwo = 32;
	wheader.Junk = JUNK;
	wheader.Junk_bytes= 32716;
	//
#ifdef _WIN32
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
	//
#else
	// manage command-line parameters
	while ((input_intch = getopt(argc, argv, "f:m:h")) != -1) {
		switch ((char)input_intch) {
			case 'f':
				strcpy(str2, optarg);
				// Markers to be read from file containing floats
				if ((Xout = fopen(str2, "r")) != NULL) {
					while ((i = (fscanf(Xout,"%f", &(marker_vec[no_markers])))) != EOF) {
						no_markers += 1;
					}
					fclose(Xout);
				} else {
					no_markers = 0;
					MESSAGE(NULL, "Can't find Markers File");
				}
				break;
			case 'm':
				sscanf(optarg, "%f", &marker_vec[no_markers++]);
				break;
			default:
			case 'h':
				printf("X32Wav_Xlive - ver 1.22 - ©2018 - Patrick-Gilles Maillot\n\n");
				printf("usage: X32Wav_Xlive [-f Marker file []: file containing markers]\n");
				printf("                    [-m marker, [,]: marker time in increasing order values]\n");
				printf("                    <Session dir> [<Session name>]\n\n");
				printf("       X32Wav_Xlive will take into account all command-line parameters and run its\n");
				printf("       'magic', generating XLive! session files from the wav data given as input.\n\n");
				printf("       Many restrictions take place: all wav data must be similar in specs:\n");
				printf("         - same sample rate (48k or 44.1k\n");
				printf("         - same length\n");
				printf("         - 24bit sample size\n");
				printf("         - wav files have to be named ch_1.wav to ch_32.wav\n\n");
				printf("       Examples:\n");
				printf("       X32Wav_Xlive -m 1.2 -m 15 ./ \"new session\"\n");
				printf("         creates as XLive! session directory in ./ based on the date and time,\n");
				printf("         and creates a session displayed as \"new session\" when loaded into XLive! card\n");
				printf("         containing a number of multi-channel wav files respective of the number of \n");
				printf("         channels found in the source directory. Markers, if present, will be added to\n");
				printf("         the created session\n\n");
				printf("       X32Wav_Xlive .\n");
				printf("         creates an XLive! session directory in . based on the date and time;\n");
				printf("         the session is named after as its creation time stamp when loaded into XLive! card\n");
				printf("         and contains a number of multi-channel wav files respective of the number of \n");
				printf("         channels found in the source directory.\n");
				return(0);
			break;
		}
	}
	// run the program
		// no confirmation, no warning, just go ahead... the magic of CLI :)
	if (argv[optind]) {
		strcpy(Xspath, argv[optind]);
		slen = strlen(Xspath);
		Xspath[slen++] = '/';
		if (argv[++optind]) strcpy(str1, argv[optind]);
		if ((i = MergeWavFiles(no_markers, marker_vec)) > 0) {
			sprintf(str1, "Elapsed time: %dms", i);
			MESSAGE(str1, "Done!");
		} else {
			MESSAGE("Something went wrong!", NULL);
		}
		return (i);
	} else {
		printf("no-op\n");
	}
#endif
return 0;
}
//
//
int	MergeWavFiles(int num_markers, float* markers) {
	int				numb_chls;
	int				fill_chls;			// numb_chls + fill_chls = 32
	int				i, j;
	unsigned int 	k, PassNumber;
	//
	//
	ftime (&start);
	numb_chls = 0;
	for (i = 0; i < 32; i++) {
		sprintf(Xspath + slen, "./ch_%d.wav", i + 1);
		if ((Xin[i] = fopen(Xspath, "rb")) == NULL) {
			numb_chls = i;
			break;
		}
	}
	if (numb_chls == 0) {
		MESSAGE("No wave files found!", NULL);
		return -1;
	}
	//calc fill channels, in case the number of wav files does not match 8, 16 or 32
	if (numb_chls <= 8) {
		fill_chls = 8 - numb_chls;
	} else if (numb_chls <= 16) {
		fill_chls = 16 - numb_chls;
	} else if (numb_chls <= 32) {
		fill_chls = 32 - numb_chls;
	} else {
		numb_chls = 32;	// limit to 32 channels
		fill_chls = 0;
		printf("More than 32 channels found, exceeding channels will be ignored!\n");
	}
	// read wav files
	for (i = 0; i < numb_chls; i++) {
		//
		// Parse header (36 or more bytes) example below for 16bits/44.1kHz audio
		// size 	Sample Value 	Description
		// 4 	"Riff" 	Marks the file as a Riff file. Characters are each 1 byte long.
		// 4 	File size (integer) 	Size of the overall file - 8 bytes, in bytes (32-bit integer). Typically, you'd fill this in after creation.
		// 4 	"WAVE" 	File Type Header. For our purposes, it always equals "WAVE".
		//
		// also:
		// 4 	"fmt " 	Format chunk marker. Includes trailing null
		// 4  	Length of fmt data
		// 2 	1 	Type of format (1 is PCM) - 2 byte integer
		// 2 	2 	Number of Channels - 2 byte integer
		// 4 	44100 	Sample Rate - 32 byte integer. Common values are 44100 (CD), 48000 (DAT). Sample Rate = Number of Samples per second, or Hertz.
		// 4 	176400 	(Sample Rate * BitsPerSample * Channels) / 8.
		// 2 	4 	(BitsPerSample * Channels) / 8. (1: 8 bit mono)(2: 8 bit stereo/16 bit mono)(4: 16 bit stereo)
		// 2 	16 	Bits per sample
		//
		// also:
		// 4	"junk", "bext", "elm1", "minf"
		// 4	if junk junk size -> n
		//
		// also:
		// 4 	"data" 	"data" chunk header. Marks the beginning of the data section.
		// 4 	File size (data) 	Size of the data section.
		fread(&Chunk, sizeof(Chunk), 1, Xin[i]);					// Chunk: RIFF/riff?
		if ((Chunk != RIFF) && (Chunk != riff)) {
			sprintf(str1, "ch_%d.wav is not a Riff file!\n", i + 1);
			MESSAGE(str1, NULL);
			return -1;
		}
		fread(&file_size[i], FOUR, 1, Xin[i]);		// size
		k = 8;
		while (k) {
			fread(&Chunk, sizeof(Chunk), 1, Xin[i]);				// Chunk
			switch (Chunk) {
			case WAVE: case wave:									// wave
				k += 4;												// skip word
				break;
			case FMT: case fmt:										// fmt
				fread(&fmt_size, sizeof(fmt_size), 1, Xin[i]);		// fmt chunk size
				fread(&wav_format, sizeof(wav_format), 1, Xin[i]);	// wave format: PCM
				if (wav_format != 1) {
					sprintf(str1, "ch_%d.wav WAV format not supported!\n", i + 1);
					MESSAGE(str1, NULL);
					return -1;
				}
				fread(&wav_chs, sizeof(wav_chs), 1, Xin[i]);		// # of channels
				if (wav_chs != 1) {
					sprintf(str1, "multichannels ch_%d.wav not supported\n", i + 1);
					MESSAGE(str1, NULL);
					return -1;
				}
				fread(&samp_rate, sizeof(samp_rate), 1, Xin[i]);				// sample rate
				if ((samp_rate != 44100) && (samp_rate != 48000)) {
					sprintf(str1, "ch_%d.wav WAV sample rate not supported!\n", i + 1);
					MESSAGE(str1, NULL);
					return -1;
				}
				wav_samp_rate[i] = samp_rate;
				fread(&dwAvgBytesPerSec, sizeof(dwAvgBytesPerSec), 1, Xin[i]);	// av. bytes per sec.
				fread(&wBlockAlign, sizeof(wBlockAlign), 1, Xin[i]);			// alignment
				fread(&bits_per_samp, sizeof(bits_per_samp), 1, Xin[i]);		// bits per sample
				if (bits_per_samp != 24) {
					sprintf(str1, "ch_%d.wav WAV bit resolution not supported!\n", i + 1);
					MESSAGE(str1, NULL);
					return -1;
				}
				k += (fmt_size + 8);						// evaluate data to skip
				fseek(Xin[i], k, SEEK_SET);					// Jump to next chunk
				break;
			case JUNK: case junk: case BEXT: case bext:
			case MINF: case minf: case ELM1: case elm1:		// don't care cases
				fread(&JunkSize, sizeof(JunkSize), 1, Xin[i]);
				k += (JunkSize + 8);						// evaluate data to skip
				fseek(Xin[i], k, SEEK_SET);					// Jump to next chunk
				break;
			case DATA: case data:							// "data"
				fread(&data_size[i], sizeof(data_size[i]), 1, Xin[i]);	// read data chunk size
				k = 0;										// stop parsing
				break;
			default:
				sprintf(str1, "ch_%d.wav is not a WAVE file!\n", i + 1);
				MESSAGE(str1, NULL);
				return -1;
				break;
			}
		}
	}
	//
	//	check that all files are of equal lengths and sample rates
	audio_len = data_size[0];
	wheader.audio_samprate = wav_samp_rate[0];
	for (i = 0; i < numb_chls; i++) {
		if (data_size[i] != audio_len) {
			MESSAGE("files are not the same length!\n", NULL);
			return -1;
		}
		if (wav_samp_rate[i] != wheader.audio_samprate) {
			MESSAGE("files are not the same sample rate!\n", NULL);
			return -1;
		}
	}
	//
	// create X-Live session parameters (session name based on time)
	time(&now);
	current_time = gmtime(&now);
    session_name = ((current_time->tm_year - 80) << 25) | ((current_time->tm_mon+1) << 21) |
    				(current_time->tm_mday << 16) | (current_time->tm_hour << 11) |
					(current_time->tm_min << 5) | (current_time->tm_sec / 2);
    //
    wheader.num_channels = numb_chls + fill_chls;
	total_length = audio_len  / 3;    				// length in # of samples
	//
	audio_bytes = (long long)total_length * FOUR * (long long)wheader.num_channels;	// total number of bytes to process
	//
	// trim to 32kB boundary
	audio_bytes &= 0xfffffffffff8000;
	//
	// How many files of 4GB (max) for our session?
	nb_takes = 0;
	while (audio_bytes >= max_take_size) {
		take_size[nb_takes] = max_take_size / FOUR;	// in 32bit samples
		nb_takes += 1;
		audio_bytes -= max_take_size;
	}
	// deal with the remaining data
	r_audio_bytes = audio_bytes;					// remaining audio data for a possible last take
	if (r_audio_bytes > 0) {
		take_size[nb_takes] = r_audio_bytes / FOUR;	// size of last take in samples
		nb_takes += 1;
	}
	//
	// update markers with sample rate
	for (i = 0; i < num_markers; i++) {
		imarker_vec[i] = (int)(marker_vec[i] * wheader.audio_samprate);
	}
	imarker_vec[i] = 0;								// last marker must be zero
	//
	// almost there!
	// creating session directory, log file, and one or more wave files
	sprintf(Xspath + slen, "%08X", session_name);
#ifdef _WIN32
	if (CreateDirectory(Xspath, NULL)) {
#else
	if (mkdir(Xspath, 0777) == 0) {
#endif
		strcat(Xspath + strlen(Xspath), "/SE_LOG.BIN");
		if ((Xout = fopen(Xspath, "wb")) != NULL) {
			fwrite(&session_name, FOUR, 1, Xout);
			j = wheader.num_channels;
			fwrite(&j, FOUR, 1, Xout);
			fwrite(&wheader.audio_samprate, FOUR, 1, Xout);
			fwrite(&session_name, FOUR, 1, Xout);
			fwrite(&nb_takes, FOUR, 1, Xout);
			fwrite(&num_markers, FOUR, 1, Xout);
			fwrite(&total_length, FOUR, 1, Xout);
			// write take size data
			for (i = 0; i < nb_takes; i++)
				fwrite(&take_size[i], sizeof(int), 1, Xout);
			for (; i < 256; i++)
				fwrite(Zero, FOUR, 1, Xout);
			//write marker data
			for (i = 0; i < num_markers; i++)
				fwrite(&imarker_vec[i], FOUR, 1, Xout);
			for (; i < 125; i++)
				fwrite(Zero, FOUR, 1, Xout);
			// write session name (16 chars max)
			// session name is in str1 (global var)
			// if no session name, write null bytes from str1
			fwrite(str1, 16, 1, Xout);
			// and fill out to 2k bytes with zeros, using zero[32], all 0.
			fwrite(Zero, 120, 1, Xout);
			fwrite(Zero, 120, 1, Xout);
			fwrite(Zero, 120, 1, Xout);
			fwrite(Zero, 120, 1, Xout);
			//
			fclose(Xout);
			//
			// create take waves
			for (i = 0; i < nb_takes; i++) {
				sprintf(Xspath + slen + 8, "/%08X.wav", i + 1);
				if ((Xout = fopen(Xspath, "wb")) != NULL) {
					wheader.wavsize = take_size[i] * 4 + 44 + wheader.Junk_bytes;
					wheader.wBlockAlign = wheader.num_channels * 4;
					wheader.dwAvgBytesPerSec = wheader.audio_samprate * wheader.wBlockAlign;
					// write wave header
					fwrite(&wheader, sizeof(wheader), 1, Xout);
					for (j = 0; j < 460 / FOUR; j++) {
						fwrite(&Blnk, FOUR, 1, Xout);
					}
					r_audio_bytes = take_size[i] * 4;
					fwrite(&Data, FOUR, 1, Xout);
					fwrite(&r_audio_bytes, FOUR, 1, Xout);
					for (j = 0; j < (wheader.Junk_bytes - 468) / 4; j++) {
						fwrite(&Blnk, 4, 1, Xout);
					}
					fwrite(&Data, FOUR, 1, Xout);
					fwrite(&r_audio_bytes, FOUR, 1, Xout);
					//
					// writing new multichannel wav data
					sample.s[0] = 0;
					PassNumber = take_size[i] / wheader.num_channels;
					if (fill_chls){
						for (k = 0; k < PassNumber; k++) {
							for (j = 0; j < numb_chls; j++) {
								fread(&sample.s[1], 3, 1, Xin[j]);	// read 3 bytes audio sample
								fwrite(&sample.i, FOUR, 1, Xout);
							}
							// Complete number of channels with 0's
							fwrite(Zero, FOUR * fill_chls, 1, Xout);
						}
					} else {
						for (k = 0; k < PassNumber; k++) {
							for (j = 0; j < numb_chls; j++) {
								fread(&sample.s[1], 3, 1, Xin[j]);	// read 3 bytes audio sample
								fwrite(&sample.i, FOUR, 1, Xout);
							}
						}
					}
					fclose(Xout);
				} else {
					MESSAGE("Cannot create session wav file!\n", NULL);
				}
			}
			// All done hopefully without errors
			ftime (&end);
			k = (int)(1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
		} else {
			MESSAGE("Cannot create session log file!\n", NULL);
			k = -1;
		}
	} else {
		MESSAGE("Cannot create session directory!\n", NULL);
		k = -1;
	}
	//
	for (i = 0; i < numb_chls; i++) {
		fclose(Xin[i]);
	}
	//
	return k;
}
