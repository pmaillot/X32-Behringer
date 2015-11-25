/*
 * Xdump.c
 *
 *  Created on: Nov. 5, 2014
 *      Author: Patrick-Gilles Maillot
 */

#include <stdio.h>
#include <string.h>



void Xdump(char *buf, int len, int debug)
{
	int i, k, n, j, l, comma = 0, data = 0, dtc = 0;
	unsigned char c;
	union littlebig {
		char	c1[4];
		short	si[2];
		int		i1;
		float	f1;
	} endian;

	for (i = 0; i < len; i++) {
		c = (unsigned char)buf[i];
		if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
		if (debug) printf(" %c", c);
		else printf("%c", c);
		if (c == ',') {
			comma = i;
			dtc = 1;
		}
		if (dtc && (buf[i] == 0)) {
			data = (i + 4) & ~3;
			for (dtc = i + 1; dtc < data ; dtc++) {
				if (dtc < len) {
					if (debug) printf(" ~");
					else printf("~");
				}
			}
			dtc = 0;
			l = data;
			while (++comma < l && data < len) {
				switch (buf[comma]) {
				case 's':
					k = (strlen((char*)(buf+data)) + 4) & ~3;
					for (j = 0; j < k; j++) {
						if (data < len) {
							c = (unsigned char)buf[data++];
							if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
							if (debug) printf(" %c", c);
							else printf("%c", c);
						}
					}
//						printf("str: %s ",((char*)(buf+data)));
//						data += (strlen((char*)(buf+data)) + 4) & ~3;
					break;
				case 'i':
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					printf("[%6d]", endian.i1);
					break;
				case 'f':
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					if (endian.f1 < 10.) printf("[%06.4f]", endian.f1);
					else if (endian.f1 < 100.) printf("[%06.3f]", endian.f1);
					else if (endian.f1 < 1000.) printf("[%06.2f]", endian.f1);
					else if (endian.f1 < 10000.) printf("[%06.1f]", endian.f1);
					break;
				case 'b':
					// Get the number of bytes
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					n = endian.i1;
					// Get the number of data (floats or ints ???) in little-endian format
					for (k = 0; k < 4; endian.c1[k++] = buf[data++]);
					// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					if (n == endian.i1) {
						// Display blob as string
						printf("%d chrs: ", n);
						for (j = 0; j < n; j++) printf("%c ", buf[data++]);
					} else {
						// Display blob depending on command (as floats most of the time)
						if(strncmp(buf, "/meters/15", 10) == 0) {
							n = endian.i1 * 2;
							printf("%d rta: \n", n);
							for (j = 0; j < n; j++) {
								//data as short ints, little-endian format
								for (k = 0; k < 2; endian.c1[k++] = buf[data++]);
								endian.f1 = (float)endian.si[0] / 256.0;
								// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
								printf("%07.2f ", endian.f1);
							}
						} else {
							n = endian.i1;
							printf("%d flts: ", n);
							for (j = 0; j < n; j++) {
								//floats are little-endian format
								for (k = 0; k < 4; endian.c1[k++] = buf[data++]);
								// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
								printf("%06.2f ", endian.f1);
							}
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
	printf("\n");
}



void Xfdump(char *header, char *buf, int len, int debug) {
int i;

if (debug) {
		printf("%s, %4d B: ", header, len);
		for (i = 0; i < len; i++) {
			printf("%02x", (unsigned char)buf[i]);
		}
		printf("\n");
	}
	if (debug) for (i = 0; i < (strlen(header) + 10); i++) printf(" ");
	else       printf("%s, %4d B: ", header, len);
	Xdump(buf, len, debug);
}

void Xsdump(char *str_out, char *buf, int len)
{
	int i, k, n, j, l, comma = 0, data = 0, dtc = 0;
	unsigned char c;
	char *out;
	union littlebig {
		char	c1[4];
		short	si[2];
		int		i1;
		float	f1;
	} endian;

	out = str_out;
	for (i = 0; i < len; i++) {
		c = (unsigned char)buf[i];
		if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
		sprintf(out++, "%c", c);
		if (c == ',') {
			comma = i;
			dtc = 1;
		}
		if (dtc && (buf[i] == 0)) {
			data = (i + 4) & ~3;
			for (dtc = i + 1; dtc < data ; dtc++) {
				if (dtc < len) {
					sprintf(out++, "~");
				}
			}
			dtc = 0;
			l = data;
			while (++comma < l && data < len) {
				switch (buf[comma]) {
				case 's':
					k = (strlen((char*)(buf+data)) + 4) & ~3;
					for (j = 0; j < k; j++) {
						if (data < len) {
							c = (unsigned char)buf[data++];
							if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
							sprintf(out++, "%c", c);
						}
					}
//						printf("str: %s ",((char*)(buf+data)));
//						data += (strlen((char*)(buf+data)) + 4) & ~3;
					break;
				case 'i':
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					sprintf(out, "[%6d]", endian.i1);
					out += 8;
					break;
				case 'f':
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					if (endian.f1 < 10.) sprintf(out, "[%06.4f]", endian.f1);
					else if (endian.f1 < 100.) sprintf(out, "[%06.3f]", endian.f1);
					else if (endian.f1 < 1000.) sprintf(out, "[%06.2f]", endian.f1);
					else if (endian.f1 < 10000.) sprintf(out, "[%06.1f]", endian.f1);
					out += 8;
					break;
				case 'b':
					// Get the number of bytes
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					n = endian.i1;
					// Get the number of data (floats or ints ???) in little-endian format
					for (k = 0; k < 4; endian.c1[k++] = buf[data++]);
					// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					if (n == endian.i1) {
						// Display blob as string
						sprintf(out, "%3d chrs: ", n);
						out += 10;
						for (j = 0; j < n; j++) sprintf(out++, "%c ", buf[data++]);
					} else {
						// Display blob depending on command (as floats most of the time)
						if(strncmp(buf, "/meters/15", 10) == 0) {
							n = endian.i1 * 2;
							sprintf(out, "%3d rta: \n", n);
							out += 9;
							for (j = 0; j < n; j++) {
								//data as short ints, little-endian format
								for (k = 0; k < 2; endian.c1[k++] = buf[data++]);
								endian.f1 = (float)endian.si[0] / 256.0;
								// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
								sprintf(out, "%07.2f ", endian.f1);
								out += 8;
							}
						} else {
							n = endian.i1;
							sprintf(out, "%3d flts: ", n);
							out += 10;
							for (j = 0; j < n; j++) {
								//floats are little-endian format
								for (k = 0; k < 4; endian.c1[k++] = buf[data++]);
								// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
								sprintf(out, "%06.2f ", endian.f1);
								out += 7;
							}
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
	sprintf(out++, "\n");
	sprintf(out++, "\0");
}

