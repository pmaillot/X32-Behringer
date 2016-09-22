/*
 * TestParams.c
 *
 *  Created on: Sep 1, 2016
 *      Author: Patrick-Gilles Maillot
 */
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

FILE	*fp;

typedef enum {
	P_INT,				// Parameter is an int (i.e.  xxx=10)
	P_FLT,				// Parameter is a float (i.e. xxx=10.23)
	P_STR,				// Parameter is a string (i.e. xxx=a string of some sort)
	P_HSTR,				// ... a set of hexa bytes in a string (i.e. xxx=f0 f1 f2)
	P_YN				// ... a yes or no string (i.e. xxx=Y) (i.e. xxx=no)
} Ptype;

typedef union  {
	char*	s1;
	int		i1;
	float	f1;
} Pdata;

typedef struct param {
	char*	param_name;
	Ptype   param_type;
	Pdata	param_data;
} Param;

/////////////////////////////////////////////////
// for testing purpose only:
//
//Param MyParameters[]; = {
//		{"aaa", P_INT, { 0 }},
//		{"bbb",	P_FLT, { 0 }},
//		{"ddd",	P_INT, { 0 }},
//		{"sss",	P_STR, { 0 }},
//		{}
//};
//
//#define AAA MyParameters[0].param_data.i1
//#define BBB MyParameters[1].param_data.f1
//#define DDD MyParameters[2].param_data.i1
//#define SSS MyParameters[3].param_data.s1
//
/////////////////////////////////////////////////
//
int FileParse(char* fname, Param* par_tab);
int LineParse(char* line, Param* par_tab);
//
// LineParse() parsing one line at a time in the form "xxx=yyy", and storing yyy at the right
// position in par_tab, with the right format, allocating memory for strings and alike parameters
// returns 0 if no error, -1 otherwise [more error testing, to the expense of processing time...]
int LineParse(char* line, Param* par_tab) {
	int i, j;
	char *start;
//
	line[strlen(line) - 1] = 0; // replace ending \n by a 0
	i = 0;
	while ((line[i] != '=') && (i < 256)) i++;
	if (i == 256) return -1;
	line[i] = 0;
	start = &line[i + 1];
	while (par_tab->param_name != 0) {
		if (strcmp(par_tab->param_name, line) == 0) {
			switch (par_tab->param_type) {
			case P_INT:	// save int directly in par_tab field
				sscanf(start, "%d", &par_tab->param_data.i1);
				break;
			case P_FLT:	// save float directly in par_tab field
				sscanf(start, "%f", &par_tab->param_data.f1);
				break;
			case P_STR:	// save string into pointer in par_tab field
				// allocate memory; +1 for tailing \0 and +16 to enable in-line editing
				if ((par_tab->param_data.s1 = malloc(strlen(start) + 1 + 16)) != NULL) {
					strcpy(par_tab->param_data.s1, start);
				}
				break;
			case P_HSTR: // parse space separated hexa bytes into length+string pointer in par_tab field
				j = (strlen(start) + 2) / 3;
				// allocate memory; +1 for length and +16 to enable in-line editing
				if ((par_tab->param_data.s1 = malloc(j + 1 + 16)) != NULL) {
					par_tab->param_data.s1[0] = j;
					for (i = 0; i < j; i++)
						sscanf(&start[3*i], "%2x", (unsigned int*)&par_tab->param_data.s1[i + 1]);
				}
				break;
			case P_YN: // save int 0 or 1 directly in par_tab field
				par_tab->param_data.i1 = ((line[i+1] == 'y') || (line[i+1] == 'Y'));
				break;
			default:
				return (-1);
			}
			return(0);
		}
		par_tab++;
	}
	printf("Did not find param: %s\n", line);
	return (-1);
}
//
// FileParse() is to be called at the begining of programs to parse all lines of the resource
// file into par_tab elements
int FileParse(char* fname, Param* par_tab) {
	char line[256];

	if ((fp = fopen(fname , "r")) != NULL) {							// open resource file for reading
		do {
			do {														// parse lines, ignoring
				if (fgets(line, sizeof(line), fp) == NULL) return (1);	// comments
			} while (*line == '#');										// for useful lines carrying attributes and
		} while ((LineParse(line, par_tab)) == 0);						// parameters, check and set data
	}
	return(0);
}
//
// FreeParse() is to be called at the end of programs to free the memory allocated during
// parsing for strings as similar parameters
void FreeParse(Param* par_tab) {
//
	while(par_tab->param_name) {
		if ((par_tab->param_type == P_STR) || (par_tab->param_type == P_HSTR)) {
			if(par_tab->param_data.s1) free(par_tab->param_data.s1);
		}
		par_tab++;
	}
	return;
}
/////////////////////////////////////////////////
// for testing purpose only:
//
//int main() {
//
//	if (FileParse("test.ini", MyParameters)) {
//		printf("par aaa = %d\n", AAA);
//		printf("par bbb = %f\n", BBB);
//		printf("par sss = \"%s\"\n", SSS);
//		printf("par ddd = %d\n", DDD);
//	} else {
//		printf("FileParse returned an error\n");
//	}
//	return 0;
//}
/////////////////////////////////////////////////

