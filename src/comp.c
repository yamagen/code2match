#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* getenv */
#include "config.h"

void rcwrite(int i, char *filename)
{
  FILE *rc;
  char *path = getenv("HOME");
  char rcfile[BSIZE];

/*    printf("path:%s\n",path); */
  strcpy(rcfile,path);
  strcat(rcfile,filename);
  printf("rcfile:%s\n",rcfile);

  if ((rc = fopen(rcfile, "w")) == NULL) {
    perror("fopen");
    exit(-1);
  }
  fprintf(rc,"%d\n",i);
  fclose(rc);
}

int rcinfo (char *filename)
{
  char *path = getenv("HOME");
  char rcfile[BSIZE];
  char buf[BSIZE];
  long i;
  FILE *rc;

  strcpy(rcfile,path);
  strcat(rcfile,filename);
  if ((rc = fopen(rcfile, "r")) == NULL) {
    perror("fopen");
    exit(-1);
  }
  fscanf(rc,"%d\n",&i);
  fclose(rc);
  return i;
}


int bgcomp( const void *p1, const void *p2 )
{
  BBB *t1, *t2;

  t1 = (BBB *)p1;
  t2 = (BBB *)p2;
  return strcmp( t1->hir, t2->hir );
}

int rvcomp( const void *p1, const void *p2 )
{
  RRR *t1, *t2;

  t1 = (RRR *)p1;
  t2 = (RRR *)p2;
  return strcmp( t1->sid, t2->sid );
}

int opcomp( const void *p1, const void *p2 )
{
  DDD *t1, *t2;

  t1 = (DDD *)p1;
  t2 = (DDD *)p2;
  return strcmp(t1->chir, t2->chir );
}

int bgkeycomp( const void *p1, const void *p2 )
{
  BBB *t2;

  t2 = (BBB *)p2;
  return strcmp((char *)p1, t2->hir );
}


int rvkeycomp( const void *p1, const void *p2 )
{
  RRR *t2;

  t2 = (RRR *)p2;
  return strcmp( (char *)p1, t2->sid );
}

int opkeycomp( const void *p1, const void *p2 )
{
  DDD *t2;

  t2 = (DDD *)p2;
  return strcmp((char *)p1, t2->chir );
}
