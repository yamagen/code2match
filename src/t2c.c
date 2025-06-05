/* Time-stamp: "2012-01-28 23:36:39 yamagen"
 * Hilofumi.Yamamoto@anu.edu.au
 * usage: t2c keyword
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* getenv */
#include <math.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h> /* fstat */
#include <unistd.h>   /* fstat */
#include <sys/mman.h>
#include "config.h"


int rcinfo (char *filename);

int main(int argc, char **argv)
{
  int   fd,sb;
  long  psize,size,sbsize;
  BBB   *ptr,*result;
  MMM   *sbptr,*sbresult;
  long  k,i;
  char *key;
  struct stat st;
  struct stat sbst;
  char filename[BSIZE];
  char subfname[BSIZE];
  char *path = getenv("HOME");

  strcpy(filename,path);
  strcat(filename,BGDB);
  strcpy(subfname,path);
  strcat(subfname,SBDB);

  key = argv[1];

  /* open mmapfile */
  if ((fd = open(filename,O_RDWR)) == -1) {    perror("open");    exit(-1);  }
  if ((sb = open(subfname,O_RDWR)) == -1) {    perror("open");    exit(-1);  }


  if (fstat(fd, &st) < 0) {    perror("open");    exit(-1);  }
  size = st.st_size;
  if (fstat(sb, &sbst) < 0) {    perror("open");    exit(-1);  }
  sbsize = sbst.st_size;

  /* mapping */
  ptr = (BBB *)mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
  sbptr = (MMM *)mmap(0,sbsize,PROT_READ|PROT_WRITE,MAP_SHARED,sb,0);

  i = rcinfo(RCFILE);

  result = (BBB *)bsearch(
    key,             /* address of key element */
    ptr,             /* address of array to be searched */
    i,               /* number of elements in array */ 
    sizeof( BBB ),   /* size of an element */ 
    bgkeycomp);      /* function to compare */


  if (result == NULL)
    printf("Not found.\n");
  else {
    i = result - ptr;
    /* because of bsearch, must check key for backward. */
    while (strcmp(key, ptr[i - 1].hir) == 0)
      i--; 
    /* then found the beginning point. */   
    for (; strcmp(key, ptr[i].hir) == 0; i++) {
      printf("OK:%d:%s %s %s",i,ptr[i].hir,ptr[i].kan,ptr[i].sid);
      if (ptr[i].subnum > -1)
        printf(" %s",sbptr[ptr[i].subnum].subsid);
      printf("\n");
    }
  }
  close(fd);
  close(sb);
  return 0;
}
