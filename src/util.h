#include <stdio.h>
#include <string.h>

#define TRUE		1
#define FALSE		0
#define BSIZE		512

void swap(char *v[], int i, int j);
int isdelimiter(unsigned char c);
char *get_nth_field(int n, char *str);
int dstrlen(char *str);
/*char delimiter[] = "|: \t,\n";*/
char delimiter[] = ":";
int iskanji(unsigned char c, int sjis);
char *getfld(char *str, char *d, int fld);





