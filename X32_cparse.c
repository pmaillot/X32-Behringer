/*
 * Xcparse.c
 *
 *  Created on: Oct 8, 2014
 *      Author: Patrick-Gilles Maillot
 *
 *      Fixes after bugs reported by Ted Rippert on better handling of
 *      groups after quoted strings
 *
 */

#include <string.h>
#include <stdio.h>

#define COMMA  ','
#define SPACE  ' '
#define QUOTE  '"'
#define SQUOTE '\''

extern int Xsprint(char *bd, int index, char format, void *bs);


int
Xcparse(char* buf, char* input) {

int     start_comma, end_comma, txt_start, i, k, input_len;
char    input_line[512], ccase, ccend;

union littlebig {
    char    c1[4];
    int     i1;
    float   f1;
} endian;


// make a local copy of the input string as we'll modify some characters during parsing.
	strcpy(input_line, input);
    start_comma = 0;
    input_len = strlen(input_line);
//    jump to next comma, and replace spaces by 0, prior to comma
    for (i = 0; i < input_len; i++) {
        if (input_line[i] == SPACE) input_line[i] = 0;
        if (input_line[i] == COMMA) {
            start_comma = i;
            break;
        }
    }
// Prepare first command block
    i = 0;
    i = Xsprint(buf, i, 's', input_line);
    if (start_comma) {
// Set command (",s|i|f" is present)
// look for end of formatters (s,i,f)
        k = start_comma + 1;
        while ((k < input_len) && ((input_line[k] == 's') ||
        						   (input_line[k] == 'i') ||
								   (input_line[k] == 'f'))) k++;
// Prepare command formatters' block
        input_line[k] = 0;
        i = Xsprint(buf, i, 's', input_line+start_comma);
// k points to first value of data block (text, int or float)
// process formatters (between start_comma and end_comma) to parse blocks
        start_comma += 1;	// skip the formatters' leading comma
        end_comma = k;		// last identified formatter
// manage blocks according to formatters
        while (start_comma < end_comma) {
        	// ignore spaces before new block (text, int or float)
        		k += 1;
            while ((k < input_len) && (input_line[k] == SPACE)) k++;
            ccase = input_line[start_comma++];
            // ignore white spaces out of quotes (single or double)
			switch (ccase) {
				case 's':
					// change next SPACE char to \0 to mark end of string
					// if the string must have spaces in it, it should start with " or '; "" can be used
					// in the string to depict an empty substring (String must start with a ' in that case).
					// String delimiters must be the same (" or ').
					txt_start = k;
					if ((input_line[k] == QUOTE) || (input_line[k] == SQUOTE)) {
						ccend = input_line[k];
						k += 1;
						txt_start += 1;
						while ((k < input_len) && (input_line[k] != ccend)) k++;
					} else {
						while ((k < input_len) && (input_line[k] != SPACE)) k++;
					}
					input_line[k] = 0;
					i = Xsprint(buf, i, 's', input_line+txt_start);
					break;
				case 'i':
					// read integer and move pointer k to the end of the data just read
					sscanf(input_line+k, "%d", &endian.i1);
					i = Xsprint(buf, i, 'i', endian.c1);
					while ((k < input_len) && (input_line[k] != SPACE)) k++;
					break;
				case 'f':
					// read float and move pointer k to the end of the data just read
					sscanf(input_line+k, "%f", &endian.f1);
					i = Xsprint(buf, i, 'f', endian.c1);
					while ((k < input_len) && (input_line[k] != SPACE)) k++;
					break;
				default:
					break;
			}
		}
    }
    return i;
}
