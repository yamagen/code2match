#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "util.h"

int iskanji(unsigned char c, int sjis);
void trim(char *buf);

char *takeit(const char *src, char *dest, char c, int number)
{
  char *destorg;

  destorg = dest;
  while (number-- != 0) {
    while (*src != (char)13 && *src != '\0' && *src != c) /* (char)13 ..'\n' */
      src++;
    if (*src == (char)13 || *src == '\0') {
      *dest = '\0';
      return(dest);
    }
    src++;
  }
  while (*src != '\0' && *src != c && *src != (char)13 && *src != (char)10 )
    *dest++ = *src++;
  *dest = '\0';
  return(destorg);
}

void trim( char *buf )
{
  buf[strlen(buf)-1] = '\0';
}

char *getfld(char *str, char *d, int fld)
{
  char *ptr;
  int i = 0;
  char buf[BSIZE];

  strcpy(buf,str);
  
  ptr = (char *)strtok(buf, d);
  while (ptr != NULL && i < fld) {
    ptr = (char *)strtok(NULL, d);
    i++;
  }
  return ptr;
}


int digitstr(char *str)
{
  int eval = 0;
  int sign_num = 0;
  int float_num = 0;

  if (*str == '\0')     return FALSE;
  for (; *str != '\0'; str++) {
    /* if (!isdigit(*str) && *str !='\n' && *str !='.' && *str !='-')
       return FALSE;*/
    if (isalpha(*str))	return FALSE;
    if (*str =='-' && sign_num == 0) 
      sign_num = 1;
    if (*str =='.' && float_num == 0) 
      float_num = 2;
  }
  return TRUE + sign_num + float_num;
}

int dstrlen(char *str)
{
  int n;

  for (n = 0; *str != '\0'; n++, str++)
    if (isdelimiter(*str))
      return (n);
  return (n); /* maybe not reached */
}

int isdelimiter(unsigned char c)
{
  char *d;
  for (d = delimiter; *d != '\0'; d++)
    if (*d == c)
      return (TRUE);
  return (FALSE);
}

char *get_nth_field(int n, char *str)
{
  char *p, *l;
  static char *field;
  //  char *malloc();
  int f;
	
  if (n)
    f = 0;
  else {
    f = 1;
    n = 999;
    /* There are no line which includes
       such a lot of fields, I would say. */
  }
  l = p = str;
  while (isdelimiter(*p))
    p++; /* skip delimiters */
  for (;;) {
    if (--n < 1)
      break;
    l = p;
    p += dstrlen(p);
    while (isdelimiter(*p))
      p++; /* skip delimiters */
    if (*p == '\0')
      if (f) {
        p = l;
        break;
      } else
        return (NULL);
  }
  if ((field = malloc(dstrlen(p) + 1)) == NULL) {
    fprintf(stderr,"malloc fail\n");
    exit (1);
  }
  strncpy(field, p, dstrlen(p));
  *(field + dstrlen(p)) = '\0';
  return field;  /* I need free() after using the pointer */
  free(field);
}

/* * swap - swap two elements in an array of char pointers
 * v: array of char pointers
 * i: index of first element to swap
 * j: index of second element to swap
 */
void	swap(char *v[], int i, int j) 
{
  char *temp;
  
  temp = v[i];
  v[i] = v[j];
  v[j] = temp;
}


int iskanji(unsigned char c, int sjis)
{
  if (sjis) {
    return((0x81 <= c && c <= 0x9F) || (0xE0 <= c && c <= 0xFC));  /* SJIS */
  } else {
    return(0x8E <= c && c <= 0xFE && 0x8F != c && c != 0xA0);    /* EUC  */
  }
}
