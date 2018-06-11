/*
 * validateIP4Dotted.c
 *
 *  Created on: May 29, 2016
 *      Author: Patrick-Gilles Maillot
 *
 *      Simple validation of IP4 type IP address
 *
 *      expecting an IP address in the form 123.123.123.123
 *      returns 1 if OK, 0 otherwise
 *
 */

#include <stdio.h>
#include <string.h>

int validateIP4Dotted(char *s)
{
int i;
char tail[16];
unsigned int d[4];

    i = strlen(s);
    if (i < 7 || i > 15) return 0;
    tail[0] = 0;
    int c = sscanf(s, "%3u.%3u.%3u.%3u%s", &d[0], &d[1], &d[2], &d[3], tail);
    if (c != 4 || tail[0]) return 0;
    for (i = 0; i < 4; i++)
        if (d[i] > 255)
            return 0;
    return 1;
}

