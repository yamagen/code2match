/* Time-stamp: "2006-12-20 13:02:08 yamagen"
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

int main(void)
{
  int fd;
  char c;
  long psize,size;
  DDD *ptr;
  long i,k = 0;
  char buf[BSIZE];
  char filename[BSIZE];
  char deli[] = " ";
  FILE *fs; /* for tmpfile */
  char *path = getenv("HOME");

  strcpy(filename,path);
  strcat(filename,OPDB);
  printf("mapfile:%s\n",filename);

  fs = tmpfile(); 
  if (fs == NULL) 
    exit(EXIT_FAILURE);

  /* scan the file and count the number of record */
  while (fgets(buf, BSIZE, stdin) != NULL) {
    trim(buf);
    if (strlen(buf) && buf[0] != '#') {
      fprintf(fs,"%s\n",buf);
      k++;
    }
  }
  rewind(fs);

/*    printf("number of entry:%d\n",k); */

  /* open for mapfile */
  if ((fd = open(filename,O_RDWR|O_CREAT,0666)) == -1) {
    perror("open:filename");
    exit(-1);
  }

  /* Size calculation */
  psize = sysconf(_SC_PAGE_SIZE);
  size = (k * sizeof(DDD) / psize + 1) * psize;

  /* Seek neccessary file size, then write '0' */
  if (lseek(fd,size,SEEK_SET) < 0) {
    perror("lseek");
    exit(-1);
  }
  if (read(fd,&c,sizeof(char)) == -1)
    c='\0';

  if (write(fd,&c,sizeof(char)) == -1) {
    perror("write");
    exit(-1);
  }

  /* map to the pointer 'ptr' */
  ptr = (DDD *)mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    exit(-1);
  }

  /* test */
  i = 0;
  while (fgets(buf,BSIZE,fs) != NULL) {
    trim(buf);
    if ((char *)getfld(buf,deli,6) == NULL) { 
      printf("delimiter ? %d _%s_\n",i,deli); 
      exit(-1); 
    } 
    ptr[i].hin = atoi(getfld(buf,deli,0)); 
    ptr[i].type = atoi(getfld(buf,deli,1)); 
    ptr[i].form = atoi(getfld(buf,deli,2)); 
    strcpy(ptr[i].phir, getfld(buf,deli,4)); 
    strcpy(ptr[i].pkan, getfld(buf,deli,5)); 
    strcpy(ptr[i].conj, getfld(buf,deli,6)); 
    strcpy(ptr[i].chir, getfld(buf,deli,7)); 
    strcpy(ptr[i].ckan, getfld(buf,deli,8)); 
    i++;
    /* if you want to write mmap to a file, msync(). */
    /* msync(ptr,size,MS_ASYNC); */
  }

  /* save the number of array */
  rcwrite(i,OPFILE);
  printf("total number of record: %d\n",i);

  /* qsort for bsearch() */
  qsort( ptr,            /* address of array to be sorted */
         i,              /* number of elements in array */
         sizeof(DDD),    /* size of an element */
         opcomp );       /* function to compare */

  /* Write mmap to a file and sync */

  msync(ptr,size,0);

  /* unmap */
  if (munmap(ptr,size) == -1)
    perror("munmap");

  close(fd);      /* mmap file close */
  fclose(fs);     /* tmpfile close */

  return 0;
}
