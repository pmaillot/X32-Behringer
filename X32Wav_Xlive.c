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
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>	// more precise timing functions
#include <winsock2.h>	// Windows functions for std GUI & sockets
#include <commdlg.h>	// Windows widgets functions (file handling..)
//
//
//#define Use_malloc	// comment if not using large malloc blocks (recommended);
						// set, to use malloc blocks which make the program faster, but may
						// require 64bits compile and a large amount of available RAM
//
typedef union {
	char			s[4];
	unsigned int	i;
} i4chr;
//
i4chr	uRIFF, uriff;
i4chr	uFMT, ufmt;
i4chr	uWAVE, uwave;
i4chr	uJUNK, ujunk;
i4chr	uDATA, udata;
//
// Private functions
int	MergeWavFiles(char* name_str, int num_markers, float* markers);
//
// Windows Declarations
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE		hInstance = 0;
HWND			hwndDirectory, hwndSession, hwndMarkers, hwndMerge, hwndPathList;
HFONT			hfont, htmp;
HDC				hdc, hdcMem;
HBITMAP			hBmp;
BITMAP			bmp;
PAINTSTRUCT		ps;
MSG				wMsg;
OPENFILENAME	ofn;       				// common dialog box structure
char			Xpath[128];				// file path, used for directory where to source wav files
//
#define 	max_take_size  	4294901760	// 4GB - 32KB - 32KB header
FILE			*Xin[32];				// input files
FILE			*Xout;					// output file
//
i4chr			sample;					// wav sample from 24 bits to 32 bits
int				keep_running = 1;		// main loop control
int				wWidth = 550;			// Default window size
int				wHeight = 158;			//
char			str1[64];				// used for Windows strings conversions
char			str2[1024];				// used for Windows strings conversions
//
char			WavFileName[16];		// typically "./ch_xx.wav"
char			SessDirName[16];		// an 8 char directory name made out of time data
char			SessWavName[16];		// typically "/00000xxx.wav" name of multichan wav chunks
char			SessBinName[32];		// combining SessDirName&SessWavName
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
unsigned int	TmpData;				// used for fast char[4] comparisons (using a 32bit word)
unsigned short	wav_format;				// WAVE format (expected to be PCM = 1)
unsigned short	wBlockAlign;			// block alignment
unsigned short	bits_per_samp;			// bits per sample (24)
unsigned short	wav_chs;				// numb of channels os source file (expected to be 1)
#ifdef Use_malloc
// Using large malloc blocks
unsigned int	WavFileSize[32];		// size of *WavFileData (source wave file)
unsigned char 	*WavFileData[32];		// wave data buffer for each wav file
int				WaveDataIndex[32];		// current read index in *WavFileData (source wave file)
#else
// Not using large malloc blocks
unsigned char 	WavHeader[36];			// wave file header
unsigned int	JunkSize;				// size in bytes of Junk chunk
#endif
//
time_t			now;					// time
struct tm 		*current_time;			// year...second structure
struct timeb	start,end;				// precise timers (well... to the ms)
int				nb_takes;				// number of takes (1...256); sizes in take_size[i]
int				no_markers;				// number of Markers
unsigned int	total_length;			// length in # of samples
unsigned int	x_size;					// size of unknown sub-chunk
struct {
	unsigned int	Riff;				// = "Riff";
	unsigned int	wavsize; 			// size of WAVE sub-chunk
	unsigned int	Wave; 				// = "WAVE"
	unsigned int	fmt;				// = "fmt ";
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
//unsigned int 	wavsize;				// size of WAVE sub-chunk
int				MList_File;				// Default (1) is "List of Markers"; (0) is "File Path"
int				Zero[32] = {0};			// 0 (32 times)
//
long long		audio_bytes;			// size of audio data (multi channels)
unsigned int	r_audio_bytes;			// audio data remainder on 32bits for file writing
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
				WS_VISIBLE | WS_CHILD, 432, 25, 110, 70, hwnd, (HMENU )1, NULL, NULL);

		hwndDirectory = CreateWindow("button", "Select wav files Source Directory",
				WS_VISIBLE | WS_CHILD, 128, 25, 300, 20, hwnd, (HMENU )2, NULL, NULL);

		hwndPathList = CreateWindow("button", "List of Markers",
				WS_VISIBLE | WS_CHILD, 432, 105, 110, 20, hwnd, (HMENU )3, NULL, NULL);

		hwndSession = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 178, 65, 250, 20, hwnd,
				(HMENU )0, NULL, NULL);

		hwndMarkers = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 178, 105, 250, 20, hwnd,
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
		TextOut(hdc, 128, 3, str1, wsprintf(str1, "X32Wav_Xlive - ver 0.97 - ©2017 - Patrick-Gilles Maillot"));
		TextOut(hdc, 128, 50, str1, wsprintf(str1, "Enter Session Name:"));
		TextOut(hdc, 128, 90, str1, wsprintf(str1, "Enter Markers:"));
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
				if (Xpath[0]) {
					// read Session name
					GetWindowText(hwndSession, str1, GetWindowTextLength(hwndSession) + 1);
					if (str1[0]) {
						// manage markers
						GetWindowText(hwndMarkers, str2, GetWindowTextLength(hwndMarkers) + 1);
						if (MList_File) {
							// Markers are given as a list of floats
							j = 0;
							for (no_markers = 0; no_markers < 100; no_markers++) {
								if ((i = (sscanf(str2 + j,"%f", &(marker_vec[no_markers])))) != EOF) {
									while (((int)str2[j] > 45) && ((int)str2[j] < 58)) j++;
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
								MessageBox(NULL, "Can't find Markers File", NULL, MB_OK);
							}
						}
						marker_vec[no_markers] = 0.0;	// the last marker must be 0.0
						//
						// launch Merge!
						if ((i = MergeWavFiles(str1, no_markers, marker_vec)) > 0) {
							sprintf(str1, "Elapsed time: %dms", i);
							MessageBox(NULL, str1, "Done!", MB_OK);
						} else {
							MessageBox(NULL, "Something went wrong!", NULL, MB_OK);
						}
					} else {
						MessageBox(NULL, "No Session Name!", NULL, MB_OK);
					}
				}
				break;
			case 2:
				// Select Source Directory (must contain .wav files)
				Xpath[0] = 0; // no/empty directory name
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = Xpath;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(Xpath);
				ofn.lpstrFilter = "*.wav\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrTitle = (LPCTSTR) "Select first wav file to set directory\0";
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST;
				//
				// Display the Open dialog box.
				if (GetOpenFileName(&ofn)) {
					// remove filename from returned path so we can either save to
					// existing directory or newly created one
					i = strlen(Xpath);
					while (i && (Xpath[i] != '\\')) --i;
					Xpath[i] = 0;
					if (i > 0) {
						if (MessageBox(NULL, Xpath, "Source directory: ", MB_OK) == IDOK) {
							// the message is purely informative
							SetCurrentDirectory(Xpath);
						}
					} else {
						MessageBox(NULL, "Invalid directory!\n", NULL, MB_OK);
					}
				}
				break;
			case 3:
				// Select between List of markers or file containing markers
				SetWindowText(hwndPathList, (MList_File ^= 1) ? "List of Markers" : "File Path");
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
int main(int argc, char **argv) {
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmdFile = 0;
	//
	// Init structures
	// some initializations needed
	strncpy(uriff.s, "riff",4);
	strncpy(uRIFF.s, "RIFF",4);
	strncpy(uwave.s, "wave",4);
	strncpy(uWAVE.s, "WAVE",4);
	strncpy(ufmt.s, "fmt ",4);
	strncpy(uFMT.s, "FMT ",4);
	strncpy(ujunk.s, "junk",4);
	strncpy(uJUNK.s, "JUNK",4);
	strncpy(udata.s, "data",4);
	strncpy(uDATA.s, "DATA",4);
	MList_File = 1;						// Default is "List of Markers"
	wheader.Riff = uRIFF.i;
	wheader.wavsize = 0;
	wheader.Wave = uWAVE.i;
	wheader.fmt = ufmt.i;
	wheader.Sixteen = 16;
	wheader.One = 1;
	wheader.num_channels = 0;
	wheader.audio_samprate = 0;
	wheader.dwAvgBytesPerSec = 0;
	wheader.wBlockAlign = 0;
	wheader.Thirtytwo = 32;
	wheader.Junk = uJUNK.i;
	wheader.Junk_bytes= 32716;
	//
	ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
	//
	return 0;
}
//
//
int	MergeWavFiles(char* name_str, int num_markers, float* markers) {
	int				numb_chls;
	int				fill_chls;			// numb_chls + fill_chls = 32
	int				i, j, k;
	unsigned int 	PassNumber;
#ifdef Use_malloc
// Using large malloc blocks
	int				l;
#endif
	//
	//
	ftime (&start);
	numb_chls = 0;
	for (i = 0; i < 32; i++) {
		sprintf(WavFileName, "./ch_%d.wav", i + 1);
		if ((Xin[i] = fopen(WavFileName, "rb")) == NULL) {
			numb_chls = i;
			break;
		}
	}
	if (numb_chls == 0) {
		MessageBox(NULL, "No wave files found!", NULL, MB_OK);
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
		// analyze file
		//
#ifdef Use_malloc
// Using large malloc blocks
		if (fseek(Xin[i], 0, SEEK_END) == 0) {
			WavFileSize[i] = ftell(Xin[i]);						// determine file size
			fseek(Xin[i], 0, SEEK_SET);  						// rewind(f);
			WavFileData[i] = malloc(WavFileSize[i]);			// allocate buffer
			fread(WavFileData[i], WavFileSize[i], 1, Xin[i]);	// read file into buffer
		}
		fclose(Xin[i]);
		//
		if (strncmp((char *)WavFileData[i], "Riff", 4) != 0) {
			sprintf(str1, "ch_%d.wav is not a Riff file!\n", i + 1);
			MessageBox(NULL, str1, NULL, MB_OK);
			return -1;
		}
		file_size[i] = *(unsigned int *)(WavFileData[i]+4);
		if ((*(unsigned int *)(WavFileData[i]+8) != uWAVE.i) &&
			(*(unsigned int *)(WavFileData[i]+8) != uwave.i)) {
			sprintf(str1, "ch_%d.wav is not a WAVE file!\n", i + 1);
			MessageBox(NULL, str1, NULL, MB_OK);
			return -1;
		}
		// go through sub-chunks
		k = 12;
		PassNumber = 0;
		while (PassNumber != 2) {
			TmpData = *((unsigned int *)(WavFileData[i]+k));
			k += 4;
			if ((TmpData == uFMT.i) || (TmpData == ufmt.i)) {
				fmt_size = *(unsigned int *)(WavFileData[i]+k);
				if (fmt_size != 16) {
					sprintf(str1, "ch_%d.wav wrong fmt size!\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				wav_format = *(unsigned short *)(WavFileData[i]+k+4);
				if (wav_format != 1) {
					sprintf(str1, "ch_%d.wav WAV format not supported!\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				wav_chs = *(unsigned short *)(WavFileData[i]+k+6);
				if (wav_chs != 1) {
					sprintf(str1, "multichannels ch_%d.wav not supported\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				wav_samp_rate[i] = *(unsigned int *)(WavFileData[i]+k+8);
				if ((wav_samp_rate[i] != 44100) && (wav_samp_rate[i] != 48000)) {
					sprintf(str1, "ch_%d.wav WAV sample rate not supported!\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				dwAvgBytesPerSec = *(unsigned int *)(WavFileData[i]+k+12);
				wBlockAlign = *(unsigned short *)(WavFileData[i]+k+16);
				bits_per_samp = *(unsigned short *)(WavFileData[i]+k+18);
				if (bits_per_samp != 24) {
					sprintf(str1, "ch_%d.wav WAV bit resolution not supported!\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				k += 20;
				PassNumber += 1;
			} else if ((TmpData == uJUNK.i) || (TmpData == ujunk.i)) {
				k += *(int *)(WavFileData[i]+k) + 4;
			} else if ((TmpData == uDATA.i) || (TmpData == udata.i)) {
				data_size[i] = *(unsigned int *)(WavFileData[i]+k);
				k += 4;
				WaveDataIndex[i] = k;
				k += data_size[i];
				PassNumber += 1;
			} else {
				k += *(int *)(WavFileData[i]+k) + 4;
			}
		}
#else
// Not using large malloc blocks
		//
		// Read header (36 bytes) example below for 16bits/44.1kHz audio
		// index 	Sample Value 	Description
		// 0 - 3 	"Riff" 	Marks the file as a Riff file. Characters are each 1 byte long.
		// 4 - 7 	File size (integer) 	Size of the overall file - 8 bytes, in bytes (32-bit integer). Typically, you'd fill this in after creation.
		// 8 -11 	"WAVE" 	File Type Header. For our purposes, it always equals "WAVE".
		// 12-15 	"fmt " 	Format chunk marker. Includes trailing null
		// 16-19 	16 	Length of format data as listed above
		// 20-21 	1 	Type of format (1 is PCM) - 2 byte integer
		// 22-23 	2 	Number of Channels - 2 byte integer
		// 24-27 	44100 	Sample Rate - 32 byte integer. Common values are 44100 (CD), 48000 (DAT). Sample Rate = Number of Samples per second, or Hertz.
		// 28-31 	176400 	(Sample Rate * BitsPerSample * Channels) / 8.
		// 32-33 	4 	(BitsPerSample * Channels) / 8. (1: 8 bit mono)(2: 8 bit stereo/16 bit mono)(4: 16 bit stereo)
		// 34-35 	16 	Bits per sample
		//
		// Then:
		// 36-39	"junk" or "data" (if "data", 0 ->n)
		// 40-41	if junk junk size -> n
		// 36+n-39+n 	"data" 	"data" chunk header. Marks the beginning of the data section.
		// 40+n-43+n 	File size (data) 	Size of the data section.
		fread(WavHeader, 36, 1, Xin[i]);	// read file header into buffer
		if ((*(unsigned int *)(WavHeader+0) != uRIFF.i) &&
			(*(unsigned int *)(WavHeader+0) != uriff.i)) {
			sprintf(str1, "ch_%d.wav is not a Riff file!\n", i + 1);
			MessageBox(NULL, str1, NULL, MB_OK);
			return -1;
		}
		file_size[i] = *(unsigned int *)(WavHeader+4);
		if ((*(unsigned int *)(WavHeader+8) != uWAVE.i) &&
			(*(unsigned int *)(WavHeader+8) != uwave.i)) {
			sprintf(str1, "ch_%d.wav is not a WAVE file!\n", i + 1);
			MessageBox(NULL, str1, NULL, MB_OK);
			return -1;
		}
		// go through sub-chunks
		k = 12;
		while (k < 36) {
			TmpData = *(unsigned int *)(WavHeader+k);
			k += 4;
			if ((TmpData == uFMT.i) || (TmpData == ufmt.i)) {
				fmt_size = *(unsigned int *)(WavHeader+k);
				if (fmt_size != 16) {
					sprintf(str1, "ch_%d.wav wrong fmt size!\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				wav_format = *(unsigned short *)(WavHeader+k+4);
				if (wav_format != 1) {
					sprintf(str1, "ch_%d.wav WAV format not supported!\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				wav_chs = *(unsigned short *)(WavHeader+k+6);
				if (wav_chs != 1) {
					sprintf(str1, "multichannels ch_%d.wav not supported\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				wav_samp_rate[i] = *(unsigned int *)(WavHeader+k+8);
				if ((wav_samp_rate[i] != 44100) && (wav_samp_rate[i] != 48000)) {
					sprintf(str1, "ch_%d.wav WAV sample rate not supported!\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				wheader.dwAvgBytesPerSec = *(unsigned int *)(WavHeader+k+12);
				wheader.wBlockAlign = *(unsigned short *)(WavHeader+k+16);
				bits_per_samp = *(unsigned short *)(WavHeader+k+18);
				if (bits_per_samp != 24) {
					sprintf(str1, "ch_%d.wav WAV bit resolution not supported!\n", i + 1);
					MessageBox(NULL, str1, NULL, MB_OK);
					return -1;
				}
				k += 20;
			} else {
				sprintf(str1, "ch_%d.wav not a wav file!\n", i + 1);
				MessageBox(NULL, str1, NULL, MB_OK);
				return -1;
			}
		}
		// At that point k >= 36; we read data as we go
		// expect "data" and/or "junk" chunks
		PassNumber = 1;
		while (PassNumber) {
			fread(&TmpData, sizeof(TmpData), 1, Xin[i]);	// read TmpData
			k += 4;
			if ((TmpData == uDATA.i) || (TmpData == udata.i)) {
				fread(&data_size[i], sizeof(data_size[i]), 1, Xin[i]);	// read data chunk size
				PassNumber = 0;		// k +=4; // but don't need k anymore
			} else {
				fread(&JunkSize, sizeof(JunkSize), 1, Xin[i]);	// consider Junk chunk
				k += (JunkSize + 4);
				// Jump to next chunk
				fseek(Xin[i], k, SEEK_SET);
			}
		}
#endif
	}
	//
	//	check that all files are of equal lengths and sample rates
	audio_len = data_size[0];
	wheader.audio_samprate = wav_samp_rate[0];
	for (i = 0; i < numb_chls; i++) {
		if (data_size[i] != audio_len) {
			MessageBox(NULL, "files are not the same length!\n", NULL, MB_OK);
			return -1;
		}
		if (wav_samp_rate[i] != wheader.audio_samprate) {
			MessageBox(NULL, "files are not the same sample rate!\n", NULL, MB_OK);
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
	audio_bytes = (long long)total_length * 4 * (long long)wheader.num_channels;	// total number of bytes to process
	//
	// trim to 32kB boundary
	audio_bytes &= 0xfffffffffff8000;
	//
	// How many files of 4GB (max) for our session?
	nb_takes = 0;
	while (audio_bytes >= max_take_size) {
		take_size[nb_takes] = max_take_size / 4;	// in 32bit samples
		nb_takes += 1;
		audio_bytes -= max_take_size;
	}
	// deal with the remaining data
	r_audio_bytes = audio_bytes;					// remaining audio data for a possible last take
	if (r_audio_bytes > 0) {
		take_size[nb_takes] = r_audio_bytes / 4;	// size of last take in samples
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
	sprintf(SessDirName, "%08X", session_name);
	if (CreateDirectory(SessDirName, NULL)) {
		strcpy(SessBinName, SessDirName);
		strcat(SessBinName, "/SE_LOG.BIN");
		if ((Xout = fopen(SessBinName, "wb")) != NULL) {
			fwrite(&session_name, sizeof(session_name), 1, Xout);
			j = wheader.num_channels;
			fwrite(&j, sizeof(j), 1, Xout);
			fwrite(&wheader.audio_samprate, sizeof(wheader.audio_samprate), 1, Xout);
			fwrite(&session_name, sizeof(session_name), 1, Xout);
			fwrite(&nb_takes, sizeof(nb_takes), 1, Xout);
			fwrite(&num_markers, sizeof(num_markers), 1, Xout);
			fwrite(&total_length, sizeof(total_length), 1, Xout);
			//
			// todo: Calls to fwrite() below could be optimized
			for (i = 0; i < nb_takes; i++)
				fwrite(&take_size[i], sizeof(int), 1, Xout);
			for (; i < 256; i++)
				fwrite(&Zero, sizeof(*Zero), 1, Xout);
			for (i = 0; i < num_markers; i++)
				fwrite(&imarker_vec[i], sizeof(unsigned int), 1, Xout);
			for (; i < 101; i++)
				fwrite(&Zero, sizeof(*Zero), 1, Xout);
			for (i = 0; i < 24; i++)
				fwrite(&Zero, sizeof(*Zero), 1, Xout);
			i = strlen(name_str);
			if (i > 19) i = 19;
			fwrite(name_str, i, 1, Xout);
			// complete to 2kbytes with 0's
			while(ftell(Xout) < 2048)
				fwrite(&Zero, sizeof(char), 1, Xout);
			fclose(Xout);
			//
			// create take waves
			for (i = 0; i < nb_takes; i++) {
				strcpy(SessBinName, SessDirName);
				sprintf(SessWavName, "/%08X.wav", i + 1);
				strcat(SessBinName, SessWavName);
				if ((Xout = fopen(SessBinName, "wb")) != NULL) {
					wheader.wavsize = take_size[i] * 4 + 44 + wheader.Junk_bytes;
					wheader.wBlockAlign = wheader.num_channels * 4;
					wheader.dwAvgBytesPerSec = wheader.audio_samprate * wheader.wBlockAlign;
					// write wave header
					fwrite(&wheader, sizeof(wheader), 1, Xout);
					for (j = 0; j < 460 / 4; j++) {
						fwrite("    ", 4, 1, Xout);
					}
					fwrite("data", 4, 1, Xout);
					fwrite(&r_audio_bytes, sizeof(r_audio_bytes), 1, Xout);
					for (j = 0; j < (wheader.Junk_bytes - 468) / 4; j++) {
						fwrite("    ", 4, 1, Xout);
					}
					fwrite("data", 4, 1, Xout);
					fwrite(&r_audio_bytes, sizeof(r_audio_bytes), 1, Xout);
					//
					// writing new multichannel wav data
					sample.s[0] = 0;
					PassNumber = take_size[i] / wheader.num_channels;
					if (fill_chls){
						for (k = 0; k < PassNumber; k++) {
							for (j = 0; j < numb_chls; j++) {
#ifdef Use_malloc
// Using large malloc blocks
								l = WaveDataIndex[j];
								sample.s[1] = WavFileData[i][l];
								sample.s[2] = WavFileData[i][l+1];
								sample.s[3] = WavFileData[i][l+2];
								WaveDataIndex[j] += 3;
								fwrite(&sample.i, sizeof(sample.i), 1, Xout);
#else
// Not using large malloc blocks
								fread(&sample.s[1], 3, 1, Xin[j]);	// read 3 bytes audio sample
								fwrite(&sample.i, sizeof(sample.i), 1, Xout);
#endif
							}
							// Complete number of channels with 0's
							fwrite(Zero, sizeof(*Zero), fill_chls, Xout);
						}
					} else {
						for (k = 0; k < PassNumber; k++) {
							for (j = 0; j < numb_chls; j++) {
#ifdef Use_malloc
// Using large malloc blocks
								l = WaveDataIndex[j];
								sample.s[1] = WavFileData[i][l];
								sample.s[2] = WavFileData[i][l+1];
								sample.s[3] = WavFileData[i][l+2];
								WaveDataIndex[j] += 3;
								fwrite(&sample.i, sizeof(sample.i), 1, Xout);
#else
// Not using large malloc blocks
								fread(&sample.s[1], 3, 1, Xin[j]);	// read 3 bytes audio sample
								fwrite(&sample.i, sizeof(sample.i), 1, Xout);
#endif
							}
						}
					}
					fclose(Xout);
				} else {
					MessageBox(NULL, "Cannot create session wav file!\n", NULL, MB_OK);
				}
			}
			// All done hopefully without errors
			ftime (&end);
			k = (int)(1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
		} else {
			MessageBox(NULL, "Cannot create session log file!\n", NULL, MB_OK);
			k = -1;
		}
	} else {
		MessageBox(NULL, "Cannot create session directory!\n", NULL, MB_OK);
		k = -1;
	}
	//
#ifdef Use_malloc
// Using large malloc blocks
	// free allocated memory buffers
	for (i = 0; i < numb_chls; i++) {
		free(WavFileData[i]);
	}
#else
// Not using large malloc blocks
	for (i = 0; i < numb_chls; i++) {
		fclose(Xin[i]);
	}
#endif
	//
	return k;
}
