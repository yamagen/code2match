/* Time-stamp: "2013-09-06 15:29:34 yamagen"
 * Hilofumi.Yamamoto@anu.edu.au
 * usage: cat op.dat ct.dat | pex [options] > file.txt
 */
#include <stdio.h>
#include <stdlib.h> /* getenv */
#include <string.h> /* strtok */
#include <math.h>
#include <fcntl.h>
#include <math.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h> /* fstat */
#include <unistd.h>   /* fstat */
#include <sys/mman.h>
#include "pex.h"

char *author;
int poem_number = 0;
CCC *opptr, *ctptr, *opadd, *ctadd;
int opnum = 0;
int ctnum = 0;
int inline_out      = 0;


int rcinfo (char *filename);
void trim(char *buf);


int jiritsugo(int pos) {
  int r = FALSE;

  if ( (0 < pos && pos < 41) || ( 45 < pos && pos < 57) )
    r = TRUE; 

  return r;
}

int invalidate_line (CCC *rstr, int num, int once)
{
  int i,j = 0;
  int debugon = 0;

  for (i = 0; i < num; i++)
    if (rstr[i].sub == 2 && rstr[i-1].sub == 0)
      rstr[i-1].sub = 1;

  for (i = 0; i < num; i++)
    if (rstr[i].hin > KIGOU_____ || strcmp(rstr[i].hir,"〜") == 0)
      rstr[i].e_valid = INVALID; 

/* eliminate the same stuff */
  if (once)
    for (i = 0; i < num; i++)
      for (j = i + 1; j < num; j++)
        if (rstr[j].e_valid && rstr[i].e_valid && 
            strcmp(rstr[i].sid,rstr[j].sid) == 0) {
          if (debugon) {
            printf("same: %s = %s:  %s = %s\n",rstr[i].sid,rstr[j].sid,
                   rstr[i].kan,rstr[j].kan);
          }
          rstr[j].e_valid = INVALID; 
        }

  return j;
}

int untie_match(int pos)
{
  int i;

  for (i = 0; i < opnum; i++)
    if (opptr[i].hin == pos) {
      opptr[i].e_size = 0;
      ctptr[opptr[i].e_pair].e_size = 0;
      ctptr[opptr[i].e_pair].e_pair = -1;
      opptr[i].e_pair = -1;
    }
  
  return 0;
}

int match_uni(int pos,int size)
{
  int i,k,p,j;

  for (i = 0; i < opnum; i++) {
    j = 0;
    if (opptr[i].hin == pos)
      for (k = 0; k < ctnum; k++) { 
        /*  have to avoid to register at the alt-place which the parent has */
        /*  already registered.                                             */
        if (ctptr[k].sub == PARENT) /* save parent's address */
          p = k;
        if (ctptr[k].sub == ALT) {  /* check if already occupied */
          for (; p <= k; p++)
            if (ctptr[p].e_size != NOMATCH) {
              k++;
              break; /* you can't register k here */
            }
        }
        if (strncmp(opptr[i].sid,ctptr[k].sid,size) == 0
            && ctptr[k].hin != 48 )
          j++;
        if (j > 1)
          opptr[i].e_nalt = 1;  /* found two candidates */
      }
  }

/*    The matching for the tokens which appear only once begins. */
  for (i = 0; i < opnum; i++)
    if (opptr[i].hin == pos && opptr[i].e_nalt == 0)
      for (k = 0; k < ctnum; k++) { 
        if (ctptr[k].sub == PARENT) /* save parent's address */
          p = k;
        if (ctptr[k].sub == ALT) {  /* check if already occupied */
          for (; p <= k; p++)
            if (ctptr[p].e_size != NOMATCH) {
              k++;
              break; /* you can't register k here */
            }
        }
        if (strncmp(opptr[i].sid,ctptr[k].sid,size) == 0
            && opptr[i].e_size < size
            && ctptr[k].e_size < size
            && ctptr[k].hin != 48 ) {
          opptr[i].e_pair = k;
          ctptr[k].e_pair = i;
          opptr[i].e_size = size; 
          ctptr[k].e_size = size; 
          break;
        }
      }

  return 0;
}

int pos_condition(CCC opptr, CCC ctptr)
{
  int r = FALSE;
/*    the condition is that not adverb(55) and not bound verb */
/*    or that if adverb(55) and ct must be (55) as well. */
/*   check okumura 375 */
/*    if ((ophin != 55 && cthin != 48) || (ophin == 55 && cthin == 55)) */
  if (opptr.type == ctptr.type && opptr.hin == P_KAKARIJO)
    r = TRUE;
  if (opptr.hin != P_KAKARIJO)
    r = TRUE;

  return r;
}

int match_pos(int pos,int size)
{
  int i,k,p,j;
  
  for (i = 0; i < opnum; i++) {
    if (opptr[i].hin != pos)
      continue;
    for (k = 0; k < ctnum; k++) { 
      if (ctptr[k].sub == PARENT) /* save parent's address */
        p = k;
      if (ctptr[k].sub == ALT) {  /* check if already occupied */
        for (; p <= k; p++)
          if (ctptr[p].e_size != NOMATCH) {
            k++;
            break; /* you can't register k here */
          }
      }
      if (strncmp(opptr[i].sid,ctptr[k].sid,size) == 0
          && opptr[i].e_size < size && ctptr[k].e_size < size 
        ) {
        opptr[i].e_size = size; opptr[i].e_pair = k;
        ctptr[k].e_size = size; ctptr[k].e_pair = i;
        break; /* found place for k */
      }
    }
  }
  return 0;
}

int match_bwn(int pos,int size)
{
  int i,k,p,j,m,n,result;
  static int number = 0;

  for (i = 0; i < opnum; i++) {
    if (opptr[i].hin != pos)
      continue;
    for (k = 0; k < ctnum; k++) { 
      /* found right place */
      if (strncmp(opptr[i].sid, ctptr[k].sid, size) == 0
          && opptr[i].e_size < size
          && ctptr[k].e_size < size
        ) {
        /* valid range calc from m to n */
          
        for (m = i; m > 0; m--) {
          if (opptr[m].e_pair > -1 && opptr[m].e_size > 10) 
            break;
        }
        for (n = i; n < opnum; n++) 
          if (opptr[n].e_pair > -1 && opptr[n].e_size > 10)
            break;
        
        if (opptr[n].e_pair < opptr[m].e_pair) {
          /* in case of wrong anchor word  */
          m = opptr[m].e_pair;
          n = ctnum;
        } else {
          m = opptr[m].e_pair;
          n = opptr[n].e_pair;
        }

        if ( k > m && k < n) {
          opptr[i].e_size = size; opptr[i].e_pair = k;
          ctptr[k].e_size = size; ctptr[k].e_pair = i;
          result = 1;
        } else
          result = 0;
      }
    }
  }
  return 0;
}


int show_pairs(void)
{
  int i, k = 1;

  for (i = 0; i < opnum; i++)
    if (opptr[i].e_pair != NOPAIR && opptr[i].e_valid == VALID)
      printf("%2d %2d %2d %24s %02d <-> %02d %s\n", 
             k++,
             opptr[i].e_size,
             opptr[i].hin,
             opptr[i].kan,i,
             opptr[i].e_pair,
             ctptr[opptr[i].e_pair].kan);
  printf("\n");
  return 0;
}

int select_item(CCC *aptr, int anum)
{
  int i, j, change = 0, p = 0, m = 0;

  /* from op side */
  for (i = 0; i < anum; i++) { /* seek selection block */
    change = 0;
    if (aptr[i].sub == PARENT) 
      p = i;
    if (aptr[i].sub == ALT) {
      if (aptr[p].e_size < aptr[i].e_size) {
        aptr[p].e_valid = 0;
        p = i;
      } else 
        aptr[i].e_valid = 0;
    }
    
    if (aptr[i].sub == CHILD) {
      for (j = i; aptr[j].sub == CHILD; j++) {
/*      printf("parent %s(%d)[%d] => ",aptr[p].kan,aptr[p].e_size,p); */
/*      printf("child  %s(%d)[%d]\n",aptr[j].kan,aptr[j].e_size,j);   */
        /* over 13 size match then swap cf. komachiya 657 */
        if (aptr[p].e_size < 13 && aptr[p].e_size < aptr[j].e_size ) {
          change = 1;
          m = j;
        }
      }
      if (change) {
        aptr[p].e_valid = 0;
        i = j - 1;
        p = m;
      } else {
        for (;aptr[i].sub == CHILD; i++) {
          aptr[i].e_valid = 0;
        }
        i = i - 1 ;
      }
    }
  }
  return 0; 
}

#if 0
/*
  Estimation of the CT portion corresponding to the unmatched OP token.

  Here, I will estimate the location in CT of unmatched token of OP.
  First of all, I seek for the number of the unmatched token of OP, 
  if I find the number, I get the begining number, OP(B) and the ending 
  number, OP(E) by seeking forward and backward for the number of the 
  MATCHED token. We assume that the CT portion corresponding to the 
  unmatched OP token can be located between CT(B) and CT(E).
*/
#endif

int  b_word(int num) {
  int r = FALSE;

/*  Only 64 "て" should be admitted. */
  if (num == 74 || num == 69 || num == 49  ||
      num == 64 || num == 48 || num == 52  ||    num == 47 )
    r = TRUE;
  return r;
}

int  kigo(int num) {
  int r = FALSE;
  if (num > 76 && num < 84)
    r = TRUE;
  return r;
}

int show_predicate(void)
{

  int i,j,k,m = 0,n = 0,p, found = 0, prev_out = -1;

  for (i = 0; i < opnum; i++) 
    if ( opptr[i].e_valid && opptr[i].hin == 47 && opptr[i].e_pair > -1) {
      m = opptr[i].e_pair;
      for (j = i + 1; j < opnum; j++)
        if (opptr[j].e_valid)
          if (b_word(opptr[j].hin))
            n = opptr[j].e_pair;
          else
            break;

      if (n > m) {
        found = 1;
      } else {
        found = 0;
        n = 0;
        for (k = m + 1; k < ctnum; k++)
          if (m + 1 > ctnum)
            break;
          if (ctptr[k].e_valid)
            if (b_word(ctptr[k].hin)) {
              found = 1;
              n = k;
            } else
              break;
      }

      if (found) {
        printf("PRED: %s %3d [%02d",author,poem_number,i);
        for (k = i; k < j ; k++)
          if (opptr[k].e_valid)
            if (strcmp(opptr[k].sur,"--") != 0) {
              if (strcmp(opptr[k].sur,opptr[prev_out].sur) != 0) {
                printf("|%s",opptr[k].sur);
                prev_out = k;
              }
            } else {
              for (p = k; strcmp(opptr[p].sur,"--") == 0 ; p--);
              if (strcmp(opptr[p].sur,opptr[prev_out].sur) != 0) {
                printf("|%s",opptr[p].sur);
                prev_out = p;
              }
            }
        i = j;
        printf("|%02d] => ",k - 1);
        printf("[%02d",m);
        for (k = m; k <= n ; k++) 
          if (ctptr[k].e_valid && strcmp(ctptr[k].sur,"--") != 0) {
            if ( strcmp(ctptr[k].sur,ctptr[prev_out].sur) != 0) {
              printf("|%s",ctptr[k].sur);
              prev_out = k;
            }
          } else {
              for (p = k; strcmp(ctptr[p].sur,"--") == 0 ; p--);
              if ( strcmp(ctptr[p].sur,ctptr[prev_out].sur) != 0) {
                printf("|%s",ctptr[p].sur);
                prev_out = p;
              }
          }
        printf("|%02d]",n);
        printf("\n");
      }
    }
  return 0; 
}

int show_verbseg(void)
{
  int i,j,k;
  int m = 0;
  int n = 0;
  int found = 0;

  for (i = opnum; i >= 0; i--) {
    if (i == 0) { // 発見できず
      printf("%04d %-10s ", poem_number, author);
      printf("could not find the corresponding verb.\n");
      exit(0);
    }
    if (!opptr[i].e_valid)
      continue;
    if (opptr[i].hin != 47)
      continue;
    if (opptr[i].e_pair < 0) 
      continue;
    if (opptr[i].sub > 1) 
      continue;
    if (opptr[i].form == 20)  // 連体形
      continue;
    m = opptr[i].e_pair;
    break;
  }

  putchar('\n');

  for (k = i; k < opnum; k++) {
    if (opptr[k].sub < 2) {
      printf("%04d %02d %-10s ", poem_number, k, author);
      printf("OP %s %02d %02d %02d %s\n", 
             opptr[k].sid,opptr[k].hin,opptr[k].sub,opptr[k].form,
             opptr[k].sur);
    }
    if ((k != i && opptr[k + 1].hin == 47) ||  // 動詞
        opptr[k + 1].hin ==  2 ||  // 名詞
        opptr[k + 1].hin == 61 ||  // 助詞
        opptr[k + 1].hin == 65 ||  // 助詞
        opptr[k + 1].hin == 16 ) { // 副詞
      printf("%04d %-10s ", poem_number, author);
      printf("NX[%02d]%s(%02d)-(%02d)\n", 
             k+1, opptr[k + 1].kan,opptr[k + 1].hin,opptr[k + 1].sub);
      break;
    }
  }

  for (;m < ctnum; m++) {
    if (ctptr[m].hin == 2) // 普通名詞
      break; 
    if (ctptr[m].hin == 16) // 数量名詞
      break; 
    if (ctptr[m].hin == 51) // 形容詞
      break; 
    if (ctptr[m].hin == 57)
      break; 
    if (ctptr[m].hin == 61) // 引用と
      break; 
    if (ctptr[m].hin == 62) // 引用と
      break; 
    if (ctptr[m].hin == 14)
      break; 
    if (!ctptr[m].e_valid)
      continue; 
//    if (ctptr[m].sub > 1) 
//      continue;
    for (n = m; n > 0; n--)
      if (strcmp(ctptr[n].sur,"--"))
        break;
    printf("%04d %02d %-10s ", poem_number, m, author);
    printf("CT %s %02d %02d %02d %s\n",
           ctptr[m].sid,ctptr[m].hin,
           ctptr[m].sub,ctptr[m].e_valid,
           ctptr[n].sur);
  }

  return 0; 
}


int confirm_item(void)
{

/*    最終的にここでチェックして序列がおかしい場合にはおかしい組合せの箇所を */
/*    記録しておいて再計算する。 */

  int i;

  /* check out if the op tokens judged as a pair is valid or not */
  for (i = 0; i < ctnum; i++)
    if (ctptr[i].e_pair != NOPAIR && opptr[ctptr[i].e_pair].e_valid == INVALID)
      ctptr[i].e_pair = NOPAIR;

  return 0; 
}

int calc_point(int line)
{
  int i;
  int opm = 0;
  int opv = 0;
  int ctv = 0;
  int ope = 0;
  int opf = 0;
  int opg = 0;
  int cte = 0;
  int ctp  = 0;
  int subflag = 0;

  double e = 0.0;
  double f = 0.0;
  double g = 0.0;
  double t = 0.0;
  double r = 0.0;
  double w = 0.0;
  double a = 0.0;
  double p1 = 0.0;
  double p2 = 0.0;
  double d = 0.0;
  double thv = 0.0;

  for (i = 0; i < opnum; i++)
    if (opptr[i].e_valid){
      if (opptr[i].e_size >= EXACT)
        ope++;
      else if (opptr[i].e_size >= FIELD)
        opf++;
      else if (opptr[i].e_size >= GROUP)
        opg++;
      if (opptr[i].e_size > NOMATCH)
        opm++;
      opv++;
    }

  if (line) 
    printf("OP=%d ",opv);
  else {
    printf("\n");
    printf("OP(valid number of items)              = %d\n",opv);
  }

  e = (double)ope/opv;
  if (line)
    printf("%.2f:", e);
  else
    printf("E (ratio of exact agreement)     %2d/%d = %5.3f\n", ope,opv,e);

  f = (double)opf/opv;
  if (line)
    printf("%.2f:", f);
  else
    printf("F (ratio of field agreement)     %2d/%d = %5.3f\n", opf,opv,f);

  g = (double)opg/opv;
  if (line)
    printf("%.2f ", g);
  else
    printf("G (ratio of group agreement)     %2d/%d = %5.3f\n", opg,opv,g);

  t = (double)opm/opv;
  if (line)
    printf("%.2f", t);
  else
    printf("T (ratio of total agreement)     %2d/%d = %5.3f\n", opm,opv,t);

  r = 1 - (double)opm/opv;
  if (line)
    printf("(%.2f) ", r);
  else
    printf("U (ratio of unmatched)           1 - T = %5.3f\n",r);

  for (i = 0; i < ctnum; i++) 
    if (ctptr[i].e_valid) {
      if (ctptr[i].e_size >= EXACT)
        cte++;
      else if (ctptr[i].e_size > NOMATCH)
        ctp++;
      ctv++;
    }

  if (line)
    printf("CT=%d ", ctv);
  else {
    printf("\n");
    printf("CT(valid number of items)              = %d\n",ctv);
  }

  w = (double)cte/ctv;
  if (line)
    printf("%.2f:", w);
  else
    printf("W (ratio of original word use)   %2d/%2d = %5.3f\n", cte,ctv, w);

  a = 1 - (double)cte/ctv;
  if (line)
    printf("%.2f ", a);
  else {
    printf("A (ratio of annotation)          1 - W = %5.3f\n",a);
    printf("- breakdown of the annotation -\n");
  }

  p1 = (double)ctp/ctv;
  if (line)
    printf("%.2f:", p1);
  else
    printf("  P1(ratio of FG paraphrased)  (F+G)/V = %5.3f\n",p1);

  p2 = (a - p1) * r;
  if (line)
    printf("%.2f:", p2);
  else
    printf("  P2(ratio of  U paraphrased) (A-P1)*U = %5.3f\n",p2);

  d = a - p1 - p2;
  if (line)
    printf("%.2f ", d);
  else
    printf("  D (ratio of purely added)   A-(P1+P2)= %5.3f\n",d);

  thv = 1.0 - (double)opv/ctv;
  if (line)
    printf("%.2f", thv);
  else
    printf("H (theoretical value)          1-%d/%d = %5.3f\n",opv,ctv,thv);

  if (line)
    printf("(%.2f)", fabs(d-thv));
  else
    printf("Gap:                          fabs(D-H)= %5.3f\n",fabs(d-thv));

/*    if (line) */
/*      printf("(%.2f)", fabs(d-thv)); */
/*    else */
/*      printf("Chi^2:                       Chi^2     = %5.3f\n", */
/*             (d-thv) * (d-thv) / thv ); */

  printf("\n");

  return 0;
}

int evaluate(void)
{
  int pos_check = 1;
  int group_out = 1;
/*
    First, you should evaluate exactly matched items and fieldly matched 
    items.
    Next, if an exactly matched item is failed, 
    then the item should be checked out if it can be fieldly matched.
 */

/*  jiritsugo check  */

/*  check jiritsugo, nouns and verbs */
  match_uni(VERB_IPPAN, EXACT);
  match_uni(VERB_IPPAN, FIELD); /* matsuda 470 */


  match_uni(AUXILIARY_, EXACT);
  match_uni(AUXILIARY_, FIELD);
  match_uni(AUXILIARY_, GROUP);
            
  match_uni(NOUN_ADVER, EXACT);
  match_uni(NOUN_SVERB, EXACT);
  match_uni(NOUN_PLACE, EXACT);
  match_uni(NOUN_IPPAN, EXACT);
  match_uni(NOUN_HJADV, EXACT);
            
  match_uni(NOUN_SVERB, FIELD);
  match_uni(NOUN_ADVER, FIELD);
  match_uni(NOUN_PLACE, FIELD);
  match_uni(NOUN_IPPAN, FIELD); /* matsuda 317 miyuki */
  match_uni(NOUN_HJADV, FIELD);
            
  match_bwn(P_RENTAIKA, EXACT);
  match_bwn(P_RENTAIKA, FIELD);
  match_bwn(P_RENTAIKA, GROUP);

/*  if found enough anchor tokens, then check it out by bwn. */

  match_pos(PRONOUN_IP, EXACT);/* komachiya 232 tare */
  match_bwn(PRONOUN_IP, FIELD);
  match_bwn(PRONOUN_IP, GROUP);
            
  match_bwn(NOUN_HJIPP, EXACT);
  match_bwn(NOUN_HJIPP, FIELD);
  match_bwn(NOUN_HJIPP, GROUP);
            
  match_bwn(VERB_SUFIX, EXACT);
  match_bwn(VERB_SUFIX, FIELD);
  match_bwn(VERB_SUFIX, GROUP);
            
  match_bwn(VERB_HIJIR, EXACT);
  match_bwn(VERB_HIJIR, FIELD);
  match_bwn(VERB_HIJIR, GROUP);

/* the same process as 138, tara -> ba  */

  match_pos(ADV__IPPAN, EXACT);
  match_pos(ADV__JOSHI, EXACT);
  match_pos(ADV__IPPAN, FIELD);
  match_pos(ADV__JOSHI, FIELD);
  match_pos(ADV__IPPAN, GROUP);
  match_pos(ADV__JOSHI, GROUP);
            
  match_pos(INTERJECT_, EXACT);
  match_pos(INTERJECT_, FIELD);
  match_pos(INTERJECT_, GROUP);

/*  particle check */

  match_bwn(P_KA_IPPAN, EXACT);
  match_bwn(P_KA_IPPAN, FIELD);
  match_bwn(P_KA_IPPAN, GROUP);
            
  match_bwn(P_KA_RENGO, EXACT);
  match_bwn(P_KA_RENGO, FIELD);
  match_bwn(P_KA_RENGO, GROUP);
            
  match_bwn(P_SETUZOKU, EXACT);
  match_bwn(P_SETUZOKU, FIELD);
  match_bwn(P_SETUZOKU, GROUP);
            
  match_bwn(P_SHU_JOSI, EXACT);
  match_bwn(P_SHU_JOSI, FIELD);
  match_bwn(P_SHU_JOSI, GROUP);
            
  match_bwn(ADJ_SUFFIX, EXACT);
  match_bwn(ADJ_SUFFIX, FIELD);
  match_bwn(ADJ_SUFFIX, GROUP);
            
  match_pos(P_FUKUJOSI, EXACT);
  match_bwn(P_KA_INNYO, EXACT);
  match_bwn(P_SPECIAL_, EXACT);
  match_bwn(P_ADV_____, EXACT);
  match_pos(P_FUKUJOSI, FIELD);
  match_bwn(P_KA_INNYO, FIELD);
  match_bwn(P_SPECIAL_, FIELD);
  match_bwn(P_ADV_____, FIELD);
            
  match_pos(NOUN_SUIPP, EXACT);
  match_pos(NOUN_SUIPP, FIELD);

/*  recheck */

  match_bwn(NOUN_SVERB, EXACT);
  match_bwn(NOUN_ADVER, EXACT);
  match_bwn(NOUN_PLACE, EXACT);
            
  match_bwn(NOUN_SVERB, FIELD);
  match_bwn(NOUN_ADVER, FIELD);
  match_bwn(NOUN_PLACE, FIELD);
  match_bwn(NOUN_IPPAN, FIELD);

  untie_match(VERB_IPPAN);

  match_bwn(VERB_IPPAN, EXACT);
  match_bwn(VERB_IPPAN, FIELD);
            
  match_bwn(AUXILIARY_, EXACT);
  match_bwn(AUXILIARY_, FIELD);
  match_bwn(AUXILIARY_, GROUP);

/*  group level check have to be done at last */

  match_bwn(CONJUNC___, EXACT);
  match_bwn(CONJUNC___, FIELD);
            
  match_bwn(NOUN_IPPAN, GROUP);
  match_bwn(NOUN_PLACE, GROUP);
  match_bwn(NOUN_ADVER, GROUP);
  match_bwn(NOUN_SVERB, GROUP);
  match_bwn(VERB_IPPAN, GROUP);
            
  match_bwn(NOUN_SUIPP, GROUP);
  match_bwn(CONJUNC___, GROUP);
            
  match_pos(P_FUKUJOSI, GROUP);
  match_bwn(P_KA_INNYO, GROUP);
  match_bwn(P_SPECIAL_, GROUP);
  match_bwn(P_ADV_____, GROUP);

#if 0
/*
  Since the order of kakari-joshi often different from that of Modern langauage,
  use only pos algorithm.
  Also, kakari joshi cannot be compared by the group matching level since 
  that level can match many joshi too much easily.
*/
#endif

  match_pos(P_KAKARIJO, EXACT);
  match_pos(P_KAKARIJO, FIELD);
/*    match_pos(P_KAKARIJO, GROUP); */

/*    252 ぞ...鳴くなる(連体形:係結び) */

#if 0
/*
  Since an adjective can become both a predicate and a modifier,
  its location does not depend on the order and we can not use
  the bwn algorithm.
*/
#endif

  match_pos(NOUN_ADNOM, EXACT);        
  match_pos(NOUN_ADNOM, FIELD);
  match_pos(NOUN_ADNOM, GROUP);
            
  match_pos(ADNOMINAL_, EXACT);        
  match_pos(ADNOMINAL_, FIELD);
  match_pos(ADNOMINAL_, GROUP);
            
  match_pos(ADJ_JIRITU, EXACT);
  match_pos(ADJ_JIRITU, FIELD);
  match_pos(ADJ_JIRITU, GROUP);

  return 0;
}

int get_op_number (CCC *rstr)
{
  return rstr[0].num;
}

char *get_ct_author (CCC *rstr)
{
  return rstr[0].name;
}

int show_valid (CCC *rstr, int num)
{
  int i;
  for (i = 0; i < num; i++)
    if (rstr[i].e_valid != INVALID)
      printf("%2d %2d %s %s\n",i,rstr[i].hin,rstr[i].sid,rstr[i].sur);
  return 0;
}

int browse (CCC *rstr, int num)
{
  int i;

  for (i = 0; i < num; i++) { 
    printf("%2d %2d %2d %2d %2d %2d %2d %2d  %s %s %s",
           i,
           rstr[i].sub,
           rstr[i].e_valid, 
           rstr[i].e_size, 
           rstr[i].e_pair,
           rstr[i].hin,
           rstr[i].type,
           rstr[i].form,
           rstr[i].sid,
           rstr[i].sur,
           rstr[i].kan);
    if (rstr[i].e_valid && rstr[i].e_pair < 0)
      printf("*");
    if (rstr[i].e_nalt > 0)
      printf("+");
    printf("\n");
  }
  return 0;
}

int show_original (CCC *rstr, int num)
{
  int i;
  for (i = 0; i < num; i++)
    if (rstr[i].sub < 2)
      printf("%s",rstr[i].sur);
  printf("\n");
  return 0;
}

int line2token (char *istr, CCC *rstr)
{
  int i = 0, idx = 0;
  char *ptr;

  ptr = strtok(istr," ");  idx = atoi(ptr);
  ptr = strtok(NULL," ");  strcpy(rstr->name,ptr);
  ptr = strtok(NULL," ");  rstr->num = atoi(ptr) ;
  ptr = strtok(NULL," ");  rstr->sub = atoi(ptr);
  ptr = strtok(NULL," ");  rstr->hin = atoi(ptr);
  ptr = strtok(NULL," ");  rstr->type= atoi(ptr);
  ptr = strtok(NULL," ");  rstr->form= atoi(ptr);
  ptr = strtok(NULL," ");  strcpy(rstr->sid,ptr);
  ptr = strtok(NULL," ");  strcpy(rstr->sur,ptr);
  ptr = strtok(NULL," ");  strcpy(rstr->hir,ptr);
  ptr = strtok(NULL," ");  strcpy(rstr->kan,ptr);
  rstr->e_valid = VALID;
  rstr->e_size = NOMATCH;
  rstr->e_pair = NOPAIR;
  rstr->e_nalt = FALSE;

  return 0;
}

int main(int argc, char **argv)
{
  long i,k,j,t;
  char buf[BSIZE];
  FILE *fs; /* for tmpfile */
  int original_out    = 0;
  int pair_out        = 0;
  int predicate_out   = 0;
  int verbseg_out     = 0;
  int show_valid_on   = 0;
  int c;

  memset(buf,0,BSIZE);

  while(1) {
    c = getopt(argc, argv, "adhiloprstuvV"); /* "abc" is an option list */
    if (c == -1)
      break;
    switch (c) {
    case 'i': inline_out = 1;      break;
    case 'd': predicate_out = 1;   break;
    case 'v': verbseg_out = 1;   break;
    case 'o': original_out = 1;    break;
    case 'p': pair_out = 1;        break;
    case 's': show_valid_on = 1;   break;
    case 'a':
      original_out = pair_out = 1;
      break;
    case 'h': fprintf(stderr,USAGE); break;
      exit(EXIT_FAILURE);
    case 'V':
      fprintf(stdout,VERSION);
      break;
      exit(EXIT_FAILURE);
    default:
      fprintf(stderr, "%s: unknown arg %s\n", PROG_NAME, argv[1]);
      exit(EXIT_FAILURE);
    }
  }

  /* argc and argv are not affected by parsing */
  while (argc > 0) {
    /* printf ("[%s]", argv[0]); */
    --argc;
    ++argv;
  }

  fs = tmpfile(); 
  if (fs == NULL)
    exit(EXIT_FAILURE);

  /* scan the file and count the number of record. */
  i = 0;
  k = 0;
  j = 0;
  while (fgets(buf, BSIZE, stdin) != NULL) {
    trim(buf);
    if (strlen(buf) && buf[0] != '#') { /*  skip comment lines. */
      if (buf[0] == 'N') {
        printf("%s\n",buf);
        continue;
      }
      if (buf[2] == 'K') /* kokinshu */
        i++;
      else
        k++;
      fprintf(fs,"%s\n",buf);
    }
  }
  rewind(fs);

  opnum = i;
  ctnum = k;

  opptr = (CCC *)malloc( sizeof(CCC) * (i + 1)); 
  if (opptr == NULL) {    perror("malloc");        exit(EXIT_FAILURE);  }
  ctptr = (CCC *)malloc( sizeof(CCC) * (k + 1)); 
  if (ctptr == NULL) {    perror("malloc");        exit(EXIT_FAILURE);  }

  opadd = opptr;
  ctadd = ctptr;
  while (fgets(buf,BSIZE,fs) != NULL) {
    trim(buf);
    if (buf[2] == 'K') /* kokinshu */
      line2token(buf,opadd++);
    else
      line2token(buf,ctadd++);
  }
  fclose(fs);

  j = invalidate_line(opptr,opnum,0);
  j = invalidate_line(ctptr,ctnum,0);

  if (show_valid_on) {
    show_valid(opptr,opnum);
    show_valid(ctptr,ctnum);
  }
  poem_number = get_op_number(ctptr);
  author = get_ct_author(ctptr);

  evaluate();
  select_item(opptr,opnum);
  select_item(ctptr,ctnum);

/*    confirm_item(opptr,i,ctptr,k); */

  if (original_out) {
    printf("OP: ");
    show_original(opptr,opnum);
    printf("CT: ");
    show_original(ctptr,ctnum);
    printf("\n");
  }

  if (pair_out)      show_pairs();
  if (predicate_out) show_predicate();
  if (verbseg_out) show_verbseg();

  free(opptr);
  free(ctptr);

  return EXIT_SUCCESS;
}

