/*
 * X32logf.c
 *
 *  Created on: Feb 3, 2019
 *      Author: Patrick-Gilles Maillot
 */

#include <stdio.h>
#include <string.h>


FILE	*log_file;

// type cast union
union littlebig {
	char	cc[4];
	int		ii;
	float	ff;
} endian;

void X32logfile(FILE *f) {
	log_file = f;
}

void X32logf(char *header, char *buf, int len)
{
	int i, k, n, j, l, comma = 0, data = 0, dtc = 0;
	unsigned char c;

	fprintf (log_file, "%s, %4d B: ", header, len);
	for (i = 0; i < len; i++) {
		c = (unsigned char)buf[i];
		if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
		fprintf (log_file, "%c", c);
		if (c == ',') {
			comma = i;
			dtc = 1;
		}
		if (dtc && (buf[i] == 0)) {
			data = (i + 4) & ~3;
			for (dtc = i + 1; dtc < data ; dtc++) {
				if (dtc < len) {
					fprintf (log_file, "~");
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
								fprintf (log_file, "%c", c);
						}
					}
					break;
				case 'i':
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					fprintf (log_file, "[%6d]", endian.ii);
					break;
				case 'f':
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					if (endian.ff < 10.) fprintf (log_file, "[%06.4f]", endian.ff);
					else if (endian.ff < 100.) fprintf (log_file, "[%06.3f]", endian.ff);
					else if (endian.ff < 1000.) fprintf (log_file, "[%06.2f]", endian.ff);
					else if (endian.ff < 10000.) fprintf (log_file, "[%06.1f]", endian.ff);
					break;
				case 'b':
					// Get the number of bytes
					for (k = 4; k > 0; endian.cc[--k] = buf[data++]);
					n = endian.ii;
					// Get the number of data (floats or ints ???) in little-endian format
					for (k = 0; k < 4; endian.cc[k++] = buf[data++]);
					if (n == endian.ii) {
						// Display blob as string
						fprintf (log_file, "%d chrs: ", n);
						for (j = 0; j < n; j++) fprintf (log_file, "%c ", buf[data++]);
					} else {
						// Display blob as floats
						n = endian.ii;
						fprintf (log_file, "%d flts: ", n);
						for (j = 0; j < n; j++) {
							//floats are little-endian format
							for (k = 0; k < 4; endian.cc[k++] = buf[data++]);
							fprintf (log_file, "%06.2f ", endian.ff);
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
	fprintf (log_file, "\r\n");
}
