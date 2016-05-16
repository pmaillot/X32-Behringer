/*
 * Xcparse.c
 *
 *  Created on: Oct 8, 2014
 *      Author: patrick
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

int     start_comma, end_comma, start_values, i, k, input_len;
char    input_line[128], ccase, ccend;

union littlebig {
    char    c1[4];
    int     i1;
    float   f1;
} endian;


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
//
// Set command (",s|i|f" is present)
// look for end of formatters (s,i,f)
        k = start_comma + 1;
        while (k < input_len) {
            if ((input_line[k] != 's') &&
                (input_line[k] != 'i') &&
                (input_line[k] != 'f')) break;
            k += 1;
        }
// Prepare formatters command block
        while(k < input_len) {
            if (input_line[k] == SPACE) {
                input_line[k] = 0;
                break;
            }
            k += 1;
        }
        i = Xsprint(buf, i, 's', input_line+start_comma);
//
// k point at the first value
// start_values points to first value, too
// process formatters (between start_comma and end_comma)
        start_comma += 1;
        end_comma = k;
        start_values = k + 1;
        while (start_comma < end_comma) {
            ccase = input_line[start_comma++];
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
                    if ((input_line[k] == QUOTE) ||(input_line[k] == SQUOTE)) {
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
                    } else {
                        while(k < input_len) {
                            if (input_line[k] == SPACE) {
                                input_line[k] = 0;
                                break;
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
    }
    return i;
}
