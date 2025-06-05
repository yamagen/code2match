/* Time-stamp: "2012-01-28 23:35:55 yamagen"
 * Hilofumi.Yamamoto@anu.edu.au
 * usage: t2c keyword
 */
#include <stdio.h>
#include <stdlib.h> /* getenv */
#include <math.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h> /* fstat */
#include <unistd.h>   /* fstat */
#include <sys/mman.h>
#include <string.h>
#include "config.h"

int rcinfo (char *filename);
void trim(char *buf);

int main(int argc, char **argv)
{
  int   fd;
  long  psize,size;
  DDD   *ptr,*result;
  long  k,i;
  char *key;
  struct stat st;
  char filename[BSIZE];
  char *path = getenv("HOME");

  strcpy(filename,path);
  strcat(filename,OPDB);

  key = argv[1];

  /* open mmapfile */
  if ((fd = open(filename,O_RDWR)) == -1) {
    perror("open");
    exit(-1);
  }

  if (fstat(fd, &st) < 0) {
    perror("open");
    exit(-1);
  }
  /* mapping */
  size = st.st_size;
  ptr = (DDD *)mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

  i = rcinfo(OPFILE);

  result = (DDD *)bsearch(
    key,             /* address of key element */
    ptr,             /* address of array to be searched */
    i,               /* number of elements in array */ 
    sizeof(DDD),   /* size of an element */ 
    opkeycomp);    /* function to compare */

  if (result == NULL)
    printf("Not found.\n");
  else {
    i = result - ptr;
    /* because of bsearch, must check key for backward. */
    while (strcmp(key, ptr[i - 1].chir) == 0)
      i--; 
    /* then found the beginning point. */    
    for (; strcmp(key, ptr[i].chir) == 0; i++)
      printf("OK:%d:%s,%s,%s,%s,%02d,%s\n",
             i, ptr[i].chir, ptr[i].ckan, ptr[i].phir,ptr[i].pkan,
             ptr[i].hin,ptr[i].conj);
  }
  close(fd);
  return 0;
}
