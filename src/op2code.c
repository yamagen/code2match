/* Time-stamp: "2012-01-28 23:36:17 yamagen"
 * Hilofumi.Yamamoto@anu.edu.au
 * usage: file2code kokin-segment-list
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


int lookup_hin (char *hir, char *kan, char *hin, DDD *ptr, int num);
int rcinfo (char *filename);
void trim(char *buf);

void cat(FILE *fp)
{
  int c;

  while ((c = getc(fp)) != EOF)
    putchar(c);
}

void getcont (char *num, char *cont, DDD *ptr, int size)
{
  char buf[BSIZE];
  char seg[6][128], *s;
  char tok[6][128], *t;
  char pos[6][128], *p;
  char *sur,*tmp,*pinfo,*kanji,*sid;
  long i = 0,k = 0, j = 0;
  long nseg = 0,ntok = 0, npos = 0, nsid = 0;
  cont = (char *)strtok(cont, "/");
  do {
    strcpy(seg[nseg++],cont);
  } while ((cont = (char *)strtok(NULL, "/")) != NULL);
  
  for (i = 0; i < nseg; i++) {
    ntok = 0;
    s = (char *)strtok(seg[i], ",");
    do {
      strcpy(tok[ntok++],s);
    } while ((s = (char *)strtok(NULL, ",")) != NULL);

    for (k = 0; k < ntok; k++) {
      npos = 0;
      t = (char *)strtok(tok[k], "]");
      do {
        strcpy(pos[npos++],t);
      } while ((t = (char *)strtok(NULL, "]")) != NULL);

      for (j = 0; j < npos; j++) {
        nsid = 1;
        sur = (char *)strtok(pos[j], "[");
        pinfo = (char *)strtok(NULL, ":");
        if ((kanji = (char *)strtok(NULL, ":")) == NULL)
          kanji = sur;
        if ((sid = (char *)strtok(NULL, ":")) == NULL)
          nsid = 0;
        printf("%s %1d%1d%1d",num,i+1,k+1,j+1);
        lookup_hin (sur, kanji, pinfo, ptr, size);
        if (nsid)
          printf(" %s",sid);
        else
          printf(" none");
        printf("\n"); 
      }
    }
  }
}

void getb (char *fld, DDD *ptr, int size)
{
  char *num,*tag,*cont;
  int i;
  num = (char *)strtok(fld, "|");
  tag = (char *)strtok(NULL, "|");
  cont = (char *)strtok(NULL, "|");
  if (strcmp("B",tag) == 0)
    getcont(num,cont,ptr,size);
}

void gettoken(FILE *fp, DDD *ptr, int size)
{
  char buf[BSIZE],str[BSIZE];
  long i;
  char *num,*hir,*kan,*hin,*name,*sel,*cont,*fld;
  char deli[] = " ";
  while (fgets(buf,BSIZE,fp) != NULL) {
    trim(buf); 
    if(strlen(buf) > 0 && buf[0] != '#') {
      strcpy(str,buf);
      getb (str,ptr,size);
    }
  }
}

int lookup_hin (char *hir, char *kan, char *hin, DDD *ptr, int num)
{
  DDD   *result;
  int i = 0,k = FALSE;
  /* search */

  result = (DDD *)bsearch(
    hir,             /* address of key element */
    ptr,             /* address of array to be searched */
    num,             /* number of elements in array */ 
    sizeof(DDD),     /* size of an element */ 
    opkeycomp);      /* function to compare */

  if (result != NULL) {
    i = result - ptr;
    /* because of bsearch, must check key for backward. */
    while (strcmp(hir, ptr[i - 1].chir) == 0)
      i--; 
    /* then found the beginning point. */    
    for (; strcmp(hir, ptr[i].chir) == 0;i++)
      if (strcmp(kan, ptr[i].ckan) == 0 && strcmp(hin, ptr[i].conj) == 0) {
        k = TRUE;
        printf(" %02d %02d %02d %s %s %s",
               ptr[i].hin, 
               ptr[i].type, 
               ptr[i].form, 
               ptr[i].chir, ptr[i].pkan, ptr[i].phir);
      }
  }
  if (k == FALSE)
    printf(" -- Not found in DB: %s %s %s\n",hir, kan, hin);

  return 0;
}

int main(int argc, char **argv)
{
  int   fd;
  long  psize,size;
  DDD   *ptr,*result;
  long  k,i;
  char filename[BSIZE];
  char *iofilename;
  FILE *tf;
  struct stat st;

  char *path = getenv("HOME");
/*    char *path = "/home/yamagen"; */
  strcpy(filename,path);
  strcat(filename,OPDB);

   /* open mmapfile */
  if ((fd = open(filename,O_RDWR)) == -1) {
    perror("open");
    exit(-1);
  }

  if (fstat(fd, &st) < 0) {
    perror("open");
    exit(-1);
  }
  size = st.st_size;
  /* mapping */
  ptr = (DDD *)mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

  if ( argv[1] == NULL || argv[1][0] == '-') 
    tf = stdin;
  else {
    iofilename = argv[1];
    if ((tf = fopen(iofilename,"r")) == NULL) {
      perror("fopen");
      exit(1);
    }
  }
/*  cat(tf);*/
  i = rcinfo(OPFILE);
  gettoken(tf,ptr,i);
  fclose(tf);

  close(fd);
  return(0);
}
