/*
 * Xcparse.c
 *
 *  Created on: Oct 8, 2014
<<<<<<< HEAD
 *      Author: patrick
 *
 *  Modified Jan 17, 2018
 *		by Ted Rippert
 *		Added test for multiple spaces between values
=======
 *      Author: Patrick-Gilles Maillot
 *
 *      Fixes after bugs reported by Ted Rippert on better handling of
 *      groups after quoted strings
 *
>>>>>>> 86ed4805776db0b58bbc08678653519d1bceaaf0
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
<<<<<<< HEAD
            switch (ccase) {
                case 's':
                    // change next space char to \0 to mark end of string
                    // may have to change convention here if spaces are needed in strings
                    // for ex. in file names. In that case one may have to ask for "\" as a
                    // end of string char to be replaced by \0
                	// if the string has spaces in it, it can start with " or '; "" can be used
                	// in the string to depict an empty substring (start with a ' in that case).
                	// String must end with the same character it started.
                    k = start_values;
                    if ((input_line[k] == QUOTE) || (input_line[k] == SQUOTE)) {
                    	ccend = input_line[k];
                        k += 1;
                        while(k < input_len) {
                            if (input_line[k] == ccend) {
                                input_line[k] = 0;
                                break;
                            }
                            k += 1;
                        }
                        i = Xsprint(buf, i, 's', input_line+start_values+1);
                        if (input_line[k+1] == SPACE) k += 1;
                        while(k < input_len && input_line[k] == SPACE) {
                            input_line[k] = 0;
                            if (input_line[k+1] != SPACE) break;
                            k += 1;
                        }
                    } else {
                        while(k < input_len) {
                            if (input_line[k] == SPACE) {
                                input_line[k] = 0;
                                if (input_line[k+1] != SPACE) break;
                            }
                            k += 1;
                        }
                        i = Xsprint(buf, i, 's', input_line+start_values);
                    }
                    start_values = k + 1;
                    break;
                case 'i':
                    sscanf(input_line+start_values, "%d", &endian.i1);
                    i = Xsprint(buf, i, 'i', endian.c1);
                    while (start_values < input_len) {
                        ccase = input_line[start_values];
                        start_values += 1;
                        if (ccase == SPACE) break;
                    }
                    break;
                case 'f':
                    sscanf(input_line+start_values, "%f", &endian.f1);
//                    for (k = 0; k < 4; k++) input_line[k] = endian.c1[k];
                    i = Xsprint(buf, i, 'f', endian.c1);
                    while (start_values < input_len) {
                        ccase = input_line[start_values];
                        start_values += 1;
                        if (ccase == SPACE) break;
                    }
                    break;
                default:
                    break;
            }
        }
=======
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
>>>>>>> 86ed4805776db0b58bbc08678653519d1bceaaf0
    }
    return i;
}
