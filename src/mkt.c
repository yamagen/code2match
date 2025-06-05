/* Time-stamp: "2004-08-08 17:50:16 yamagen"
 * Hilofumi.Yamamoto@anu.edu.au
 * usage: mkt < table.txt
 */
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include "config.h"


int rcinfo (char *filename);
void trim(char *buf);

int main(int argc, char **argv)
{
  int bg,sb,rv;
  char c;
  long psize,bgsize,sbsize,rvsize;
  BBB *bgptr;
  MMM *sbptr;
  RRR *rvptr;
  long i,k;
  char buf[BSIZE],tmpbuf[BSIZE];
  char bgname[BSIZE];
  char sbname[BSIZE];
  char rvname[BSIZE];
  char deli[] = ":";
  FILE *fs; /* for tmpfile */
  char *path = getenv("HOME");
  char *tmpstr;

  strcpy(bgname,path);
  strcat(bgname,BGDB);
  strcpy(sbname,path);
  strcat(sbname,SBDB);
  strcpy(rvname,path);
  strcat(rvname,RVDB);


  printf("mapfile:%s\n",bgname);
  printf("subfile:%s\n",sbname);
  printf("revfile:%s\n",rvname);

  fs = tmpfile(); 
  if (fs == NULL) 
    exit(EXIT_FAILURE);

  /* scan the file and count the number of record */
  i = 0;
  k = 0;
  while (fgets(buf, BSIZE, stdin) != NULL) {
    if (buf[0] != '#' ) {
      trim(buf);
      fprintf(fs,"%s\n",buf);
      tmpstr = takeit(buf,tmpbuf,':',6);
      if(strlen(tmpstr))
        i++;
      k++;
    }
  }
  rewind(fs); 

  printf("record = %d / subcode = %d\n",k,i);

  /* open for mapfile */
  if ((bg = open(bgname,O_RDWR|O_CREAT,0666)) == -1) {
    perror("open:bgname");
    exit(-1);
  }

  /* open for subfile */
  if ((sb = open(sbname,O_RDWR|O_CREAT,0666)) == -1) {
    perror("open:sbname");
    exit(-1);
  }
  /* open for revfile */
  if ((rv = open(rvname,O_RDWR|O_CREAT,0666)) == -1) {
    perror("open:rvname");
    exit(-1);
  }

  /* Size calculation */
  psize = sysconf(_SC_PAGE_SIZE);
  bgsize = (k * sizeof(BBB) / psize + 1) * psize;
  sbsize = (i * sizeof(MMM) / psize + 1) * psize;
  rvsize = (k * sizeof(RRR) / psize + 1) * psize;

  /* Seek neccessary file size, then write '0' */
  if (lseek(bg,bgsize,SEEK_SET) < 0) {    perror("lseek");    exit(-1);  }
  if (lseek(sb,sbsize,SEEK_SET) < 0) {    perror("lseek");    exit(-1);  }
  if (lseek(rv,rvsize,SEEK_SET) < 0) {    perror("lseek");    exit(-1);  }

  if (read(bg,&c,sizeof(char)) == -1)    c='\0';
  if (read(sb,&c,sizeof(char)) == -1)    c='\0';
  if (read(rv,&c,sizeof(char)) == -1)    c='\0';

  if (write(bg,&c,sizeof(char)) == -1) { perror("write"); exit(-1); }
  if (write(sb,&c,sizeof(char)) == -1) { perror("write"); exit(-1); }
  if (write(rv,&c,sizeof(char)) == -1) { perror("write"); exit(-1); }

  /* map to the pointer 'bgptr' */
  bgptr = (BBB *)mmap(0,bgsize,PROT_READ|PROT_WRITE,MAP_SHARED,bg,0);
  if (bgptr == MAP_FAILED) {
    perror("mmap");
    exit(-1);
  }

  sbptr = (MMM *)mmap(0,sbsize,PROT_READ|PROT_WRITE,MAP_SHARED,sb,0);
  if (sbptr == MAP_FAILED) {
    perror("mmap");
    exit(-1);
  }

  rvptr = (RRR *)mmap(0,rvsize,PROT_READ|PROT_WRITE,MAP_SHARED,rv,0);
  if (rvptr == MAP_FAILED) {
    perror("mmap");
    exit(-1);
  }

  /* test */
  i = k = 0;
  while (fgets(buf,BSIZE,fs) != NULL) {
    if (buf[0] != '#') {
      trim(buf);
      strcpy(bgptr[i].sid, takeit(buf,tmpbuf,':',0));
      strcpy(rvptr[i].sid, bgptr[i].sid);
      bgptr[i].hin = atoi(takeit(buf,tmpbuf,':',1));
      rvptr[i].hin = bgptr[i].hin;
      bgptr[i].type= atoi(takeit(buf,tmpbuf,':',2));
      rvptr[i].type= bgptr[i].type;
      bgptr[i].form= atoi(takeit(buf,tmpbuf,':',3));
      rvptr[i].form= bgptr[i].form;
      strcpy(bgptr[i].kan, takeit(buf,tmpbuf,':',4));
      strcpy(rvptr[i].kan, bgptr[i].kan);
      strcpy(bgptr[i].hir, takeit(buf,tmpbuf,':',5));
      strcpy(rvptr[i].hir, bgptr[i].hir);
      tmpstr = takeit(buf,tmpbuf,':',8);
      if (strlen(tmpstr)) {
        bgptr[i].subnum = k;
        strcpy(sbptr[k++].subsid, tmpstr);
      } else {
        bgptr[i].subnum = -1;
      }
      i++;
    }
    /* if you want to write mmap to a file, msync(). */
    /* msync(bgptr,size,MS_ASYNC); */
  }

  /* save the number of array */
  rcwrite(i,RCFILE);
  
  printf("total number of record: %d\n",i);

  /* qsort for bsearch() */
  qsort( bgptr,          /* address of array to be sorted */
         i,              /* number of elements in array */
         sizeof(BBB),    /* size of an element */
         bgcomp );         /* function to compare */

  qsort( rvptr,          /* address of array to be sorted */
         i,              /* number of elements in array */
         sizeof(RRR),    /* size of an element */
         rvcomp );         /* function to compare */

  /* Write mmap to a file and sync */

  msync(bgptr,bgsize,0);
  msync(sbptr,sbsize,0);
  msync(rvptr,rvsize,0);

  /* unmap */
  if (munmap(bgptr,bgsize) == -1)    perror("munmap");
  if (munmap(sbptr,sbsize) == -1)    perror("munmap");
  if (munmap(rvptr,rvsize) == -1)    perror("munmap");

  close(bg);      /* mmap file close */
  close(sb);      /* mmap file close */
  close(rv);      /* mmap file close */
  fclose(fs);     /* tmpfile close */

  return 0;
}


