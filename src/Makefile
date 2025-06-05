# Generated automatically from Makefile.in by configure.
#
# Makefile for t2c and mkt
#

CC = gcc
DEFS =  -DHAVE_LIBM=1 
LIBS = -lm 

SRCS1 = util.c comp.c t2c.c
SRCS2 = util.c comp.c mkt.c
SRCS3 = util.c comp.c file2code.c
SRCS4 = util.c comp.c mkj.c
SRCS5 = util.c comp.c o2c.c
SRCS6 = util.c comp.c op2code.c
SRCS7 = util.c comp.c code2match.c
SRCS8 = util.c comp.c c2t.c
SRCS9 = util.c comp.c pex.c

OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
OBJS3 = $(SRCS3:.c=.o)
OBJS4 = $(SRCS4:.c=.o)
OBJS5 = $(SRCS5:.c=.o)
OBJS6 = $(SRCS6:.c=.o)
OBJS7 = $(SRCS7:.c=.o)
OBJS8 = $(SRCS8:.c=.o)
OBJS9 = $(SRCS9:.c=.o)

PROG1 = t2c
PROG2 = mkt
PROG3 = file2code
PROG4 = mkj
PROG5 = o2c
PROG6 = op2code
PROG7 = code2match
PROG8 = c2t
PROG9 = pex

JUNK = *.aux *.log *.dvi *~ $(PROG1) $(PROG2) $(PROG3) $(PROG4) $(PROG5) $(PROG6) $(PROG7) $(PROG8) $(PROG9)

all:	$(PROG1) $(PROG2) $(PROG3) $(PROG4) $(PROG5) $(PROG6) $(PROG7) $(PROG8) $(PROG9)

$(PROG1):$(OBJS1)
	$(CC) -o $@ $(OBJS1) $(LIBS)
$(PROG2):$(OBJS2)
	$(CC) -o $@ $(OBJS2) $(LIBS)
$(PROG3):$(OBJS3)
	$(CC) -o $@ $(OBJS3) $(LIBS)
$(PROG4):$(OBJS4)
	$(CC) -o $@ $(OBJS4) $(LIBS)
$(PROG5):$(OBJS5)
	$(CC) -o $@ $(OBJS5) $(LIBS)
$(PROG6):$(OBJS6)
	$(CC) -o $@ $(OBJS6) $(LIBS)
$(PROG7):$(OBJS7)
	$(CC) -o $@ $(OBJS7) $(LIBS)
$(PROG8):$(OBJS8)
	$(CC) -o $@ $(OBJS8) $(LIBS)
$(PROG9):$(OBJS9)
	$(CC) -o $@ $(OBJS9) $(LIBS)
.c.o:
	$(CC) $(DEFS) -c -o $@ $<
clean:
	rm -f $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6) $(OBJS7) $(OBJS8) $(OBJS9)
	rm -f $(JUNK)
