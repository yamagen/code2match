# code2match: code matching tools

## Directory Structure

- src: code matching tools
- data: code matching data
  - kokin: Kokin Wakashu data with wslp codes
  - kaneko: Kaneko's translation data with wslp codes
- tables: list of code:
  - bg: basic word list
  - ch: place name list
  - pn: personal name list
  - ka: kago (poetic word) list
  - category: cagegory list of bg-codes

### **code2match**: Code Matching Tools

```
% code2match [-ahv] file....
  -a   print all data
  -b   print between check
  -c   print calculation table
  -d   print predicate part out
  -e   once matched out (bag of words option)
       use it with other options
  -i   print calculation in line style
  -l   print token list table
  -o   print original poem out
  -p   print pair token table
  -r   print residual
  -s   print valid on
  -t   print title
  -u   print unmatched portion
  -h   print this help
  -v   print code2match version
(c) 2025 H. Yamamoto yamagen@ila.titech.ac.jp

```

### **data**: Code Matching Data

- data/kokin/: Kokin Wakashu data with wslp codes
- data/kaneko/: Kaneko's translation data with wslp codes

### **example**

```
% cat data/kokin/0005.db.txt data/kaneko/0005.db.txt | code2match -a
```

### Makefile for code2match tools

```
Makefile
c2t.c
code2match.c
comp.c
config.h
file2code.c
mkj.c
mkt.c
o2c.c
op2code.c
pex.c
pex.h
t2c.c
util.c
util.h
```
