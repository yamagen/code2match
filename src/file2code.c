/* Last change: 2025/06/05-13:24:09.
 * Hilofumi.Yamamoto@anu.edu.au
 * usage: file2code kokin-segment-list
 */
#include <stdio.h>
#include <stdlib.h> /* getenv */
#include <string.h> /* strtok */
#include <math.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h> /* fstat */
#include <unistd.h>   /* fstat */
#include <sys/mman.h>
#include "config.h"


int rcinfo (char *filename);
void trim(char *buf);

int sublookup(RRR *ptr, int num, const char *str, char *name, 
              char *lin, int j, int hin, int type, int form)
{
  RRR *result;
  char *key;
  char tmp[128];
  long i = 0, k = 0, m = 2;

  strcpy(tmp,str);

  if (tmp[19] == '+')
    m = 3;
  else 
    m = 2;

  key = strtok(tmp,"+/");
  while (key != NULL) {
    result = (RRR *)bsearch(
      key,          /* address of key element */
      ptr,        /* address of array to be searched */
      num,          /* number of elements in array */ 
      sizeof(RRR),  /* size of an element */ 
      rvkeycomp);   /* function to compare */

    if (result != NULL) { 
      i = result - ptr;
      /* because of bsearch, must check key for backward. */
      while (strcmp(key, ptr[i - 1].sid) == 0)
        i--; 
      /* then found the beginning point. */
      printf("%d %s %s %d %02d %02d %02d %s -- %s %s\n",
             j,name,lin,m,hin,type,form,ptr[i].sid,ptr[i].hir,ptr[i].kan);
    } else {
        printf("N %s:%s:%s\n",name,lin);
        printf("Not found in DB: %s %2d %2d\n",key,type,form);
    }
    key = strtok(NULL,"+/");
  }
  return 0;
}

int lookup (char *buf,BBB *ptr,MMM *sbptr,RRR *rvptr,int num)
{
  char tmp[BSIZE];
  BBB   *result;
  int i = 0,k = FALSE, j = 0, m = 0, hin, type, form, rank = 0;
  char dl[] = " ";
  char *name,*lin,*sur,*kan,*hir,*sel,*substr;

  strcpy(tmp,buf);
  trim(tmp);

/*    i = sscanf(tmp,"%s %s %02d %02d %02d %s %s %s %s\n", */
/*           name,lin,&hin,&type,&form,sur,kan,hir,sel); */

  if ((name = strtok(tmp, dl)) == NULL)   exit(0);
  if ((lin  = strtok(NULL,dl)) == NULL)    exit(0);
  hin  = atoi(strtok(NULL,dl));
  type = atoi(strtok(NULL,dl));
  form = atoi(strtok(NULL,dl));
  if ((sur  = strtok(NULL,dl)) == NULL) exit(0);
  if ((kan  = strtok(NULL,dl)) == NULL) exit(0);
  if ((hir  = strtok(NULL,dl)) == NULL) exit(0);
  if ((sel  = strtok(NULL,dl)) == NULL);

  /* search */
  result = (BBB *)bsearch(
    hir,             /* address of key element */
    ptr,             /* address of array to be searched */
    num,             /* number of elements in array */ 
    sizeof(BBB),     /* size of an element */ 
    bgkeycomp);      /* function to compare */

  if (result != NULL) { 
    i = result - ptr;
    /* because of bsearch, must check key for backward. */
    while (strcmp(hir, ptr[i - 1].hir) == 0)
      i--; 
    /* then found the beginning point. */

    for (; strcmp(hir, ptr[i].hir) == 0; i++)
      if (strcmp(kan, ptr[i].kan) == 0 && hin == ptr[i].hin 
/*  for conjugation dependent classification e.g. "tara" -> "ba"   */
/*  If there is a conjugation classification, then you have to see */
/*  it's form. */
          && ( ptr[i].type == 0 || (ptr[i].type > 0 && form == ptr[i].form) )
        )
        if (strcmp(sel, "none") == 0 ) {
          k = TRUE;
          j++;
          if (j > 1)
            rank = 2;
          else if (ptr[i].subnum > -1)
            rank = 1;
          else
            rank = 0;
          printf("%d %s %s %d %02d %02d %02d %s ",
                 j,name,lin,rank,hin,type,form,ptr[i].sid);
          printf("%s %s %s\n",sur,hir,ptr[i].kan);
          if (ptr[i].subnum > -1) {
            substr = sbptr[ptr[i].subnum].subsid;
            sublookup(rvptr,num,substr,name,lin,j,hin,type,form);
          }
        } else
          if (strcmp(sel, ptr[i].sid) == 0) {
/*              printf("sel:%s %s\n",sel, ptr[i].sid); */
            k = TRUE;
            if (ptr[i].subnum > -1)
              rank = 1;
            else
              rank = 0;
            j++;
            printf("%d %s %s %d %02d %02d %02d %s ",
                   j,name,lin,rank,hin,type,form,ptr[i].sid);
            printf("%s %s %s\n",sur,hir,ptr[i].kan);
            if (ptr[i].subnum > -1) {
              substr = sbptr[ptr[i].subnum].subsid;
              sublookup(rvptr,num,substr,name,lin,j,hin,type,form);
            }
          }
  }
  if (k == FALSE)
    printf("N %s %s - %02d %02d %02d --Not found in DB-- %s %s %s\n"
           ,name,lin,hin,type,form,sur,hir,kan);
  return 0;
}

int main(int argc, char **argv)
{
  int   bg,sb,rv;
  BBB   *bgptr,*result;
  MMM   *sbptr;
  RRR   *rvptr;
  long  k,i;
  char buf[BSIZE];
  char bgname[BSIZE];
  char sbname[BSIZE];
  char rvname[BSIZE];
  char *iofilename;
  FILE *tf;
  struct stat bgst,sbst,rvst;

  char *path = getenv("HOME");
/*    char *path = "/home/yamagen"; */
  strcpy(bgname,path);  strcat(bgname,BGDB);
  strcpy(sbname,path);  strcat(sbname,SBDB);
  strcpy(rvname,path);  strcat(rvname,RVDB);

   /* open mmapfile */
  if ((bg = open(bgname,O_RDWR)) == -1) {    perror("open");    exit(-1);  }
  if ((sb = open(sbname,O_RDWR)) == -1) {    perror("open");    exit(-1);  }
  if ((rv = open(rvname,O_RDWR)) == -1) {    perror("open");    exit(-1);  }

  /* get filesize and mapping */
  if (fstat(bg, &bgst) < 0) {    perror("open");    exit(-1);  }
  bgptr = (BBB *)mmap(0,bgst.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,bg,0);
  if (fstat(sb, &sbst) < 0) {    perror("open");    exit(-1);  }
  sbptr = (MMM *)mmap(0,sbst.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,sb,0);
  if (fstat(rv, &rvst) < 0) {    perror("open");    exit(-1);  }
  rvptr = (RRR *)mmap(0,rvst.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,rv,0);

  if ( argv[1] == NULL || argv[1][0] == '-') 
    tf = stdin;
  else {
    iofilename = argv[1];
    if ((tf = fopen(iofilename,"r")) == NULL) {
      perror("fopen");
      exit(1);
    }
  }

  i = rcinfo(RCFILE);
  while (fgets(buf,BSIZE,tf) != NULL)
    lookup(buf,bgptr,sbptr,rvptr,i);

  fclose(tf);
  close(bg);
  close(sb);
  close(rv);
  return(0);
}



