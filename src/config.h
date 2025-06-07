#define PROG_NAME "code2match"
#define VERSION PROG_NAME " Last change: 2025/06/05-21:01:04.\n"
#define TRUE 1
#define FALSE 0
#define USAGE                                                                  \
  "%% " PROG_NAME " [-ahv] file....\n"                                         \
  "  -a   print all data\n"                                                    \
  "  -b   print between check\n"                                               \
  "  -c   print calculation table\n"                                           \
  "  -d   print predicate part out\n"                                          \
  "  -e   once matched out (bag of words option)\n"                            \
  "       use it with other options\n"                                         \
  "  -i   print calculation in line style\n"                                   \
  "  -l   print token list table\n"                                            \
  "  -o   print original poem out\n"                                           \
  "  -p   print pair token table\n"                                            \
  "  -r   print residual\n"                                                    \
  "  -s   print valid on\n"                                                    \
  "  -t   print title\n"                                                       \
  "  -u   print unmatched portion\n"                                           \
  "  -h   print this help\n"                                                   \
  "  -v   print " PROG_NAME " version\n"                                       \
  "(c) 2025 H. Yamamoto yamagen@ila.titech.ac.jp\n"

/*  #define NUMBER 45000 */
#define BSIZE 512
#define RCFILE "/.t2crc"
#define OPFILE "/.opt2crc"
#define OPDB "/.op.map"
#define BGDB "/.bg.map"
#define SBDB "/.sb.map"
#define RVDB "/.rv.map"

#define NOCHILD 0
#define PARENT 1
#define ALT 2
#define CHILD 3
/*  12345678901234567890 */
/*  BG-01-1234-01-010-A  */
#define EXACT 17 /*  BG-01-1234-01-010    */
#define FIELD 13 /*  BG-01-1234-01        */
#define GROUP 10 /*  BG-01-1234           */
#define HLIST 8  /*  BG-01-12             */
#define NOMATCH 0
#define INVALID 0
#define VALID 1
#define NOPAIR -1

#define NOUN______ 1
#define NOUN_IPPAN 2
#define NOUN_PROPE 3
#define NOUN_P_IPP 4
#define NOUN_PERON 5
#define NOUN_PE_IP 6
#define NOUN_PE_LA 7
#define NOUN_PE_FI 8
#define NOUN_P_ORG 9
#define NOUN_PLACE 11
#define PRONOUN_IP 14
#define PRONOUN_CO 15
#define NOUN_ADVER 16
#define NOUN_SVERB 17
#define NOUN_ADNOM 18
#define NOUN_NUMBE 19
#define NOUN_HJIPP 21
#define NOUN_HJADV 22
#define NOUN_HJAUX 23
#define NOUN_HJADJ 24
#define NOUN_SPECI 26
#define NOUN_SUIPP 28
#define NOUN_SUPER 29
#define NOUN_SUPLA 30
#define NOUN_SAHEN 31
#define NOUN_SUAUX 32
#define NOUN_SUADJ 33
#define NOUN_SUADV 34
#define NOUN_SUNUM 35
#define NOUN_SUSPE 36
#define NOUN_CONJU 37

#define PRE_______ 41
#define PRE_NOUN__ 42
#define PRE_VERB__ 43
#define PRE_ADJ___ 44
#define PRE_NUM___ 45
#define VERB______ 46
#define VERB_IPPAN 47
#define VERB_HIJIR 48
#define VERB_SUFIX 49
#define ADJ_______ 50
#define ADJ_JIRITU 51
#define ADJ_HIJIRI 52
#define ADJ_SUFFIX 53
#define ADV_______ 54
#define ADV__IPPAN 55
#define ADV__JOSHI 56
#define ADNOMINAL_ 57
#define CONJUNC___ 58
#define PARTICLE__ 59
#define P_KAKUJOSI 60
#define P_KA_IPPAN 61
#define P_KA_INNYO 62
#define P_KA_RENGO 63
#define P_SETUZOKU 64
#define P_KAKARIJO 65
#define P_FUKUJOSI 66
#define P_INTERJEC 67
#define P_PARALLEL 68
#define P_SHU_JOSI 69
#define P_ADV_____ 70
#define P_RENTAIKA 71
#define P_FUKUSIKA 72
#define P_SPECIAL_ 73
#define AUXILIARY_ 74
#define INTERJECT_ 75

#define KIGOU_____ 76
#define KI_IPPAN__ 77
#define KI_MARU___ 78
#define KI_TENN___ 79
#define KI_SPACE__ 80
#define KI_ALPHA__ 81
#define KI_KAKKO__ 82
#define KI_KOKKA__ 83

#define SUR_MARU "¡£"
#define SUR_TENN "¡¢"

#define HIN_MARU 78
#define HIN_TENN 79

#define LISTNAME                                                               \
  "\n"                                                                         \
  " +-- number of token\n"                                                     \
  " |  +-- subordination: no=0, parent=1, alternative=2, child=3\n"            \
  " |  |  +-- validity of token: valid=1, invalid=0\n"                         \
  " |  |  |  +-- value of exact=17, field=13, group=10\n"                      \
  " |  |  |  |  +-- number of agreed pair: no pair=-1\n"                       \
  " |  |  |  |  |  +-- number of POS\n"                                        \
  " |  |  |  |  |  |  +-- type of conjugation\n"                               \
  " |  |  |  |  |  |  |  +-- form of conjugation\n"                            \
  " |  |  |  |  |  |  |  |  +-- number of semantic identification\n"           \
  " |  |  |  |  |  |  |  |  |\n"

#define LISTPAIR                                                               \
  "\n"                                                                         \
  " +-------- number of pair\n"                                                \
  " |  +----- value of exact=17, field=13, group=10\n"                         \
  " |  |  +-- number of POS\n"                                                 \
  " |  |  |\n"                                                                 \
  " |  |  |   number of OP token -----+     +----- number of CT token\n"       \
  " |  |  |             OP token --+  |     |  +-- CT token\n"                 \
  " |  |  |                        |  |     |  |\n"

/*234567890123456789012345678901234567890123456789012345678901234567890123456*/

#define LISTBWN                                                                \
  "\n"                                                                         \
  " +-------- number of range check\n"                                         \
  " |  +----- value of exact=17, field=13, group=10\n"                         \
  " |  |  +-- number of POS\n"                                                 \
  " |  |  |\n"                                                                 \
  " |  |  |  OP token # ---+      +--------------- CT token #\n"               \
  " |  |  |  OP token --+  |      |    +---------- assumed place in CT\n"      \
  " |  |  |             |  |      |    |    +----- result: Yes=1, No=0\n"      \
  " |  |  |             |  |      |  __|__  |  +-- CT token\n"                 \
  " |  |  |             |  |      | |     | |  |\n"

/*234567890123456789012345678901234567890123456789012345678901234567890123456*/

typedef struct {
  char hir[40];
  char kan[40];
  char sid[20];
  int hin;
  int type;
  int form;
  long subnum;
} BBB;

typedef struct {
  char sid[20];
  char hir[40];
  char kan[40];
  int hin;
  int type;
  int form;
} RRR; /* semantic ID search index */

typedef struct {
  char name[20]; /* ct:author or op:kks number           */
  char sur[40];  /* surface                              */
  char hir[40];  /* hiragana                             */
  char kan[40];  /* kanji                                */
  char sid[20];  /* semantic ID                          */
  int hin;       /* Chasen POS number                    */
  int type;      /* Chasen POS type number               */
  int form;      /* Chasen POS form number               */
  int num;       /* ct:kks number or op:segment number   */
  int sub;       /* has any children? no=0 yes=1 child=2 */
  int e_valid;   /* altvalid == 1                        */
  int e_size;    /* Exact, Field, Group, Host            */
  int e_pair;    /* ct:op number  op:ct number           */
  int e_nalt;    /* the number of alternative            */
} CCC;

typedef struct {
  char chir[40];
  char phir[40];
  char ckan[40];
  char pkan[40];
  char conj[40];
  int hin;
  int type;
  int form;
} DDD;

typedef struct {
  char subsid[128];
} MMM;

int bgcomp(const void *p1, const void *p2);    /* compare for qsort */
int rvcomp(const void *p1, const void *p2);    /* compare for qsort */
int opcomp(const void *p1, const void *p2);    /* compare for bsearch */
int bgkeycomp(const void *p1, const void *p2); /* compare for bsearch */
int rvkeycomp(const void *p1, const void *p2); /* compare for bsearch */
int opkeycomp(const void *p1, const void *p2); /* compare for bsearch */
char *getfld(const char *str, const char *d, int fld);
void rcwrite(int i, char *filename);
char *takeit(char *src, char *dest, char c, int number);
