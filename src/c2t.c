/* Time-stamp: "2012-01-28 23:35:14 yamagen"
 * Hilofumi.Yamamoto@anu.edu.au
 * usage: c2t keyword
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
  long  size;
  RRR   *ptr,*result;
  long  k,i;
  char *key;
  struct stat st;
  char filename[BSIZE];
  char *path = getenv("HOME");

  key = argv[1];
/*    printf("key is %s\n",key); */

  strcpy(filename,path);
  strcat(filename,RVDB);

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
  ptr = (RRR *)mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

  i = rcinfo(RCFILE);

  result = (RRR *)bsearch(
    key,             /* address of key element */
    ptr,             /* address of array to be searched */
    i,               /* number of elements in array */ 
    sizeof( RRR ),   /* size of an element */ 
    rvkeycomp);      /* function to compare */

  if (result == NULL)
    printf("Not found.\n");
  else {
    i = result - ptr;
//    printf("OK:%d:%s %s %s",i,ptr[i].hir,ptr[i].kan,ptr[i].sid);
//    printf("\n");

    // backward
    while (strcmp(key,result->sid) == 0)
      result--;

    result++;
    while (strcmp(key,result->sid) == 0) {
        printf("%s %s %s",result->hir,result->kan,result->sid);
        printf("\n");
        result++;
    }
  }
  close(fd);
  return 0;
}



